﻿/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
    @brief 独自のバイナリデータ交換形式。
 */
#ifndef PSYQ_BINARC_HPP_
#define PSYQ_BINARC_HPP_

#include <memory>
#include <algorithm>

#ifndef PSYQ_ASSERT
#include <assert.h>
#define PSYQ_ASSERT assert
#endif // PSYQ_ASSERT

namespace psyq
{
    namespace binarc
    {
        typedef std::uint32_t memory_unit;
        class archive;
        class node;

        /// ノードが指す値の種別。
        enum kind: std::uint8_t
        {
            kind_NIL,      ///< 空。
            kind_BOOLEAN,  ///< 真偽値。
            kind_STRING,   ///< 文字列。
            kind_EXTENDED, ///< 拡張バイト列。
            kind_ARRAY,    ///< 配列コンテナ。
            kind_MAP,      ///< 辞書コンテナ。
            kind_UNSIGNED, ///< 符号なし整数。
            kind_NEGATIVE = kind_UNSIGNED + 3, ///< 負の整数。
            kind_FLOATING = kind_NEGATIVE + 3, ///< IEEE754浮動小数点数。
        }; // enum kind

        /// 辞書要素が存在しない場合のインデックス番号。
        std::size_t const MAP_INDEX_NONE = 0 - std::size_t(1);

        /// この名前空間をユーザーが直接アクセスするのは禁止。
        namespace _private
        {
            class map_key;
            class less_map;

            /// ノードが指す数値の格納形式。
            enum numerics: std::uint8_t
            {
                numerics_UNSIGNED_IMMEDIATE = psyq::binarc::kind_UNSIGNED,
                numerics_UNSIGNED_32,
                numerics_UNSIGNED_64,
                numerics_NEGATIVE_IMMEDIATE = psyq::binarc::kind_NEGATIVE,
                numerics_NEGATIVE_32,
                numerics_NEGATIVE_64,
                numerics_FLOATING_32 = psyq::binarc::kind_FLOATING,
                numerics_FLOATING_64,
            }; // enum numerics

            unsigned const TAG_FORMAT_BITS_SIZE = 4;
            unsigned const TAG_FORMAT_BITS_MAX = (1 << TAG_FORMAT_BITS_SIZE) - 1;
            unsigned const TAG_FORMAT_BITS_POSITION
                = sizeof(psyq::binarc::memory_unit) * 4 - TAG_FORMAT_BITS_SIZE;
            unsigned const TAG_IMMEDIATE_BITS_MASK
                = (1 << TAG_FORMAT_BITS_POSITION) - 1;
            unsigned const CONTAINER_HASH = 0;
            unsigned const CONTAINER_SIZE = CONTAINER_HASH + 1;
            unsigned const CONTAINER_FRONT = CONTAINER_SIZE + 1;
            unsigned const EXTENDED_KIND = CONTAINER_HASH + 1;
            unsigned const EXTENDED_FRONT = EXTENDED_KIND + 1;
            unsigned const UNIT_COUNT_PER_MAP_ELEMENT = 2;
        } // namespace _private
    } // namespace binarc
} // namespace psyq

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// binarc形式の書庫。
class psyq::binarc::archive
{
    /// thisが指す値の型。
    private: typedef archive this_type;

    public: typedef std::shared_ptr<this_type> shared_ptr;
    public: typedef std::weak_ptr<this_type> weak_ptr;

    public: typedef std::uint32_t unit;

    public: archive(
        void const* const in_front,
        std::size_t const in_size):
    unit_front_(static_cast<this_type::unit const*>(in_front)),
    unit_count_(in_size / sizeof(this_type::unit))
    {
        PSYQ_ASSERT(
            reinterpret_cast<std::size_t>(in_front) % sizeof(std::uint64_t) == 0
            && in_size % sizeof(this_type::unit) == 0);
        PSYQ_ASSERT(in_front != nullptr || in_size == 0);
    }

    /** @brief binarc書庫内のメモリ位置を取得する。
        @param[in] in_index メモリ単位のインデックス番号。
     */
    public: this_type::unit const* get_unit(std::size_t const in_index) const
    {
        return in_index < this->unit_count_?
            this->unit_front_ + in_index: nullptr;
    }

    private: this_type::unit const* unit_front_;
    private: std::size_t unit_count_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// binarc辞書の検索に使うキー。
class psyq::binarc::_private::map_key
{
    private: typedef map_key this_type;

    //-------------------------------------------------------------------------
    /** @brief 真偽値で、辞書検索キーを初期化する。
        @param[in] in_key 辞書検索キーの初期値。
     */
    public: map_key(bool const in_key)
    {
        this->bits_32[0] = in_key;
        this->hash = this_type::make_hash(this->bits_32[0]);
        this->format = psyq::binarc::kind_BOOLEAN;
    }

    /** @brief 無符号整数で、辞書検索キーを初期化する。
        @param[in] in_key 辞書検索キーの初期値。
     */
    public: map_key(std::uint64_t const in_key)
    {
        if ((std::numeric_limits<std::uint32_t>::max)() < in_key)
        {
            // 64ビット無符号整数キーとして初期化。
            this->bits_64 = in_key;
            this->hash = this_type::make_hash(in_key);
            this->format = psyq::binarc::_private::numerics_UNSIGNED_64;
        }
        else
        {
            // 無符号整数キーとして初期化。
            auto const local_key(static_cast<std::uint32_t>(in_key));
            this->bits_32[0] = local_key;
            this->hash = this_type::make_hash(local_key);
            this->format = psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK < local_key?
                psyq::binarc::_private::numerics_UNSIGNED_32:
                psyq::binarc::_private::numerics_UNSIGNED_IMMEDIATE;
        }
    }

    /** @brief 有符号整数で、辞書検索キーを初期化する。
        @param[in] in_key 辞書検索キーの初期値。
     */
    public: map_key(std::int64_t const in_key)
    {
        if (0 <= in_key)
        {
            // 無符号整数キーとして初期化
            new(this) this_type(static_cast<std::uint64_t>(in_key));
        }
        else if (in_key < (std::numeric_limits<std::int32_t>::min)())
        {
            // 64ビットで負の整数のキーとして初期化。
            this->bits_64 = static_cast<std::uint64_t>(in_key);
            this->hash = this_type::make_hash(static_cast<std::uint64_t>(in_key));
            this->format = psyq::binarc::_private::numerics_NEGATIVE_64;
        }
        else
        {
            // 負の整数のキーとして初期化。
            auto const local_key(static_cast<std::uint32_t>(in_key));
            this->bits_32[0] = local_key;
            auto const local_immediate_limits(
                psyq::binarc::_private::TAG_FORMAT_BITS_MAX
                << psyq::binarc::_private::TAG_FORMAT_BITS_POSITION);
            this->hash = this_type::make_hash(local_key);
            this->format = local_key < local_immediate_limits?
                psyq::binarc::_private::numerics_NEGATIVE_32:
                psyq::binarc::_private::numerics_NEGATIVE_IMMEDIATE;
        }
    }

    /** @brief 浮動小数点数で、辞書検索キーを初期化する。
        @param[in] in_key 辞書検索キーの初期値。
     */
    public: map_key(double const in_key)
    {
        auto const local_integer_key(static_cast<std::int64_t>(in_key));
        if (in_key == local_integer_key)
        {
            // 整数キーとして初期化。
            new(this) this_type(local_integer_key);
            return;
        }
        this->floating_32 = static_cast<float>(in_key);
        if (in_key == this->floating_32)
        {
            // 単精度浮動小数点数キーとして初期化。
            this->hash = this_type::make_hash(this->bits_32[0]);
            this->format = psyq::binarc::_private::numerics_FLOATING_32;
        }
        else
        {
            // 倍精度浮動小数点数キーとして初期化。
            this->floating_64 = in_key;
            this->hash = this_type::make_hash(this->bits_64);
            this->format = psyq::binarc::_private::numerics_FLOATING_64;
        }
    }

    /** @brief 文字列で、辞書検索キーを初期化する。
        @param[in] in_raw_data 文字列の先頭位置。
        @param[in] in_raw_size 文字列のバイト数。
     */
    public: map_key(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    {
        PSYQ_ASSERT(in_raw_data != nullptr || in_raw_data <= 0);
        this->raw.data = in_raw_data;
        this->raw.size = in_raw_size;
        this->hash = this_type::make_hash(in_raw_data, in_raw_size);
        this->format = psyq::binarc::kind_STRING;
    }

    //-------------------------------------------------------------------------
    /** @brief *thisとノードが指す値を比較する。
        @param[in] in_archive  ノードを含む書庫。
        @param[in] in_node_tag ノードのタグ値。
        @retval 正 *thisとノードは、ハッシュ値が異なる。
        @retval 0  *thisとノードが指す値は等値。
        @retval 負 *thisとノードのハッシュ値は等値だが、値は異なる。
     */
    public: int compare_value(
        psyq::binarc::archive const& in_archive,
        psyq::binarc::memory_unit const in_node_tag)
    const
    {
        auto const local_node_format(
            in_node_tag >> psyq::binarc::_private::TAG_FORMAT_BITS_POSITION);
        if (local_node_format != this->format)
        {
            return 1;
        }
        auto const local_node_immediate(
            in_node_tag & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK);
        switch (local_node_format)
        {
        case psyq::binarc::kind_BOOLEAN:
        case psyq::binarc::_private::numerics_UNSIGNED_IMMEDIATE:
            return local_node_immediate != this->bits_32[0]? 1: 0;
        case psyq::binarc::_private::numerics_NEGATIVE_IMMEDIATE:
        {
            auto const local_value(
                local_node_immediate | (
                    psyq::binarc::_private::TAG_FORMAT_BITS_MAX
                    << psyq::binarc::_private::TAG_FORMAT_BITS_POSITION));
            return local_value != this->bits_32[0]? 1: 0;
        }
        case psyq::binarc::_private::numerics_UNSIGNED_32:
        case psyq::binarc::_private::numerics_NEGATIVE_32:
        case psyq::binarc::_private::numerics_FLOATING_32:
        {
            auto const local_body(
                in_archive.get_unit(local_node_immediate));
            if (local_body == nullptr)
            {
                break;
            }
            return *local_body != this->bits_32[0]? 1: 0;
        }
        case psyq::binarc::_private::numerics_UNSIGNED_64:
        case psyq::binarc::_private::numerics_NEGATIVE_64:
        case psyq::binarc::_private::numerics_FLOATING_64:
        {
            auto const local_body(
                reinterpret_cast<std::uint64_t const*>(
                    in_archive.get_unit(local_node_immediate)));
            if (local_body == nullptr)
            {
                break;
            }
            if (this->hash != this_type::make_hash(*local_body))
            {
                return 1;
            }
            return *local_body != this->bits_64? -1: 0;
        }
        case psyq::binarc::kind_STRING:
        case psyq::binarc::kind_EXTENDED:
        {
            auto const local_body(
                in_archive.get_unit(local_node_immediate));
            if (local_body == nullptr)
            {
                break;
            }
            if (this->hash != local_body[psyq::binarc::_private::CONTAINER_HASH])
            {
                return 1;
            }
            if (this->raw.size != local_body[psyq::binarc::_private::CONTAINER_SIZE])
            {
                return -1;
            }
            auto local_node_raw(local_body);
            if (local_node_format == psyq::binarc::kind_STRING)
            {
                local_node_raw += psyq::binarc::_private::CONTAINER_FRONT;
            }
            else if (this->raw.kind == local_body[psyq::binarc::_private::EXTENDED_KIND])
            {
                local_node_raw += psyq::binarc::_private::EXTENDED_FRONT;
            }
            else
            {
                return -1;
            }
            return std::memcmp(local_node_raw, this->raw.data, this->raw.size) != 0? 1: 0;
        }
        case psyq::binarc::kind_ARRAY:
        case psyq::binarc::kind_MAP:
        default:
            break;
        }
        return 1;
    }

    /** @brief *thisとノードのハッシュ値を比較する。
        @param[in] in_archive  ノードを含む書庫。
        @param[in] in_node_tag ノードのタグ値。
        @retval 正 *thisのほうが大きい。
        @retval 0  *thisとノードのハッシュ値は同じ。
        @retval 負 *thisのほうが小さい。
     */
    public: int compare_hash(
        psyq::binarc::archive const& in_archive,
        psyq::binarc::memory_unit const in_node_tag)
    const
    {
        auto const local_this_hash(this->hash);
        auto const local_node_hash(
            this_type::get_node_hash(in_archive, in_node_tag));
        if (local_this_hash < local_node_hash)
        {
            return -1;
        }
        if (local_node_hash < local_this_hash)
        {
            return 1;
        }
        auto const local_this_format(this->format);
        auto const local_node_format(
            in_node_tag >> psyq::binarc::_private::TAG_FORMAT_BITS_POSITION);
        if (local_this_format < local_node_format)
        {
            return -1;
        }
        if (local_node_format < local_this_format)
        {
            return 1;
        }
        return 0;
    }

    /** @brief ノードが指す値のハッシュ値を取得する。
        @param[in] in_archive  ノードを含む書庫。
        @param[in] in_node_tag ノードのタグ値。
        @return ノードが指す値のハッシュ値。
     */
    private: static psyq::binarc::memory_unit get_node_hash(
        psyq::binarc::archive const& in_archive,
        psyq::binarc::memory_unit const in_node_tag)
    {
        auto const local_format(
            in_node_tag >> psyq::binarc::_private::TAG_FORMAT_BITS_POSITION);
        auto const local_immediate(
            in_node_tag & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK);
        switch (local_format)
        {
        case psyq::binarc::kind_BOOLEAN:
        case psyq::binarc::_private::numerics_UNSIGNED_IMMEDIATE:
            return local_immediate;
        case psyq::binarc::_private::numerics_NEGATIVE_IMMEDIATE:
            return local_immediate | (
                psyq::binarc::_private::TAG_FORMAT_BITS_MAX
                << psyq::binarc::_private::TAG_FORMAT_BITS_POSITION);
        case psyq::binarc::_private::numerics_UNSIGNED_32:
        case psyq::binarc::_private::numerics_NEGATIVE_32:
        case psyq::binarc::_private::numerics_FLOATING_32:
        case psyq::binarc::kind_STRING:
        case psyq::binarc::kind_EXTENDED:
        case psyq::binarc::kind_ARRAY:
        case psyq::binarc::kind_MAP:
        {
            auto const local_body(in_archive.get_unit(local_immediate));
            if (local_body == nullptr)
            {
                break;
            }
            static_assert(psyq::binarc::_private::CONTAINER_HASH == 0, "");
            return *local_body;
        }
        case psyq::binarc::_private::numerics_UNSIGNED_64:
        case psyq::binarc::_private::numerics_NEGATIVE_64:
        case psyq::binarc::_private::numerics_FLOATING_64:
        {
            auto const local_body(
                reinterpret_cast<std::uint64_t const*>(
                    in_archive.get_unit(local_immediate)));
            if (local_body == nullptr)
            {
                break;
            }
            return this_type::make_hash(*local_body);
        }
        default:
            break;
        }
        PSYQ_ASSERT(false);
        return 0 - psyq::binarc::memory_unit(1);
    }

    /** @brief 32ビット整数のハッシュ値を算出する。
        @param[in] in_value ハッシュ値を求める32ビット整数。
        @return 32ビット整数のハッシュ値。
     */
    private: static std::uint32_t make_hash(std::uint32_t const in_value)
    {
        // 32ビット整数をそのままハッシュ値として使う。
        return in_value;
    }

    /** @brief 64ビット整数のハッシュ値を算出する。
        @param[in] in_value ハッシュ値を求める64ビット整数。
        @return 64ビット整数のハッシュ値。
     */
    private: static std::uint32_t make_hash(std::uint64_t const& in_value)
    {
        return static_cast<std::uint32_t>(in_value)
            ^  static_cast<std::uint32_t>(in_value >> 32);
    }

    /** @brief バイト列のハッシュ値を算出する。
        @param[in] in_raw_data バイト列の先頭位置。
        @param[in] in_raw_size バイト列のバイト数。
        @return バイト列のハッシュ値。
     */
    private: static std::uint32_t make_hash(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    {
        // FNV-1でハッシュ化する。
        std::uint32_t const FNV_OFFSET_BASIS_32(0x811c9dc5U);
        std::uint32_t const FNV_PRIME_32(0x1000193U);
        auto const local_begin(static_cast<std::uint8_t const*>(in_raw_data));
        auto const local_end(local_begin + in_raw_size);
        auto local_hash(FNV_OFFSET_BASIS_32);
        for (auto i(local_begin); i < local_end ; ++i)
        {
            local_hash = (FNV_PRIME_32 * local_hash) ^ *i;
        }
        return local_hash;
    }

    //-------------------------------------------------------------------------
    private: psyq::binarc::memory_unit hash;
    private: psyq::binarc::memory_unit format;
    private: union
    {
        double floating_64;
        float floating_32;
        std::uint64_t bits_64;
        std::uint32_t bits_32[2];
        struct
        {
            void const* data;
            std::size_t size;
            psyq::binarc::memory_unit kind;
        } raw;
    };

}; // class psyq::binarc::_private::map_key

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// binarc辞書の要素を比較する関数オブジェクト。
class psyq::binarc::_private::less_map
{
    private: typedef less_map this_type;

    /// binarc辞書の要素。
    public: typedef psyq::binarc::memory_unit element[2];

    public: less_map(psyq::binarc::archive const& in_archive):
    archive(in_archive)
    {}

    public: bool operator()(
        psyq::binarc::_private::map_key const& in_left_key,
        this_type::element const& in_right_element)
    const
    {
        auto const local_compare(
            in_left_key.compare_hash(this->archive, in_right_element[0]));
        return local_compare < 0;
    }

    public: bool operator()(
        this_type::element const& in_left_element,
        psyq::binarc::_private::map_key const& in_right_key)
    const
    {
        auto const local_compare(
            in_right_key.compare_hash(this->archive, in_left_element[0]));
        return 0 < local_compare;
    }

    private: psyq::binarc::archive const& archive;

}; // class psyq::binarc::_private::less_map

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// binarc形式の書庫が含む値を指すノード。
class psyq::binarc::node
{
    /// thisが指す値の型。
    private: typedef node this_type;

    private: struct container_header
    {
        psyq::binarc::memory_unit hash;
        psyq::binarc::memory_unit size;
    };

    //-------------------------------------------------------------------------
    /// @name ノードの構築
    //@{
    /** @brief 空のノードを構築する。
     */
    public: node(): tag_(nullptr) {}

    /** @brief binarc書庫の最上位にある値を指すノードを構築する。
        @param[in] in_archive 保持するbinarc書庫。
     */
    public: explicit node(psyq::binarc::archive::shared_ptr in_archive):
    archive_(std::move(in_archive))
    {
        auto const local_archive(this->archive_.get());
        if (local_archive != nullptr)
        {
            this->tag_ = local_archive->get_unit(1);
            if (this->tag_ == nullptr)
            {
                this->archive_.reset();
            }
        }
        else
        {
            this->tag_ = nullptr;
        }
    }
    //@}
    private: node(
        psyq::binarc::memory_unit const* const in_tag,
        psyq::binarc::archive::shared_ptr const& in_archive):
    tag_(in_tag),
    archive_(in_archive)
    {}

    //-------------------------------------------------------------------------
    /// @name ノードの情報
    //@{
    /** @brief *thisが指す値を含む書庫を取得する。
        @return *thisが指す値を含む書庫。
     */
    public: binarc::archive::shared_ptr const& get_archive() const
    {
        return this->archive_;
    }

    /** @brief *thisが空か判定する。
        @retval true  *thisは値を指しておらず、空。
        @retval false *thisは値を指していて、空ではない。
     */
    public: bool is_empty() const
    {
        return this->get_archive().get() == nullptr;
    }

    /** @brief *thisが指す値の種別を取得する。
        @return *thisが指す値の種別。
     */
    public: psyq::binarc::kind get_kind() const
    {
        return this_type::make_kind(this->get_format());
    }
    //@}
    private: unsigned get_format() const
    {
        return this->tag_ != nullptr?
            *(this->tag_) >> psyq::binarc::_private::TAG_FORMAT_BITS_POSITION:
            psyq::binarc::kind_NIL;
    }

    private: static psyq::binarc::kind make_kind(unsigned const in_format)
    {
        switch (in_format)
        {
        case psyq::binarc::_private::numerics_UNSIGNED_32:
        case psyq::binarc::_private::numerics_UNSIGNED_64:
            return psyq::binarc::kind_UNSIGNED;
        case psyq::binarc::_private::numerics_NEGATIVE_32:
        case psyq::binarc::_private::numerics_NEGATIVE_64:
            return psyq::binarc::kind_NEGATIVE;
        case psyq::binarc::_private::numerics_FLOATING_64:
            return psyq::binarc::kind_FLOATING;
        default:
            return in_format <= psyq::binarc::_private::numerics_FLOATING_64?
                static_cast<psyq::binarc::kind>(in_format): psyq::binarc::kind_NIL;
        }
    }

    //-------------------------------------------------------------------------
    /// @name 数値ノード
    //@{
    /** @brief *thisが数値を指すか判定する。
        @retval true  *thisは数値を指している。
        @retval false *thisは数値を指していない。
     */
    public: bool is_numerics() const
    {
        return psyq::binarc::kind_UNSIGNED <= this->get_format();
    }

    /** @brief *thisが指す数値を取得する。
        @param[in] in_default 数値の取得に失敗した場合に返す値。
        @return
            *thisが指す値を template_numerics にキャストした値。
            ただし、*thisが数値を指してない場合は、 in_default を返す。
     */
    public: template<typename template_numerics>
    template_numerics get_numerics(template_numerics const in_default) const
    {
        template_numerics local_numerics;
        return this->read_numerics(local_numerics) < 0?
            in_default: local_numerics;
    }

    /** @brief *thisが指す数値を取得する。
        @param[out] out_numerics *thisから取得した数値が代入される。
        @retval 正 *thisが指す値の等値を out_numerics へ代入。
        @retval 0  *thisが指す値を template_numerics にキャストして out_numerics へ代入。
        @retval 負 失敗。*thisは数値を指してない。 out_numerics は変わらない。
     */
    public: template<typename template_numerics>
    int read_numerics(template_numerics& out_numerics) const
    {
        switch (this->get_format())
        {
        case psyq::binarc::_private::numerics_UNSIGNED_IMMEDIATE:
            return this->read_immediate_numerics<
                template_numerics, std::make_unsigned<binarc::memory_unit>::type>(
                    out_numerics);
        case psyq::binarc::_private::numerics_UNSIGNED_32:
            return this->read_body_numerics<template_numerics, std::uint32_t>(out_numerics);
        case psyq::binarc::_private::numerics_UNSIGNED_64:
            return this->read_body_numerics<template_numerics, std::uint64_t>(out_numerics);
        case psyq::binarc::_private::numerics_NEGATIVE_IMMEDIATE:
            return this->read_immediate_numerics<
                template_numerics, std::make_signed<binarc::memory_unit>::type>(
                    out_numerics);
        case psyq::binarc::_private::numerics_NEGATIVE_32:
            return this->read_body_numerics<template_numerics, std::int32_t>(out_numerics);
        case psyq::binarc::_private::numerics_NEGATIVE_64:
            return this->read_body_numerics<template_numerics, std::int64_t>(out_numerics);
        case psyq::binarc::_private::numerics_FLOATING_32:
            return this->read_body_numerics<template_numerics, float>(out_numerics);
        case psyq::binarc::_private::numerics_FLOATING_64:
            return this->read_body_numerics<template_numerics, double>(out_numerics);
        default:
            return -1;
        }
    }
    //@}
    private: template<typename template_write, typename template_read>
    bool read_immediate_numerics(template_write& out_numerics) const
    {
        static_assert(std::is_integral<template_read>::value, "");
        PSYQ_ASSERT(this->tag_ != nullptr);
        auto const local_immediate(
            *this->tag_ & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK);
        auto const local_sign(
            (psyq::binarc::_private::TAG_FORMAT_BITS_MAX << psyq::binarc::_private::TAG_FORMAT_BITS_POSITION)
            * std::is_signed<template_read>::value);
        return this_type::read_argument_numerics(
            out_numerics,
            static_cast<template_read>(local_immediate | local_sign));
    }

    private: template<typename template_write, typename template_read>
    bool read_body_numerics(template_write& out_numerics) const
    {
        PSYQ_ASSERT(this->tag_ != nullptr && this->archive_.get() != nullptr);
        auto const local_body(
            this->archive_->get_unit(
                *this->tag_ & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK));
        PSYQ_ASSERT(local_body != nullptr);
        PSYQ_ASSERT(reinterpret_cast<std::size_t>(local_body) % sizeof(template_read) == 0);
        return this_type::read_argument_numerics(
            out_numerics, *reinterpret_cast<template_read const*>(local_body));
    }

    private: template<typename template_write, typename template_read>
    static bool read_argument_numerics(
        template_write& out_numerics,
        template_read const in_numerics)
    {
        // 読み込み値と書き込み値のビット配置が異なるか、
        // 符号が異なる場合は、等値が代入できてない。
        out_numerics = static_cast<template_write>(in_numerics);
        return in_numerics == out_numerics
            && ((in_numerics < 0) ^ (out_numerics < 0)) == 0;
    }

    //-------------------------------------------------------------------------
    /// @name 真偽値ノード
    //@{
    /** @brief *thisが指す真偽値を取得する。
        @retval 正 *thisはtrueを指している。
        @retval 0  *thisはfalseを指している。
        @retval 負 *thisは真偽値を指してない。
     */
    public: int get_boolean_state() const
    {
        return this->get_format() == psyq::binarc::kind_BOOLEAN?
            (*this->tag_ & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK) != 0: -1;
    }

    /** @brief *thisが指す真偽値を取得する。
        @param[out] out_boolean 取得した真偽値が代入される。
        @retval true 成功。 out_boolean に取得した真偽値を代入。
        @retval false
            失敗。*thisが真偽値を指してない。 out_boolean は変化しない。
     */
    public: bool read_boolean(bool& out_boolean) const
    {
        auto const local_state(this->get_boolean_state());
        if (local_state < 0)
        {
            return false;
        }
        out_boolean = (0 < local_state);
        return true;
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 文字列ノード
    //@{
    /** @brief *thisが指す文字列のバイト数を取得する。
        @return
            *thisが指す文字列のバイト数。
            ただし、*thisが文字列を指してない場合は、0を返す。
     */
    public: std::size_t get_string_size() const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_STRING));
        return local_body != nullptr?
            local_body[psyq::binarc::_private::CONTAINER_SIZE]: 0;
    }

    /** @brief *thisが指す文字列の先頭位置を取得する。
        @return
            *thisが指す文字列の先頭位置。
            ただし、*thisが文字列を指してない場合は、nullptrを返す。
     */
    public: char const* get_string_data() const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_STRING));
        return local_body != nullptr?
            reinterpret_cast<char const*>(
                local_body + psyq::binarc::_private::CONTAINER_FRONT):
            nullptr;
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 拡張バイト列ノード
    //@{
    /** @brief *thisが指す拡張バイト列のバイト数を取得する。
        @return
            *thisが指す拡張バイト列のバイト数。
            ただし、*thisが拡張バイト列を指してない場合は、0を返す。
     */
    public: std::size_t get_extended_size() const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_EXTENDED));
        return local_body != nullptr?
            local_body[psyq::binarc::_private::CONTAINER_SIZE]: 0;
    }

    /** @brief *thisが指す拡張バイト列の種別を取得する。
        @return
            *thisが指す拡張バイト列の種別。
            ただし、*thisが拡張バイト列を指してない場合は、0を返す。
     */
    public: psyq::binarc::memory_unit get_extended_kind() const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_EXTENDED));
        return local_body != nullptr?
            local_body[psyq::binarc::_private::EXTENDED_KIND]: 0;
    }

    /** @brief *thisが指す拡張バイト列の先頭位置を取得する。
        @return
            *thisが指す拡張バイト列の先頭位置。
            ただし、*thisが拡張バイト列を指してない場合は、nullptrを返す。
     */
    public: void const* get_extended_data() const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_EXTENDED));
        return local_body != nullptr
            && 0 < local_body[psyq::binarc::_private::CONTAINER_SIZE]?
                local_body + psyq::binarc::_private::EXTENDED_FRONT: nullptr;
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name コンテナノード
    //@{
    /** @brief *thisがコンテナを指すか判定する。
        @retval true  *thisはコンテナを指している。
        @retval false *thisはコンテナを指していない。
     */
    public: bool is_container() const
    {
        switch (this->get_format())
        {
        case psyq::binarc::kind_ARRAY:
        case psyq::binarc::kind_MAP:
            return true;
        default:
            return false;
        }
    }

    /** @brief *thisが指すコンテナから、要素の数を取得する。
        @return
            *thisが指すコンテナの要素の数。
            ただし、*thisがコンテナを指していない場合は、0を返す。
     */
    public: std::size_t get_container_size() const
    {
        switch (this->get_format())
        {
        case psyq::binarc::kind_ARRAY:
            return this->get_container().size;
        case psyq::binarc::kind_MAP:
            return this->get_container().size / 2;
        default:
            return 0;
        }
    }

    /** @brief *thisが指すコンテナから、要素のキーを取得する。
        @param[in] in_index 取得する要素の要素のインデックス番号。
        @return
            *thisが指すコンテナの、要素のキーを指すノード。
            ただし、該当する要素がコンテナに存在しない場合は、空ノードを返す。
     */
    public: this_type get_container_key(std::size_t const in_index) const
    {
        auto local_key(*this);
        return local_key.switch_container_key(in_index)?
            local_key: this_type();
    }

    /** @brief *thisが指すコンテナから、要素の値を取得する。
        @param[in] in_index 取得する要素のインデックス番号。
        @return
            *thisが指すコンテナの、要素の値を指すノード。
            ただし、該当する要素がコンテナに存在しない場合は、空ノードを返す。
     */
    public: this_type get_container_value(std::size_t const in_index) const
    {
        auto local_value(*this);
        return local_value.switch_container_value(in_index)?
            local_value: this_type();
    }

    /** @brief *thisが指すコンテナの、要素のキーへ切り替える。
        @param[in] in_index 切り替える要素のインデックス番号。
        @retval true  成功。*thisは、コンテナの要素のキーへ切り替わった。
        @retval false 失敗。該当する要素が存在しない。*thisは変わらない。
     */
    public: bool switch_container_key(std::size_t in_index)
    {
        switch (this->get_format())
        {
        case psyq::binarc::kind_ARRAY:
            break;
        case psyq::binarc::kind_MAP:
            in_index *= psyq::binarc::_private::UNIT_COUNT_PER_MAP_ELEMENT;
        default:
            return false;
        }
        return this->switch_container_node(in_index);
    }

    /** @brief *thisが指すコンテナの、要素の値へ切り替える。
        @param[in] in_index 切り替える要素のインデックス番号。
        @retval true  成功。*thisは、コンテナの要素の値へ切り替わった。
        @retval false 失敗。該当する要素がコンテナに存在しない。*thisは変わらない。
     */
    public: bool switch_container_value(std::size_t in_index)
    {
        switch (this->get_format())
        {
        case psyq::binarc::kind_ARRAY:
            break;
        case psyq::binarc::kind_MAP:
            in_index = in_index * psyq::binarc::_private::UNIT_COUNT_PER_MAP_ELEMENT + 1;
        default:
            return false;
        }
        return this->switch_container_node(in_index);
    }
    //@}
    /** @brief *thisが指すコンテナの、下位ノードへ切り替える。
        @param[in] in_index 切り替える下位ノードのインデックス番号。
        @retval true  成功。*thisは、コンテナの下位ノードへ切り替わった。
        @retval false 失敗。該当する下位ノードがコンテナに存在しない。*thisは変わらない。
     */
    private: bool switch_container_node(std::size_t const in_index)
    {
        auto const& local_container(this->get_container());
        if (local_container.size <= in_index)
        {
            return false;
        }
        this->tag_ = in_index
            + reinterpret_cast<psyq::binarc::memory_unit const*>(
                &local_container + 1);
        return true;
    }

    private: this_type::container_header const& get_container() const
    {
        PSYQ_ASSERT(this->is_container());
        auto const local_container(
            reinterpret_cast<this_type::container_header const*>(
                this->archive_->get_unit(
                    *this->tag_
                    & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK)));
        PSYQ_ASSERT(local_container != nullptr);
        return *local_container;
    }

    //-------------------------------------------------------------------------
    /// @name 辞書ノード
    //@{
    /** @brief *thisが指す辞書から、要素のキーを取得する。
        @param[in] in_key 取得する要素に対応するキーとなる数値。
        @return
            辞書の要素のキーを指すノード。
            ただし、該当する要素が辞書に存在しない場合は、空ノードを返す。
     */
    public: template<typename template_numerics>
    this_type get_map_key(template_numerics const in_key) const
    {
        auto local_node(*this);
        return local_node.switch_map_key(in_key)? local_node: this_type();
    }

    /** @brief *thisが指す辞書から、要素のキーを取得する。
        @param[in] in_raw_data 取得する要素に対応するキー文字列の先頭位置。
        @param[in] in_raw_size 取得する要素に対応するキー文字列のバイト数。
        @return
            辞書の要素のキーを指すノード。
            ただし、該当する要素が辞書に存在しない場合は、空ノードを返す。
     */
    public: this_type get_map_key(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    const
    {
        auto local_node(*this);
        return local_node.switch_map_key(in_raw_data, in_raw_size)?
            local_node: this_type();
    }

    /** @brief *thisが指す辞書から、要素の値を取得する。
        @param[in] in_key 取得する辞書要素に対応するキー数値。
        @return
            辞書の要素の値を指すノード。
            ただし、該当する要素が辞書に存在しない場合は、空ノードを返す。
     */
    public: template<typename template_numerics>
    this_type get_map_value(template_numerics const in_key) const
    {
        auto local_node(*this);
        return local_node.switch_map_value(in_key)? local_node: this_type();
    }

    /** @brief *thisが指す辞書から、要素の値を取得する。
        @param[in] in_raw_data 取得する要素に対応するキー文字列の先頭位置。
        @param[in] in_raw_size 取得する要素に対応するキー文字列のバイト数。
        @return
            辞書の要素の値を指すノード。
            ただし、該当する要素が辞書に存在しない場合は、空ノードを返す。
     */
    public: this_type get_map_value(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    const
    {
        auto local_node(*this);
        return local_node.switch_map_value(in_raw_data, in_raw_size)?
            local_node: this_type();
    }

    /** @brief *thisが指す辞書の、要素のキーへ切り替える。
        @param[in] in_key 切り替える要素に対応するキー。
        @retval true  成功。*thisは、辞書の要素のキーへ切り替わった。
        @retval false 失敗。該当する要素が辞書に存在しない。*thisは変わらない。
     */
    public: template<typename template_key>
    bool switch_map_key(template_key const in_key)
    {
        return this->switch_container_key(this->find_map_index(in_key));
    }

    /** @brief *thisが指す辞書の、要素のキーへ切り替える。
        @param[in] in_raw_data 切り替える要素に対応するキー文字列の先頭位置。
        @param[in] in_raw_size 切り替える要素に対応するキー文字列のバイト数。
        @retval true  成功。*thisは、辞書の要素のキーへ切り替わった。
        @retval false 失敗。該当する辞書要素が存在しない。*thisは変わらない。
     */
    public: bool switch_map_key(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    {
        return 0 < in_raw_size?
            this->switch_container_key(
                this->find_map_index(in_raw_data, in_raw_size)):
            false;
    }

    /** @brief *thisが指す辞書の、要素の値へ切り替える。
        @param[in] in_key 切り替える要素のキー。
        @retval true  成功。*thisは、辞書の要素の値へ切り替わった。
        @retval false 失敗。該当する要素が辞書に存在しない。*thisは変わらない。
     */
    public: template<typename template_numerics>
    bool switch_map_value(template_numerics const in_key)
    {
        if (this->switch_map_key(in_key))
        {
            ++this->tag_;
            return true;
        }
        return false;
    }

    /** @brief *thisが指す辞書の、要素の値へ切り替える。
        @param[in] in_raw_data 切り替える要素に対応するキー文字列の先頭位置。
        @param[in] in_raw_size 切り替える要素に対応するキー文字列のバイト数。
        @retval true  成功。*thisは、辞書の要素の値へ切り替わった。
        @retval false 失敗。該当する要素が辞書に存在しない。*thisは変わらない。
     */
    public: bool switch_map_value(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    {
        if (this->switch_map_key(in_raw_data, in_raw_size))
        {
            ++this->tag_;
            return true;
        }
        return false;
    }

    /** @brief *thisが指す辞書から、要素を検索する。
        @param[in] in_key 検索する要素に対応するキー。
        @retval true  キーに対応する要素が存在する。
        @retval false キーに対応する要素が存在しない。
     */
    public: template<typename template_numerics>
    bool find_map_key(template_numerics const in_key) const
    {
        return this->find_map_index(in_key) != this_type::MAP_INDEX_NONE;
    }

    /** @brief *thisが指す辞書から、要素を検索する。
        @param[in] in_raw_data 検索する要素に対応するキー文字列の先頭位置。
        @param[in] in_raw_size 検索する要素に対応するキー文字列のバイト数。
        @retval true  キーに対応する要素が存在する。
        @retval false キーに対応する要素が存在しない。
     */
    public: bool find_map_key(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    const
    {
        return 0 < in_raw_size
            && psyq::binarc::MAP_INDEX_NONE != this->find_map_index(
                in_raw_data, in_raw_size);
    }

    /** @brief *thisが指す辞書から、要素のインデックス番号を検索する。
        @param[in] in_key 検索する要素の真偽値キー。
        @retval !=MAP_INDEX_NONE 真偽値キーに対応する要素のインデックス番号。
        @retval ==MAP_INDEX_NONE 真偽値キーに対応する要素が存在しない。
     */
    public: std::size_t find_map_index(bool const in_key) const
    {
        return this->find_map_index(psyq::binarc::_private::map_key(in_key));
    }

    /** @brief *thisが指す辞書から、要素のインデックス番号を検索する。
        @param[in] in_key 検索する要素のキー。
        @retval !=MAP_INDEX_NONE キーに対応する要素のインデックス番号。
        @retval ==MAP_INDEX_NONE キーに対応する要素が存在しない。
     */
    public: std::size_t find_map_index(this_type const& in_key) const;

    /** @brief *thisが指す辞書から、要素のインデックス番号を検索する。
        @param[in] in_key 検索する辞書要素の数値キー。
        @retval !=MAP_INDEX_NONE 数値キーに対応する辞書要素のインデックス番号。
        @retval ==MAP_INDEX_NONE 数値キーに対応する辞書要素が存在しない。
     */
    public: template<typename template_key>
    std::size_t find_map_index(template_key const in_key) const
    {
        static_assert(
            std::is_floating_point<template_key>::value
            || std::is_unsigned<template_key>::value
            || std::is_signed<template_key>::value,
            "");
        if (std::is_floating_point<template_key>::value)
        {
            return this->find_map_index(
                psyq::binarc::_private::map_key(static_cast<double>(in_key)));
        }
        if (std::is_unsigned<template_key>::value)
        {
            return this->find_map_index(
                psyq::binarc::_private::map_key(static_cast<std::uint64_t>(in_key)));
        }
        if (std::is_signed<template_key>::value)
        {
            return this->find_map_index(
                psyq::binarc::_private::map_key(static_cast<std::int64_t>(in_key)));
        }
        return psyq::binarc::MAP_INDEX_NONE;
    }

    /** @brief *thisが指す辞書から、要素のインデックス番号を検索する。
        @param[in] in_raw_data 検索する要素のキーとなる文字列の先頭位置。
        @param[in] in_raw_size 検索する要素のキーとなる文字列のバイト数。
        @retval !=MAP_INDEX_NONE 文字列キーに対応する要素のインデックス番号。
        @retval ==MAP_INDEX_NONE 文字列キーに対応する要素が存在しない。
     */
    public: std::size_t find_map_index(
        void const* const in_raw_data,
        std::size_t const in_raw_size)
    const
    {
        if (in_raw_size <= 0)
        {
            return psyq::binarc::MAP_INDEX_NONE;
        }
        return this->find_map_index(
            psyq::binarc::_private::map_key(in_raw_data, in_raw_size));
    }
    //@}
    /** @brief *thisが指す辞書から、要素のインデックス番号を検索する。
        @param[in] in_key 検索する要素のキー。
        @retval !=MAP_INDEX_NONE キーに対応する要素のインデックス番号。
        @retval ==MAP_INDEX_NONE キーに対応する要素が存在しない。
     */
    private: std::size_t find_map_index(
        psyq::binarc::_private::map_key const& in_key)
    const
    {
        auto const local_body(this->get_body(psyq::binarc::kind_MAP));
        if (local_body != nullptr)
        {
            auto const local_archive(this->archive_.get());
            PSYQ_ASSERT(local_archive != nullptr);
            auto const local_begin(
                reinterpret_cast<psyq::binarc::_private::less_map::element const*>(
                    local_body + psyq::binarc::_private::CONTAINER_FRONT));
            auto const local_end(
                local_begin + local_body[psyq::binarc::_private::CONTAINER_SIZE]);
            auto const local_position(
                std::lower_bound(
                    local_begin,
                    local_end,
                    in_key,
                    psyq::binarc::_private::less_map(*local_archive)));
            for (auto i(local_position); i < local_end; ++i)
            {
                auto const local_compare(
                    in_key.compare_value(*local_archive, (*i)[0]));
                if (0 < local_compare)
                {
                    break;
                }
                if (local_compare == 0)
                {
                    return i - local_begin;
                }
            }
        }
        return psyq::binarc::MAP_INDEX_NONE;
    }

    //-------------------------------------------------------------------------
    private: psyq::binarc::memory_unit const* get_body(unsigned const in_format)
    const
    {
        if (this->tag_ != nullptr)
        {
            auto const local_tag(*this->tag_);
            if ((local_tag >> psyq::binarc::_private::TAG_FORMAT_BITS_POSITION) == in_format)
            {
                auto const local_archive(this->archive_.get());
                if (local_archive != nullptr)
                {
                    auto const local_body(
                        local_archive->get_unit(
                            local_tag & psyq::binarc::_private::TAG_IMMEDIATE_BITS_MASK));
                    PSYQ_ASSERT(local_body != nullptr);
                    return local_body;
                }
                PSYQ_ASSERT(false);
            }
        }
        return nullptr;
    }

    //-------------------------------------------------------------------------
    /// ノードのタグ値。
    private: psyq::binarc::memory_unit const* tag_;
    /// ノードを含むbinarc書庫。
    private: psyq::binarc::archive::shared_ptr archive_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class binarc_to_block_yaml
{
    /// thisが指す値の型。
    private: typedef binarc_to_block_yaml this_type;

    //-------------------------------------------------------------------------
    public: static void convert(
        std::ostringstream& out_stream,
        psyq::binarc::archive::shared_ptr const& in_archive)
    {
        psyq::binarc::node const local_node(in_archive);
local_node.get_map_value(false);
local_node.get_map_value(0.1);
std::string local_map_key("key");
local_node.get_map_value(local_map_key.data(), local_map_key.size());
        if (!local_node.is_empty())
        {
            this_type::convert_node(out_stream, local_node);
        }
    }

    //-------------------------------------------------------------------------
    private: static void convert_node(
        std::ostringstream& out_stream,
        psyq::binarc::node const& in_node)
    {
        switch (in_node.get_kind())
        {
        case psyq::binarc::kind_UNSIGNED:
            out_stream << in_node.get_numerics<std::uint64_t>(0);
            break;
        case psyq::binarc::kind_NEGATIVE:
            out_stream << in_node.get_numerics<std::int64_t>(0);
            break;
        case psyq::binarc::kind_FLOATING:
            out_stream << in_node.get_numerics<double>(0);
            break;
        case psyq::binarc::kind_STRING:
            this_type::convert_string(out_stream, in_node);
            break;
        case psyq::binarc::kind_ARRAY:
            this_type::convert_sequence(out_stream, in_node);
            break;
        case psyq::binarc::kind_MAP:
            this_type::convert_mapping(out_stream, in_node);
            break;
        default:
            PSYQ_ASSERT(false);
            break;
        }
    }

    private: static void convert_string(
        std::ostringstream& out_stream,
        psyq::binarc::node const& in_node)
    {
        auto const local_data(in_node.get_string_data());
        assert(local_data != nullptr);
        out_stream << '\'';
        out_stream.write(local_data, in_node.get_string_size());
        out_stream << '\'';
    }

    private: static void convert_sequence(
        std::ostringstream& out_stream,
        psyq::binarc::node const& in_node)
    {
        out_stream << '[';
        auto const local_size(in_node.get_container_size());
        for (unsigned i(0); i < local_size; ++i)
        {
            if (0 < i)
            {
                out_stream << ',';
            }
            this_type::convert_node(out_stream, in_node.get_container_value(i));
        }
        out_stream << ']';
    }

    private: static void convert_mapping(
        std::ostringstream& out_stream,
        psyq::binarc::node const& in_node)
    {
        out_stream << '{';
        auto const local_size(in_node.get_container_size());
        for (unsigned i(0); i < local_size; ++i)
        {
            if (0 < i)
            {
                out_stream << ',';
            }
            this_type::convert_node(out_stream, in_node.get_container_key(i));
            out_stream << ':';
            this_type::convert_node(out_stream, in_node.get_container_value(i));
        }
        out_stream << '}';
    }
};

#endif // PSYQ_BINARC_HPP_
