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
#ifndef PSYQ_IMMUTABLE_STRING_HPP_
#define PSYQ_IMMUTABLE_STRING_HPP_

//#include "reference_string.hpp"

#ifndef PSYQ_BASIC_IMMUTABLE_STRING_ALLOCATOR_DEFAULT
#define PSYQ_BASIC_IMMUTABLE_STRING_ALLOCATOR_DEFAULT\
    std::allocator<typename template_char_traits::char_type>
#endif // !PSYQ_BASIC_IMMUTABLE_STRING_ALLOCATOR_DEFAULT

/** @brief ������̎Q��count��thread-safe�ɂ��Ȃ����ǂ����B
    @warning ���̂Ƃ���thread-safe�ɂ͖��Ή��B
 */
#ifndef PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
#define PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS 1
#endif // PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS

namespace psyq
{
    /// @cond
    template<
        typename template_char_type,
        typename template_char_traits = PSYQ_BASIC_REFERENCE_STRING_DEFAULT,
        typename template_allocator_type =
            PSYQ_BASIC_IMMUTABLE_STRING_ALLOCATOR_DEFAULT>
                class basic_immutable_string;
    /// @endcond

    /// char�^�̕��������� basic_immutable_string
    typedef psyq::basic_immutable_string<char> immutable_string;

    /// wchar_t�^�̕��������� basic_immutable_string
    typedef psyq::basic_immutable_string<wchar_t> const_wstring;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string ��interface��͂����Aimmutable�ȕ�����B

    - immutable�ȕ�������Q��count�ŊǗ�����B
    - ���̌^�̕������copy����Ƃ������Amemory���蓖�Ă��s���B
    - �����������������interface�͂Ȃ��B
    - not thread-safe

    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
 */
template<
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
class psyq::basic_immutable_string:
    public psyq::basic_reference_string<
        template_char_type, template_char_traits>
{
    /// this���w���l�̌^�B
    public: typedef psyq::basic_immutable_string<
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
    public: explicit basic_immutable_string(
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        buffer_(nullptr),
        allocator_(in_allocator)
    {}

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_source copy���̕�����B
     */
    public: basic_immutable_string(self const& in_source)
    :
        super(in_source),
        allocator_(in_source.allocator_)
    {
        this->hold_buffer(in_source.buffer_);
    }

    /** @brief ��������ړ�����Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in,out] io_source move���̕�����B
     */
    public: basic_immutable_string(self&& io_source)
    :
        super(io_source),
        buffer_(io_source.buffer_),
        allocator_(io_source.allocator_)
    {
        new(&io_source) super();
        io_source.buffer_ = nullptr;
    }

    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string    �Q�Ƃ��镶����literal�B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: template <std::size_t template_size>
    basic_immutable_string(
        typename self::value_type const (&in_string)[template_size],
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        super(in_string),
        buffer_(nullptr),
        allocator_(in_allocator)
    {}

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: basic_immutable_string(
        self const&                    in_string,
        typename self::size_type const in_offset,
        typename self::size_type const in_count = super::npos)
    :
        super(
            in_string.data() + in_offset,
            super::trim_count(in_string, in_offset, in_count)),
        allocator_(in_string.allocator_)
    {
        this->hold_buffer(in_string.buffer_);
    }

    /** @brief memory�����蓖�Ă��s���Aimmutable�ȕ�������\�z����B
        @param[in] in_string    copy���̕�����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_immutable_string(
        super const&                         in_string,
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        allocator_(in_allocator)
    {
        this->create_buffer(in_string);
    }

    /** @brief memory�����蓖�Ă��s���Aimmutable�ȕ�������\�z����B
        @param[in] in_string    copy���̕�����̐擪�ʒu�B
        @param[in] in_length    copy���̕�����̒����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_immutable_string(
        typename self::const_pointer const   in_string,
        typename self::size_type const       in_length,
        typename self::allocator_type const& in_allocator
        = self::allocator_type())
    :
        allocator_(in_allocator)
    {
        this->create_buffer(super(in_string, in_length));
    }

    /// destructor
    public: ~basic_immutable_string()
    {
        this->release_buffer();
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

    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string    �Q�Ƃ��镶����literal�B
     */
    public: template <std::size_t template_size>
    self& operator=(
        typename self::value_type const (&in_string)[template_size])
    {
        this->release_buffer();
        new(this) super(in_string);
        this->buffer_ = nullptr;
        return *this;
    }

    /** @brief ���������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_target)
    {
        this->super::swap(io_target);
        std::swap(this->buffer_, io_target.buffer_);
        std::swap(this->allocator_, io_target.allocator_);
    }

    //-------------------------------------------------------------------------
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
     */
    public: self& assign(self&& io_string)
    {
        *this = io_string;
        return *this;
    }

    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string �Q�Ƃ��镶����literal�B
     */
    public: template <std::size_t template_size>
    self& assign(typename self::value_type const (&in_string)[template_size])
    {
        this->release_buffer();
        new(this) super(in_string);
        this->buffer_ = nullptr;
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
        if (this->buffer_ != in_string.buffer_)
        {
            this->release_buffer();
            this->hold_buffer(in_string.buffer_);
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
        auto const local_buffer(this->buffer_);
        if (local_buffer == nullptr
            || in_string.data() < local_buffer->get_begin()
            || local_buffer->get_end() <= in_string.data())
        {
            this->release_buffer();
            this->create_buffer(in_string);
        }
        else
        {
            new(this) super(in_string);
        }
        return *this;
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string copy���̕�����̐擪�ʒu�B
        @param[in] in_length copy���̕�����̒����B
     */
    public: self& assign(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        return this->assign(super(in_string, in_length));
    }

    //-------------------------------------------------------------------------
    public: typename self::allocator_type const& get_allocator() const
    {
        return this->allocator_;
    }

    /** @brief ������������擾����B
        @param[in] in_offset ����������̊J�noffset�ʒu�B
        @param[in] in_count  ����������̕������B
     */
    public: self substr(
        typename self::size_type in_offset = 0,
        typename self::size_type in_count = self::npos)
    const
    {
        return self(*this, in_offset, in_count);
    }

    //-------------------------------------------------------------------------
    /// ������buffer�B
    private: struct buffer
    {
        explicit buffer(std::size_t const in_capacity)
        :
            reference_count_(1),
            capacity_(in_capacity)
        {}

        value_type* get_begin()
        {
            return reinterpret_cast<value_type*>(this + 1);
        }

        value_type* get_end()
        {
            return this->get_begin() + this->capacity_;
        }

#if PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        std::size_t reference_count_; ///< ������buffer�̔�Q�Ɛ��B
#else
        std::atomic<std::size_t> reference_count_;
#endif // PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        std::size_t capacity_;        ///< ������buffer�̑傫���B
    };

    /// ������ێ��q��memory�����q�B
    private: typedef typename self::allocator_type::template
        rebind<std::size_t>::other
            buffer_allocator;

    private: typename self::value_type* allocate_buffer(
        typename self::size_type const in_length)
    {
        if (in_length <= 0)
        {
            return nullptr;
        }

        // ������buffer���\�z����B
        typename self::buffer_allocator local_allocator(this->allocator_);
        auto const local_allocate_size(self::count_allocate_size(in_length));
        auto const local_buffer(
            reinterpret_cast<typename self::buffer*>(
                local_allocator.allocate(local_allocate_size)));
        if (local_buffer == nullptr)
        {
            PSYQ_ASSERT(false);
            return nullptr;
        }
        auto const local_allocate_bytes(
            local_allocate_size
            * sizeof(typename self::buffer_allocator::value_type));
        new(local_buffer) typename self::buffer(
            (local_allocate_bytes - sizeof(typename self::buffer))
            / sizeof(typename self::value_type));

        // ������buffer��ێ�����B
        auto const local_buffer_begin(local_buffer->get_begin());
        new(this) super(local_buffer_begin, in_length);
        this->buffer_ = local_buffer;
        return local_buffer_begin;
    }

    private: void create_buffer(super const& in_string)
    {
        if (in_string.empty())
        {
            this->buffer_ = nullptr;
            return;
        }

        // ������buffer���m�ۂ���B
        auto const local_buffer(this->allocate_buffer(in_string.length()));
        if (local_buffer == nullptr)
        {
            this->buffer_ = nullptr;
            return;
        }

        // ������buffer������������B
        super::traits_type::copy(
            local_buffer, in_string.data(), in_string.length());
        local_buffer[in_string.length()] = 0;
    }

    private: void hold_buffer(typename self::buffer* const io_buffer)
    {
        this->buffer_ = io_buffer;
        if (io_buffer != nullptr)
        {
#if PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
            ++io_buffer->reference_count_;
#else
            std::atomic_fetch_add_explicit(
                &io_buffer->reference_count_, 1, std::memory_order_relaxed);
#endif // PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS 
        }
    }

    private: bool release_buffer()
    {
        auto const local_buffer(this->buffer_);
        if (local_buffer == nullptr)
        {
            return false;
        }
#if PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        --local_buffer->reference_count_;
        if (0 < local_buffer->reference_count_)
#else
        auto const local_last_count(
            std::atomic_fetch_sub_explicit(
                &local_buffer->reference_count_,
                1,
                std::memory_order_release));
        if (1 < local_last_count)
#endif // PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        {
            return false;
        }
#if !PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        std::atomic_thread_fence(std::memory_order_acquire);
#endif // PSYQ_BASIC_IMMUTABLE_STRING_DISABLE_THREADS
        auto const local_capacity(local_buffer->capacity_);
        local_buffer->~buffer();
        typename self::buffer_allocator(this->allocator_).deallocate(
            reinterpret_cast<typename self::buffer_allocator::pointer>(
                local_buffer),
            self::count_allocate_size(local_capacity));
        return true;
    }

    private:
    static typename self::buffer_allocator::size_type count_allocate_size(
        typename self::size_type const in_string_length)
    {
        auto const local_string_bytes(
            sizeof(typename self::value_type) * (in_string_length + 1));
        auto const local_header_bytes(sizeof(typename self::buffer));
        auto const local_unit_bytes(
            sizeof(typename self::buffer_allocator::value_type));
        return (local_header_bytes + local_string_bytes + local_unit_bytes - 1)
            / local_unit_bytes;
    }

    //-------------------------------------------------------------------------
    private: typename self::buffer*        buffer_;    ///< ������buffer�B
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
        psyq::basic_immutable_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_left,
        psyq::basic_immutable_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_right)
    {
        io_left.swap(io_right);
    }
};

#endif // PSYQ_IMMUTABLE_STRING_HPP_
