﻿/*
Copyright (c) 2013, Hillco Psychi, All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 
ソースコード形式かバイナリ形式か、変更するかしないかを問わず、
以下の条件を満たす場合に限り、再頒布および使用が許可されます。

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer. 
   ソースコードを再頒布する場合、上記の著作権表示、本条件一覧、
   および下記の免責条項を含めること。
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 
   バイナリ形式で再頒布する場合、頒布物に付属のドキュメント等の資料に、
   上記の著作権表示、本条件一覧、および下記の免責条項を含めること。

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
本ソフトウェアは、著作権者およびコントリビューターによって
「現状のまま」提供されており、明示黙示を問わず、商業的な使用可能性、
および特定の目的に対する適合性に関する暗黙の保証も含め、
またそれに限定されない、いかなる保証もありません。
著作権者もコントリビューターも、事由のいかんを問わず、
損害発生の原因いかんを問わず、かつ責任の根拠が契約であるか厳格責任であるか
（過失その他の）不法行為であるかを問わず、
仮にそのような損害が発生する可能性を知らされていたとしても、
本ソフトウェアの使用によって発生した（代替品または代用サービスの調達、
使用の喪失、データの喪失、利益の喪失、業務の中断も含め、
またそれに限定されない）直接損害、間接損害、偶発的な損害、特別損害、
懲罰的損害、または結果損害について、一切責任を負わないものとします。
 */
/// @file
/// @brief @copybrief psyq::string::storage
/// @author Hillco Psychi (https://twitter.com/psychi)
#ifndef PSYQ_STRING_STORAGE_HPP_
#define PSYQ_STRING_STORAGE_HPP_

#include <array>
#include "./view.hpp"
#include "./interface_mutable.hpp"

/// psyq::string::storage で使う、最大要素数のデフォルト値。
#ifndef PSYQ_STRING_STORAGE_MAX_SIZE_DEFAULT
#define PSYQ_STRING_STORAGE_MAX_SIZE_DEFAULT 99
#endif // !defined(PSYQ_STRING_STORAGE_MAX_SIZE_DEFAULT)

namespace psyq
{
    namespace string
    {
        template<typename, std::size_t, typename> class storage;

        namespace _private
        {
            /// @cond
            template<typename, std::size_t> class storage_base;
            /// @endcond
        } // namespace _private
    } // namespace string
} // namespace psyq

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief 固定長領域を使った文字列の基底型。
/// @details
/// - this_type::MAX_SIZE より多い文字数は保持できない。
/// - 動的メモリ割当を一切行わない。
/// @tparam template_char_traits @copydoc this_type::traits_type
/// @tparam template_max_size    @copydoc this_type::MAX_SIZE
template<typename template_char_traits, std::size_t template_max_size>
class psyq::string::_private::storage_base
{
    /// @copydoc psyq::string::view::this_type
    private: typedef storage_base this_type;

    //-------------------------------------------------------------------------
    /// @brief 文字特性の型。
    public: typedef template_char_traits traits_type;

    public: enum: std::size_t
    {
        MAX_SIZE = template_max_size, ///< 文字列の最大要素数。
    };

    //-------------------------------------------------------------------------
    /// @brief 文字列を格納する領域として使う配列。
    private: typedef
         std::array<
             typename this_type::traits_type::char_type,
             this_type::MAX_SIZE + 1>
         char_array;

    //-------------------------------------------------------------------------
    /// @name 文字列のプロパティ
    /// @{

    /// @brief 文字列の先頭要素へのポインタを取得する。
    /// @return 終端が空文字となっている文字列の、先頭要素へのポインタ。
    public: typename this_type::traits_type::char_type const* c_str()
    const PSYQ_NOEXCEPT
    {
        return &this->storage_[0];
    }

    /// @copydoc this_type::c_str()
    public: typename this_type::traits_type::char_type const* data()
    const PSYQ_NOEXCEPT
    {
        return this->c_str();
    }

    /// @copydoc psyq::string::view::size()
    public: std::size_t size() const PSYQ_NOEXCEPT
    {
        return this->size_;
    }

    /// @brief メモリ割当せずに格納できる最大の要素数を取得する。
    /// @return メモリ割当せずに格納できる最大の要素数。
    public: std::size_t capacity() const PSYQ_NOEXCEPT
    {
        return this_type::MAX_SIZE;
    }

    /// @copydoc psyq::string::view::max_size()
    public: std::size_t max_size() const PSYQ_NOEXCEPT
    {
        return this_type::MAX_SIZE;
    }
    /// @}
    //-------------------------------------------------------------------------
    /// @name 文字列の変更
    /// @{

    /// @brief 文字列を空にする。
    public: void clear() PSYQ_NOEXCEPT
    {
        this->size_ = 0;
        this->storage_[0] = 0;
    }

    /// @brief std::basic_string::reserve と互換性を保つためのダミー関数。
    /// @details 実際は何もしない。
    public: void reserve(std::size_t const in_capacity)
    {
        PSYQ_ASSERT_THROW(
            in_capacity <= this_type::MAX_SIZE, std::out_of_range);
    }

    /// @brief std::basic_string::shrink_to_fit と互換性を保つためのダミー関数。
    /// @details 実際は何もしない。
    public: void shrink_to_fit() PSYQ_NOEXCEPT {}
    /// @}
    //-------------------------------------------------------------------------
    /// @brief 文字列をコピー構築する。
    protected: storage_base(
        /// [in] コピー元となる文字列。
        this_type const& in_string)
    PSYQ_NOEXCEPT
    {
        this->assign_external_noexcept(
            in_string.data(), in_string.data() + in_string.size());
    }

    /// @copydoc storage_base(this_type const&)
    /// @return *this
    protected: this_type& operator=(this_type const& in_string) PSYQ_NOEXCEPT
    {
        return *new(this) this_type(in_string);
    }

    /// @brief 空の文字列を構築する。
    /// @return 空の文字列。
    protected: static this_type make() PSYQ_NOEXCEPT
    {
        return this_type();
    }

    /// @brief 要素を挿入する。
    protected: void insert(
        /// [in] 要素を挿入するオフセット位置。
        std::size_t const in_offset,
        /// [in] 挿入する要素の数。
        std::size_t const in_count,
        /// [in] 挿入する要素。
        typename this_type::traits_type::char_type const in_char)
    {
        auto const local_gap(this->make_gap(in_offset, in_count));
        this_type::traits_type::assign(
            this->begin() + local_gap.first, local_gap.second, in_char);
    }

    /// @brief 文字列を挿入する。
    /// @return 挿入した文字列の最初の要素を指すポインタ。
    protected: template<typename template_iterator>
    typename this_type::traits_type::char_type* insert(
        /// [in] 文字列を挿入する位置。
        typename this_type::traits_type::char_type const* const in_position,
        /// [in] 挿入する文字列の先頭を指す反復子。
        template_iterator const in_begin,
        /// [in] 挿入する文字列の末尾を指す反復子。
        template_iterator const in_end)
    {
        if (in_begin == in_end || &(*in_begin) == nullptr)
        {
            PSYQ_ASSERT_THROW(in_begin == in_end, std::invalid_argument);
            return this->begin() + this->size();
        }
        else if (this->is_included(this_type::MAX_SIZE, &(*in_begin)))
        {
            // *thisの部分文字列が挿入されるので、
            // 別の文字列にコピーしてから挿入する。
            this_type local_string;
            local_string.assign_external(in_begin, in_end);
            return this->insert_external(
                in_position,
                local_string.data(),
                local_string.data() + local_string.size());
        }
        else
        {
            return this->insert_external(in_position, in_begin, in_end);
        }
    }

    /// @brief 文字列の要素を削除する。
    /// @return 最後に削除した要素の次を指す反復子。
    protected: typename this_type::traits_type::char_type* erase(
        /// [in] 削除範囲の先頭を指す反復子。
        typename this_type::traits_type::char_type const* in_begin,
        /// [in] 削除範囲の末尾を指す反復子。
        typename this_type::traits_type::char_type const* in_end)
    {
        auto const local_end(this->data() + this->size());
        if (in_begin < this->data())
        {
            PSYQ_ASSERT_THROW(false, std::out_of_range);
            in_begin = this->data();
        }
        else if (local_end < in_begin)
        {
            PSYQ_ASSERT_THROW(false, std::out_of_range);
            in_begin = local_end;
        }

        if (in_begin == in_end)
        {
            return in_begin;
        }
        else if (in_end < in_begin)
        {
            PSYQ_ASSERT_THROW(false, std::invalid_argument);
            std::swap(in_begin, in_end);
        }

        if (in_end < local_end)
        {
            this_type::traits_type::move(
                const_cast<typename this_type::traits_type::char_type*>(in_begin),
                in_end,
                local_end - in_end);
            this->size_ -= in_end - in_begin;
        }
        else
        {
            PSYQ_ASSERT_THROW(in_end == local_end, std::out_of_range);
            this->size_ = in_begin - this->data();
        }
        this->storage_[this->size()] = 0;
        return const_cast<typename this_type::traits_type::char_type*>(in_begin);
    }

    /// @brief 外部の文字列をコピーする。
    protected: template<typename template_iterator>
    void assign_external(
        /// [in] コピー元となる文字列の先頭位置。
        template_iterator const in_begin,
        /// [in] コピー元となる文字列の末尾位置。
        template_iterator const in_end)
    {
        if (in_begin != in_end)
        {
            PSYQ_ASSERT_THROW(
                &(*in_begin) != nullptr, std::invalid_argument);
            PSYQ_ASSERT_THROW(
                std::distance(in_begin, in_end) <= this_type::MAX_SIZE,
                std::length_error);
        }
        this->assign_external_noexcept(in_begin, in_end);
    }

    //-------------------------------------------------------------------------
    /// @brief 空の文字列を構築する。
    private: storage_base() PSYQ_NOEXCEPT: size_(0)
    {
        this->storage_[0] = 0;
    }

    /// @copydoc this_type::c_str()
    private: typename this_type::traits_type::char_type* begin() PSYQ_NOEXCEPT
    {
        return const_cast<typename this_type::traits_type::char_type*>(this->data());
    }

    /// @brief 外部の文字列を挿入する。
    private: template<typename template_iterator>
    typename this_type::traits_type::char_type* insert_external(
        /// [in] 文字列を挿入する位置。
        typename this_type::traits_type::char_type const* const in_position,
        /// [in] 挿入する文字列の先頭位置。
        template_iterator const in_begin,
        /// [in] 挿入する文字列の末尾位置。
        template_iterator const in_end)
    {
        std::size_t local_offset;
        if (in_position < this->begin())
        {
            PSYQ_ASSERT_THROW(false, std::range_error);
            local_offset = 0;
        }
        else
        {
            local_offset = in_position - this->begin();
            if (this->size() < local_offset)
            {
                PSYQ_ASSERT_THROW(false, std::range_error);
                local_offset = this->size();
            }
        }
        auto const local_gap(
            this->make_gap(local_offset, std::distance(in_begin, in_end)));
        this->replace_external_noexcept(
            local_gap.first, in_begin, std::next(in_begin, local_gap.second));
        return this->begin() + local_gap.first;
    }

    /// @copydoc assign_external
    private: template<typename template_iterator>
    void assign_external_noexcept(
        template_iterator const in_begin,
        template_iterator const in_end)
    PSYQ_NOEXCEPT
    {
        this->size_ = (std::min<std::size_t>)(
            std::distance(in_begin, in_end), this_type::MAX_SIZE);
        this->replace_external_noexcept(0, in_begin, in_end);
        this->storage_[this->size()] = 0;
    }

    private: template<typename template_iterator>
    void replace_external_noexcept(
        std::size_t const in_offset,
        template_iterator const in_begin,
        template_iterator const in_end)
    PSYQ_NOEXCEPT
    {
        PSYQ_ASSERT(&(*in_begin) != nullptr || in_begin == in_end);
        PSYQ_ASSERT(
            in_offset + std::distance(in_begin, in_end) <= this->size());
        auto local_last(this->begin() + in_offset);
        auto const local_limit(this->begin() + this_type::MAX_SIZE);
        for (auto i(in_begin); i != in_end; ++i, ++local_last)
        {
            if (local_limit <= local_last)
            {
                PSYQ_ASSERT(false);
                break;
            }
            *local_last = *i;
        }
    }

    /// @brief 文字列に隙間を作る。
    /// @return 隙間の先頭位置と要素数のペア。
    private: std::pair<std::size_t, std::size_t> make_gap(
        /// [in] 隙間の位置。
        std::size_t const in_offset,
        /// [in] 隙間の要素数。
        std::size_t const in_size)
    {
        // 挿入する文字列の要素数を決定する。
        auto const local_offset((std::min)(in_offset, this->size()));
        PSYQ_ASSERT_THROW(in_offset == local_offset, std::overflow_error);
        auto const local_insert_size_max(this_type::MAX_SIZE - local_offset);
        auto const local_insert_size(
            (std::min)(local_insert_size_max, in_size));
        PSYQ_ASSERT_THROW(in_size == local_insert_size, std::overflow_error);

        // 移動する文字列の要素数を決定する。
        auto const local_move_size_max(
            local_insert_size_max - local_insert_size);
        auto const local_move_size(
            (std::min)(local_move_size_max, this->size() - local_offset));
        PSYQ_ASSERT_THROW(
            local_move_size <= local_move_size_max, std::overflow_error);

        // 挿入位置より後ろの文字列を移動する。
        auto const local_position(&this->storage_[local_offset]);
        this_type::traits_type::move(
            local_position + local_insert_size,
            local_position,
            local_move_size);

        // 文字列の長さを書き換える。
        this->size_ += local_insert_size;
        this->storage_[this->size()] = 0;
        return std::make_pair(local_offset, local_insert_size);
    }

    private: bool is_included(
        std::size_t const in_size,
        void const* const in_string)
    const PSYQ_NOEXCEPT
    {
        return this->data() <= in_string
            && in_string <= this->data() + in_size;
    }

    //-------------------------------------------------------------------------
    /// @brief 文字列の要素数。
    private: std::size_t size_;
    /// @brief 文字列を保存する領域。
    private: typename this_type::char_array storage_;

}; // class psyq::string::_private::storage_base

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief std::basic_string を模した、固定長領域を使った文字列。
/// @details
/// - base_type::MAX_SIZE より多い文字数は保持できない。
/// - 動的メモリ割当を一切行わない。
/// @tparam template_char_type   @copydoc base_type::value_type
/// @tparam template_max_size    @copydoc base_type::MAX_SIZE
/// @tparam template_char_traits @copydoc base_type::traits_type
/// @ingroup psyq_string
template<
    typename template_char_type,
    std::size_t template_max_size = PSYQ_STRING_STORAGE_MAX_SIZE_DEFAULT,
    typename template_char_traits = PSYQ_STRING_VIEW_TRAITS_DEFAULT>
class psyq::string::storage:
public psyq::string::_private::interface_mutable<
    psyq::string::_private::storage_base<
        template_char_traits, template_max_size>>
{
    /// @copydoc psyq::string::view::this_type
    private: typedef storage this_type;
    /// @copydoc psyq::string::view::base_type
    public: typedef
        psyq::string::_private::interface_mutable<
            psyq::string::_private::storage_base<
                template_char_traits, template_max_size>>
        base_type;

    //-------------------------------------------------------------------------
    /// @name 文字列の構築
    /// @{

    /// @brief 空文字列を構築する。
    public: storage() PSYQ_NOEXCEPT: base_type(base_type::base_type::make()) {}

    /// @brief 文字列をコピーして構築する。
    public: storage(
        /// [in] コピー元の文字列。
        this_type const& in_string)
    PSYQ_NOEXCEPT: base_type(in_string)
    {}

    /// @brief 文字列をコピーして構築する。
    public: storage(
        /// [in] コピー元の文字列。
        typename base_type::view const& in_string):
    base_type(base_type::base_type::make())
    {
        this->assign_external(in_string.begin(), in_string.end());
    }

    /// @brief 文字列をコピーして構築する。
    public: storage(
        /// [in] コピー元の文字列の先頭位置。
        typename base_type::const_pointer const in_data,
        /// [in] コピー元の文字列の要素数。 base_type::npos
        /// が指定された場合は空文字を検索し、空文字までを要素数とする。
        typename base_type::size_type const in_size):
    base_type(base_type::base_type::make())
    {
        this->assign_external(
            in_data, in_data + base_type::adjust_size(in_data, in_size));
    }

    /// @brief 文字列をコピーして構築する。
    public: template<typename template_iterator>
    storage(
        /// [in] コピー元の文字列の先頭位置。
        template_iterator const in_begin,
        /// [in] コピー元の文字列の末尾位置。
        template_iterator const in_end):
    base_type(base_type::base_type::make())
    {
        this->assign_external(in_begin, in_end);
    }

    /// @brief 文字列の一部をコピーして構築する。
    public: storage(
        /// [in] コピーする文字列。
        typename base_type::view const& in_string,
        /// [in] コピーする文字列の開始オフセット位置。
        typename base_type::size_type const in_offset,
        /// [in] コピーする要素数。
        typename base_type::size_type const in_count = base_type::npos)
    :
    base_type(base_type::base_type::make())
    {
        auto const local_substr(in_string.substr(in_offset, in_count));
        this->assign_external(local_substr.begin(), local_substr.end());
    }

    /// @brief 要素を繰り返す文字列を構築する。
    public: storage(
        /// [in] 繰り返す要素。
        typename base_type::traits_type::char_type const in_char,
        /// [in] 要素を繰り返す回数。
        typename base_type::size_type const in_count)
    :
    base_type(base_type::base_type::make())
    {
        this->insert(0, in_count, in_char);
    }
    /// @}
    //-------------------------------------------------------------------------
    /// @name 文字列の代入
    /// @{

    /// @copydoc storage(this_type const&)
    /// @return *this
    public: this_type& operator=(this_type const& in_string) PSYQ_NOEXCEPT
    {
        this->base_type::base_type::operator=(in_string);
        return *this;
    }

    /// @copydoc storage(typename base_type::view const&)
    /// @return *this
    public: this_type& operator=(typename base_type::view const& in_string)
    {
        return *new(this) this_type(in_string);
    }
    /// @}
}; // class psyq::string::storage

#endif // !defined(PSYQ_STRING_STORAGE_HPP_)
// vim: set expandtab:
