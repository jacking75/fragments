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
/// @file
/// @brief @copybrief psyq::string::flyweight
/// @author Hillco Psychi (https://twitter.com/psychi)
#ifndef PSYQ_STRING_FLYWEIGHT_HPP_
#define PSYQ_STRING_FLYWEIGHT_HPP_

#include "../hash/murmur3.hpp"
#include "./view.hpp"
#include "./flyweight_handle.hpp"

/// @brief フライ級文字列生成器に適用するデフォルトのメモリ割当子の型
#ifndef PSYQ_STRING_FLYWEIGHT_ALLOCATOR_DEFAULT
#define PSYQ_STRING_FLYWEIGHT_ALLOCATOR_DEFAULT std::allocator<void*>
#endif // PSYQ_STRING_FLYWEIGHT_ALLOCATOR_DEFAULT

/// @brief フライ級文字列生成器の文字列チャンクのデフォルト容量。
#ifndef PSYQ_STRING_FLYWEIGHT_CHUNK_SIZE_DEFAULT
#define PSYQ_STRING_FLYWEIGHT_CHUNK_SIZE_DEFAULT 4096
#endif // !defined(PSYQ_STRING_FLYWEIGHT_CHUNK_SIZE_DEFAULT)

#ifndef PSYQ_STRING_FLYWEIGHT_HASHER_DEFAULT
#define PSYQ_STRING_FLYWEIGHT_HASHER_DEFAULT\
    psyq::hash::string_murmur3a<psyq::string::view<char>>
#endif // !defined(PSYQ_STRING_FLYWEIGHT_HASHER_DEFAULT)

#ifndef PSYQ_STRING_FLYWEIGHT_WEAK
#define PSYQ_STRING_FLYWEIGHT_WEAK true
#endif // !defined(PSYQ_STRING_FLYWEIGHT_WEAK)

/// @cond
namespace psyq
{
    namespace string
    {
        template<typename, typename> class flyweight;
    } // namespace string
} // namespace psyq
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief std::basic_string_view を模した、flyweightパターンの文字列。
/// @details base_type::base_type::factory を介して文字列を管理する。
/// @tparam template_hasher    @copydoc _private::flyweight_handle::hasher
/// @tparam template_allocator @copydoc _private::flyweight_handle::allocator_type
/// @ingroup psyq_string
template<
    typename template_hasher = PSYQ_STRING_FLYWEIGHT_HASHER_DEFAULT,
    typename template_allocator = PSYQ_STRING_FLYWEIGHT_ALLOCATOR_DEFAULT>
class psyq::string::flyweight:
public psyq::string::_private::interface_immutable<
    typename psyq::string::_private::flyweight_handle<
        template_hasher, template_allocator, PSYQ_STRING_FLYWEIGHT_WEAK>>
{
    /// @copydoc psyq::string::view::this_type
    private: typedef flyweight this_type;
    /// @copydoc psyq::string::view::base_type
    public: typedef
        psyq::string::_private::interface_immutable<
            typename psyq::string::_private::flyweight_handle<
                template_hasher, template_allocator, PSYQ_STRING_FLYWEIGHT_WEAK>>
        base_type;

    //-------------------------------------------------------------------------
    /// @brief 空の文字列を構築する。動的メモリ割当は行わない。
    public: flyweight()
    PSYQ_NOEXCEPT: base_type(base_type::base_type::make())
    {}

    /// @brief 文字列をコピー構築する。動的メモリ割当は行わない。
    public: flyweight(
        /// [in] コピー元となる文字列。
        this_type const& in_source)
    PSYQ_NOEXCEPT: base_type(in_source)
    {}

    /// @brief 文字列をムーブ構築する。動的メモリ割当は行わない。
    public: flyweight(
        /// [in,out] ムーブ元となる文字列。
        this_type&& io_source)
    PSYQ_NOEXCEPT: base_type(std::move(io_source))
    {}

    /// @brief 文字列を構築する。動的メモリ割当を行う場合ある。
    /// @details
    ///   in_string と等価な文字列が in_factory に…
    ///   - あれば、動的メモリ割当てを行わない。
    ///   - なければ、動的メモリ割当てを行う場合がある。
    public: flyweight(
        /// [in] コピー元となる文字列。
        typename base_type::view const& in_string,
        /// [in] フライ級文字列の生成器。
        typename base_type::base_type::factory_smart_ptr const& in_factory,
        /// [in] 文字列チャンクを生成する場合の、デフォルトのチャンク容量。
        std::size_t const in_chunk_size =
            PSYQ_STRING_FLYWEIGHT_CHUNK_SIZE_DEFAULT):
    base_type(
        base_type::base_type::make(in_factory, in_string, in_chunk_size))
    {
        PSYQ_ASSERT(in_string.empty() || !this->empty());
    }

    /// @brief 文字列をコピー代入する。動的メモリ割当は行わない。
    /// @return *this
    public: this_type& operator=(
        /// [in] コピー元となる文字列。
        this_type const& in_source)
    {
        this->base_type::base_type::operator=(in_source);
        return *this;
    }

    /// @brief 文字列をムーブ代入する。動的メモリ割当は行わない。
    /// @return *this
    public: this_type& operator=(
        /// [in,out] ムーブ元となる文字列。
        this_type&& io_source)
    {
        this->base_type::base_type::operator=(std::move(io_source));
        return *this;
    }

}; // class psyq::string::flyweight

#endif // !defined(PSYQ_STRING_FLYWEIGHT_HPP_)
// vim: set expandtab:
