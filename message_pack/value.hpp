﻿/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
    @brief @copydoc psyq::internal::message_pack_value
 */
#ifndef PSYQ_MESSAGE_PACK_VALUE_HPP_
#define PSYQ_MESSAGE_PACK_VALUE_HPP_

//#include "psyq/message_pack/container.hpp"

/// 倍精度浮動小数点数で許容する誤差の最大値。
#ifndef PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON
#define PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON\
    (std::numeric_limits<self::floating_point_32>::epsilon() * 4)
#endif // !defined(PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON)

/// 単精度浮動小数点数で許容する誤差の最大値。
#ifndef PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON
#define PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON\
    (std::numeric_limits<self::floating_point_64>::epsilon() * 4)
#endif // !defined(PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON)

namespace psyq
{
    namespace message_pack
    {
        /// @cond
        class object;
        /// @endcond
    }

    /// この名前空間をuserが直接accessするのは禁止。
    namespace internal
    {
        /// @cond
        union message_pack_value;
        /// @endcond
    }
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief MessagePackオブジェクトの値。
    @sa psyq::message_pack::object
 */
union psyq::internal::message_pack_value
{
    private: typedef message_pack_value self; ///< thisが指す値の型。

    //-------------------------------------------------------------------------
    public: struct type
    {
        /// @brief MessagePackオブジェクトに格納する値の種別。
        enum value: std::uint8_t
        {
            NIL,               ///< 空。
            BOOLEAN,           ///< 真偽値。
            UNSIGNED_INTEGER,  ///< 0以上の整数。
            NEGATIVE_INTEGER,  ///< 0未満の整数。
            FLOATING_POINT_32, ///< 単精度浮動小数点数。
            FLOATING_POINT_64, ///< 倍精度浮動小数点数。
            STRING,            ///< 文字列を保持するRAWバイト列。
            BINARY,            ///< バイナリを保持するRAWバイト列。
            EXTENDED,          ///< 拡張バイナリを保持するRAWバイト列。
            ARRAY,             ///< MessagePack配列。
            UNORDERED_MAP,     ///< ソートしてないMessagePack連想配列。
            MAP,               ///< ソート済みのMessagePack連想配列。
        };
    };

    //-------------------------------------------------------------------------
    /// @name MessagePackオブジェクトに格納する値の型
    //@{
    /// @copydoc self::type::UNSIGNED_INTEGER
    public: typedef std::uint64_t unsigned_integer;
    /// @copydoc self::type::NEGATIVE_INTEGER
    public: typedef std::int64_t negative_integer;
    /// @copydoc self::type::FLOATING_POINT_32
    public: typedef float floating_point_32;
    /// @copydoc self::type::FLOATING_POINT_64
    public: typedef double floating_point_64;
    /// @copydoc self::type::STRING
    public: typedef psyq::internal::message_pack_container<char const> string;
    /// @copydoc self::type::BINARY
    public: typedef psyq::internal::message_pack_extended::super binary;
    /// @copydoc self::type::EXTENDED
    public: typedef psyq::internal::message_pack_extended extended;
    /// @copydoc self::type::ARRAY
    public: typedef psyq::internal::message_pack_container<psyq::message_pack::object> array;
    /// @copydoc self::type::MAP
    public: typedef psyq::internal::message_pack_map<psyq::message_pack::object> map;
    /// @copydoc self::type::UNORDERED_MAP
    public: typedef self::map::super unordered_map;
    //@}
    //-------------------------------------------------------------------------
    /// @name MessagePackオブジェクト値の構築
    //@{
    /// 空のMessagePackオブジェクト値を構築する。
    public: PSYQ_CONSTEXPR message_pack_value() PSYQ_NOEXCEPT {}

    /** @brief MessagePackオブジェクトに真偽値を格納する。
        @param[in] in_boolean MessagePackオブジェクトに格納する真偽値。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(bool const in_boolean)
    PSYQ_NOEXCEPT:
        boolean_(in_boolean)
    {}

    /** @brief MessagePackオブジェクトに整数を格納する。
        @param[in] in_integer MessagePackオブジェクトに格納する整数。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        std::int64_t const in_integer)
    PSYQ_NOEXCEPT:
        negative_integer_(in_integer)
    {}

    /** @brief MessagePackオブジェクトに浮動小数点数を格納する。
        @param[in] in_float MessagePackオブジェクトに格納する浮動小数点数。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::floating_point_64 const in_float)
    PSYQ_NOEXCEPT:
        floating_point_64_(in_float)
    {}
    /// @copydoc message_pack_value(self::floating_point_64 const)
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::floating_point_32 const in_float)
    PSYQ_NOEXCEPT:
        floating_point_32_(in_float)
    {}

    /** @brief MessagePackオブジェクトに文字列を格納する。
        @param[in] in_string MessagePackオブジェクトに格納する文字列。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::string const& in_string)
    PSYQ_NOEXCEPT:
        string_(in_string)
    {}
    /** @brief MessagePackオブジェクトにバイナリを格納する。
        @param[in] in_binary MessagePackオブジェクトに格納するバイナリ。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::binary const& in_binary)
    PSYQ_NOEXCEPT:
        binary_(in_binary)
    {}
    /** @brief MessagePackオブジェクトに文字列を格納する。
        @param[in] in_extended MessagePackオブジェクトに格納する文字列。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::extended const& in_extended)
    PSYQ_NOEXCEPT:
        extended_(in_extended)
    {}

    /** @brief MessagePackオブジェクトに配列を格納する。
        @param[in] in_array MessagePackオブジェクトに格納する配列。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::array const& in_array)
    PSYQ_NOEXCEPT:
        array_(in_array)
    {}

    /** @brief MessagePackオブジェクトに連想配列を格納する。
        @param[in] in_map MessagePackオブジェクトに格納する連想配列。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::unordered_map const& in_map)
    PSYQ_NOEXCEPT:
        unordered_map_(in_map)
    {}
    /** @brief MessagePackオブジェクトに連想配列を格納する。
        @param[in] in_map MessagePackオブジェクトに格納する連想配列。
     */
    public: explicit PSYQ_CONSTEXPR message_pack_value(
        self::map const& in_map)
    PSYQ_NOEXCEPT:
        map_(in_map)
    {}
    //@}
    //-------------------------------------------------------------------------
    /// @name MessagePackオブジェクト値の比較
    //@{
    /** @brief MessagePackオブジェクト値が等値か比較する。
        @param[in] in_left_value  左辺のMessagePackオブジェクト値。
        @param[in] in_left_type   左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_value 右辺のMessagePackオブジェクト値。
        @param[in] in_right_type  右辺のMessagePackオブジェクト値の種別。
        @retval true  左辺と右辺は等値。
        @retval false 左辺と右辺は非等値。
     */
    public: static bool equal(
        self const& in_left_value,
        self::type::value in_left_type,
        self const& in_right_value,
        self::type::value in_right_type)
    PSYQ_NOEXCEPT
    {
        if (in_left_type == self::type::MAP)
        {
            in_left_type = self::type::UNORDERED_MAP;
        }
        if (in_right_type == self::type::MAP)
        {
            in_right_type = self::type::UNORDERED_MAP;
        }
        if (in_left_type != in_right_type)
        {
            return false;
        }
        switch (in_left_type)
        {
        case self::type::NIL:
            return true;
        case self::type::BOOLEAN:
            return in_left_value.boolean_ == in_right_value.boolean_;
        case self::type::UNSIGNED_INTEGER:
            return in_left_value.unsigned_integer_
                == in_right_value.unsigned_integer_;
        case self::type::NEGATIVE_INTEGER:
            return in_left_value.negative_integer_
                == in_right_value.negative_integer_;
        case self::type::FLOATING_POINT_32:
            return 0 == self::compare_floating_point(
                in_left_value.floating_point_32_,
                in_right_value.floating_point_32_,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON);
        case self::type::FLOATING_POINT_64:
            return 0 == self::compare_floating_point(
                in_left_value.floating_point_64_,
                in_right_value.floating_point_64_,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON);
        case self::type::STRING:
            return in_left_value.string_ == in_right_value.string_;
        case self::type::BINARY:
            return in_left_value.binary_ == in_right_value.binary_;
        case self::type::EXTENDED:
            return in_left_value.extended_
                == in_right_value.extended_;
        case self::type::ARRAY:
            return in_left_value.array_ == in_right_value.array_;
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return in_left_value.map_ == in_right_value.map_;
        default:
            PSYQ_ASSERT(false);
            return false;
        }
    }

    /** @brief MessagePackオブジェクト値を比較する。
        @param[in] in_left_value  左辺のMessagePackオブジェクト値。
        @param[in] in_left_type   左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_value 右辺のMessagePackオブジェクト値。
        @param[in] in_right_type  右辺のMessagePackオブジェクト値の種別。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    public: static int compare(
        self const& in_left_value,
        self::type::value const in_left_type,
        self const& in_right_value,
        self::type::value const in_right_type)
    PSYQ_NOEXCEPT
    {
        switch (in_right_type)
        {
        case self::type::NIL:
            return in_left_type != self::type::NIL? 1: 0;
        case self::type::BOOLEAN:
            return self::compare_boolean(
                in_left_value, in_left_type, in_right_value.boolean_);
        case self::type::UNSIGNED_INTEGER:
            return self::compare_unsigned_integer(
                in_left_value, in_left_type, in_right_value.unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return self::compare_signed_integer(
                in_left_value, in_left_type, in_right_value.negative_integer_);
        case self::type::FLOATING_POINT_32:
            return self::compare_floating_point(
                in_left_value, in_left_type, in_right_value.floating_point_32_);
        case self::type::FLOATING_POINT_64:
            return self::compare_floating_point(
                in_left_value, in_left_type, in_right_value.floating_point_64_);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return self::compare_raw(
                in_left_value, in_left_type, in_right_value, in_right_type);
        case self::type::ARRAY:
            return self::compare_array(
                in_left_value, in_left_type, in_right_value.array_);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return self::compare_map(
                in_left_value, in_left_type, in_right_value.map_);
        default:
            PSYQ_ASSERT(false);
            return self::is_valid_type(in_left_type)? -1: 0;
        }
    }
    //@}

    /** @brief 正規の種別か判定する。
        @param[in] in_type 判定する種別。
        @retval true  正規の種別だった。
        @retval false 不正な種別だった。
     */
    private: static bool is_valid_type(self::type::value const in_type)
    {
        return in_type <= self::type::MAP;
    }
    //-------------------------------------------------------------------------
    /// @name MessagePackオブジェクトコンテナとの比較
    //@{
    /** @brief MessagePackオブジェクト値と連想配列を比較する。
        @param[in] in_left_value 左辺のMessagePackオブジェクト値。
        @param[in] in_left_type  左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_map  右辺のMessagePackオブジェクト連想配列。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: static int compare_map(
        self const& in_left_value,
        self::type::value const in_left_type,
        self::map const& in_right_map)
    PSYQ_NOEXCEPT
    {
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -1;//-self::compare_map(in_right_map, this->boolean_);
        case self::type::UNSIGNED_INTEGER:
            return -1;//-self::compare_map(in_right_map, this->unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return -1;//-self::compare_map(in_right_map, this->negative_integer_);
        case self::type::FLOATING_POINT_32:
            return -1;//-self::compare_map(in_right_map, this->floating_point_32_);
        case self::type::FLOATING_POINT_64:
            return -1;//-self::compare_map(in_right_map, this->floating_point_64_);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return -1;//-self::compare_map(in_right_map, this->raw_);
        case self::type::ARRAY:
            return -1;//-self::compare_map(in_right_map, this->array_);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return in_left_value.map_.compare(in_right_map);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief MessagePackオブジェクト値と配列を比較する。
        @param[in] in_left_value  左辺のMessagePackオブジェクト値。
        @param[in] in_left_type   左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_array 右辺のMessagePackオブジェクト配列。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: static int compare_array(
        self const& in_left_value,
        self::type::value const in_left_type,
        self::array const& in_right_array)
    PSYQ_NOEXCEPT
    {
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -1;//-self::compare_array(in_right_array, in_left_value.boolean_);
        case self::type::UNSIGNED_INTEGER:
            return -1;//-self::compare_array(in_right_array, in_left_value.unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return -1;//-self::compare_array(in_right_array, in_left_value.negative_integer_);
        case self::type::FLOATING_POINT_32:
            return -1;//-self::compare_array(in_right_array, in_left_value.floating_point_32_);
        case self::type::FLOATING_POINT_64:
            return -1;//-self::compare_array(in_right_array, in_left_value.floating_point_64_);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return -1;//-self::compare_array(in_right_array, in_left_value.raw_);
        case self::type::ARRAY:
            return in_left_value.array_.compare(in_right_array);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_array);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief MessagePackオブジェクト値とRAWバイト列を比較する。
        @param[in] in_left_value  左辺のMessagePackオブジェクト値。
        @param[in] in_left_type   左辺のMessagePackオブジェクト種別。
        @param[in] in_right_value 右辺のRAWバイト列。
        @param[in] in_right_type  右辺のMessagePackオブジェクト種別。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: static int compare_raw(
        self const& in_left_value,
        self::type::value const in_left_type,
        self const& in_right_value,
        self::type::value const in_right_type)
    PSYQ_NOEXCEPT
    {
        if (in_left_type != in_right_type)
        {
            return in_left_type < in_right_type? -1: 1;
        }
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -1;//-self::compare_raw(in_right_raw, in_left_value.boolean_);
        case self::type::UNSIGNED_INTEGER:
            return -1;//-self::compare_raw(in_right_raw, in_left_value.unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return -1;//-self::compare_raw(in_right_raw, in_left_value.negative_integer_);
        case self::type::FLOATING_POINT_32:
            return -1;//-self::compare_raw(in_right_raw, in_left_value.floating_point_32_);
        case self::type::FLOATING_POINT_64:
            return -1;//-self::compare_raw(in_right_raw, in_left_value.floating_point_64_);
        case self::type::STRING:
            return in_left_value.string_.compare(in_right_value.string_);
        case self::type::BINARY:
            return in_left_value.binary_.compare(in_right_value.binary_);
        case self::type::EXTENDED:
            return in_left_value.extended_.compare(
                in_right_value.extended_);
        case self::type::ARRAY:
            return 1;//self::compare_array(in_left_value.array_, in_right_raw);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_raw);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 浮動小数点数との比較
    //@{
    /** @brief MessagePackオブジェクト値と浮動小数点数を比較する。
        @param[in] in_left_value  左辺のMessagePackオブジェクト値。
        @param[in] in_left_type   左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_float 右辺の浮動小数点数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_float_type>
    static int compare_floating_point(
        self const& in_left_value,
        self::type::value const in_left_type,
        template_float_type const in_right_float)
    PSYQ_NOEXCEPT
    {
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -self::compare_floating_point(
                in_right_float,
                in_left_value.boolean_,
                self::get_epsilon(template_float_type(0)));
        case self::type::UNSIGNED_INTEGER:
            return -self::compare_floating_point(
                in_right_float,
                in_left_value.unsigned_integer_,
                self::get_epsilon(template_float_type(0)));
        case self::type::NEGATIVE_INTEGER:
            return -self::compare_floating_point(
                in_right_float,
                in_left_value.negative_integer_,
                self::get_epsilon(template_float_type(0)));
        case self::type::FLOATING_POINT_32:
            return self::compare_floating_point<template_float_type>(
                in_left_value.floating_point_32_,
                in_right_float,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON);
        case self::type::FLOATING_POINT_64:
            return self::compare_floating_point<self::floating_point_64>(
                in_left_value.floating_point_64_,
                in_right_float,
                self::get_epsilon(template_float_type(0)));
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return 1;//self::compare_raw(in_left_value.raw_, in_right_float);
        case self::type::ARRAY:
            return 1;//self::compare_array(in_left_value.array_, in_right_float);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_float);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief 浮動小数点数を比較する。
        @param[in] in_left    左辺の浮動小数点数。
        @param[in] in_right   右辺の浮動小数点数。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_float_type>
    static int compare_floating_point(
        template_float_type const in_left,
        template_float_type const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        auto const local_diff(in_left - in_right);
        return local_diff < -in_epsilon? -1: (in_epsilon < local_diff? 1: 0);
    }

    /** @brief 浮動小数点数と有符号整数を比較する。
        @param[in] in_left    左辺の浮動小数点数。
        @param[in] in_right   右辺の有符号整数。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_float_type>
    static int compare_floating_point(
        template_float_type const in_left,
        std::int64_t const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        PSYQ_ASSERT(in_right < 0);
        return in_left < -in_epsilon?
            self::compare_floating_point<self::floating_point_64>(
                in_left, static_cast<self::floating_point_64>(in_right), in_epsilon):
            1;
    }

    /** @brief 浮動小数点数と無符号整数を比較する。
        @param[in] in_left    左辺の浮動小数点数。
        @param[in] in_right   右辺の無符号整数。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_float_type>
    static int compare_floating_point(
        template_float_type const in_left,
        self::unsigned_integer const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        return in_left < -in_epsilon?
            -1:
            self::compare_floating_point(
                in_left,
                static_cast<template_float_type>(in_right),
                in_epsilon);
    }

    /** @brief 浮動小数点数と真偽値を比較する。
        @param[in] in_left    左辺の浮動小数点数。
        @param[in] in_right   右辺の真偽値。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_float_type>
    static int compare_floating_point(
        template_float_type const in_left,
        bool const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        // C++の仕様に基づき、trueは1、falseは0に変換して判定する。
        return in_left < -in_epsilon?
            -1:
            self::compare_floating_point(
                in_left,
                static_cast<template_float_type>(in_right? 1: 0),
                in_epsilon);
    }

    /** @brief 倍精度浮動小数点数で許容する誤差の最大値を取得する。
        @return 倍精度浮動小数点数で許容する誤差の最大値。
     */
    private: static self::floating_point_64 get_epsilon(self::floating_point_64 const)
    {
        return PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON;
    }
    /** @brief 単精度浮動小数点数で許容する誤差の最大値を取得する。
        @return 単精度浮動小数点数で許容する誤差の最大値。
     */
    private: static self::floating_point_32 get_epsilon(self::floating_point_32 const)
    {
        return PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON;
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 有符号整数との比較
    //@{
    /** @brief MessagePackオブジェクト値と有符号整数を比較する。
        @param[in] in_left_value    左辺のMessagePackオブジェクト値。
        @param[in] in_left_type     左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_integer 右辺の有符号整数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_signed_type>
    static int compare_signed_integer(
        self const& in_left_value,
        self::type::value const in_left_type,
        template_signed_type const in_right_integer)
    PSYQ_NOEXCEPT
    {
        static_assert(
            !std::is_unsigned<template_signed_type>::value,
            "template_signed_type is not signed integer type.");
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -self::compare_signed_integer(
                in_right_integer, in_left_value.boolean_);
        case self::type::UNSIGNED_INTEGER:
            return -self::compare_signed_integer(
                in_right_integer, in_left_value.unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return -self::compare_signed_integer(
                in_right_integer, in_left_value.negative_integer_);
        case self::type::FLOATING_POINT_32:
            return self::compare_floating_point<self::floating_point_64>(
                in_left_value.floating_point_32_,
                static_cast<self::floating_point_64>(in_right_integer),
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON);
        case self::type::FLOATING_POINT_64:
            return self::compare_floating_point<self::floating_point_64>(
                in_left_value.floating_point_64_,
                static_cast<self::floating_point_64>(in_right_integer),
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return 1;//self::compare_raw(in_left_value.raw_, in_right_integer);
        case self::type::ARRAY:
            return 1;//self::compare_array(in_left_value.array_, in_right_integer);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_integer);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief 有符号整数と浮動小数点数を比較する。
        @param[in] in_left    左辺の有符号整数。
        @param[in] in_right   右辺の浮動小数点数。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<
        typename template_signed_type,
        typename template_float_type>
    static int compare_signed_integer(
        template_signed_type const in_left,
        template_float_type const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        return self::compare_floating_point(
            static_cast<template_float_type>(in_left), in_right, in_epsilon);
    }

    /** @brief 有符号整数を比較する。
        @param[in] in_left  左辺の有符号整数。
        @param[in] in_right 右辺の有符号整数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_signed_type>
    static int compare_signed_integer(
        template_signed_type const in_left,
        std::int64_t const in_right)
    PSYQ_NOEXCEPT
    {
        return in_left < in_right? -1: (in_right < in_left? 1: 0);
    }

    /** @brief 有符号整数と無符号整数を比較する。
        @param[in] in_left  左辺の有符号整数。
        @param[in] in_right 右辺の無符号整数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_signed_type>
    static int compare_signed_integer(
        template_signed_type const in_left,
        self::unsigned_integer const in_right)
    PSYQ_NOEXCEPT
    {
        typedef typename std::make_unsigned<template_signed_type>::type
            unsigned_type;
        return in_left < 0?
            -1:
            self::compare_unsigned_integer(
                static_cast<unsigned_type>(in_left), in_right);
    }

    /** @brief 有符号整数と真偽値を比較する。
        @param[in] in_left  左辺の有符号整数。
        @param[in] in_right 右辺の真偽値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_signed_type>
    static int compare_signed_integer(
        template_signed_type const in_left,
        bool const in_right)
    PSYQ_NOEXCEPT
    {
        typedef typename std::make_unsigned<template_signed_type>::type
            unsigned_type;
        return in_left < 0?
            -1:
            self::compare_unsigned_integer(
                static_cast<unsigned_type>(in_left), in_right);
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 無符号整数との比較
    //@{
    /** @brief MessagePackオブジェクト値と無符号整数を比較する。
        @param[in] in_left_value    左辺のMessagePackオブジェクト値。
        @param[in] in_left_type     左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_integer 右辺の無符号整数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_unsigned_type>
    static int compare_unsigned_integer(
        self const& in_left_value,
        self::type::value const in_left_type,
        template_unsigned_type const in_right_integer)
    PSYQ_NOEXCEPT
    {
        static_assert(
            std::is_unsigned<template_unsigned_type>::value,
            "template_unsigned_type is not unsigned integer type.");
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return -self::compare_unsigned_integer(
                in_right_integer, in_left_value.boolean_);
        case self::type::UNSIGNED_INTEGER:
            return -self::compare_unsigned_integer(
                in_right_integer, in_left_value.unsigned_integer_);
        case self::type::NEGATIVE_INTEGER:
            return -1;
        case self::type::FLOATING_POINT_32:
            return -self::compare_unsigned_integer(
                in_right_integer,
                static_cast<self::floating_point_64>(in_left_value.floating_point_32_),
                static_cast<self::floating_point_64>(
                    PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON));
        case self::type::FLOATING_POINT_64:
            return -self::compare_unsigned_integer(
                in_right_integer,
                in_left_value.floating_point_64_,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return 1;//self::compare_raw(in_left_value.raw_, in_right_integer);
        case self::type::ARRAY:
            return 1;//self::compare_array(in_left_value.array_, in_right_integer);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_integer);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief 無符号整数と浮動小数点数を比較する。
        @param[in] in_left    左辺の無符号整数。
        @param[in] in_right   右辺の浮動小数点数。
        @param[in] in_epsilon 許容する誤差の最大値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<
        typename template_unsigned_type,
        typename template_float_type>
    static int compare_unsigned_integer(
        template_unsigned_type const in_left,
        template_float_type const in_right,
        template_float_type const in_epsilon)
    PSYQ_NOEXCEPT
    {
        static_assert(
            std::is_unsigned<template_unsigned_type>::value,
            "template_unsigned_type is not unsigned integer type.");
        return in_right < -in_epsilon?
            1:
            self::compare_floating_point(
                static_cast<template_float_type>(in_left),
                in_right,
                in_epsilon);
    }

    /** @brief 無符号整数を比較する。
        @param[in] in_left  左辺の無符号整数。
        @param[in] in_right 右辺の無符号整数。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_unsigned_type>
    static int compare_unsigned_integer(
        template_unsigned_type const in_left,
        self::unsigned_integer const in_right)
    PSYQ_NOEXCEPT
    {
        return in_left < in_right? -1: (in_right < in_left? 1: 0);
    }

    /** @brief 無符号整数と真偽値を比較する。
        @param[in] in_left  左辺の無符号整数。
        @param[in] in_right 右辺の真偽値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: template<typename template_unsigned_type>
    static int compare_unsigned_integer(
        template_unsigned_type const in_left,
        bool const in_right)
    PSYQ_NOEXCEPT
    {
        // C++の仕様に基づき、trueは1、falseは0に変換して判定する。
        return in_right?
            (1 < in_left? 1: (in_left < 1? -1: 0)):
            (0 < in_left? 1: 0);
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 真偽値の比較
    //@{
    /** @brief MessagePackオブジェクト値と真偽値を比較する。
        @param[in] in_left_value    左辺のMessagePackオブジェクト値。
        @param[in] in_left_type     左辺のMessagePackオブジェクト値の種別。
        @param[in] in_right_boolean 右辺の真偽値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: static int compare_boolean(
        self const& in_left_value,
        self::type::value const in_left_type,
        bool const in_right_boolean)
    PSYQ_NOEXCEPT
    {
        switch (in_left_type)
        {
        case self::type::NIL:
            return 1;
        case self::type::BOOLEAN:
            return self::compare_boolean(
                in_left_value.boolean_, in_right_boolean);
        case self::type::UNSIGNED_INTEGER:
            return self::compare_unsigned_integer(
                in_left_value.unsigned_integer_, in_right_boolean);
        case self::type::NEGATIVE_INTEGER:
            return -1;
        case self::type::FLOATING_POINT_32:
            return self::compare_floating_point(
                in_left_value.floating_point_32_,
                in_right_boolean,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_32_EPSILON);
        case self::type::FLOATING_POINT_64:
            return self::compare_floating_point(
                in_left_value.floating_point_64_,
                in_right_boolean,
                PSYQ_MESSAGE_PACK_VALUE_FLOATING_POINT_64_EPSILON);
        case self::type::STRING:
        case self::type::BINARY:
        case self::type::EXTENDED:
            return 1;//self::compare_raw(in_left_value.raw_, in_right_boolean);
        case self::type::ARRAY:
            return 1;//self::compare_array(in_left_value.array_, in_right_boolean);
        case self::type::UNORDERED_MAP:
        case self::type::MAP:
            return 1;//self::compare_map(in_left_value.map_, in_right_boolean);
        default:
            PSYQ_ASSERT(false);
            return 1;
        }
    }

    /** @brief 真偽値を比較する。
        @param[in] in_left  左辺の真偽値。
        @param[in] in_right 右辺の真偽値。
        @retval 正 左辺のほうが大きい。
        @retval 0  等値。
        @retval 負 左辺のほうが小さい。
     */
    private: static int compare_boolean(
        bool const in_left,
        bool const in_right)
    PSYQ_NOEXCEPT
    {
        return in_left == in_right? 0: (in_right? 1: -1);
    }
    //@}
    //-------------------------------------------------------------------------
    /// @copydoc self::type::BOOLEAN
    public: bool boolean_;
    /// @copydoc self::type::UNSIGNED_INTEGER
    public: self::unsigned_integer unsigned_integer_;
    /// @copydoc self::type::NEGATIVE_INTEGER
    public: self::negative_integer negative_integer_;
    /// @copydoc self::type::FLOATING_POINT_32
    public: self::floating_point_32 floating_point_32_;
    /// @copydoc self::type::FLOATING_POINT_64
    public: self::floating_point_64 floating_point_64_;
    /// @copydoc self::string
    public: self::string string_;
    /// @copydoc self::binary
    public: self::binary binary_;
    /// @copydoc self::extended
    public: self::extended extended_;
    /// @copydoc self::type::ARRAY
    public: self::array array_;
    /// @copydoc self::type::UNORDERED_MAP
    public: self::unordered_map unordered_map_;
    /// @copydoc self::type::MAP
    public: self::map map_;
};

#endif // !defined(PSYQ_MESSAGE_PACK_VALUE_HPP_)