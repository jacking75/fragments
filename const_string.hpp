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

    ������萔�Ȃ̂ŁA�����������������interface�͎����Ȃ��B

    constructor�ŕ�����literal��n���ƁAmemory���蓖�Ă��s��Ȃ��B

    constructor�ŕ�����literal�ȊO�̕������n���ƁA
    memory���蓖�Ă��s���A�������copy����B

    constructor�ȊO�ł́Amemory���蓖�Ă���؍s��Ȃ��B

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
class psyq::basic_const_string:
    private psyq::basic_reference_string<
        template_char_type, template_char_traits>
{
    /// this���w���l�̌^�B
    public: typedef psyq::basic_const_string<
        template_char_type, template_char_traits, template_allocator_type>
            self;

    /// self �̏�ʌ^�B
    private: typedef psyq::basic_reference_string<
        template_char_type, template_char_traits>
            super;

    /// memory�����q�̌^�B
    public: typedef template_allocator_type allocator_type;

    // public�ɂ����ʌ^�̓����^�B
    public: using super::traits_type;
    public: using super::value_type;
    public: using super::size_type;
    public: using super::difference_type;
    public: using super::pointer;
    public: using super::const_pointer;
    public: using super::reference;
    public: using super::const_reference;
    public: using super::iterator;
    public: using super::const_iterator;
    public: using super::reverse_iterator;
    public: using super::const_reverse_iterator;

    // public�ɂ����ʌ^��member�֐��B
    public: using super::data;
    public: using super::begin;
    public: using super::end;
    public: using super::cbegin;
    public: using super::cend;
    public: using super::rbegin;
    public: using super::rend;
    public: using super::crbegin;
    public: using super::crend;
    public: using super::front;
    public: using super::back;
    public: using super::length;
    public: using super::size;
    public: using super::max_size;
    public: using super::capacity;
    public: using super::empty;
    public: using super::at;
    public: using super::operator[];
    public: using super::compare;
    public: using super::find;
    public: using super::rfind;
    public: using super::find_first_of;
    public: using super::find_last_of;
    public: using super::find_first_not_of;
    public: using super::find_last_not_of;

    // public�ɂ����ʌ^��member�萔�B
    public: using super::npos;

    //-------------------------------------------------------------------------
    /** @brief memory�����q���g�킸�A�󕶎�����\�z�B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: explicit basic_const_string(
        typename self::allocator_type const& in_allocator =
            self::allocator_type())
    :
        super(),
        is_allocated_(false),
        allocator_(in_allocator)
    {
        // pass
    }

    /** @brief memory�����q���g�킸�A������literal�𒼐ڊ��蓖�Ăč\�z�B
        @tparam template_size ���蓖�Ă镶����literal�̗v�f���B
        @param[in] in_string    ���蓖�Ă镶����literal�B
        @param[in] in_allocator memory�����q�̏����l�B
        @note
            ���̊֐����������邽�߁A
            basic_const_string(self::const_pointer) ��omit�����B
     */
    public: template <std::size_t template_size>
    basic_const_string(
        typename self::value_type const      (&in_string)[template_size],
        typename self::allocator_type const& in_allocator =
            self::allocator_type())
    :
        super(in_string),
        is_allocated_(false),
        allocator_(in_allocator)
    {
        PSYQ_ASSERT(0 < template_size);
    }

    /** @brief memory���蓖�Ă��s���A������copy���č\�z�B
        @param[in] in_count     �������B
        @param[in] in_char      copy���镶���B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_const_string(
        typename self::size_type const      in_count,
        typename self::value_type const     in_char,
        typename self::allocator_type const in_allocator =
            self::allocator_type())
    :
        super(),
        is_allocated_(false),
        allocator_(in_allocator)
    {
        self::value_type* const local_pointer(this->allocate_string(in_count));
        if (local_pointer != NULL)
        {
            for (typename self::size_type i(0); i < in_count; ++i)
            {
                local_pointer[i] = in_char;
            }
            local_pointer[in_count] = 0;
        }
    }

    /** @brief memory���蓖�Ă��s���A�������copy���č\�z�B
        @param[in] in_string    copy���镶����̐擪�ʒu�B
        @param[in] in_size      copy���镶����̒����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_const_string(
        typename self::const_pointer const  in_string,
        typename self::size_type const      in_size,
        typename self::allocator_type const in_allocator =
            self::allocator_type())
    :
        super(),
        is_allocated_(false),
        allocator_(in_allocator)
    {
        this->create_string(in_string, in_size);
    }

    /** @brief memory���蓖�Ă��s���A�C�ӌ^�̕������copy���č\�z�B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string    copy���镶����B
        @param[in] in_offset    copy���镶����̊J�noffset�ʒu�B
        @param[in] in_count     copy���镶�����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: template<typename template_string_type>
    basic_const_string(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset,
        typename template_string_type::size_type const in_count =
            template_string_type::npos,
        typename self::allocator_type const            in_allocator =
            self::allocator_type())
    :
        super(),
        is_allocated_(false),
        allocator_(in_allocator)
    {
        this->create_string(
            in_string.data() + in_offset,
            super::trim_count(in_string, in_offset, in_count));
    }

    /// destructor
    public: ~basic_const_string()
    {
        if (this->is_allocated())
        {
            this->allocator_.deallocate(
                const_cast<typename self::value_type*>(this->data()),
                this->size() + 1);
        }
    }

    //-------------------------------------------------------------------------
    /** @brief memory���蓖�Ă��s�������ǂ����B
     */
    public: bool is_allocated() const
    {
        return this->is_allocated_;
    }

    /** @brief �ێ����Ă���memory�����q���擾�B
     */
    public: typename self::allocator_type const& get_allocator() const
    {
        return this->allocator_;
    }

    /** @brief NULL�����ŏI��镶������擾�B
        @return NULL�����ŏI��镶����̐擪�ʒu�B
     */
    public: typename self::const_pointer c_str() const
    {
        return this->super::data();
    }

    /** @brief ������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_target)
    {
        this->super::swap(io_target);
        std::swap(this->is_allocated_, io_target.is_allocated_);
        std::swap(this->allocator_, io_target.allocator_);
    }

    //-------------------------------------------------------------------------
    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� == �E��
     */
    public: bool operator==(
        self const& in_right)
    const
    {
        return this->super::is_equal(in_right.data(), in_right.size());
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� == �E��
     */
    public: template<typename template_string_type>
    bool operator==(
        template_string_type const& in_right)
    const
    {
        return this->super::is_equal(in_right.data(), in_right.size());
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� == �E��
     */
    public: template<std::size_t template_size>
    bool operator==(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return this->super::is_equal(&in_right[0], template_size - 1);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� != �E��
     */
    public: bool operator!=(
        self const& in_right)
    const
    {
        return !this->super::is_equal(in_right.data(), in_right.size());
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� != �E��
     */
    public: template<typename template_string_type>
    bool operator!=(
        template_string_type const& in_right)
    const
    {
        return !this->super::is_equal(in_right.data(), in_right.size());
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� != �E��
     */
    public: template<std::size_t template_size>
    bool operator!=(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return !this->super::is_equal(&in_right[0], template_size - 1);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: bool operator<(
        self const& in_right)
    const
    {
        return this->super::compare(in_right) < 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: template<typename template_string_type>
    bool operator<(
        template_string_type const& in_right)
    const
    {
        return this->super::compare(in_right) < 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: template<std::size_t template_size>
    bool operator<(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return this->super::compare(super(in_right)) < 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� <= �E��
     */
    public: bool operator<=(
        self const& in_right)
    const
    {
        return this->super::compare(in_right) <= 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� <= �E��
     */
    public: template<typename template_string_type>
    bool operator<=(
        template_string_type const& in_right)
    const
    {
        return this->super::compare(in_right) <= 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: template<std::size_t template_size>
    bool operator<=(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return this->super::compare(super(in_right)) <= 0;
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� > �E��
     */
    public: bool operator>(
        self const& in_right)
    const
    {
        return 0 < this->super::compare(in_right);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� > �E��
     */
    public: template<typename template_string_type>
    bool operator>(
        template_string_type const& in_right)
    const
    {
        return 0 < this->super::compare(in_right);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: template<std::size_t template_size>
    bool operator>(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return 0 < this->super::compare(super(in_right));
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� >= �E��
     */
    public: bool operator>=(
        self const& in_right)
    const
    {
        return 0 <= this->super::compare(in_right);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� >= �E��
     */
    public: template<typename template_string_type>
    bool operator>=(
        template_string_type const& in_right)
    const
    {
        return 0 <= this->super::compare(in_right);
    }

    /** @brief ������̔�r�B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r�B

        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: template<std::size_t template_size>
    bool operator>=(
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return 0 <= this->super::compare(super(in_right));
    }

    //-------------------------------------------------------------------------
    /** @brief
        memory���蓖�Ă�}�����邽�߁Acopy-constructor��copy���Z�q�͎g�p�֎~�B

        copy-constructor��copy���Z�q���g����ƕ֗������A
        copy�����ɔ����d�݂��������ɂ����B
        copy�������s���ꍇ�́A����ɔ����d�݂��������₷���悤�ɁA
        �T�^�I�Ȋ֐��Ăяo���̌`���ŋL�q����̂𐄏�����B

        @note
        copy-constructor��copy���Z�q���ǂ����Ă��K�v�ɂȂ����ꍇ�́A
        psyq::basic_reference_string ��smart-pointer�ŎQ�Ƃ��������
        �l���邱�ƁB
     */
    private: basic_const_string(
        self const& in_source)
    :
        super(),
        is_allocated_(in_source.is_allocated_),
        allocator_(in_source.allocator_)
    {
        if (in_source.is_allocated())
        {
            // ������literal�łȂ���΁Amemory���蓖�Ă��s���A�������copy�B
            this->create_string(in_source.data(), in_source.size());
        }
        else
        {
            // ������literal�Ȃ�A������literal�𒼐ڊ��蓖�Ă�B
            static_cast<super&>(*this) = in_source;
        }
    }

    /// @copydoc basic_const_string(self const&)
    private: self& operator=(self const&);

    private: typename self::value_type* allocate_string(
        typename self::size_type const in_size)
    {
        if (in_size <= 0)
        {
            return NULL;
        }
        typename self::size_type const local_capacity(in_size + 1);
        typename self::value_type* const local_pointer(
            this->allocator_.allocate(local_capacity));
        if (local_pointer == NULL)
        {
            PSYQ_ASSERT(false);
            return NULL;
        }
        this->is_allocated_ = true;
        new(this) super(local_pointer, in_size);
        return local_pointer;
    }

    private: void create_string(
        typename self::const_pointer const  in_string,
        typename self::size_type const      in_size)
    {
        if (in_string == NULL)
        {
            PSYQ_ASSERT(in_size <= 0);
            return;
        }
        self::value_type* const local_pointer(this->allocate_string(in_size));
        if (local_pointer != NULL)
        {
            self::traits_type::copy(local_pointer, in_string, in_size);
            local_pointer[in_size] = 0;
        }
    }

    //-------------------------------------------------------------------------
    /// memory���蓖�Ă��s�������ǂ����B
    private: bool is_allocated_;

    /// �g���Ă���memory�����q�B
    private: typename self::allocator_type allocator_;
};

//-----------------------------------------------------------------------------
/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� == �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator==(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator==(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� != �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator!=(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator!=(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� < �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator<(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator>(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� <= �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator<=(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator>=(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� > �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator>(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator<(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� >= �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
bool operator>=(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type,
        template_char_traits,
        template_allocator_type> const&
            in_right)
{
    return in_right.operator<=(in_left);
}

//-----------------------------------------------------------------------------
namespace std
{
    /** @brief ������̌����B
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
