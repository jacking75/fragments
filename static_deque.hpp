﻿/*
Copyright (c) 2015, Hillco Psychi, All rights reserved.

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
/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
    @copybrief psyq::static_deque
 */
#ifndef PSYQ_STATIC_DEQUE_HPP_
#define PSYQ_STATIC_DEQUE_HPP_

#include <initializer_list>
//#include "./assert.hpp"

#if defined(NDEBUG) && !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
#define PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
#endif // !defined(NDEBUG) && !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)

#ifndef PSYQ_ALIGNOF
#   if defined(_MSC_VER)
#       define PSYQ_ALIGNOF __alignof
#   else
#       define PSYQ_ALIGNOF alignof
#   endif
#endif // !defined(PSYQ_ALIGNOF)

/// @cond
namespace psyq
{
    template<typename, std::size_t> class static_deque;
} // namespace psyq
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 動的メモリ割当を行わない、二重終端キューコンテナ。

    std::deque を模したインターフェイスを持つ。

    @tparam template_value    @copydoc static_deque::value_type
    @tparam template_max_size @copydoc static_deque::MAX_SIZE
 */
template<typename template_value, std::size_t template_max_size>
class psyq::static_deque
{
    /// @brief thisが指す値の型。
    private: typedef static_deque this_type;

    //-------------------------------------------------------------------------
    public: class iterator;
    public: class const_iterator;
    /// @brief 要素を指す逆反復子を表す型。
    public: typedef std::reverse_iterator<typename this_type::iterator>
        reverse_iterator;
    /// @brief 読み取り専用要素を指す逆反復子を表す型。
    public: typedef std::reverse_iterator<typename this_type::const_iterator>
        reverse_const_iterator;

    //-------------------------------------------------------------------------
    /// @brief コンテナに格納する要素を表す型。
    public: typedef typename this_type::iterator::value_type value_type;
    /// @brief 要素の数を表す型。
    public: typedef std::size_t size_type;
    /// @brief 要素を指すポインタの差分を表す型。
    public: typedef typename this_type::iterator::difference_type
        difference_type;
    /// @brief 要素を指すポインタを表す型。
    public: typedef typename this_type::iterator::pointer pointer;
    /// @brief 読み取り専用要素を指すポインタを表す型。
    public: typedef typename this_type::const_iterator::pointer const_pointer;
    /// @brief 要素への参照を表す型。
    public: typedef typename this_type::iterator::reference reference;
    /// @brief 読み取り専用要素への参照を表す型。
    public: typedef typename this_type::const_iterator::reference
        const_reference;

    /// @brief コンテナに格納できる要素の最大数。
    public: static typename this_type::size_type const MAX_SIZE =
        template_max_size;
    static_assert(0 < template_max_size, "'template_max_size' is empty.");

    //-------------------------------------------------------------------------
    /// @brief コンテナとして使う固定長のメモリ領域を表す型。
    private: typedef typename std::aligned_storage<
        sizeof(template_value) * template_max_size,
        PSYQ_ALIGNOF(template_value)>
            ::type
                storage;

    /// @brief デバッグ時にメモリ領域の内容を見るために使う。
    private: typedef template_value array_view[template_max_size];

    //-------------------------------------------------------------------------
    /// @name 構築と解体
    //@{
    /** @brief 空のコンテナを構築する。
     */
    public: static_deque()
    PSYQ_NOEXCEPT:
    begin_(this->get_pointer(0)),
    end_(this->get_pointer(0))
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    , array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {}

    /** @brief コンテナを構築する。
        @param[in] in_count コンテナの要素数。
     */
    public: explicit static_deque(
        typename this_type::size_type const in_count)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        auto const local_end(this->initialize_range(in_count));
        for (auto i(this->begin_); i != local_end; ++i)
        {
            new(i) typename this_type::value_type;
        }
    }

    /** @brief コンテナをコピー構築する。
        @param[in] in_count コピーする要素の数。
        @param[in] in_value コピーする値。
     */
    public: explicit static_deque(
        typename this_type::size_type const in_count,
        typename this_type::value_type const& in_value)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        auto const local_end(this->initialize_range(in_count));
        for (auto i(this->begin_); i != local_end; ++i)
        {
            new(i) typename this_type::value_type(in_value);
        }
    }

    /** @brief コンテナをコピー構築する。
        @param[in] in_first コピー元コンテナの先頭位置を指す反復子。
        @param[in] in_last  コピー元コンテナの末尾位置を指す反復子。
     */
    public: template<typename template_iterator>
    static_deque(
        template_iterator const& in_first,
        template_iterator const& in_last)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        this->copy_construct_deque(
            in_first, in_last, std::distance(in_first, in_last));
    }

    /** @brief コンテナをコピー構築する。
        @param[in] in_source コピー元となる初期化子リスト。
     */
    public: static_deque(
        std::initializer_list<typename this_type::value_type> const& in_source)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        this->copy_construct_deque(in_source);
    }

    /** @brief コンテナをコピー構築する。
        @param[in] in_source コピー元となるコンテナ。
     */
    public: static_deque(this_type const& in_source)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        this->copy_construct_deque(in_source);
    }

    /** @brief コンテナをムーブ構築する。
        @param[in,out] io_source ムーブ元となるコンテナ。
     */
    public: static_deque(this_type&& io_source)
#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    : array_view_(
        *reinterpret_cast<typename this_type::array_view*>(&this->storage_))
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)
    {
        this->move_construct_deque(io_source);
    }

    /// @brief コンテナを解体する。
    public: ~static_deque()
    {
        this->clear();
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 代入
    //@{
    /** @brief コンテナにコピー代入する。
        @param[in] in_source コピー元となるコンテナ。
        @return *this
     */
    public: this_type& operator=(this_type const& in_source)
    {
        if (this != &in_source)
        {
            this->~this_type();
            new(this) this_type(in_source);
        }
        return *this;
    }

    /** @brief コンテナにコピー代入する。
        @param[in] in_source コピー元となる初期化子リスト。
        @return *this
     */
    public: this_type& operator=(
        std::initializer_list<typename this_type::value_type> const& in_source)
    {
        this->~this_type();
        return *new(this) this_type(in_source);
    }

    /** @brief コンテナにムーブ代入する。
        @param[in,out] io_source ムーブ元となるコンテナ。
        @return *this
     */
    public: this_type& operator=(this_type&& io_source)
    {
        if (this != &io_source)
        {
            this->~this_type();
            new(this) this_type(std::move(io_source));
        }
        return *this;
    }

    /** @brief コンテナにコピー代入する。
        @param[in] in_first コピー元コンテナの先頭位置を指す反復子。
        @param[in] in_last  コピー元コンテナの末尾位置を指す反復子。
     */
    public: template<typename template_iterator>
    void assign(
        template_iterator const& in_first,
        template_iterator const& in_last)
    {
        auto const local_distance(std::distance(in_first, in_last));
        if (0 < local_distance)
        {
            *this = this_type(in_first, in_last);
        }
        else if (local_distance == 0)
        {
            this->clear();
        }
        else if (local_distance < 0)
        {
            PSYQ_ASSERT(false);
        }
    }

    /** @brief コンテナにコピー代入する。
        @param[in] in_count コピーする要素の数。
        @param[in] in_value コピーする値。
     */
    public: void assign(
        typename this_type::size_type const in_count,
        typename this_type::value_type const in_value)
    {
        this->~this_type();
        new(this) this_type(in_count, in_value);
    }

    /** @brief コンテナにコピー代入する。
        @param[in] in_source コピー元となる初期化子リスト。
        @return *this
     */
    public: void assign(
        std::initializer_list<typename this_type::value_type> const& in_source)
    {
        this->operator=(in_source);
    }

    /// @brief コンテナを交換する。
    public: void swap(this_type& io_target)
    {
        if (this != &io_target)
        {
            auto local_target(std::move(io_target));
            io_target.move_construct_deque(*this);
            this->move_construct_deque(local_target);
        }
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name コンテナの大きさ
    //@{
    /** @brief コンテナが満杯か判定する。
        @retval true  コンテナは満杯。
        @retval false コンテナは満杯ではない。
     */
    public: bool full() const PSYQ_NOEXCEPT
    {
        return this->end_ == nullptr;
    }

    /** @brief コンテナが空か判定する。
        @retval true  コンテナは空。
        @retval false コンテナは空ではない。
     */
    public: bool empty() const PSYQ_NOEXCEPT
    {
        return this->end_ == this->begin_;
    }

    /** @brief コンテナに格納されている要素の数を取得する。
        @return コンテナに格納されている要素の数。
     */
    public: typename this_type::size_type size() const PSYQ_NOEXCEPT
    {
        return this->compute_distance(this->end_);
    }

    /** @brief コンテナに格納できる要素の最大数を取得する。
        @return コンテナに格納できる要素の最大数。
     */
    public: typename this_type::size_type max_size() const PSYQ_NOEXCEPT
    {
        return this_type::MAX_SIZE;
    }

    /** @brief コンテナの要素数を変更する。
        @param[in] in_size  コンテナの新たな要素数。
        @param[in] in_value コンテナの要素数を増やした場合の初期値。
     */
    public: void resize(
        typename this_type::size_type const in_size,
        typename this_type::value_type const& in_value = value_type())
    {
        auto const local_old_size(this->size());
        if (in_size < local_old_size)
        {
            auto const local_new_end(
                this->forward_pointer(this->begin_, in_size));
            this->destruct_element(local_new_end, this->get_end_pointer());
            this->end_ = local_new_end;
        }
        else if (local_old_size < in_size)
        {
            this->insert(this->cend(), in_size - local_old_size, in_value);
        }
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 反復子の取得
    //@{
    /** @brief コンテナの先頭位置を指す反復子を取得する。
        @return コンテナの先頭位置を指す反復子。
     */
    public: typename this_type::iterator begin() PSYQ_NOEXCEPT
    {
        return typename this_type::iterator(this, this->begin_);
    }

    /// @copydoc begin
    public: typename this_type::const_iterator begin() const PSYQ_NOEXCEPT
    {
        return this_type::cbegin();
    }

    /// @copydoc begin
    public: typename this_type::const_iterator cbegin() const PSYQ_NOEXCEPT
    {
        return typename this_type::const_iterator(this, this->begin_);
    }

    /** @brief コンテナの末尾位置を指す反復子を取得する。
        @return コンテナの末尾位置を指す反復子。
     */
    public: typename this_type::iterator end() PSYQ_NOEXCEPT
    {
        return typename this_type::iterator(this, nullptr);
    }

    /// @copydoc end
    public: typename this_type::const_iterator end() const PSYQ_NOEXCEPT
    {
        return this_type::cend();
    }

    /// @copydoc end
    public: typename this_type::const_iterator cend() const PSYQ_NOEXCEPT
    {
        return typename this_type::const_iterator(this, nullptr);
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 逆反復子の取得
    //@{
    /** @brief コンテナの末尾位置を指す逆反復子を取得する。
        @return コンテナの末尾位置を指す逆反復子。
     */
    public: typename this_type::reverse_iterator rbegin() PSYQ_NOEXCEPT
    {
        return typename this_type::reverse_iterator(this->end());
    }

    /// @copydoc rbegin
    public: typename this_type::reverse_const_iterator rbegin()
    const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->rbegin();
    }

    /// @copydoc rbegin
    public: typename this_type::reverse_const_iterator crbegin()
    const PSYQ_NOEXCEPT
    {
        return this->rbegin();
    }

    /** @brief コンテナの先頭位置を指す逆反復子を取得する。
        @return コンテナの先頭位置を指す逆反復子。
     */
    public: typename this_type::reverse_iterator rend() PSYQ_NOEXCEPT
    {
        return typename this_type::reverse_iterator(this->begin());
    }

    /// @copydoc rend
    public: typename this_type::reverse_const_iterator rend()
    const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->rend();
    }

    /// @copydoc rend
    public: typename this_type::reverse_const_iterator crend()
    const PSYQ_NOEXCEPT
    {
        return this->rend();
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 要素の参照
    //@{
    /** @brief インデクス番号に対応する要素を参照する。

        インデクス番号に対応する要素がない場合はassertする。

        @param[in] in_index 参照する要素のインデクス番号。
        @return インデクス番号に対応する要素への参照。
     */
    public: typename this_type::reference operator[](
        typename this_type::size_type const in_index)
    PSYQ_NOEXCEPT
    {
        auto const local_pointer(this->forward_pointer(this->begin_, in_index));
        PSYQ_ASSERT(local_pointer != nullptr);
        return *local_pointer;
    }

    /// @copydoc operator[]
    public: typename this_type::const_reference operator[](
        typename this_type::size_type const in_index)
    const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->operator[](in_index);
    }

    /// @copydoc operator[]
    public: typename this_type::reference at(
        typename this_type::size_type const in_index)
    PSYQ_NOEXCEPT
    {
        /// @note 例外を使いたくないので、範囲外でも例外を出さない。
        return this->operator[](in_index);
    }

    /// @copydoc at
    public: typename this_type::const_reference at(
        typename this_type::size_type const in_index)
    const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->at(in_index);
    }

    /** @brief コンテナの先頭要素を参照する。

        コンテナが空だった場合はassertする。

        @return 先頭要素への参照。
     */
    public: typename this_type::reference front() PSYQ_NOEXCEPT
    {
        PSYQ_ASSERT(!this->empty());
        return *this->begin_;
    }

    /// @copydoc front
    public: typename this_type::const_reference front() const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->front();
    }

    /** @brief コンテナの末尾要素を参照する。

        コンテナが空だった場合はassertする。

        @return 末尾要素への参照。
     */
    public: typename this_type::reference back() PSYQ_NOEXCEPT
    {
        PSYQ_ASSERT(!this->empty());
        return this->backward_pointer(this->end_, 1);
    }

    /// @copydoc back
    public: typename this_type::const_reference back() const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->back();
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 要素の挿入
    //@{
    /** @brief コンテナの先頭位置に要素を挿入する。
        @param[in] in_value 挿入する要素の初期値。
        @retval !=nullptr コンテナの先頭要素を指すポインタ。
        @retval ==nullptr 失敗。要素を挿入できなかった。
     */
    public: typename this_type::pointer push_front(
        typename this_type::value_type in_value)
    {
        auto const local_storage(this->allocate_front(1));
        return local_storage != nullptr?
            new(local_storage)
                typename this_type::value_type(std::move(in_value)):
            nullptr;
    }

    /** @brief コンテナの末尾位置に要素を挿入する。
        @param[in] in_value 挿入する要素の初期値。
        @retval !=nullptr コンテナの末尾要素を指すポインタ。
        @retval ==nullptr 失敗。要素を挿入できなかった。
     */
    public: typename this_type::pointer push_back(
        typename this_type::value_type in_value)
    {
        auto const local_storage(this->allocate_back(1));
        return local_storage != nullptr?
            new(local_storage)
                typename this_type::value_type(std::move(in_value)):
            nullptr;
    }

    /** @brief コンテナに要素を挿入する。
        @param[in] in_position 要素を挿入する位置。
        @param[in] in_value    挿入する要素の初期値。
        @retval !=this->end() 挿入した要素を指す反復子。
        @retval ==this->end() 失敗。要素を挿入できなかった。
     */
    public: typename this_type::iterator insert(
        typename this_type::const_iterator const& in_position,
        typename this_type::value_type const& in_value)
    {
        return this->insert(in_position, 1, in_value);
    }

    /** @brief コンテナに要素を挿入する。
        @param[in] in_position 要素を挿入する位置。
        @param[in] in_source   挿入する要素のコンテナ。
        @retval !=this->end() 挿入した要素の先頭を指す反復子。
        @retval ==this->end() 失敗。要素を挿入できなかった。
     */
    public: typename this_type::iterator insert(
        typename this_type::const_iterator const& in_position,
        std::initializer_list<typename this_type::value_type> const& in_source)
    {
        return this->insert(in_position, in_source.begin(), in_source.end());
    }

    /** @brief コンテナに要素を挿入する。
        @param[in] in_position 要素を挿入する位置。
        @param[in] in_count    挿入する要素の数。
        @param[in] in_value    挿入する要素の初期値。
        @retval !=this->end() 挿入した要素の先頭を指す反復子。
        @retval ==this->end() 失敗。要素を挿入できなかった。
     */
    public: typename this_type::iterator insert(
        typename this_type::const_iterator const& in_position,
        typename this_type::size_type const in_count,
        typename this_type::value_type const& in_value)
    {
        typename this_type::iterator const local_result(
            this, this->allocate_element(in_position.pointer_, in_count));
        if (local_result.pointer_ != nullptr)
        {
            auto local_target(local_result);
            for (auto i(in_count); 0 < i; --i, ++local_target)
            {
                new(local_target.operator->())
                    typename this_type::value_type(in_value);
            }
        }
        else if (in_count == 0)
        {
            return typename this_type::iterator(
                this,
                const_cast<typename this_type::pointer>(
                    in_position.operator->()));
        }
        return local_result;
    }

    /** @brief コンテナに要素を挿入する。
        @param[in] in_position 要素を挿入する位置。
        @param[in] in_first    挿入する要素の先頭位置。
        @param[in] in_last     挿入する要素の末尾位置。
        @retval !=this->end() 挿入した要素の先頭を指す反復子。
        @retval ==this->end() 失敗。要素を挿入できなかった。
     */
    public: template<typename template_iterator>
    typename this_type::iterator insert(
        typename this_type::const_iterator const& in_position,
        template_iterator const& in_first,
        template_iterator const& in_last)
    {
        auto const local_size(std::distance(in_first, in_last));
        typename this_type::iterator const local_result(
            this, this->allocate_element(in_position.pointer_, local_size));
        if (local_result.pointer_ != nullptr)
        {
            auto local_target(local_result);
            for (auto i(in_first); i != in_last; ++i, ++local_target)
            {
                new(local_target.operator->())
                    typename this_type::value_type(*i);
            }
        }
        else if (local_size == 0)
        {
            return typename this_type::iterator(
                this,
                const_cast<typename this_type::pointer>(
                    in_position.operator->()));
        }
        return local_result;
    }
    //@}
    //-------------------------------------------------------------------------
    /// @name 要素の削除
    //@{
    /** @brief コンテナの先頭要素を削除する。
        @retval true  コンテナの先頭要素を削除した。
        @retval false 失敗。コンテナが空なので、削除できなかった。
     */
    public: bool pop_front()
    {
        // すでに空なら失敗する。
        if (this->empty())
        {
            PSYQ_ASSERT(false);
            return false;
        }

        // 先頭要素を破棄し、先頭位置を更新する。
        this->begin_->~value_type();
        this->begin_ = this->forward_pointer(this->begin_, 1);
        return true;
    }

    /** @brief コンテナの末尾要素を削除する。
        @retval true  コンテナの末尾要素を削除した。
        @retval false 失敗。コンテナが空なので、削除できなかった。
     */
    public: bool pop_back()
    {
        // すでに空なら失敗する。
        if (this->empty())
        {
            PSYQ_ASSERT(false);
            return false;
        }

        // 末尾位置を更新し、直前の末尾要素を破棄する。
        this->end_ = this->backward_pointer(this->end_, 1);
        this->end_->~value_type();
        return true;
    }

    /** @brief コンテナから要素を削除する。
        @param[in] in_position 削除する要素を指す反復子。
        @return 削除した要素の次を指す反復子。
     */
    public: typename this_type::iterator erase(
        typename this_type::const_iterator const& in_position)
    {
#if 1
        this->erase(in_position, in_position + 1);
#else
        if (in_position.pointer_ == nullptr || in_position.deque_ != this)
        {
            // コンテナの範囲外なので削除しない。
            PSYQ_ASSERT(in_position.deque_ == this);
            return this->end();
        }
        if (in_position.pointer_ == this->begin_)
        {
            // 先頭要素を削除する。
            this->pop_front();
            return this->begin();
        }
        auto const local_index(this->compute_index(in_position.pointer_))
        auto const local_size(this->size());
        if (local_index + 1 < local_size)
        {
            // 先頭と末尾の間にある要素を削除する。w
            return this->erase_element(
                local_size,
                in_position.pointer_,
                local_index,
                this->forward_pointer(in_position.pointer_, 1),
                local_index + 1);
        }
        if (local_index < local_size)
        {
            // 末尾要素を削除する。
            this->pop_back();
        }
        else
        {
            // 範囲外なので削除しない。
            PSYQ_ASSERT(false);
        }
        return this->end();
#endif // 1
    }

    /** @brief コンテナから要素を削除する。
        @param[in] in_first 削除する範囲の先頭位置。
        @param[in] in_last  削除する範囲の末尾位置。
        @return 削除した要素の次を指す反復子。
     */
    public: typename this_type::iterator erase(
        typename this_type::const_iterator const& in_first,
        typename this_type::const_iterator const& in_last)
    {
        auto const local_size(this->size());
        auto const local_first_index(this->compute_index(in_first.pointer_));
        auto const local_last_index(this->compute_index(in_last.pointer_));
        PSYQ_ASSERT(
            local_last_index < local_size || in_last.pointer_ == nullptr);
        if (local_last_index <= local_first_index
            || local_size <= local_first_index)
        {
            // コンテナの範囲外だったので削除しない。
            PSYQ_ASSERT(
                local_first_index <= local_last_index
                && local_last_index <= local_size);
            return this->end();
        }
        else if (in_last.pointer_ == nullptr)
        {
            // 末尾位置に接する範囲を削除する。
            this->destruct_element(in_first.pointer_, this->get_end_pointer());
            this->end_ = in_first.pointer_;
            return in_last;
        }
        else if (in_first.pointer_ == this->begin_)
        {
            // 先頭位置に接する範囲を削除する。
            this->destruct_element(this->begin_, in_last.pointer_);
            if (this->end_ == nullptr)
            {
                this->end_ = this->begin_;
            }
            this->begin_ = in_last.pointer_;
            return in_last;
        }
        else
        {
            // 先頭と末尾の間にある要素を削除する。
            return this->erase_element(
                local_size,
                in_first.pointer_,
                local_first_index,
                in_last.pointer_,
                local_last_index);
        }
    }

    /** @brief コンテナの要素を全て削除する。
     */
    public: void clear()
    {
        if (!this->empty())
        {
            this->destruct_element(this->begin_, this->get_end_pointer());
            this->begin_ = this->get_pointer(0);
            this->end_ = this->begin_;
        }
    }
    //@}
    //-------------------------------------------------------------------------
    /** @brief コンテナ先頭位置からの距離を算出する。
        @param[in] in_pointer 距離を算出する要素を指すポインタ。
        @return コンテナ先頭位置からの距離。
     */
    private: typename this_type::size_type compute_distance(
        typename this_type::const_pointer const in_pointer)
    const PSYQ_NOEXCEPT
    {
        return in_pointer != nullptr?
            static_cast<typename this_type::size_type>(
                in_pointer - this->begin_
                + this_type::MAX_SIZE * (in_pointer < this->begin_)):
            this_type::MAX_SIZE;
    }

    /** @brief コンテナ先頭位置からのインデクス番号を算出する。
        @param[in] in_pointer インデクス番号を算出する要素を指すポインタ。
        @return 要素のインデクス番号。
     */
    private: typename this_type::size_type compute_index(
        typename this_type::const_pointer const in_pointer)
    const PSYQ_NOEXCEPT
    {
        return this->compute_distance(
            in_pointer != nullptr? in_pointer: this->end_);
    }

    /** @brief 要素を指すポインタを取得する。
        @param[in] in_index コンテナ領域先頭位置からの要素のインデクス番号。
        @return 要素を指すポインタ。
     */
    private: typename this_type::pointer get_pointer(
        typename this_type::size_type const in_index)
    PSYQ_NOEXCEPT
    {
        return reinterpret_cast<this_type::pointer>(&this->storage_) + in_index;
    }

    /// @copydoc get_pointer
    private: typename this_type::const_pointer get_pointer(
        typename this_type::size_type const in_index)
    const PSYQ_NOEXCEPT
    {
        return const_cast<this_type*>(this)->get_pointer(in_index);
    }

    /** @brief コンテナの末尾位置を指すポインタを取得する。
        @return コンテナの末尾位置。
     */
    private: typename this_type::pointer get_end_pointer() const PSYQ_NOEXCEPT
    {
        return this->full()? this->begin_: this->end_;
    }

    /** @brief ポインタを進める。
     */
    private: typename this_type::pointer forward_pointer(
        typename this_type::pointer const in_pointer,
        typename this_type::difference_type const in_offset)
    const PSYQ_NOEXCEPT
    {
        if (in_offset <= 0 || in_pointer == nullptr)
        {
            PSYQ_ASSERT(in_offset == 0);
            return in_pointer;
        }
        auto local_pointer(in_pointer + in_offset);
        local_pointer = this->round_pointer(
            local_pointer - this_type::MAX_SIZE * (
                this->get_pointer(this_type::MAX_SIZE) <= local_pointer));
        return local_pointer != this->begin_? local_pointer: nullptr;
    }

     /// @copydoc forward_pointer
    private: typename this_type::const_pointer forward_pointer(
        typename this_type::const_pointer const in_pointer,
        typename this_type::difference_type const in_offset)
    const PSYQ_NOEXCEPT
    {
        return this->forward_pointer(
            const_cast<typename this_type::pointer>(in_pointer), in_offset);
    }

    /** @brief ポインタを後方に進める。
     */
    private: typename this_type::pointer backward_pointer(
        typename this_type::pointer const in_pointer,
        typename this_type::difference_type const in_offset)
    const PSYQ_NOEXCEPT
    {
        if (in_offset <= 0)
        {
            PSYQ_ASSERT(in_offset == 0);
            return in_pointer;
        }
        auto const local_pointer(
            (in_pointer != nullptr? in_pointer: this->begin_) - in_offset);
        return this->round_pointer(
            local_pointer + this_type::MAX_SIZE * (
                local_pointer < this->get_pointer(0)));
    }

    /// @copydoc backward_pointer
    private: typename this_type::const_pointer backward_pointer(
        typename this_type::const_pointer const in_pointer,
        typename this_type::difference_type const in_offset)
    const PSYQ_NOEXCEPT
    {
        return this->backward_pointer(
            const_cast<typename this_type::pointer>(in_pointer), in_offset);
    }

    /** @brief コンテナの範囲に収まるようポインタを調整する。
        @param[in] in_pointer 調整するポインタ。
     */
    private: typename this_type::pointer round_pointer(
        typename this_type::pointer const in_pointer)
    const PSYQ_NOEXCEPT
    {
        if (this->get_pointer(static_deque::MAX_SIZE) <= in_pointer
            || in_pointer < this->get_pointer(0))
        {
            PSYQ_ASSERT(false);
        }
        else if (
            !this->full()
            && this->end_ <= in_pointer
            && (in_pointer < this->begin_ || this->begin_ <= this->end_))
        {
            PSYQ_ASSERT(this->end_ == in_pointer);
        }
        else
        {
            return in_pointer;
        }
        return nullptr;
    }

    //-------------------------------------------------------------------------
    /** @brief コンテナに空要素を挿入する。
        @param[in] in_position 空要素を挿入する位置。
        @param[in] in_size     挿入する空要素の数。
        @retval !=nullptr 挿入した空要素の先頭を指すポインタ。
        @retval ==nullptr 失敗。空要素を挿入できなかった。
     */
    private: typename this_type::pointer allocate_element(
        typename this_type::const_pointer const in_position,
        typename this_type::size_type const in_size)
    PSYQ_NOEXCEPT
    {
        if (in_size <= 0)
        {
            PSYQ_ASSERT(in_size == 0);
            return nullptr;
        }
        auto const local_index(this->compute_index(in_position));
        auto const local_size(this->size());
        PSYQ_ASSERT(local_index <= local_size);
        if (local_index * 2 < local_size)
        {
            // コンテナの前側に空要素を確保する。
            auto const local_old_begin(this->begin_);
            auto const local_new_begin(this->allocate_front(in_size));
            if (local_new_begin == nullptr)
            {
                return nullptr;
            }
            if (local_old_begin == in_position)
            {
                return local_new_begin;
            }

            // コンテナ前側の要素を移動する。
            return this->move_forward(
                local_old_begin, in_position, local_new_begin);
        }
        else
        {
            // コンテナの後側に空要素を確保する。
            auto const local_old_end(this->allocate_back(in_size));
            if (local_old_end == nullptr)
            {
                return nullptr;
            }
            if (in_position == nullptr)
            {
                return local_old_end;
            }

            // コンテナ後側の要素を移動する。
            this->move_backward(
                in_position, local_old_end, this->get_end_pointer());
            return const_cast<typename this_type::pointer>(in_position);
        }
    }

    /** @brief コンテナの先頭に空要素を挿入する。
        @param[in] in_size 挿入する空要素の数。
        @retval !=nullptr 挿入した空要素の先頭を指すポインタ。
        @retval ==nullptr 失敗。空要素を挿入できなかった。
     */
    private: typename this_type::pointer allocate_front(
        typename this_type::size_type const in_size)
    PSYQ_NOEXCEPT
    {
        // 残り容量がなければ、失敗する。
        if (this->full() || this_type::MAX_SIZE < in_size)
        {
            return nullptr;
        }

        // 新たな先頭位置を決定する。
        auto local_new_begin(this->begin_ - in_size);
        bool const local_circulate(local_new_begin < this->get_pointer(0));
        if (local_circulate)
        {
            local_new_begin += this_type::MAX_SIZE;
        }
        if (local_new_begin < this->end_
            && (local_circulate || this->end_ < this->begin_))
        {
            // 新たな先頭位置が末尾位置を追い越していたら、失敗する。
            return nullptr;
        }

        // 先頭位置を更新する。
        this->begin_ = local_new_begin;
        if (this->end_ == local_new_begin)
        {
            this->end_ = nullptr;
        }
        return local_new_begin;
    }

    /** @brief コンテナの末尾に空要素を挿入する。
        @param[in] in_size 挿入する空要素の数。
        @retval !=nullptr 挿入した空要素の先頭を指すポインタ。
        @retval ==nullptr 失敗。空要素を挿入できなかった。
     */
    private: typename this_type::pointer allocate_back(
        typename this_type::size_type const in_size)
    PSYQ_NOEXCEPT
    {
        // 残り容量がなければ、失敗する。
        if (this->full() || this_type::MAX_SIZE < in_size)
        {
            return nullptr;
        }

        // 新たな末尾位置を決定する。
        auto local_new_end(this->end_ + in_size);
        bool const local_circulate(
            this->get_pointer(this_type::MAX_SIZE) <= local_new_end);
        if (local_circulate)
        {
            local_new_end -= this_type::MAX_SIZE;
        }
        if (this->begin_ < local_new_end
            && (local_circulate || this->end_ < this->begin_))
        {
            // 新たな末尾位置が先頭位置を追い越していたら、失敗する。
            return nullptr;
        }

        // 末尾位置を更新する。
        auto const local_last_end(this->end_);
        this->end_ = local_new_end != this->begin_? local_new_end: nullptr;
        return local_last_end;
    }

    private: typename this_type::pointer move_forward(
        typename this_type::pointer const in_source_first,
        typename this_type::const_pointer const in_source_last,
        typename this_type::pointer const in_target_first)
    const
    {
        auto local_source(in_source_first);
        auto local_target(in_target_first);
        while(local_source != in_source_last)
        {
            new(local_target)
                typename this_type::value_type(std::move(*local_source));
            local_source->~value_type();
            local_source = this->forward_pointer(local_source, 1);
            local_target = this->forward_pointer(local_target, 1);
        }
        return local_target;
    }

    private: typename this_type::pointer move_backward(
        typename this_type::const_pointer const in_source_first,
        typename this_type::pointer const in_source_last,
        typename this_type::pointer const in_target_last)
    const
    {
        auto local_target(in_target_last);
        auto local_source(in_source_last);
        while(in_source_first != local_source)
        {
            local_source = this->backward_pointer(local_source, 1);
            local_target = this->backward_pointer(local_target, 1);
            new(local_target)
                typename this_type::value_type(std::move(*local_source));
            local_source->~value_type();
        }
        return local_target;
    }

    private: typename this_type::iterator erase_element(
         typename this_type::size_type const in_size,
         typename this_type::pointer const in_first_pointer,
         typename this_type::size_type const in_first_index,
         typename this_type::pointer const in_last_pointer,
         typename this_type::size_type const in_last_index)
    {
        PSYQ_ASSERT(
            in_size == this->size()
            && 0 < in_first_index
            && in_last_index < in_size
            && in_first_index < in_last_index
            && in_first_index == this->compute_index(in_first_pointer)
            && in_last_index == this->compute_index(in_last_pointer));

        this->destruct_element(in_first_pointer, in_last_pointer);
        if (in_first_index + in_last_index < in_size)
        {
            // コンテナ前側の要素を後へ移動する。
            this->begin_ = this->move_backward(
                this->begin_, in_first_pointer, in_last_pointer);
            return typename this_type::iterator(this, in_last_pointer);
        }
        else
        {
            // コンテナ後側の要素を前へ移動する。
            this->end_ = this->move_forward(
                in_last_pointer, this->get_end_pointer(), in_first_pointer);
            return typename this_type::iterator(this, in_first_pointer);
        }
    }

    private: void destruct_element(
        typename this_type::pointer const in_first,
        typename this_type::pointer const in_last)
    const
    {
        PSYQ_ASSERT(
            this->get_pointer(0) <= in_first
            && in_first < this->get_pointer(this_type::MAX_SIZE)
            && this->get_pointer(0) <= in_last
            && in_last < this->get_pointer(this_type::MAX_SIZE));

        // 末尾から逆順に要素を解体する。
        for (auto i(in_last);;)
        {
            i = this->backward_pointer(i, 1);
            i->~value_type();
            if (i <= in_first)
            {
                return;
            }
        }
    }

    //-------------------------------------------------------------------------
    private: template<typename template_container>
    void copy_construct_deque(template_container const& in_source)
    {
        this->copy_construct_deque(
            in_source.begin(), in_source.end(), in_source.size());
    }

    private: template<typename template_iterator>
    void copy_construct_deque(
        template_iterator const& in_first,
        template_iterator const& in_last,
        typename this_type::size_type const in_size)
    {
        this->construct_deque(
            in_first,
            in_last,
            in_size,
            [](
                typename this_type::value_type* const in_target_element,
                typename this_type::value_type const& in_source_element)
            {
                new(in_target_element) value_type(in_source_element);
            });
    }

    private: void move_construct_deque(this_type& io_source)
    {
        this->move_construct_deque(
            io_source.begin(), io_source.end(), io_source.size());
        io_source.clear();
    }

    private: template<typename template_iterator>
    void move_construct_deque(
        template_iterator const& in_first,
        template_iterator const& in_last,
        typename this_type::size_type const in_size)
    {
        this->construct_deque(
            in_first,
            in_last,
            in_size,
            [](
                typename this_type::value_type* const in_target_element,
                typename this_type::value_type const& io_source_element)
            {
                new(in_target_element) value_type(std::move(io_source_element));
            });
    }

    private: template<typename template_iterator, typename template_constructor>
    void construct_deque(
        template_iterator const& in_first,
        template_iterator const& in_last,
        typename this_type::size_type const in_size,
        template_constructor const& in_constructor)
    {
        auto const local_end(this->initialize_range(in_size));
        auto local_source(in_first);
        for (auto i(this->begin_); i != local_end; ++i, ++local_source)
        {
            in_constructor(i, *local_source);
        }
    }

    private: typename this_type::const_pointer initialize_range(
        typename this_type::size_type const in_count)
    {
        auto local_end(this->get_pointer(0));
        this->begin_ = local_end;
        if (in_count < this_type::MAX_SIZE)
        {
            local_end += in_count;
            this->end_ = local_end;
        }
        else
        {
            PSYQ_ASSERT(in_count <= this_type::MAX_SIZE);
            local_end += this_type::MAX_SIZE;
            this->end_ = nullptr;
        }
        return local_end;
    }

    //-------------------------------------------------------------------------
    /// @brief 要素を格納するメモリ領域。
    private: typename this_type::storage storage_;
    /// @brief コンテナの先頭位置。
    private: typename this_type::pointer begin_;
    /// @brief コンテナの末尾位置。
    private: typename this_type::pointer end_;

#ifndef PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW
    private: typename this_type::array_view& array_view_;
#endif // !defined(PSYQ_STATIC_DEQUE_NO_ARRAY_VIEW)

}; // class static_deque

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief 読み取り専用の要素を指す反復子。
template<typename template_value, std::size_t template_max_size>
class psyq::static_deque<template_value, template_max_size>::const_iterator:
    public std::iterator<
        std::random_access_iterator_tag, template_value const>
{
    friend static_deque;
    /// @brief thisが指す値の型。
    private: typedef const_iterator this_type;
    /// @brief this_type の基底型。
    public: typedef std::iterator<
        std::random_access_iterator_tag, template_value const>
            base_type;

    //---------------------------------------------------------------------
    /// @name 値の参照
    //@{
    public: typename base_type::pointer operator->() const PSYQ_NOEXCEPT
    {
        PSYQ_ASSERT(this->deque_ != nullptr && this->pointer_ != nullptr);
        return this->pointer_;
    }

    public: typename base_type::reference operator*() const PSYQ_NOEXCEPT
    {
        return *(this->operator->());
    }
    //@}
    //---------------------------------------------------------------------
    /// @name 比較
    //@{
    public: bool operator==(this_type const& in_right) const PSYQ_NOEXCEPT
    {
        return this->deque_ == in_right.deque_
            && this->pointer_ == in_right.pointer_;
    }

    public: bool operator!=(this_type const& in_right) const PSYQ_NOEXCEPT
    {
        return !this->operator==(in_right);
    }
    //@}
    //---------------------------------------------------------------------
    /// @name 加算
    //@{
    public: this_type operator+(
        typename base_type::difference_type const in_offset)
    const PSYQ_NOEXCEPT
    {
        auto local_new(*this);
        return local_new.operator+=(in_offset);
    }

    public: this_type operator++(int) PSYQ_NOEXCEPT
    {
        auto const local_old(*this);
        this->operator++();
        return local_old;
    }

    public: this_type& operator++() PSYQ_NOEXCEPT
    {
        return this->operator+=(1);
    }

    public: this_type& operator+=(
        typename base_type::difference_type const in_offset)
    PSYQ_NOEXCEPT
    {
        if (this->deque_ == nullptr)
        {
            PSYQ_ASSERT(false);
        }
        else if (in_offset != 0)
        {
            this->pointer_ = in_offset < 0?
                this->deque_->backward_pointer(this->pointer_, -in_offset):
                this->deque_->forward_pointer(this->pointer_, in_offset);
        }
        return *this;
    }
    //@}
    //---------------------------------------------------------------------
    /// @name 減算
    //@{
    public: typename base_type::difference_type operator-(
        this_type const& in_right)
    const PSYQ_NOEXCEPT
    {
        auto const local_deque(this->deque_);
        PSYQ_ASSERT(
            local_deque != nullptr && local_deque == in_right.deque_);
        auto const local_left_index(
            local_deque->compute_index(this->pointer_));
        auto const local_right_index(
            local_deque->compute_index(in_right.pointer_));
        return local_left_index - local_right_index;
    }

    public: this_type operator-(
        typename base_type::difference_type const in_offset)
    PSYQ_NOEXCEPT
    {
        auto local_new(*this);
        return local_new.operator-=(in_offset);
    }

    public: this_type operator--(int) PSYQ_NOEXCEPT
    {
        auto const local_old(*this);
        this->operator--();
        return local_old;
    }

    public: this_type& operator--() PSYQ_NOEXCEPT
    {
        return this->operator-=(1);
    }

    public: this_type& operator-=(
        typename base_type::difference_type const in_offset)
    PSYQ_NOEXCEPT
    {
        return this->operator+=(-in_offset);
    }
    //@}
    //---------------------------------------------------------------------
    protected: const_iterator(
        static_deque const* const in_deque,
        typename base_type::pointer const in_pointer)
    PSYQ_NOEXCEPT:
    deque_(in_deque),
    pointer_(in_pointer)
    {}

    //---------------------------------------------------------------------
    private: static_deque const* deque_;
    private: typename base_type::pointer pointer_;

}; // class psyq::static_deque::const_iterator

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief 要素を指す反復子。
template<typename template_value, std::size_t template_max_size>
class psyq::static_deque<template_value, template_max_size>::iterator:
    public static_deque::const_iterator
{
    friend static_deque;
    /// @brief thisが指す値の型。
    private: typedef iterator this_type;
    /// @brief this_type の基底型。
    public: typedef typename static_deque::const_iterator base_type;

    //---------------------------------------------------------------------
    /// @copydoc static_deque::value_type
    public: typedef template_value value_type;
    /// @copydoc static_deque::pointer
    public: typedef typename this_type::value_type* pointer;
    /// @copydoc static_deque::reference
    public: typedef typename this_type::value_type& reference;

    //---------------------------------------------------------------------
    /// @name 値の参照
    //@{
    public: typename this_type::pointer operator->() const PSYQ_NOEXCEPT
    {
        return const_cast<typename this_type::pointer>(
            this->base_type::operator->());
    }

    public: typename this_type::reference operator*() const PSYQ_NOEXCEPT
    {
        return const_cast<typename this_type::reference>(
            this->base_type::operator*());
    }
    //@}
    //---------------------------------------------------------------------
    protected: iterator(
        static_deque* const in_deque,
        typename this_type::pointer const in_pointer)
    PSYQ_NOEXCEPT: base_type(in_deque, in_pointer)
    {}

}; // class psyq::static_deque::iterator

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
namespace psyq_test
{
    void static_deque()
    {
        typedef psyq::static_deque<float, 16> float_deque;
        float_deque local_deque_a;
        float_deque local_deque_b(5);
        float_deque local_deque_c(float_deque::MAX_SIZE, 0.5f);
        float_deque local_deque_d({1, 2, 3, 4});
        float_deque local_deque_e(std::move(local_deque_c));

        local_deque_a = local_deque_d;
        local_deque_a.push_front(10);
        local_deque_a.push_back(20);
        local_deque_a.pop_front();
        local_deque_a.pop_back();
        local_deque_a.insert(local_deque_a.end(), 10);
        local_deque_a.insert(
            local_deque_a.begin(), local_deque_b.begin(), local_deque_b.end());
    }
} // namespace psyq_test

#endif // !defined(PSYQ_STATIC_DEQUE_HPP_)
// vim: set expandtab:
