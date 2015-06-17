﻿/** @file
    @brief @copybrief psyq::scenario_engine::_private::state_registry
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_SCENARIO_ENGINE_STATE_CHUNK_HPP_
#define PSYQ_SCENARIO_ENGINE_STATE_CHUNK_HPP_

/// @cond
namespace psyq
{
    namespace scenario_engine
    {
        namespace _private
        {
            template<typename, typename, typename> class state_chunk;
        } // namespace _private
    } // namespace scenario_engine
} // namespace psyq
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 状態値を格納するビット列のチャンク。
    @tparam template_key             @copydoc state_chunk::key
    @tparam template_block_container @copydoc state_chunk::block_container
    @tparam template_field_container @copydoc state_chunk::field_container
*/
template<
    typename template_key,
    typename template_block_container,
    typename template_field_container>
class psyq::scenario_engine::_private::state_chunk
{
    /// @brief thisが指す値の型。
    private: typedef state_chunk this_type;

    /// @brief チャンクの識別値。
    public: typedef template_key key;

    /// @brief 状態値ビット列ブロックを格納するコンテナ。
    public: typedef template_block_container block_container;

    /// @brief 空き領域のコンテナ。
    public: typedef template_field_container field_container;

    /// @brief 状態値ビット列の単位を表す型。
    public: typedef typename this_type::block_container::value_type block;

    public: enum: std::uint8_t
    {
        /// @brief 状態値ビット列ブロックのビット数。
        BLOCK_SIZE = sizeof(typename this_type::block) *
            psyq::scenario_engine::_private::BITS_PER_BYTE,
    };

    //-------------------------------------------------------------------------
    public: template<typename template_allocator>
    state_chunk(
        typename this_type::key in_key,
        template_allocator const& in_allocator)
    :
    blocks_(in_allocator),
    empty_fields_(in_allocator),
    key_(std::move(in_key))
    {}

    /** @brief ムーブ構築子。
        @param[in,out] io_source ムーブ元となるインスタンス。
     */
    public: state_chunk(this_type&& io_source):
    blocks_(std::move(io_source.blocks_)),
    empty_fields_(std::move(io_source.empty_fields_)),
    key_(std::move(io_source.key_))
    {
        io_source.blocks_.clear();
        io_source.empty_fields_.clear();
    }

    /** @brief ムーブ代入演算子。
        @param[in,out] io_source ムーブ元となるインスタンス。
        @return *this
     */
    public: this_type& operator=(this_type&& io_source)
    {
        if (this != &io_source)
        {
            this->blocks_ = std::move(io_source.blocks_);
            this->empty_fields_ = std::move(io_source.empty_fields_);
            this->key_ = std::move(io_source.key_);
            io_source.blocks_.clear();
            io_source.empty_fields_.clear();
        }
        return *this;
    }

    /** @brief 状態値を格納するビット領域を生成する。
        @param[in] in_size 生成するビット領域のビット数。
        @return 生成したビット領域の、ビット列ブロックでのビット位置。
     */
    public: template<typename template_state>
    std::size_t make_state_field(
        typename template_state::bit_size const in_size)
    {
        // 状態値を格納するビット領域を用意する。
        auto const local_empty_field(
            std::lower_bound(
                this->empty_fields_.begin(),
                this->empty_fields_.end(),
                in_size,
                typename template_state::format_less()));
        if (local_empty_field != this->empty_fields_.end())
        {
            // 既存の空き領域を再利用する。
            return this->reuse_empty_field<template_state>(
                in_size, local_empty_field);
        }
        else
        {
            // 新たな領域を追加する。
            return this->add_state_field<template_state>(in_size);
        }
    }

    /** @brief ビット列から値を取得する。
        @param[in] in_position 値を取得するビット列のビット位置。
        @param[in] in_size     値を取得するビット列のビット数。
        @return
            ビット列から取得した値。
            ただし、該当する値がない場合は、0を返す。
     */
    public: typename this_type::block get_bits(
        std::size_t const in_position,
        std::size_t const in_size)
    const PSYQ_NOEXCEPT
    {
        if (this_type::BLOCK_SIZE < in_size)
        {
            PSYQ_ASSERT(false);
            return 0;
        }
        auto const local_block_index(in_position / this_type::BLOCK_SIZE);
        if (this->blocks_.size() <= local_block_index)
        {
            PSYQ_ASSERT(false);
            return 0;
        }

        // ビット列ブロックでのビット位置を決定し、値を取り出す。
        auto const local_position(
            in_position - local_block_index * this_type::BLOCK_SIZE);
        PSYQ_ASSERT(
            (in_size == this_type::BLOCK_SIZE && local_position == 0)
            || (in_size < this_type::BLOCK_SIZE
                && local_position < this_type::BLOCK_SIZE));
        return (this->blocks_.at(local_block_index) >> local_position) &
            this_type::make_block_mask(in_size);
    }

    /** @brief ビット列に値を設定する。
        @param[in] in_position 値を設定するビット列のビット位置。
        @param[in] in_size     値を設定するビット列のビット数。
        @param[in] in_value    設定する値。
        @retval 正 元とは異なる値を設定した。
        @retval  0 元と同じ値を設定した。
        @retval 負 失敗。値を設定できなかった。
     */
    public: std::int8_t set_bits(
        std::size_t const in_position,
        std::size_t const in_size,
        typename this_type::block const in_value)
    PSYQ_NOEXCEPT
    {
        return this_type::set_bits(
            this->blocks_, in_position, in_size, in_value);
    }

    public:
    static typename this_type::block make_block_mask(
        std::size_t const in_size)
    PSYQ_NOEXCEPT
    {
        auto const local_max((std::numeric_limits<block>::max)());
        return in_size < this_type::BLOCK_SIZE?
            ~(local_max << in_size): local_max;
    }

    //-------------------------------------------------------------------------
    /** @brief ビット列に値を設定する。
        @param[in,out] io_blocks 値を設定するビット列のコンテナ。
        @param[in] in_position   値を設定するビット列のビット位置。
        @param[in] in_size       値を設定するビット列のビット数。
        @param[in] in_value      設定する値。
        @retval 正 元とは異なる値を設定した。
        @retval  0 元と同じ値を設定した。
        @retval 負 失敗。値を設定できなかった。
     */
    private: static std::int8_t set_bits(
        typename this_type::block_container& io_blocks,
        std::size_t const in_position,
        std::size_t const in_size,
        typename this_type::block const in_value)
    PSYQ_NOEXCEPT
    {
        auto const local_mask(this_type::make_block_mask(in_size));
        if ((~local_mask & in_value) != 0)
        {
            PSYQ_ASSERT(false);
            return -1;
        }
        auto const local_block_index(in_position / this_type::BLOCK_SIZE);
        if (io_blocks.size() <= local_block_index)
        {
            PSYQ_ASSERT(false);
            return -1;
        }

        // ビット列ブロックでのビット位置を決定し、値を埋め込む。
        auto const local_position(
            in_position - local_block_index * this_type::BLOCK_SIZE);
        PSYQ_ASSERT(local_position + in_size <= this_type::BLOCK_SIZE);
        auto& local_block(io_blocks.at(local_block_index));
        auto const local_last_block(local_block);
        local_block = (~(local_mask << local_position) & local_block)
            | ((in_value & local_mask) << local_position);
        return local_last_block != local_block;
    }

    private: template<typename template_state>
    std::size_t reuse_empty_field(
        typename template_state::bit_size const in_size,
        typename this_type::field_container::iterator const in_empty_field)
    {
        // 既存の空き領域を再利用する。
        auto const local_empty_position(
            template_state::get_position(*in_empty_field));

        // 空き領域を更新する。
        auto const local_empty_size(template_state::get_size(*in_empty_field));
        this->empty_fields_.erase(in_empty_field);
        if (in_size < local_empty_size)
        {
            this_type::add_empty_field<template_state>(
                this->empty_fields_,
                local_empty_position + in_size,
                local_empty_size - in_size);
        }
        return local_empty_position;
    }

    private: template<typename template_state>
    std::size_t add_state_field(
        typename template_state::bit_size const in_size)
    {
        // 新たにビット列ブロックを追加する。
        auto const local_position(
            this->blocks_.size() * this_type::BLOCK_SIZE);
        if (local_position <= template_state::format_POSITION_MASK)
        {
            auto const local_add_block_size(
                (in_size + this_type::BLOCK_SIZE - 1) / this_type::BLOCK_SIZE);
            this->blocks_.insert(this->blocks_.end(), local_add_block_size, 0);

            // 空き領域を追加する。
            auto const local_add_size(
                local_add_block_size * this_type::BLOCK_SIZE);
            if (in_size < local_add_size)
            {
                this_type::add_empty_field<template_state>(
                    this->empty_fields_,
                    local_position + in_size,
                    local_add_size - in_size);
            }
        }
        else
        {
            PSYQ_ASSERT(false);
        }
        return local_position;
    }

    private: template<typename template_state>
    static void add_empty_field(
        typename this_type::field_container& io_empty_fields,
        std::size_t const in_position,
        std::size_t const in_size)
    {
        if (in_position <= template_state::format_POSITION_MASK
            && in_size <= template_state::format_SIZE_MASK)
        {
            auto const local_empty_field(
               static_cast<typename template_state::format>(
                   (in_size << template_state::format_SIZE_FRONT)
                   | (in_position << template_state::format_POSITION_FRONT)));
            io_empty_fields.insert(
                std::lower_bound(
                    io_empty_fields.begin(),
                    io_empty_fields.end(),
                    local_empty_field,
                    typename template_state::format_less()),
                local_empty_field);
        }
        else
        {
            PSYQ_ASSERT(false);
        }
    }

    //-------------------------------------------------------------------------
    /// @copydoc block_container
    public: typename this_type::block_container blocks_;
    /// @copydoc field_container
    public: typename this_type::field_container empty_fields_;
    /// @brief key
    public: typename this_type::key key_;

}; // class psyq::scenario_engine::_private::state_chunk

#endif // defined(PSYQ_SCENARIO_ENGINE_STATE_CHUNK_HPP_)
// vim: set expandtab:
