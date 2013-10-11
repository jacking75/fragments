/* Copyright (c) 2013, Hillco Psychi, All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_CONST_STRING_HPP_
#define PSYQ_CONST_STRING_HPP_

//#include "reference_string.hpp"

#ifndef PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT
#define PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT\
    std::allocator<typename template_char_traits::char_type>
#endif // !PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT

namespace psyq
{
    /// @cond
    template<
        typename template_char_type,
        typename template_char_traits = PSYQ_BASIC_REFERENCE_STRING_DEFAULT,
        typename template_allocator_type =
            PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT>
                class basic_const_string;
    /// @endcond

    /// char�^�̕��������� basic_const_string
    typedef psyq::basic_const_string<char> const_string;

    /// wchar_t�^�̕��������� basic_const_string
    typedef psyq::basic_const_string<wchar_t> const_wstring;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string ��interface��͂����A������萔�B

    - constructor�ŕ���pointer��n����memory���蓖�Ă��s���A�������copy����B
    - ��L�ȊO�ł́Amemory���蓖�Ă���؍s��Ȃ��B
    - ������萔�Ȃ̂ŁA�����������������interface�͎����Ȃ��B
    - not thread-safe

    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B

    @note �Q��counter�����ł�thread-safe�ɂ������B
 */
template<
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
class psyq::basic_const_string:
    public psyq::basic_reference_string<
        template_char_type, template_char_traits>
{
    /// this���w���l�̌^�B
    public: typedef psyq::basic_const_string<
        template_char_type, template_char_traits, template_allocator_type>
            self;

    /// self �̏�ʌ^�B
    public: typedef psyq::basic_reference_string<
        template_char_type, template_char_traits>
            super;

    /// memory�����q�̌^�B
    public: typedef template_allocator_type allocator_type;

    //-------------------------------------------------------------------------
    /** @brief �󕶎�����\�z����Bmemory���蓖�Ă͍s��Ȃ��B
     */
    public: explicit basic_const_string(
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        holder_(nullptr),
        allocator_(in_allocator)
    {}

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_source copy���̕�����B
     */
    public: basic_const_string(self const& in_source)
    :
        super(in_source),
        allocator_(in_source.allocator_)
    {
        this->hold_string(in_source.holder_);
    }

    /** @brief ��������ړ�����Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in,out] io_source move���̕�����B
     */
    public: basic_const_string(self&& io_source)
    :
        super(io_source),
        holder_(io_source.holder_),
        allocator_(io_source.allocator_)
    {
        new(&io_source) super();
        io_source.holder_ = nullptr;
    }

    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string    �Q�Ƃ��镶����literal�B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: template <std::size_t template_size>
    basic_const_string(
        typename self::value_type const (&in_string)[template_size],
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        super(in_string),
        holder_(nullptr),
        allocator_(in_allocator)
    {}

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: basic_const_string(
        self const&                    in_string,
        typename self::size_type const in_offset,
        typename self::size_type const in_count = super::npos)
    :
        super(
            in_string.data() + in_offset,
            super::trim_count(in_string, in_offset, in_count)),
        allocator_(in_string.allocator_)
    {
        this->hold_string(in_string.holder_);
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string    copy���̕�����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_const_string(
        super const&                         in_source,
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        allocator_(in_allocator)
    {
        if (!this->create_string(in_source.data(), in_source.length()))
        {
            this->holder_ = nullptr;
        }
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string    copy���̕�����̐擪�ʒu�B
        @param[in] in_length    copy���̕�����̒����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_const_string(
        typename self::const_pointer const   in_string,
        typename self::size_type const       in_length,
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        allocator_(in_allocator)
    {
        if (!this->create_string(in_string, in_length))
        {
            this->holder_ = nullptr;
        }
    }

    /// destructor
    public: ~basic_const_string()
    {
        this->release_string();
    }

    //-------------------------------------------------------------------------
    public: self& operator=(self const& in_right)
    {
        self(in_right).swap(*this);
        return *this;
    }

    public: self& operator=(self&& io_right)
    {
        this->swap(io_right);
        return *this;
    }

    public: typename self::allocator_type const& get_allocator() const
    {
        return this->allocator_;
    }

    /** @brief ���������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_target)
    {
        this->super::swap(io_target);
        std::swap(this->holder_, io_target.holder_);
        std::swap(this->allocator_, io_target.allocator_);
    }

    //-------------------------------------------------------------------------
    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string �Q�Ƃ��镶����literal�B
     */
    public: template <std::size_t template_size>
    self& assign(typename self::value_type const (&in_string)[template_size])
    {
        this->release_string();
        new(this) super(in_string);
        this->holder_ = nullptr;
        return *this;
    }

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_string �Q�Ƃ��镶����B
     */
    public: self& assign(self const& in_string)
    {
        *this = in_string;
        return *this;
    }

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: self& assign(
        self const&                    in_string,
        typename self::size_type const in_offset,
        typename self::size_type const in_count = super::npos)
    {
        if (this->holder_ != in_string.holder_)
        {
            this->release_string();
            this->hold_string(in_string.holder_);
        }
        new(this) super(
            in_string.data() + in_offset,
            super::trim_count(in_string, in_offset, in_count));
        return *this;
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string copy���̕�����B
     */
    public: self& assign(super const& in_string)
    {
        return this->assign(in_string.data(), in_string.size());
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string copy���̕�����̐擪�ʒu�B
        @param[in] in_length copy���̕�����̒����B
     */
    public: self& assign(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        if (in_string < this->begin() || this->end() <= in_string)
        {
            this->release_string();
            if (!this->create_string(in_string, in_length))
            {
                this->holder_ = nullptr;
            }
        }
        else
        {
            new(this) super(in_string, in_length);
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    /// ������ێ��q�B
    private: struct holder
    {
        std::size_t refrence_count_; ///< ������̔�Q�Ɛ��B
        std::size_t string_length_;  ///< �Q�Ƃ��Ă镶����̕������B
    };

    /// ������ێ��q��memory�����q�B
    private: typedef typename self::allocator_type::template
        rebind<typename self::holder>::other
            holder_allocator;

    private: bool create_string(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        if (in_length <= 0)
        {
            return false;
        }
        if (in_string == nullptr)
        {
            PSYQ_ASSERT(false);
            return false;
        }

        // ������buffer���m�ۂ���B
        typename self::holder_allocator local_allocator(this->allocator_);
        auto const local_holder(
            local_allocator.allocate(self::count_allocate_size(in_length)));
        if (local_holder == nullptr)
        {
            PSYQ_ASSERT(false);
            return false;
        }

        // ������buffer������������B
        auto const local_string(
            reinterpret_cast<self::value_type*>(local_holder + 1));
        local_holder->refrence_count_ = 1;
        local_holder->string_length_ = in_length;
        super::traits_type::copy(local_string, in_string, in_length);
        local_string[in_length] = 0;

        // ������buffer��ێ�����B
        new(this) super(local_string, in_length);
        this->holder_ = local_holder;
        return true;
    }

    private: void hold_string(typename self::holder* const io_holder)
    {
        this->holder_ = io_holder;
        if (io_holder != nullptr)
        {
            //std::atomic_fetch_add_explicit(
            //    &io_holder->refrence_count_, 1, std::memory_order_relaxed);
            ++io_holder->refrence_count_;
        }
    }

    private: bool release_string()
    {
        auto const local_holder(this->holder_);
        if (local_holder == nullptr)
        {
            return false;
        }
        //bool const local_destroy(
        //    1 == std::atomic_fetch_sub_explicit(
        //        &local_holder->refrence_count_, 1, std::memory_order_release));
        //if (local_destroy)
        if (0 < (--local_holder->refrence_count_))
        {
            return false;
        }
        //std::atomic_thread_fence(std::memory_order_acquire);
        auto const local_length(local_holder->string_length_);
        local_holder->~holder();
        typename self::holder_allocator(this->allocator_).deallocate(
            local_holder, self::count_allocate_size(local_length));
        return true;
    }

    private:
    static typename self::holder_allocator::size_type count_allocate_size(
        typename self::size_type const in_string_length)
    {
        auto const local_holder_size(sizeof(typename self::holder));
        auto const local_string_length(
            sizeof(typename self::value_type) * (in_string_length + 1)
            + local_holder_size - 1);
        return 1 + local_string_length / local_holder_size;
    }

    //-------------------------------------------------------------------------
    private: typename self::holder*        holder_;    ///< ������ێ��q�B
    private: typename self::allocator_type allocator_; ///< �g�p����memory�����q�B
};

//-----------------------------------------------------------------------------
namespace std
{
    /** @brief ���������������B
        @tparam template_char_type
            @copydoc psyq::basic_reference_string::value_type
        @tparam template_char_traits
            @copydoc psyq::basic_reference_string::traits_type
        @tparam template_allocator_type memory�����q�̌^�B
        @param[in] in_left  �������镶����B
        @param[in] in_right �������镶����B
     */
    template<
        typename template_char_type,
        typename template_char_traits,
        typename template_allocator_type>
    void swap(
        psyq::basic_const_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_left,
        psyq::basic_const_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_right)
    {
        io_left.swap(io_right);
    }
};

#endif // PSYQ_CONST_STRING_HPP_
