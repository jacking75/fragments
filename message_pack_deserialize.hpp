﻿/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
    @brief
 */
#ifndef PSYQ_MESSAGE_PACK_DESIRIALIZE_HPP_
#define PSYQ_MESSAGE_PACK_DESIRIALIZE_HPP_

//#include "psyq/message_pack_serializer.hpp"
//#include "psyq/message_pack_pool.hpp"
//#include "psyq/message_pack_object.hpp"

/// psyq::message_pack::deserializer のスタック限界数のデフォルト値。
#ifndef PSYQ_MESSAGE_PACK_DESERIALIZER_STACK_CAPACITY_DEFAULT
#define PSYQ_MESSAGE_PACK_DESERIALIZER_STACK_CAPACITY_DEFAULT 32
#endif // !defined(PSYQ_MESSAGE_PACK_SERIALIZER_STACK_CAPACITY_DEFAULT)

namespace psyq
{
    namespace message_pack
    {
        /// @cond
        template<
            std::size_t
                = PSYQ_MESSAGE_PACK_DESERIALIZER_STACK_CAPACITY_DEFAULT>
                    class deserializer;
        /// @endcond
    }
    namespace internal
    {
        /// @cond
        template<typename = std::int8_t const*> class message_pack_istream;
        /// @endcond
    }
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<typename template_iterator>
class psyq::internal::message_pack_istream
{
    private: typedef message_pack_istream<template_iterator> self;

    public: typedef template_iterator iterator;
    public: typedef typename
        std::iterator_traits<template_iterator>::value_type char_type;
    public: typedef std::char_traits<typename self::char_type> traits_type;
    public: typedef typename self::traits_type::int_type int_type;
    //public: typedef typename self::traits_type::pos_type pos_type;
    //public: typedef typename self::traits_type::off_type off_type;
    public: typedef std::size_t pos_type;
    public: typedef int off_type;

    public: message_pack_istream(
        typename self::iterator const in_begin,
        typename self::iterator const in_end)
    :
        current_(in_begin),
        begin_(in_begin),
        end_(in_end),
        size_(std::distance(in_begin, in_end))
    {}

    public: typename self::iterator const& begin() const
    {
        return this->begin_;
    }
    public: typename self::iterator const& end() const
    {
        return this->end_;
    }
    public: typename self::iterator const& current() const
    {
        return this->current_;
    }

    public: bool eof() const
    {
        return this->end() <= this->current();
    }
    public: bool fail() const
    {
        return false;
    }

    public: typename self::char_type get()
    {
        if (this->eof())
        {
            return 0;
        }
        auto const local_char(*this->current());
        ++this->current_;
        return local_char;
    }

    public: self& read(
        typename self::char_type* const out_buffer,
        std::size_t const in_length)
    {
        auto const local_length(
            std::min<std::size_t>(in_length, this->end() - this->current()));
        std::memcpy(
            out_buffer,
            this->current(),
            sizeof(typename self::char_type) * local_length);
        this->current_ += local_length;
        return *this;
    }

    public: typename self::pos_type tellg() const
    {
        return this->current() - this->begin();
    }

    public: self& seekg(typename self::pos_type const in_offset)
    {
        this->current_ = in_offset < this->size_?
            std::next(this->begin_, in_offset): this->end_;
        return *this;
    }
    public: self& seekg(
        typename self::off_type const in_offset,
        std::ios::seek_dir const in_direction)
    {
        switch (in_direction)
        {
        case std::ios::beg:
            if (0 < in_offset)
            {
                return this->seekg(
                    static_cast<typename self::pos_type>(in_offset));
            }
            this->current_ = this->begin_;
            break;
        case std::ios::end:
            if (0 <= in_offset)
            {
                this->current_ = this->end_;
            }
            else if (this->size_ <= unsigned(-in_offset))
            {
                this->current_ = this->begin_;
            }
            else
            {
                this->current_ = std::prev(this->begin_, -in_offset);
            }
            break;
        case std::ios::cur:
            if (in_offset < 0)
            {
                auto const local_distance(
                    std::distance(this->begin_, this->current_));
                this->current_ = -in_offset < local_distance?
                    std::prev(this->current_, -in_offset): this->begin_;
            }
            else if (0 < in_offset)
            {
                auto const local_distance(
                    std::distance(this->current_, this->end_));
                this->current_ = in_offset < local_distance?
                    std::next(this->current_, in_offset): this->end_;
            }
            break;
        }
        return *this;
    }

    private: typename self::iterator current_;
    private: typename self::iterator begin_;
    private: typename self::iterator end_;
    private: std::size_t size_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<std::size_t template_stack_capacity>
class psyq::message_pack::deserializer
{
    /// thisが指す値の型。
    private: typedef deserializer<template_stack_capacity> self;

    /** @brief 直列化途中のコンテナのスタック限界数。
     */
    public: static std::size_t const stack_capacity = template_stack_capacity;

    private: enum read_result
    {
        read_result_FAILED,
        read_result_FINISH,
        read_result_ABORT,
        read_result_CONTINUE,
    };

    /// 復元中のオブジェクトの種別。
    private: enum stack_kind
    {
        stack_kind_ARRAY_ELEMENT, ///< 配列の要素。
        stack_kind_MAP_KEY,       ///< 連想配列の要素のキー。
        stack_kind_MAP_VALUE,     ///< 連想配列の要素の値。
    };

    private: struct stack
    {
        psyq::message_pack::object object;  ///< 復元中のオブジェクト。
        psyq::message_pack::object map_key; ///< 直前に復元した連想配列キー。
        std::size_t rest_size;              ///< コンテナ要素の残数。
        typename self::stack_kind kind;     ///< 復元中のオブジェクトの種別。
    };

    //-------------------------------------------------------------------------
    /** @brief ストリームを読み込み、MessagePackオブジェクトを復元する。
        @param[in,out] io_istream MessagePackを読み込む入力ストリーム。
        @param[in]     in_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[out]    out_result
            復元状態を格納する。nullptrの場合は、格納されない。
            - 正なら、MessagePackオブジェクトの復元を終了。
            - 0 なら、MessagePackオブジェクトの復元途中で終了。
            - 負なら、復元に失敗。
        @return 復元したMessagePackオブジェクト。
     */
    public: template<typename template_stream, typename template_pool>
    psyq::message_pack::root_object<template_pool> read_object(
        template_stream& io_istream,
        template_pool in_pool,
        int* const out_result = nullptr)
    {
        this->stack_.front().object.reset();
        this->stack_size_ = 0;
        this->allocate_raw_ = true;// !in_pool.is_from(in_data);
        this->sort_map_ = true;

        // ストリームの終端位置を取得する。
        auto const local_current(io_istream.tellg());
        io_istream.seekg(0, std::ios::end);
        auto const local_end(io_istream.tellg());
        io_istream.seekg(local_current);

        // ストリームからMessagePackを読み込む。
        typedef psyq::message_pack::root_object<template_pool> root_object;
        psyq::message_pack::object local_object;
        for (;;)
        {
            switch (
                this->read_sub_object(
                    local_object, io_istream, local_end, in_pool))
            {
            case self::read_result_FINISH:
                if (out_result != nullptr)
                {
                    *out_result = 1;
                }
                return root_object(local_object, std::move(in_pool));

            case self::read_result_CONTINUE:
                if (!io_istream.eof())
                {
                    break;
                }
                else if (out_result != nullptr)
                {
                    *out_result = 0;
                }
                return root_object();

            case self::read_result_ABORT:
                if (out_result != nullptr)
                {
                    *out_result = 0;
                }
                return root_object();

            case self::read_result_FAILED:
                if (out_result != nullptr)
                {
                    *out_result = -1;
                }
                return root_object();

            default:
                PSYQ_ASSERT(false);
                if (out_result != nullptr)
                {
                    *out_result = -1;
                }
                return root_object();
            }
        }
    }

    //-------------------------------------------------------------------------
    /** @brief ストリームを読み込み、MessagePackオブジェクトを復元する。
        @param[out]    out_object 復元したMessagePackオブジェクトの格納先。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream MessagePackを読み込む入力ストリーム。
     */
    private: template<typename template_stream, typename template_pool>
    typename self::read_result read_sub_object(
        psyq::message_pack::object& out_object,
        template_stream& io_istream,
        typename template_stream::streampos const& in_end,
        template_pool& io_pool)
    {
        // ストリームからMessagePack直列化形式を読み込む。
        auto const local_header(static_cast<unsigned>(io_istream.get()));
        if (io_istream.fail())
        {
            PSYQ_ASSERT(false);
            return self::read_result_FAILED;
        }

        // MessagePackの直列化形式によって、復元処理を分岐する。
        std::size_t local_read(0);
        if (local_header <= psyq::message_pack::header_FIX_INT_MAX)
        {
            // [0x00, 0x7f]: positive fixnum
            out_object = local_header;
        }
        else if (local_header <= psyq::message_pack::header_FIX_MAP_MAX)
        {
            // [0x80, 0x8f]: fix map
            return this->make_container<psyq::message_pack::object::unordered_map>(
                out_object, io_pool, local_header & 0x0f);
        }
        else if (local_header <= psyq::message_pack::header_FIX_ARRAY_MAX)
        {
            // [0x90, 0x9f]: fix array
            return this->make_container<psyq::message_pack::object::array>(
                out_object, io_pool, local_header & 0x0f);
        }
        else if (local_header <= psyq::message_pack::header_FIX_STR_MAX)
        {
            // [0xa0, 0xbf]: fix str
            return this->read_raw<psyq::message_pack::object::string>(
                out_object, io_istream, io_pool, local_header & 0x1f);
        }
        else if (local_header == psyq::message_pack::header_NIL)
        {
            // 0xc0: nil
            out_object.reset();
        }
        else if (local_header == psyq::message_pack::header_NEVER_USED)
        {
            // 0xc1: never used
            PSYQ_ASSERT(false);
            return self::read_result_FAILED;
        }
        else if (local_header == psyq::message_pack::header_FALSE)
        {
            // 0xc2: false
            out_object = false;
        }
        else if (local_header == psyq::message_pack::header_TRUE)
        {
            // 0xc3: true
            out_object = true;
        }
        else if (local_header <= psyq::message_pack::header_BIN32)
        {
            // 0xc4: bin 8
            // 0xc5: bin 16
            // 0xc6: bin 32
            local_read = 1 << (local_header - psyq::message_pack::header_BIN8);
        }
        else if (local_header <= psyq::message_pack::header_EXT32)
        {
            // 0xc7: ext 8
            // 0xc8: ext 16
            // 0xc9: ext 32
            local_read = 1 << (local_header - psyq::message_pack::header_EXT8);
        }
        else if (local_header <= psyq::message_pack::header_INT64)
        {
            // 0xca: float 32
            // 0xcb: float 64
            // 0xcc: unsigned int 8
            // 0xcd: unsigned int 16
            // 0xce: unsigned int 32
            // 0xcf: unsigned int 64
            // 0xd0: signed int 8
            // 0xd1: signed int 16
            // 0xd2: signed int 32
            // 0xd3: signed int 64
            local_read = 1 << (local_header & 0x3);
        }
        else if (local_header <= psyq::message_pack::header_FIX_EXT16)
        {
            // 0xd4: fix ext 1
            // 0xd5: fix ext 2
            // 0xd6: fix ext 4
            // 0xd7: fix ext 8
            // 0xd8: fix ext 16
            return this->read_raw<psyq::message_pack::object::extended>(
                out_object,
                io_istream,
                io_pool,
                1 << (local_header - psyq::message_pack::header_FIX_EXT1));
        }
        else if (local_header <= psyq::message_pack::header_STR32)
        {
            // 0xd9: str 8
            // 0xda: str 16
            // 0xdb: str 32
            local_read = 1 << (local_header - psyq::message_pack::header_STR8);
        }
        else if (local_header <= psyq::message_pack::header_MAP32)
        {
            // 0xdc: array 16
            // 0xdd: array 32
            // 0xde: map 16
            // 0xdf: map 32
            local_read = 2 << (local_header & 0x1);
        }
        else
        {
            // [0xe0, 0xff]: negative fixnum
            PSYQ_ASSERT(local_header <= 0xff);
            out_object = static_cast<std::int8_t>(local_header);
        }

        // 次。
        if (in_end - io_istream.tellg() < local_read)
        {
            PSYQ_ASSERT(false);
            return self::read_result_ABORT;
        }
        return 0 < local_read?
            this->read_value(out_object, io_istream, io_pool, local_header):
            this->update_container_stack(out_object);
    }

    //-------------------------------------------------------------------------
    /** @brief ストリームを読み込み、MessagePack値を復元する。
        @param[out]    out_object 復元した値の格納先。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream MessagePackを読み込む入力ストリーム。
        @param[in]     in_header  復元するMessagePack値のヘッダ。
     */
    private: template<typename template_stream, typename template_pool>
    typename self::read_result read_value(
        psyq::message_pack::object& out_object,
        template_stream& io_istream,
        template_pool& io_pool,
        unsigned const in_header)
    {
        switch (in_header)
        {
        // 無符号整数
        case psyq::message_pack::header_UINT8:
            return this->read_big_endian<std::uint8_t>(out_object, io_istream);
        case psyq::message_pack::header_UINT16:
            return this->read_big_endian<std::uint16_t>(out_object, io_istream);
        case psyq::message_pack::header_UINT32:
            return this->read_big_endian<std::uint32_t>(out_object, io_istream);
        case psyq::message_pack::header_UINT64:
            return this->read_big_endian<std::uint64_t>(out_object, io_istream);

        // 有符号整数
        case psyq::message_pack::header_INT8:
            return this->read_big_endian<std::int8_t>(out_object, io_istream);
        case psyq::message_pack::header_INT16:
            return this->read_big_endian<std::int16_t>(out_object, io_istream);
        case psyq::message_pack::header_INT32:
            return this->read_big_endian<std::int32_t>(out_object, io_istream);
        case psyq::message_pack::header_INT64:
            return this->read_big_endian<std::int64_t>(out_object, io_istream);

        // 浮動小数点数
        case psyq::message_pack::header_FLOAT32:
            return this->read_big_endian<psyq::message_pack::object::float32>(
                out_object, io_istream);
        case psyq::message_pack::header_FLOAT64:
            return this->read_big_endian<psyq::message_pack::object::float64>(
                out_object, io_istream);

        // 文字列
        case psyq::message_pack::header_STR8:
            return this->read_raw<
                std::uint8_t, psyq::message_pack::object::string>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_STR16:
            return this->read_raw<
                std::uint16_t, psyq::message_pack::object::string>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_STR32:
            return this->read_raw<
                std::uint32_t, psyq::message_pack::object::string>(
                    out_object, io_istream, io_pool);

        // バイナリ
        case psyq::message_pack::header_BIN8:
            return this->read_raw<
                std::uint8_t, psyq::message_pack::object::binary>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_BIN16:
            return this->read_raw<
                std::uint16_t, psyq::message_pack::object::binary>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_BIN32:
            return this->read_raw<
                std::uint32_t, psyq::message_pack::object::binary>(
                    out_object, io_istream, io_pool);

        // 拡張バイナリ
        case psyq::message_pack::header_EXT8:
            return this->read_raw<
                std::uint8_t, psyq::message_pack::object::extended>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_EXT16:
            return this->read_raw<
                std::uint16_t, psyq::message_pack::object::extended>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_EXT32:
            return this->read_raw<
                std::uint32_t, psyq::message_pack::object::extended>(
                    out_object, io_istream, io_pool);

        // 配列
        case psyq::message_pack::header_ARRAY16:
            return this->read_container<
                psyq::message_pack::object::array, std::uint16_t>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_ARRAY32:
            return this->read_container<
                psyq::message_pack::object::array, std::uint32_t>(
                    out_object, io_istream, io_pool);

        // 連想配列
        case psyq::message_pack::header_MAP16:
            return this->read_container<
                psyq::message_pack::object::unordered_map, std::uint16_t>(
                    out_object, io_istream, io_pool);
        case psyq::message_pack::header_MAP32:
            return this->read_container<
                psyq::message_pack::object::unordered_map, std::uint32_t>(
                    out_object, io_istream, io_pool);

        default:
            PSYQ_ASSERT(false);
            return self::read_result_FAILED;
        }
    }

    //-------------------------------------------------------------------------
    /** @brief ストリームを読み込み、MessagePackコンテナを復元する。
        @param[out]    out_object 復元したMessagePackコンテナの格納先。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream MessagePackを読み込む入力ストリーム。
     */
    private: template<
        typename template_container,
        typename template_length,
        typename template_pool,
        typename template_stream>
    typename self::read_result read_container(
        psyq::message_pack::object& out_object,
        template_stream& io_istream,
        template_pool& io_pool)
    {
        template_length local_length;
        if (!self::read_big_endian(local_length, io_istream))
        {
            PSYQ_ASSERT(false);
            local_length = 0;
        }
        return this->make_container<template_container>(out_object, io_pool, local_length);
    }

    /** @brief 空のMessagePackコンテナを生成する。
        @param[out]    out_object  生成したMessagePackコンテナの格納先。
        @param[in,out] io_pool     psyq::message_pack::pool 互換のメモリ割当子。
        @param[in]     in_capacity 生成するコンテナの容量。
     */
    private: template<typename template_container, typename template_pool>
    typename self::read_result make_container(
        psyq::message_pack::object& out_object,
        template_pool& io_pool,
        std::size_t const in_capacity)
    {
        static_assert(
            std::is_same<template_container, psyq::message_pack::object::array>::value
            || std::is_same<template_container, psyq::message_pack::object::unordered_map>::value,
            "");
        if (this->stack_.size() <= this->stack_size_)
        {
            PSYQ_ASSERT(false);
            return self::read_result_FAILED;
        }

        // コンテナを構築する。
        auto& local_stack_top(this->stack_[this->stack_size_]);
        bool local_make_container(
            self::make_container<template_container>(
                local_stack_top.object,
                io_pool,
                in_capacity,
                sizeof(std::int64_t)));
        if (!local_make_container)
        {
            PSYQ_ASSERT(false);
            return self::read_result_FAILED;
        }
        else if (0 < in_capacity)
        {
            // コンテナをスタックに積む。
            local_stack_top.kind =
                std::is_same<
                    template_container, psyq::message_pack::object::array>
                        ::value?
                            self::stack_kind_ARRAY_ELEMENT:
                            self::stack_kind_MAP_KEY;
            local_stack_top.rest_size = in_capacity;
            ++this->stack_size_;
            return self::read_result_CONTINUE;
        }
        else
        {
            out_object = local_stack_top.object;
            return this->update_container_stack(out_object);
        }
    }
    /** @brief 空のMessagePackコンテナを生成する。
        @tparam template_container 生成するMessagePackコンテナの型。
        @param[out]    out_object   生成したMessagePackコンテナの格納先。
        @param[in,out] io_pool      psyq::message_pack::pool 互換のメモリ割当子。
        @param[in]     in_capacity  コンテナの容量。
        @param[in]     in_alignment メモリ境界単位。
        @retval true  成功。
        @retval false 失敗。
     */
    private: template<typename template_container, typename template_pool>
    static bool make_container(
        psyq::message_pack::object& out_object,
        template_pool& io_pool,
        std::size_t const in_capacity,
        std::size_t const in_alignment)
    {
        typename template_container::pointer local_storage;
        if (0 < in_capacity)
        {
            local_storage = static_cast<typename template_container::pointer>(
                io_pool.allocate(
                    in_capacity
                        * sizeof(typename template_container::value_type),
                    in_alignment));
            if (local_storage == nullptr)
            {
                PSYQ_ASSERT(false);
                return false;
            }
        }
        else
        {
            local_storage = nullptr;
        }
        template_container local_container;
        local_container.reset(local_storage, 0);
        out_object = local_container;
        return true;
    }

    //-------------------------------------------------------------------------
    /** @brief ストリームを読み込み、MessagePackRAWバイト列を復元する。
        @tparam template_length MessagePackRAWバイト列のバイト長の型。
        @tparam template_raw    MessagePackRAWバイト列の型。
        @param[out]    out_object RAWバイト列を格納するMessagePackオブジェクト。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream RAWバイト列を読み込むストリーム。
     */
    private: template<
        typename template_length,
        typename template_raw,
        typename template_pool,
        typename template_stream>
    typename self::read_result read_raw(
        psyq::message_pack::object& out_object,
        template_stream& io_istream,
        template_pool& io_pool)
    {
        template_length local_size;
        if (!self::read_big_endian(local_size, io_istream))
        {
            PSYQ_ASSERT(false);
            local_size = 0;
        }
        return this->read_raw<template_raw>(
            out_object, io_istream, io_pool, local_size);
    }
    /** @brief ストリームを読み込み、MessagePackRAWバイト列を復元する。
        @tparam template_raw MessagePackRAWバイト列の型。
        @param[out]    out_object RAWバイト列を格納するMessagePackオブジェクト。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream RAWバイト列を読み込むストリーム。
        @param[in]     in_size    RAWバイト列のバイト数。
     */
    private: template<
        typename template_raw,
        typename template_pool,
        typename template_stream>
    typename self::read_result read_raw(
        psyq::message_pack::object& out_object,
        template_stream& io_istream,
        template_pool& io_pool,
        std::size_t in_size)
    {
        if (std::is_same<template_raw, psyq::message_pack::object::extended>::value)
        {
            ++in_size;
        }
        auto const local_bytes(
            static_cast<typename template_raw::pointer>(
                self::read_bytes(io_istream, io_pool, in_size, this->allocate_raw_)));
        if (local_bytes == nullptr && 0 < in_size)
        {
            PSYQ_ASSERT(false);
            in_size = 0;
        }
        template_raw local_raw;
        local_raw.reset(local_bytes, in_size);
        out_object = local_raw;
        return this->update_container_stack(out_object);
    }

    /** @brief ストリームを読み込み、RAWバイト列を復元する。
        @param[in,out] io_pool    psyq::message_pack::pool 互換のメモリ割当子。
        @param[in,out] io_istream RAWバイト列を読み込むストリーム。
        @param[in]     in_size    RAWバイト列のバイト数。
     */
    private: template<typename template_stream, typename template_pool>
    static void* read_bytes(
        template_stream& io_istream,
        template_pool& io_pool,
        std::size_t const in_size,
        bool const in_allocate)
    {
        typedef typename template_stream::char_type char_type;
        auto const local_length(
            (in_size + sizeof(char_type) - 1) / sizeof(char_type));
        if (local_length <= 0)
        {
            return nullptr;
        }
        else if (in_allocate)
        {
            auto const local_bytes(
                io_pool.allocate(local_length, sizeof(char_type)));
            if (local_bytes == nullptr)
            {
                PSYQ_ASSERT(false);
                return nullptr;
            }
            io_istream.read(
                static_cast<char_type*>(local_bytes), local_length);
            if (io_istream.fail())
            {
                PSYQ_ASSERT(false);
                //io_pool.deallocate(local_bytes, local_length);
                return nullptr;
            }
            return local_bytes;
        }
        else
        {
            /** @todo 2014.05.15 未実装。
                メモリ割当てを行わず、
                ストリームのバッファを参照するような処理を実装すること。
             */
            PSYQ_ASSERT(false);
            return nullptr;
        }
    }

    //-------------------------------------------------------------------------
    private: template<typename template_value, typename template_stream>
    typename self::read_result read_big_endian(
        psyq::message_pack::object& out_object,
        template_stream& io_istream)
    {
        template_value local_value;
        if (!self::read_big_endian(local_value, io_istream))
        {
            return self::read_result_FAILED;
        }
        out_object = local_value;
        return this->update_container_stack(out_object);
    }
    private: template<typename template_value, typename template_stream>
    static bool read_big_endian(
        template_value& out_value,
        template_stream& io_istream)
    {
        return psyq::message_pack::raw_bytes<template_value>::read_stream(
            out_value, io_istream, psyq::message_pack::big_endian);
    }

    //-------------------------------------------------------------------------
    /** @brief スタックからMessagePackオブジェクトを取り出す。
        @param[in,out] io_object スタックから取り出したオブジェクトが格納される。
     */
    private: typename self::read_result update_container_stack(
        psyq::message_pack::object& io_object)
    PSYQ_NOEXCEPT
    {
        while (0 < this->stack_size_)
        {
            auto& local_stack_top(this->stack_[this->stack_size_ - 1]);
            switch (local_stack_top.kind)
            {
            case self::stack_kind_ARRAY_ELEMENT:
            {
                // 配列に要素を追加する。
                auto const local_array(local_stack_top.object.get_array());
                if (local_array != nullptr)
                {
                    local_array->push_back(io_object);
                }
                else
                {
                    PSYQ_ASSERT(false);
                }

                // 残り要素数を更新する。
                --local_stack_top.rest_size;
                if (0 < local_stack_top.rest_size)
                {
                    return self::read_result_CONTINUE;
                }
                break;
            }

            case self::stack_kind_MAP_KEY:
                local_stack_top.map_key = io_object;
                local_stack_top.kind = self::stack_kind_MAP_VALUE;
                return self::read_result_CONTINUE;

            case self::stack_kind_MAP_VALUE:
            {
                // 連想配列に要素を追加する。
                auto const local_map(local_stack_top.object.get_unordered_map());
                if (local_map != nullptr)
                {
                    local_map->push_back(
                        std::make_pair(local_stack_top.map_key, io_object));
                }
                else
                {
                    PSYQ_ASSERT(false);
                    return self::read_result_FAILED;
                }

                // 残り要素数を更新する。
                --local_stack_top.rest_size;
                if (0 < local_stack_top.rest_size)
                {
                    local_stack_top.kind = self::stack_kind_MAP_KEY;
                    return self::read_result_CONTINUE;
                }

                // 連想配列の全要素が揃ったので、ソートする。
                if (this->sort_map_)
                {
                    local_stack_top.object.sort_map();
                }
                break;
            }

            default:
                PSYQ_ASSERT(false);
                return self::read_result_FAILED;
            }

            // オブジェクトをスタックから取り出す。
            io_object = local_stack_top.object;
            --this->stack_size_;
        }
        return self::read_result_FINISH;
    }

    //-------------------------------------------------------------------------
    private: std::array<typename self::stack, template_stack_capacity> stack_;
    private: std::size_t stack_size_; ///< スタックの要素数。
    private: bool sort_map_;
    private: bool allocate_raw_;
};

#endif // !defined(PSYQ_MESSAGE_PACK_DESIRIALIZE_HPP_)
