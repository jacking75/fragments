﻿/** @file
    @brief @copybrief psyq::if_then_engine::_private::reservoir
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_IF_THEN_ENGINE_RESERVOIR_HPP_
#define PSYQ_IF_THEN_ENGINE_RESERVOIR_HPP_

#include <unordered_map>
#include "./status_value.hpp"
#include "./status_property.hpp"
#include "./status_chunk.hpp"
#include "./status_operation.hpp"

/// @cond
namespace psyq
{
    namespace if_then_engine
    {
        namespace _private
        {
            template<typename, typename, typename, typename> class reservoir;
        } // namespace _private
    } // namespace if_then_engine
} // namespace psyq
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 状態貯蔵器。任意のビット長の状態値を保持する。

    ### 使い方の概略
    - reservoir::register_status で、状態値を登録する。
    - reservoir::extract_status で、状態値を取得する。
    - reservoir::assign_status で、状態値に代入する。

    @tparam template_float      @copydoc reservoir::status_value::float_type
    @tparam template_status_key @copydoc reservoir::status_key
    @tparam template_chunk_key  @copydoc reservoir::chunk_key
    @tparam template_allocator  @copydoc reservoir::allocator_type
 */
template<
    typename template_float,
    typename template_status_key,
    typename template_chunk_key,
    typename template_allocator>
class psyq::if_then_engine::_private::reservoir
{
    /// thisが指す値の型。
    private: typedef reservoir this_type;

    //-------------------------------------------------------------------------
    /// @brief 状態値を識別するための値を表す型。
    public: typedef template_status_key status_key;

    /// @brief チャンクを識別するための値を表す型。
    public: typedef template_chunk_key chunk_key;

    /// @brief 各種コンテナに用いるメモリ割当子の型。
    public: typedef template_allocator allocator_type;

    //-------------------------------------------------------------------------
    /// @brief 状態値のプロパティ情報。
    private: typedef
         psyq::if_then_engine::_private::status_property<
             typename this_type::chunk_key,
             std::uint32_t,
             std::uint32_t,
             std::uint8_t>
         status_property;

    /// @brief 状態値プロパティの辞書。
    private: typedef
         std::unordered_map<
             typename this_type::status_key,
             typename this_type::status_property,
             psyq::integer_hash<typename this_type::status_key>,
             std::equal_to<typename this_type::status_key>,
             typename this_type::allocator_type>
         property_map;

    //-------------------------------------------------------------------------
    /// @brief 状態値を格納するビット列のチャンク。
    private: typedef
        psyq::if_then_engine::_private::status_chunk<
            typename this_type::status_property,
            std::uint64_t,
            typename this_type::allocator_type>
        status_chunk;

    /// @brief 状態値ビット列チャンクの辞書。
    private: typedef
         std::unordered_map<
             typename this_type::chunk_key,
             typename this_type::status_chunk,
             psyq::integer_hash<typename this_type::chunk_key>,
             std::equal_to<typename this_type::chunk_key>,
             typename this_type::allocator_type>
         chunk_map;

    //-------------------------------------------------------------------------
    /// @brief 状態値。
    public: typedef
        psyq::if_then_engine::_private::status_value<
            typename this_type::status_chunk::bit_block, template_float>
        status_value;

    /// @brief 状態値の比較演算の引数。
    public: typedef
        psyq::if_then_engine::_private::status_operation<
            typename this_type::status_key,
            typename this_type::status_value::comparison,
            typename this_type::status_value>
        status_comparison;

    /// @brief 状態値の代入演算の引数。
    public: typedef
        psyq::if_then_engine::_private::status_operation<
            typename this_type::status_key,
            typename this_type::status_value::assignment,
            typename this_type::status_value>
        status_assignment;

    //-------------------------------------------------------------------------
    private: typedef psyq::float_bit_field<template_float> float_bit_field;

    private: typedef
         std::pair<
             typename this_type::status_chunk::bit_block,
             typename this_type::status_property::bit_width>
         bit_field_width;

    //-------------------------------------------------------------------------
    /** @name 構築と代入
        @{
     */
    /** @brief 空の状態貯蔵器を構築する。
        @param[in] in_chunk_buckets    チャンク辞書のバケット数。
        @param[in] in_property_buckets 状態値プロパティ辞書のバケット数。
        @param[in] in_allocator        使用するメモリ割当子の初期値。
     */
    public: reservoir(
        std::size_t const in_chunk_buckets,
        std::size_t const in_property_buckets,
        typename this_type::allocator_type const& in_allocator =
            this_type::allocator_type()):
    chunks_(
        in_chunk_buckets,
        typename this_type::chunk_map::hasher(),
        typename this_type::chunk_map::key_equal(),
        in_allocator),
    properties_(
        in_property_buckets,
        typename this_type::property_map::hasher(),
        typename this_type::property_map::key_equal(),
        in_allocator)
    {}

#ifdef PSYQ_NO_STD_DEFAULTED_FUNCTION
    /** @brief ムーブ構築子。
        @param[in,out] io_source ムーブ元となるインスタンス。
     */
    public: reservoir(this_type&& io_source) PSYQ_NOEXCEPT:
    chunks_(std::move(io_source.chunks_)),
    properties_(std::move(io_source.properties_))
    {}

    /** @brief ムーブ代入演算子。
        @param[in,out] io_source ムーブ元となるインスタンス。
        @return *this
     */
    public: this_type& operator=(this_type&& io_source) PSYQ_NOEXCEPT
    {
        this->chunks_ = std::move(io_source.chunks_);
        this->properties_ = std::move(io_source.properties_);
        return *this;
    }
#endif // defined(PSYQ_NO_STD_DEFAULTED_FUNCTION)

    /** @brief 状態貯蔵器で使われているメモリ割当子を取得する。
        @return 状態貯蔵器で使われているメモリ割当子。
     */
    public: typename this_type::allocator_type get_allocator()
    const PSYQ_NOEXCEPT
    {
        return this->properties_.get_allocator();
    }

    /** @brief 状態貯蔵器を再構築する。
        @param[in] in_chunk_buckets  チャンク辞書のバケット数。
        @param[in] in_status_buckets 状態値プロパティ辞書のバケット数。
     */
    public: void rebuild(
        std::size_t const in_chunk_buckets,
        std::size_t const in_status_buckets)
    {
        // 新たな状態値ビット列チャンク辞書を用意する。
        typename this_type::chunk_map local_chunks(
            in_chunk_buckets,
            this->chunks_.hash_function(),
            this->chunks_.key_eq(),
            this->chunks_.get_allocator());
        for (auto& local_old_chunk: this->chunks_)
        {
            auto const local_emplace(
                local_chunks.emplace(
                    local_old_chunk.first,
                    typename this_type::chunk_map::mapped_type(
                        local_old_chunk.second.bit_blocks_.get_allocator())));
            PSYQ_ASSERT(local_emplace.second);
            auto& local_new_chunk(local_emplace.first->second);
            local_new_chunk.bit_blocks_.reserve(
                local_old_chunk.second.bit_blocks_.size());
            local_new_chunk.empty_fields_.reserve(
                local_old_chunk.second.empty_fields_.size());
        }

        // 現在の辞書をコピーして整理し、新たな辞書を構築する。
        typename this_type::property_map local_properties(
            in_status_buckets,
            this->properties_.hash_function(),
            this->properties_.key_eq(),
            this->properties_.get_allocator());
        this_type::copy_statuses(
            local_properties, local_chunks, this->properties_, this->chunks_);
        for (auto& local_chunk: local_chunks)
        {
            local_chunk.second.bit_blocks_.shrink_to_fit();
            local_chunk.second.empty_fields_.shrink_to_fit();
        }
        this->properties_ = std::move(local_properties);
        this->chunks_ = std::move(local_chunks);
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値の登録
        @{
     */
    /** @brief 状態値を登録する。

        @param[in] in_chunk_key  登録する状態値を格納する状態値ビット列チャンクの識別値。
        @param[in] in_status_key 登録する状態値の識別番号。
        @param[in] in_value      登録する状態値の初期値。

        @retval true 成功。状態値を登録した。
        @retval false
            失敗。状態値を登録できなかった。
            - in_status_key に対応する状態値がすでに登録されていると失敗する。

        @sa
        - this_type::extract_status と this_type::assign_status
          で、登録した状態値にアクセスできる。
        - this_type::erase_chunk で、登録した状態値をチャンク毎に削除できる。
     */
    public: template<typename template_value>
    bool register_status(
        typename this_type::chunk_key const& in_chunk_key,
        typename this_type::status_key const& in_status_key,
        template_value const in_value)
    {
        if (std::is_floating_point<template_value>::value)
        {
            // 浮動小数点数型の状態値を登録する。
            /** @note
                コンパイル時にここで警告かエラーが発生する場合は、
                template_value が double 型で
                this_type::float_bit_field::float_type が float 型なのが原因。
             */
            typename this_type::float_bit_field::float_type const
                local_float(in_value);
            return this->register_status_bit_field(
                in_chunk_key,
                in_status_key,
                typename this_type::float_bit_field(local_float).bit_field_,
                this_type::status_value::kind_FLOAT);
        }
        else if (std::is_same<bool, template_value>::value)
        {
            // 論理型の状態値を登録する。
            return this->register_status_bit_field(
                in_chunk_key,
                in_status_key,
                in_value != 0,
                this_type::status_value::kind_BOOL);
        }
        else
        {
            // 整数型の状態値を登録する。
            return this->register_status(
                in_chunk_key,
                in_status_key,
                in_value,
                sizeof(template_value) * psyq::CHAR_BIT_WIDTH);
        }
    }

    /** @brief 整数型の状態値を登録する。

        @param[in] in_chunk_key  登録する状態値を格納する状態値ビット列チャンクの識別値。
        @param[in] in_status_key 登録する状態値の識別番号。
        @param[in] in_value      登録する状態値の初期値。
        @param[in] in_bit_width  登録する状態値のビット幅。

        @retval true 成功。状態値を登録した。
        @retval false
            失敗。状態値を登録できなかった。
            - in_status_key に対応する状態値がすでに登録されていると失敗する。
            - in_value のビット幅が in_bit_width を超えていると失敗する。
            - this_type::status_chunk::BLOCK_BIT_WIDTH より
              in_bit_width が大きいと失敗する。
            - in_bit_width が2未満だと失敗する。

        @sa
        - this_type::extract_status と this_type::assign_status
          で、登録した状態値にアクセスできる。
        - this_type::erase_chunk で、登録した状態値をチャンク毎に削除できる。
     */
    public: template<typename template_value>
    bool register_status(
        typename this_type::chunk_key const& in_chunk_key,
        typename this_type::status_key const& in_status_key,
        template_value const in_value,
        std::size_t const in_bit_width)
    {
        typedef typename this_type::status_property::format format;
        typedef typename this_type::status_chunk::bit_block bit_block;
        if(!std::is_integral<template_value>::value
            || std::is_same<bool, template_value>::value
            || this_type::status_chunk::BLOCK_BIT_WIDTH < in_bit_width
            || in_bit_width < 2)
        {
            // 適切な整数型ではないので、登録に失敗する。
            return false;
        }
        else if (std::is_signed<template_value>::value)
        {
            // 符号あり整数型の状態値を登録する。
            typedef typename this_type::status_value::signed_type signed_type;
            auto const local_value(static_cast<signed_type>(in_value));
            return !this_type::is_overflow(local_value, in_bit_width)
                && this->register_status_bit_field(
                    in_chunk_key,
                    in_status_key,
                    psyq::make_bit_mask<bit_block>(in_bit_width) & local_value,
                    -static_cast<format>(in_bit_width));
        }
        else //if (std::is_unsigned<template_value>::value)
        {
            // 符号なし整数型の状態値を登録する。
            auto const local_value(static_cast<bit_block>(in_value));
            return !this_type::is_overflow(local_value, in_bit_width)
                && this->register_status_bit_field(
                    in_chunk_key,
                    in_status_key,
                    local_value,
                    static_cast<format>(in_bit_width));
        }
    }

    /** @brief 状態値を登録する。
        @copydetails register_status(typename this_type::chunk_key const&, typename this_type::status_key const&, template_value);
     */
    public: bool register_status(
        typename this_type::chunk_key const& in_chunk_key,
        typename this_type::status_key const& in_status_key,
        typename this_type::status_value const& in_value)
    {
        auto const local_bool(in_value.get_bool());
        if (local_bool != nullptr)
        {
            return this->register_status(
                in_chunk_key, in_status_key, *local_bool);
        }
        auto const local_unsigned(in_value.get_unsigned());
        if (local_unsigned != nullptr)
        {
            return this->register_status(
                in_chunk_key,
                in_status_key,
                *local_unsigned,
                sizeof(*local_unsigned) * psyq::CHAR_BIT_WIDTH);
        }
        auto const local_signed(in_value.get_signed());
        if (local_signed != nullptr)
        {
            return this->register_status(
                in_chunk_key,
                in_status_key,
                *local_signed,
                sizeof(*local_signed) * psyq::CHAR_BIT_WIDTH);
        }
        auto const local_float(in_value.get_float());
        if (local_float != nullptr)
        {
            return this->register_status(
                in_chunk_key, in_status_key, *local_float);
        }
        return false;
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値の取得
        @{
     */
    /** @brief 状態値を取得する。

        in_status_key に対応する状態値が、
        this_type::register_status で登録されていること。

        @param[in] in_status_key 取得する状態値に対応する識別値。

        @return
            取得した状態値。状態値の取得に失敗した場合は、
            this_type::status_value::is_empty が真となる値を返す。

        @sa this_type::assign_status で、状態値を書き換えできる。
     */
    public: typename this_type::status_value extract_status(
        typename this_type::status_key const& in_status_key)
    const PSYQ_NOEXCEPT
    {
        // 状態値プロパティを取得する、
        auto const local_property_iterator(
            this->properties_.find(in_status_key));
        if (local_property_iterator == this->properties_.end())
        {
            return typename this_type::status_value();
        }
        auto& local_property(local_property_iterator->second);

        // 状態値ビット列チャンクから状態値のビット列を取得する。
        auto const local_chunk_iterator(
            this->chunks_.find(local_property.get_chunk_key()));
        if (local_chunk_iterator == this->chunks_.end())
        {
            // 状態値プロパティがあれば、
            // 対応する状態値ビット列チャンクもあるはず。
            PSYQ_ASSERT(false);
            return typename this_type::status_value();
        }
        auto const local_format(local_property.get_format());
        auto const local_bit_width(this_type::get_bit_width(local_format));
        auto const local_bit_field(
            local_chunk_iterator->second.get_bit_field(
                local_property.get_bit_position(), local_bit_width));

        // 状態値のビット構成から、構築する状態値の型を分ける。
        if (0 < local_format)
        {
            return local_format == this_type::status_value::kind_BOOL?
                // 論理型の状態値を構築する。
                typename this_type::status_value(local_bit_field != 0):
                // 符号なし整数型の状態値を構築する。
                typename this_type::status_value(local_bit_field);
        }
        else if (local_format == this_type::status_value::kind_FLOAT)
        {
            // 浮動小数点数型の状態値を構築する。
            typedef typename this_type::float_bit_field float_bit_field;
            typedef typename this_type::float_bit_field::bit_field bit_field;
            return typename this_type::status_value(
                float_bit_field(static_cast<bit_field>(local_bit_field)).float_);
        }
        else if (local_format < 0)
        {
            // 符号あり整数型の状態値を構築する。
            typedef typename this_type::status_value::signed_type signed_type;
            auto const local_rest_bit_width(
                this_type::status_chunk::BLOCK_BIT_WIDTH - local_bit_width);
            return typename this_type::status_value(
                psyq::shift_right_bitwise_fast(
                    psyq::shift_left_bitwise_fast(
                        static_cast<signed_type>(local_bit_field),
                        local_rest_bit_width),
                    local_rest_bit_width));
        }
        else
        {
            // 空の状態値は登録できないはず。
            PSYQ_ASSERT(false);
            return typename this_type::status_value();
        }
    }

    /** @brief 状態値のビット数を取得する。
        @param[in] in_status_key 状態値に対応する識別値。
        @retval !=0 状態値のビット数。
        @retval ==0 in_status_key に対応する状態値がない。
     */
    public: std::size_t extract_bit_width(
        typename this_type::status_key const& in_status_key)
    const PSYQ_NOEXCEPT
    {
        return this_type::get_bit_width(this->extract_format(in_status_key));
    }

    /** @brief 状態値の種別を取得する。
        @param[in] in_status_key 状態値に対応する識別値。
        @return
            in_status_key に対応する状態値の種別。
            in_status_key に対応する状態値がない場合は、 
            this_type::status_value::kind_EMPTY を返す。
     */
    public: typename this_type::status_value::kind extract_kind(
        typename this_type::status_key const& in_status_key)
    const PSYQ_NOEXCEPT
    {
        return this_type::get_kind(this->extract_format(in_status_key));
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値の比較
        @{
     */
    /** @brief 状態値を比較する。
        @param[in] in_comparison 状態値の比較式。
        @return 比較演算の評価結果。
     */
    public: psyq::if_then_engine::evaluation compare_status(
        typename this_type::status_comparison const& in_comparison)
    const PSYQ_NOEXCEPT
    {
        auto const local_right_key_pointer(in_comparison.get_right_key());
        if (local_right_key_pointer == nullptr)
        {
            return this->compare_status(
                in_comparison.get_key(),
                in_comparison.get_operator(),
                in_comparison.get_value());
        }

        // 右辺となる状態値を取得して演算する。
        auto const local_right_key(
            static_cast<typename this_type::status_key>(
                *local_right_key_pointer));
        if (local_right_key == *local_right_key_pointer)
        {
            return this->compare_status(
                in_comparison.get_key(),
                in_comparison.get_operator(),
                local_right_key);
        }
        return -1;
    }

    /** @brief 状態値を比較する。
        @param[in] in_left_key    左辺となる状態値の識別値。
        @param[in] in_operator    適用する比較演算子。
        @param[in] in_right_value 右辺となる値。
        @return 比較演算の評価結果。
     */
    public: psyq::if_then_engine::evaluation compare_status(
        typename this_type::status_key const& in_left_key,
        typename this_type::status_value::comparison const in_operator,
        typename this_type::status_value const& in_right_value)
    const PSYQ_NOEXCEPT
    {
        return this->extract_status(in_left_key).compare(
            in_operator, in_right_value);
    }

    /** @brief 状態値を比較する。
        @param[in] in_left_key  左辺となる状態値の識別値。
        @param[in] in_operator  適用する比較演算子。
        @param[in] in_right_key 右辺となる状態値の識別値。
        @return 比較演算の評価結果。
     */
    public: psyq::if_then_engine::evaluation compare_status(
        typename this_type::status_key const& in_left_key,
        typename this_type::status_value::comparison const in_operator,
        typename this_type::status_key const& in_right_key)
    const PSYQ_NOEXCEPT
    {
        return this->extract_status(in_left_key).compare(
            in_operator, this->extract_status(in_right_key));
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値の代入
        @{
     */
    /** @brief 状態値へ値を代入する。

        @param[in] in_status_key 代入先となる状態値に対応する識別値。
        @param[in] in_value      状態値へ代入する値。

        @retval true 成功。
        @retval false
            失敗。状態値は変化しない。
            - in_status_key に対応する状態値が
              this_type::register_status で登録されてない場合は失敗する。
            - in_status_key に対応する状態値のビット幅より
              大きい値を代入しようとすると失敗する。
            - in_value を状態値の型へ変換できない場合は失敗する。
            - 論理型以外の値を論理型の状態値へ代入しようとすると失敗する。
            - 論理型の値を論理型以外の状態値へ代入しようとすると失敗する。

        @sa this_type::extract_status で、代入した値を取得できる。
     */
    public: template<typename template_value>
    bool assign_status(
        typename this_type::status_key const& in_status_key,
        template_value const& in_value)
    PSYQ_NOEXCEPT
    {
        return this->assign_status_bit_field(in_status_key, in_value, false);
    }

    /** @brief 状態値を演算し、結果を代入する。
        @param[in] in_assignment 状態値の代入演算。
        @retval true  演算結果を状態値へ代入した。
        @retval false 失敗。状態値は変化しない。
     */
    public: bool assign_status(
        typename this_type::status_assignment const& in_assignment)
    {
        auto const local_right_key_pointer(in_assignment.get_right_key());
        if (local_right_key_pointer == nullptr)
        {
            return this->assign_status(
                in_assignment.get_key(),
                in_assignment.get_operator(),
                in_assignment.get_value());
        }

        // 右辺となる状態値を取得して演算する。
        auto const local_right_key(
            static_cast<typename this_type::status_key>(
                *local_right_key_pointer));
        return local_right_key == *local_right_key_pointer
            && this->assign_status(
                in_assignment.get_key(),
                in_assignment.get_operator(),
                local_right_key);
    }

    /** @brief 状態値を演算し、結果を代入する。
        @param[in] in_left_key    代入演算子の左辺となる状態値の識別値。
        @param[in] in_operator    適用する代入演算子。
        @param[in] in_right_value 代入演算子の右辺となる値。
        @retval true  演算結果を状態値へ代入した。
        @retval false 失敗。状態値は変化しない。
     */
    public: bool assign_status(
        typename this_type::status_key const& in_left_key,
        typename this_type::status_value::assignment const in_operator,
        typename this_type::status_value const& in_right_value)
    {
        if (in_operator == this_type::status_value::assignment_COPY)
        {
            return this->assign_status_bit_field(
                in_left_key, in_right_value, false);
        }
        auto local_left_value(this->extract_status(in_left_key));
        return local_left_value.assign(in_operator, in_right_value)
            && this->assign_status_bit_field(
                in_left_key, local_left_value, true);
    }

    /** @brief 状態値を演算し、結果を代入する。
        @param[in] in_left_key  代入演算子の左辺となる状態値の識別値。
        @param[in] in_operator  適用する代入演算子。
        @param[in] in_right_key 代入演算子の右辺となる状態値の識別値。
        @retval true  演算結果を状態値へ代入した。
        @retval false 失敗。状態値は変化しない。
     */
    public: bool assign_status(
        typename this_type::status_key const& in_left_key,
        typename this_type::status_value::assignment const in_operator,
        typename this_type::status_key const& in_right_key)
    {
        return this->assign_status(
            in_left_key, in_operator, this->extract_status(in_right_key));
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値の変化
        @{
     */
    /** @brief psyq::if_then_engine 管理者以外は、この関数は使用禁止。

        状態変化フラグを取得する。

        @param[in] in_status_key 変化フラグを取得する状態値に対応する識別値。

        @retval 正 状態変化フラグは真。
        @retval 0  状態変化フラグは偽。
        @retval 負 状態値がない。
     */
    public: std::int8_t _get_transition(
        typename this_type::status_key const& in_status_key)
    const PSYQ_NOEXCEPT
    {
        auto const local_iterator(this->properties_.find(in_status_key));
        return local_iterator != this->properties_.end()?
            local_iterator->second.get_transition(): -1;
    }

    /** @brief psyq::if_then_engine 管理者以外は、この関数は使用禁止。

        状態変化フラグを初期化する。
     */
    public: void _reset_transitions()
    {
        for (auto& local_property: this->properties_)
        {
            local_property.second.reset_transition();
        }
    }
    /// @}
    //-------------------------------------------------------------------------
    /** @name 状態値ビット列チャンク
        @{
     */
    /** @brief 状態値ビット列チャンクを削除する。
        @param[in] in_chunk_key 削除する状態値ビット列チャンクの識別値。
        @retval true  成功。チャンクを削除した。
        @retval false 失敗。識別値に対応するチャンクがない。
     */
    public: bool erase_chunk(typename this_type::chunk_key const& in_chunk_key)
    {
        // 状態値ビット列チャンクを削除する。
        if (this->chunks_.erase(in_chunk_key) == 0)
        {
            return false;
        }

        // 状態値プロパティを削除する。
        for (auto i(this->properties_.begin()); i != this->properties_.end();)
        {
            if (in_chunk_key != i->second.get_chunk_key())
            {
                ++i;
            }
            else
            {
                i = this->properties_.erase(i);
            }
        }
        return true;
    }

    /** @brief 状態値ビット列チャンクをシリアル化する。
        @param[in] in_chunk_key シリアル化する状態値ビット列チャンクの識別番号。
        @return シリアル化した状態値ビット列チャンク。
        @todo 未実装。
     */
    public: typename this_type::status_chunk::bit_block_container serialize_chunk(
        typename this_type::chunk_key const& in_chunk_key)
    const;

    /** @brief シリアル化された状態値ビット列チャンクを復元する。
        @param[in] in_chunk_key        復元する状態値ビット列チャンクの識別値。
        @param[in] in_serialized_chunk シリアル化された状態値ビット列チャンク。
        @todo 未実装。
     */
    public: bool deserialize_chunk(
        typename this_type::chunk_key const& in_chunk_key,
        typename this_type::status_chunk::bit_block_container const&
            in_serialized_chunk);
    /// @}
    //-------------------------------------------------------------------------
    /** @brief 状態値のビット構成を取得する。
        @param[in] in_status_key 状態値に対応する識別値。
        @return
            in_status_key に対応する状態値のビット構成。
            in_status_key に対応する状態値がない場合は、 
            this_type::status_value::kind_EMPTY を返す。
     */
    private: typename this_type::status_property::format extract_format(
        typename this_type::status_key const& in_status_key)
    const PSYQ_NOEXCEPT
    {
        auto const local_iterator(this->properties_.find(in_status_key));
        return local_iterator != this->properties_.end()?
            local_iterator->second.get_format():
            this_type::status_value::kind_EMPTY;
    }

    /** @brief 状態値のビット構成から、状態値のビット幅を取得する。
        @param[in] in_format 状態値のビット構成。
        @return 状態値のビット幅。
     */
    private: static typename this_type::status_property::bit_width get_bit_width(
        typename this_type::status_property::format const in_format)
    PSYQ_NOEXCEPT
    {
        switch (in_format)
        {
            case this_type::status_value::kind_EMPTY:
            case this_type::status_value::kind_BOOL:
            static_assert(
                this_type::status_value::kind_EMPTY == 0
                && this_type::status_value::kind_BOOL == 1,
                "");
            return in_format;

            case this_type::status_value::kind_FLOAT:
            return sizeof(typename this_type::status_value::float_type)
                * psyq::CHAR_BIT_WIDTH;

            default: return psyq::abs_integer(in_format);
        }
    }

    /** @brief 状態値のビット構成から、状態値の型の種別を取得する。
        @param[in] in_format 状態値のビット構成。
        @return 状態値の型の種別。
     */
    private: static typename this_type::status_value::kind get_kind(
        typename this_type::status_property::format const in_format)
    PSYQ_NOEXCEPT
    {
        switch (in_format)
        {
            case this_type::status_value::kind_EMPTY:
            case this_type::status_value::kind_BOOL:
            case this_type::status_value::kind_FLOAT:
            return
                static_cast<typename this_type::status_value::kind>(in_format);

            default:
            return in_format < 0?
                this_type::status_value::kind_SIGNED:
                this_type::status_value::kind_UNSIGNED;
        }
    }

    //-------------------------------------------------------------------------
    /** @brief 状態値をコピーして整理する。
        @param[in,out] io_properties コピー先となる状態値プロパティ辞書。
        @param[in,out] io_chunks     コピー先となる状態値ビット列チャンク辞書。
        @param[in] in_properties     コピー元となる状態値プロパティ辞書。
        @param[in] in_chunks         コピー元となる状態値ビット列チャンク辞書。
     */
    private: static void copy_statuses(
        typename this_type::property_map& io_properties,
        typename this_type::chunk_map& io_chunks,
        typename this_type::property_map const& in_properties,
        typename this_type::chunk_map const& in_chunks)
    {
        // 状態値プロパティのポインタのコンテナを構築する。
        std::vector<
            typename this_type::property_map::value_type const*,
            typename this_type::allocator_type>
                local_pointers(in_properties.get_allocator());
        local_pointers.reserve(in_properties.size());
        for (auto& local_property: in_properties)
        {
            local_pointers.push_back(&local_property);
        }

        // 状態値プロパティをビット幅の降順に並び替える。
        std::sort(
            local_pointers.begin(),
            local_pointers.end(),
            [](
                typename this_type::property_map::value_type const* const in_left,
                typename this_type::property_map::value_type const* const in_right)
            ->bool
            {
                auto const local_left(
                    this_type::get_bit_width(in_left->second.get_format()));
                auto const local_right(
                    this_type::get_bit_width(in_right->second.get_format()));
                return local_right < local_left;
            });

        // 状態値をコピーする。
        for (auto local_property: local_pointers)
        {
            this_type::copy_status(
                io_properties, io_chunks, *local_property, in_chunks);
        }
    }

    /** @brief 状態値をコピーする。
        @param[in,out] io_properties コピー先となる状態値プロパティ辞書。
        @param[in,out] io_chunks     コピー先となる状態値ビット列チャンク辞書。
        @param[in] in_property       コピー元となる状態値プロパティ。
        @param[in] in_chunks         コピー元となる状態値ビット列チャンク辞書。
     */
    private: static void copy_status(
        typename this_type::property_map& io_properties,
        typename this_type::chunk_map& io_chunks,
        typename this_type::property_map::value_type const& in_property,
        typename this_type::chunk_map const& in_chunks)
    {
        // コピー元となる状態値ビット列チャンクを取得する。
        auto const local_source_chunk_iterator(
            in_chunks.find(in_property.second.get_chunk_key()));
        if (local_source_chunk_iterator == in_chunks.end())
        {
            PSYQ_ASSERT(false);
            return;
        }
        auto const& local_source_chunk(local_source_chunk_iterator->second);

        // コピー先となる状態値を用意する。
        auto& local_target_chunk(
            *io_chunks.emplace(
                in_property.second.get_chunk_key(),
                typename this_type::chunk_map::mapped_type(
                    io_chunks.get_allocator())).first);
        auto const local_format(in_property.second.get_format());
        auto const local_target_property(
            this_type::allocate_status_bit_field(
                io_properties,
                local_target_chunk,
                in_property.first,
                local_format));
        if (local_target_property == nullptr)
        {
            PSYQ_ASSERT(false);
            return;
        }

        // 状態値ビット列をコピーする。
        auto const local_bit_width(this_type::get_bit_width(local_format));
        local_target_chunk.second.set_bit_field(
            local_target_property->second.get_bit_position(),
            local_bit_width,
            local_source_chunk.get_bit_field(
                in_property.second.get_bit_position(), local_bit_width));
        local_target_property->second.copy_transition(in_property.second);
    }

    //-------------------------------------------------------------------------
    /** @brief 状態値を登録する。

        @param[in] in_chunk_key  登録する状態値を格納する状態値ビット列チャンクの識別値。
        @param[in] in_status_key 登録する状態値の識別番号。
        @param[in] in_bit_field  登録する状態値の初期値となるビット列。
        @param[in] in_format     登録する状態値のビット構成。

        @retval true 成功。状態値を登録した。
        @retval false
            失敗。状態値を登録できなかった。
            - in_status_key に対応する状態値がすでに登録されていると失敗する。

        @sa
        - this_type::extract_status と this_type::assign_status
          で、登録した状態値にアクセスできる。
        - this_type::erase_chunk で、登録した状態値をチャンク毎に削除できる。
     */
    private: bool register_status_bit_field(
        typename this_type::chunk_key const& in_chunk_key,
        typename this_type::status_key const& in_status_key,
        typename this_type::status_chunk::bit_block const& in_bit_field,
        typename this_type::status_property::format const in_format)
    {
        // 状態値を登録するビット列チャンクを用意する。
        auto& local_chunk(
            *this->chunks_.emplace(
                in_chunk_key,
                typename this_type::chunk_map::mapped_type(
                    this->chunks_.get_allocator())).first);

        // 状態値を登録する。
        auto const local_property(
            this_type::allocate_status_bit_field(
                this->properties_, local_chunk, in_status_key, in_format));

        // 状態値に初期値を設定する。
        return local_property != nullptr
            && 0 <= local_chunk.second.set_bit_field(
                local_property->second.get_bit_position(),
                this_type::get_bit_width(in_format),
                in_bit_field);
    }

    /** @brief 状態値を登録する。
        @param[in,out] io_properties 状態値を登録する状態値プロパティの辞書。
        @param[in,out] io_chunk      状態値を登録する状態値ビット列チャンク。
        @param[in] in_status_key     登録する状態値に対応する識別値。
        @param[in] in_format         登録する状態値のビット構成。
        @retval !=nullptr 成功。     登録した状態値のプロパティを指すポインタ。
        @retval ==nullptr
            失敗。状態値を登録できなかった。
            - in_status_key に対応する状態値がすでに追加されていると失敗する。
     */
    private: static typename this_type::property_map::value_type*
    allocate_status_bit_field(
        typename this_type::property_map& io_properties,
        typename this_type::chunk_map::value_type& io_chunk,
        typename this_type::status_key const& in_status_key,
        typename this_type::status_property::format const in_format)
    {
        if (in_format != this_type::status_value::kind_EMPTY)
        {
            // 状態値プロパティを生成する。
            auto const local_emplace(
                io_properties.emplace(
                    in_status_key,
                    typename this_type::property_map::mapped_type(
                        io_chunk.first, in_format)));
            if (local_emplace.second)
            {
                // 状態値のビット位置を決定する。
                auto& local_property(*local_emplace.first);
                if (local_property.second.set_bit_position(
                        io_chunk.second.make_status_field(
                            this_type::get_bit_width(in_format))))
                {
                    PSYQ_ASSERT(in_format == local_property.second.get_format());
                    return &local_property;
                }
                else
                {
                    PSYQ_ASSERT(false);
                }
            }
        }
        else
        {
            PSYQ_ASSERT(false);
        }
        return nullptr;
    }

    /** @brief 状態値へ値を代入する。

        @param[in] in_status_key 代入先となる状態値に対応する識別値。
        @param[in] in_value      状態値へ代入する値。
        @param[in] in_mask       状態値のビット幅に収まるようマスクするか。

        @retval true 成功。
        @retval false
            失敗。状態値は変化しない。
            - in_status_key に対応する状態値が
              this_type::register_status で登録されてない場合は失敗する。
            - in_status_key に対応する状態値のビット幅より
              大きい値を代入しようとすると失敗する。
            - in_value を状態値の型へ変換できない場合は失敗する。
            - 論理型以外の値を論理型の状態値へ代入しようとすると失敗する。
            - 論理型の値を論理型以外の状態値へ代入しようとすると失敗する。

        @sa this_type::extract_status で、代入した値を取得できる。
     */
    private: template<typename template_value>
    bool assign_status_bit_field(
        typename this_type::status_key const& in_status_key,
        template_value const& in_value,
        bool const in_mask)
    PSYQ_NOEXCEPT
    {
        auto const local_property_iterator(this->properties_.find(in_status_key));
        if (local_property_iterator == this->properties_.end())
        {
            return false;
        }
        auto& local_property(local_property_iterator->second);
        return this_type::assign_status_bit_field(
            local_property,
            this->chunks_,
            this_type::make_bit_field_width(
                in_value, local_property.get_format(), in_mask));
    }

    private: static bool assign_status_bit_field(
        typename this_type::status_property& io_property,
        typename this_type::chunk_map& io_chunks,
        typename this_type::bit_field_width const& in_bit_field_width)
    PSYQ_NOEXCEPT
    {
        if (0 < in_bit_field_width.second)
        {
            // 状態値にビット列を設定する。
            auto const local_chunk_iterator(
                io_chunks.find(io_property.get_chunk_key()));
            if (local_chunk_iterator != io_chunks.end())
            {
                auto const local_set_bit_field(
                    local_chunk_iterator->second.set_bit_field(
                        io_property.get_bit_position(),
                        in_bit_field_width.second,
                        in_bit_field_width.first));
                if (0 <= local_set_bit_field)
                {
                    if (0 < local_set_bit_field)
                    {
                        // 状態値の変更を記録する。
                        io_property.set_transition();
                    }
                    return true;
                }
            }
            else
            {
                // 状態値プロパティがあるなら、状態値ビット列チャンクもあるはず。
                PSYQ_ASSERT(false);
            }
        }
        return false;
    }

    //-------------------------------------------------------------------------
    /** @brief 数値からビット列を構築する。
        @param[in] in_value  ビット列の元となる数値。
        @param[in] in_format 構築するビット列の構成。
        @param[in] in_mask   指定のビット幅に収まるようマスクするか。
        @return
            値から構築したビット列とビット幅のペア。
            構築に失敗した場合は、ビット幅が0となる。
     */
    private: static typename this_type::bit_field_width make_bit_field_width(
        typename this_type::status_value const& in_value,
        typename this_type::status_property::format const in_format,
        bool const in_mask)
    {
        // 入力値のビット列を取得する。
        auto const local_kind(this_type::get_kind(in_format));
        typename this_type::status_chunk::bit_block local_bit_field;
        if (local_kind != in_value.get_kind())
        {
            typename this_type::status_value const local_value(
                in_value, local_kind);
            if (local_value.is_empty())
            {
                return typename this_type::bit_field_width(0, 0);
            }
            local_bit_field = local_value.get_bit_field();
        }
        else
        {
            local_bit_field = in_value.get_bit_field();
        }

        // ビット列とビット幅を構築する。
        typedef typename this_type::status_property::bit_width bit_width;
        if (in_format == this_type::status_value::kind_BOOL)
        {
            return typename this_type::bit_field_width(local_bit_field, 1);
        }
        else if (in_format == this_type::status_value::kind_FLOAT)
        {
            return typename this_type::bit_field_width(
                local_bit_field,
                static_cast<bit_width>(
                    sizeof(typename this_type::status_value::float_type)
                    * psyq::CHAR_BIT_WIDTH));
        }
        else if (in_format < 0)
        {
            typedef typename this_type::status_value::signed_type signed_type;
            return this_type::make_bit_field_width<signed_type>(
                local_bit_field, static_cast<bit_width>(-in_format), in_mask);
        }
        else if (0 < in_format)
        {
            return this_type::make_bit_field_width<decltype(local_bit_field)>(
                local_bit_field, static_cast<bit_width>(in_format), in_mask);
        }
        else
        {
            PSYQ_ASSERT(false);
            return typename this_type::bit_field_width(0, 0);
        }
    }

    /** @brief 数値からビット列を構築する。
        @param[in] in_value  ビット列の元となる数値。
        @param[in] in_format 構築するビット列の構成。
        @param[in] in_mask   指定のビット幅に収まるようマスクするか。
        @return
            値から構築したビット列とビット幅のペア。
            構築に失敗した場合は、ビット幅が0となる。
     */
    private: template<typename template_value>
    static typename this_type::bit_field_width make_bit_field_width(
        template_value const& in_value,
        typename this_type::status_property::format const in_format,
        bool const in_mask)
    {
        typedef typename this_type::status_property::bit_width bit_width;
        typedef typename this_type::status_chunk::bit_block bit_block;
        if (in_format == this_type::status_value::kind_BOOL)
        {
            // 論理値のビット列を構築する。
            if (std::is_same<template_value, bool>::value)
            {
                return typename this_type::bit_field_width(in_value != 0, 1);
            }
        }
        else if (in_format == this_type::status_value::kind_FLOAT)
        {
            // 浮動小数点数のビット列を構築する。
            typedef typename this_type::float_bit_field float_bit_field;
            typedef typename float_bit_field::float_type float_type;
            return typename this_type::bit_field_width(
                float_bit_field(static_cast<float_type>(in_value)).bit_field_,
                static_cast<bit_width>(
                    sizeof(float_type) * psyq::CHAR_BIT_WIDTH));
        }
        else if (in_format < 0)
        {
            // 符号あり整数のビット列を構築する。
            typedef typename this_type::status_value::signed_type signed_type;
            return this_type::make_bit_field_width<signed_type>(
                in_value, static_cast<bit_width>(-in_format), in_mask);
        }
        else if (0 < in_format)
        {
            // 符号なし整数のビット列を構築する。
            return this_type::make_bit_field_width<bit_block>(
                in_value, static_cast<bit_width>(in_format), in_mask);
        }
        else
        {
            PSYQ_ASSERT(false);
            return typename this_type::bit_field_width(0, 0);
        }
    }

    /** @brief 数値を整数に変換してからビット列を構築する。
        @param[in] in_value     ビット列の元となる数値。
        @param[in] in_bit_width 構築するビット列の幅。
        @param[in] in_mask      指定のビット幅に収まるようマスクするか。
        @return
            数値から構築したビット列とビット幅のペア。
            構築に失敗した場合は、ビット幅が0となる。
     */
    private: template<typename template_integer, typename template_value>
    static typename this_type::bit_field_width make_bit_field_width(
        template_value const& in_value,
        typename this_type::status_property::bit_width in_bit_width,
        bool const in_mask)
    {
        auto local_integer(static_cast<template_integer>(in_value));
        if (in_value != static_cast<template_value>(local_integer))
        {
            in_bit_width = 0;
        }
        else if (in_mask)
        {
            local_integer &=
                psyq::make_bit_mask<template_integer>(in_bit_width);
        }
        else if (this_type::is_overflow(local_integer, in_bit_width))
        {
            in_bit_width = 0;
        }
        return typename this_type::bit_field_width(local_integer, in_bit_width);
    }

    /** @brief 論理値を整数に変換してビット列を構築させないためのダミー関数。
     */
    private: template<typename template_integer>
    static typename this_type::bit_field_width make_bit_field_width(
        bool, typename this_type::status_property::bit_width, bool)
    {
        // bool型の値を他の型へ変換できないようにする。
        return typename this_type::bit_field_width(0, 0);
    }

    //-------------------------------------------------------------------------
    /** @brief 符号なし整数がビット幅からあふれているか判定する。
        @param[in] in_integer   判定する整数。
        @param[in] in_bit_width ビット幅。
     */
    private: static bool is_overflow(
        typename this_type::status_value::unsigned_type const& in_integer,
        std::size_t const in_bit_width)
    PSYQ_NOEXCEPT
    {
        return psyq::shift_right_bitwise(in_integer, in_bit_width) != 0;
    }

    /** @brief 符号あり整数がビット幅からあふれているか判定する。
        @param[in] in_integer   判定する整数。
        @param[in] in_bit_width ビット幅。
     */
    private: static bool is_overflow(
        typename this_type::status_value::signed_type const& in_integer,
        std::size_t const in_bit_width)
    PSYQ_NOEXCEPT
    {
        auto const local_rest_field(
            psyq::shift_right_bitwise_fast(
                static_cast<typename this_type::status_value::unsigned_type>(
                    in_integer),
                in_bit_width - 1));
        auto const local_rest_mask(
            psyq::shift_right_bitwise_fast(
                static_cast<typename this_type::status_value::unsigned_type>(
                    psyq::shift_right_bitwise_fast(
                        in_integer,
                        sizeof(in_integer) * psyq::CHAR_BIT_WIDTH - 1)),
                in_bit_width - 1));
        return local_rest_field != local_rest_mask;
    }

    //-------------------------------------------------------------------------
    /// @brief 状態値ビット列チャンクの辞書。
    private: typename this_type::chunk_map chunks_;

    /// @brief 状態値プロパティの辞書。
    private: typename this_type::property_map properties_;

}; // class psyq::if_then_engine::_private::reservoir

#endif // !defined(PSYQ_IF_THEN_ENGINE_RESERVOIR_HPP_)
// vim: set expandtab:
