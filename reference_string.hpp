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
#ifndef PSYQ_REFERENCE_STRING_HPP_
#define PSYQ_REFERENCE_STRING_HPP_

#include <iosfwd>
#include <iterator>
#include <algorithm>

#ifndef PSYQ_ASSERT
#define PSYQ_ASSERT(define_expression) assert(define_expression)
#endif // !PSYQ_ASSERT

#ifndef PSYQ_BASIC_REFERENCE_STRING_DEFAULT
#define PSYQ_BASIC_REFERENCE_STRING_DEFAULT\
    std::char_traits<template_char_type>
#endif // !PSYQ_BASIC_REFERENCE_STRING_DEFAULT

namespace psyq
{
    /// @cond
    template<
        typename template_char_type,
        typename template_char_traits = PSYQ_BASIC_REFERENCE_STRING_DEFAULT>
            class basic_reference_string;
    /// @endcond

    /// char�^�̕��������� basic_reference_string
    typedef psyq::basic_reference_string<char> reference_string;

    /// wchar_t�^�̕��������� basic_reference_string
    typedef psyq::basic_reference_string<wchar_t> reference_wstring;

}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string ��interface��͂����A������萔�ւ̎Q�ƁB

    - memory���蓖�Ă���؍s��Ȃ��B
    - ������萔�Ȃ̂ŁA�����������������interface�͎����Ȃ��B
    - not thread-safe

    @warning
        ���蓖�Ă�ꂽ������𕶎���萔�Ƃ��ĎQ�Ƃ��Ă�̂ŁA
        �Q�Ɛ�̕����񂪍X�V�����ƁA�����ۏ؂ł��Ȃ��Ȃ�B
        ������萔�����S�Ɉ����ɂ́A psyq::basic_const_string ���g���B

    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
 */
template<typename template_char_type, typename template_char_traits>
class psyq::basic_reference_string
{
    /// this���w���l�̌^�B
    public: typedef psyq::basic_reference_string<
        template_char_type, template_char_traits> self;

    //-------------------------------------------------------------------------
    /// ���������̌^�B
    public: typedef template_char_traits traits_type;

    /// �����̌^�B
    public: typedef typename self::traits_type::char_type value_type;

    /// �������̌^�B
    public: typedef std::size_t size_type;

    /// �����q�̍���\���^�B
    public: typedef std::ptrdiff_t difference_type;

    /// �����ւ�pointer�B
    public: typedef typename self::value_type const* const_pointer;

    /// �����ւ�pointer�B
    public: typedef typename self::const_pointer pointer;

    /// �����ւ̎Q�ƁB
    public: typedef typename self::value_type const& const_reference;

    /// �����ւ̎Q�ƁB
    public: typedef typename self::const_reference reference;

    /// �������w�������q�B
    public: typedef typename self::const_pointer const_iterator;

    /// �������w�������q�B
    public: typedef typename self::const_iterator iterator;

    /// �������w���t�����q�B
    public: typedef std::reverse_iterator<const_iterator>
        const_reverse_iterator;

    /// �������w���t�����q�B
    public: typedef typename self::const_reverse_iterator
        reverse_iterator;

    //-------------------------------------------------------------------------
    /** @brief ��̕�������\�z����B
     */
    public: basic_reference_string()
    :
        data_(nullptr),
        length_(0)
    {}

    /** @brief ������literal���Q�Ƃ���B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string �Q�Ƃ��镶����literal�B
     */
    public: template <std::size_t template_size>
    basic_reference_string(
        typename self::value_type const (&in_string)[template_size])
    :
        data_(&in_string[0]),
        length_(template_size - 1)
    {
        PSYQ_ASSERT(0 < template_size);
    }

    /** @brief ��������Q�Ƃ���B
        @param[in] in_string �Q�Ƃ��镶����̐擪�ʒu�B
        @param[in] in_length �Q�Ƃ��镶����̒����B
     */
    public: basic_reference_string(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    :
        data_(in_string),
        length_(in_length)
    {
        if (in_string == nullptr && 0 < in_length)
        {
            PSYQ_ASSERT(false);
            this->length_ = 0;
        }
    }

    /** @brief �C�ӌ^�̕�������Q�Ƃ���B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string �Q�Ƃ��镶����B
     */
    public: template<typename template_string_type>
    basic_reference_string(template_string_type const& in_string)
    :
        data_(in_string.data()),
        length_(in_string.length())
    {}

    /** @brief �C�ӌ^�̕�������Q�Ƃ���B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: template<typename template_string_type>
    basic_reference_string(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset,
        typename template_string_type::size_type const in_count
        = template_string_type::npos)
    :
        data_(in_string.data() + in_offset),
        length_(self::trim_count(in_string, in_offset, in_count))
    {}

    //-------------------------------------------------------------------------
    /** @brief ���������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_target)
    {
        std::swap(this->data_, io_target.data_);
        std::swap(this->length_, io_target.length_);
    }

    //-------------------------------------------------------------------------
    /** @brief ������literal���Q�Ƃ���B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string �Q�Ƃ��镶����literal�B
     */
    public: template <std::size_t template_size>
    self& assign(typename self::value_type const (&in_string)[template_size])
    {
        return *new(this) self(in_string);
    }

    /** @brief ��������Q�Ƃ���B
        @param[in] in_string �Q�Ƃ��镶����̐擪�ʒu�B
        @param[in] in_length �Q�Ƃ��镶����̒����B
     */
    public: self& assign(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        return *new(this) self(in_string, in_length);
    }

    /** @brief �C�ӌ^�̕�������Q�Ƃ���B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string �Q�Ƃ��镶����B
     */
    public: template<typename template_string_type>
    self& assign(template_string_type const& in_string)
    {
        return *new(this) self(in_string);
    }

    /** @brief �C�ӌ^�̕�������Q�Ƃ���B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: template<typename template_string_type>
    self& assign(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset,
        typename template_string_type::size_type const in_count
        = template_string_type::npos)
    {
        return *new(this) self(in_string, in_offset, in_count);
    }

    //-------------------------------------------------------------------------
    /** @brief ������̐擪�ʒu���擾����B
        @return ������̐擪�ʒu�B
        @warning �����񂪋󕶎��ŏI����Ă���Ƃ͌���Ȃ��B
     */
    public: typename self::const_pointer data() const
    {
        return this->data_;
    }

    /** @brief ������̐擪�ʒu���擾����B
        @return ������̐擪�ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator begin() const
    {
        return this->data();
    }

    /** @brief ������̖����ʒu���擾����B
        @return ������̖����ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator end() const
    {
        return this->begin() + this->length();
    }

    /** @brief ������̐擪�ʒu���擾����B
        @return ������̐擪�ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator cbegin() const
    {
        return this->begin();
    }

    /** @brief ������̖����ʒu���擾����B
        @return ������̖����ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator cend() const
    {
        return this->end();
    }

    /** @brief ������̖����ʒu���擾����B
        @return ������̖����ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator rbegin() const
    {
        return typename self::const_reverse_iterator(this->end());
    }

    /** @brief ������̐擪�ʒu���擾����B
        @return ������̐擪�ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator rend() const
    {
        return typename self::const_reverse_iterator(this->begin());
    }

    /** @brief ������̖����ʒu���擾����B
        @return ������̖����ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator crbegin() const
    {
        return this->rbegin();
    }

    /** @brief ������̐擪�ʒu���擾����B
        @return ������̐擪�ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator crend() const
    {
        return this->rend();
    }

    /** @brief ������̐擪�������Q�Ƃ���B
        @return ������̐擪�����ւ̎Q�ƁB
     */
    typename self::const_reference front() const
    {
        PSYQ_ASSERT(!this->empty());
        return (*this)[0];
    }

    /** @brief ������̖����������Q�Ƃ���B
        @return ������̖��������ւ̎Q�ƁB
     */
    public: typename self::const_reference back() const
    {
        PSYQ_ASSERT(!this->empty());
        return (*this)[this->length() - 1];
    }

    //-------------------------------------------------------------------------
    /** @brief ������̒������擾����B
        @return ������̒����B
     */
    public: typename self::size_type length() const
    {
        return this->length_;
    }

    /** @brief ������̒������擾����B
        @return ������̒����B
     */
    public: typename self::size_type size() const
    {
        return this->length();
    }

    /** @brief ������̍ő咷���擾����B
        @return
            ������̍ő咷�B
            ������̏����������ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
     */
    public: typename self::size_type max_size() const
    {
        return this->length();
    }

    /** @brief ������̗e�ʂ��擾����B
        @return
           ������̗e�ʁB
           ������̏����������ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
     */
    public: typename self::size_type capacity() const
    {
        return this->length();
    }

    /** @brief ��̕����񂩔��肷��B
        @retval true  ��̕�����B
        @retval false ��̕�����ł͂Ȃ��B
     */
    public: bool empty() const
    {
        return this->length() <= 0;
    }

    //-------------------------------------------------------------------------
    /** @brief �����񂪎��������Q�Ƃ���B
        @param[in] in_index ������index�ԍ��B
        @return �����ւ̎Q�ƁB
     */
    public: typename self::const_reference at(
        typename self::size_type const in_index)
    const
    {
        if (this->length() <= in_index)
        {
            PSYQ_ASSERT(false);
            //throw std::out_of_range; // ��O�͎g�������Ȃ��B
        }
        return *(this->data() + in_index);
    }

    /** @brief �����񂪎��������Q�Ƃ���B
        @param[in] in_index ������index�ԍ��B
        @return �����ւ̎Q�ƁB
     */
    public: typename self::const_reference operator[](
        typename self::size_type const in_index)
    const
    {
        PSYQ_ASSERT(in_index < this->length());
        return *(this->data() + in_index);
    }

    //-------------------------------------------------------------------------
    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� == �E��
     */
    public: bool operator==(self const& in_right) const
    {
        return this->is_equal(in_right.data(), in_right.length());
    }

    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� != �E��
     */
    public: bool operator!=(self const& in_right) const
    {
        return !this->is_equal(in_right.data(), in_right.length());
    }

    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� < �E��
     */
    public: bool operator<(self const& in_right) const
    {
        return this->compare(in_right) < 0;
    }

    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� <= �E��
     */
    public: bool operator<=(self const& in_right) const
    {
        return this->compare(in_right) <= 0;
    }

    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� > �E��
     */
    public: bool operator>(self const& in_right) const
    {
        return 0 < this->compare(in_right);
    }

    /** @brief ��������r����B

        *this�����ӂƂ��āA�E�ӂ̕�����Ɣ�r����B

        @param[in] in_right �E�ӂ̕�����B
        @return ���� >= �E��
     */
    public: bool operator>=(self const& in_right) const
    {
        return 0 <= this->compare(in_right);
    }

    //-------------------------------------------------------------------------
    /** @brief ��������r����B
        @param[in] in_right �E�ӂ̕�����̐擪�ʒu�B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<std::size_t template_size>
    int compare(
        typename self::value_type const (&in_right)[template_size])
    {
        return this->compare(0, this->length(), in_right);
    }

    /** @brief ��������r����B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_right �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_string_type>
    int compare(template_string_type const& in_right) const
    {
        return this->compare(
            0, this->length(), in_right.data(), in_right.length());
    }

    /** @brief ��������r����B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right       �E�ӂ̕�����̐擪�ʒu�B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template <std::size_t template_size>
    int compare(
        typename self::size_type const  in_left_offset,
        typename self::size_type const  in_left_count,
        typename self::value_type const (&in_right)[template_size])
    const
    {
        return this->compare(
            in_left_offset, in_left_count, &in_right[0], template_size - 1);
    }

    /** @brief ��������r����B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right       �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_string_type>
    int compare(
        typename self::size_type const in_left_offset,
        typename self::size_type const in_left_count,
        template_string_type const&    in_right)
    const
    {
        return this->compare(
            in_left_offset, in_left_count, in_right.data(), in_right.length());
    }

    /** @brief ��������r����B
        @param[in] in_left_offset  ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count   ���ӂ̕�����̕������B
        @param[in] in_right_begin  �E�ӂ̕�����̐擪�ʒu�B
        @param[in] in_right_length �E�ӂ̕�����̒����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        typename self::size_type const     in_left_offset,
        typename self::size_type const     in_left_count,
        typename self::const_pointer const in_right_begin,
        typename self::size_type const     in_right_length)
    const
    {
        auto const local_left_length(
            self::trim_count(*this, in_left_offset, in_left_count));
        bool const local_less(local_left_length < in_right_length);
        int const local_compare(
            template_char_traits::compare(
                this->data() + in_left_offset,
                in_right_begin,
                local_less? local_left_length: in_right_length));
        if (local_compare != 0)
        {
            return local_compare;
        }
        if (local_less)
        {
            return -1;
        }
        if (in_right_length < local_left_length)
        {
            return 1;
        }
        return 0;
    }

    /** @brief ��������r����B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_left_offset  ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count   ���ӂ̕�����̕������B
        @param[in] in_right        �E�ӂ̕�����B
        @param[in] in_right_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_right_count  �E�ӂ̕�����̕������B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_string_type>
    int compare(
        typename self::size_type const in_left_offset,
        typename self::size_type const in_left_count,
        template_string_type const&    in_right,
        typename self::size_type const in_right_offset,
        typename self::size_type const in_right_count)
    const
    {
        return this->compare(
            in_left_offset,
            in_left_count,
            in_right.data() + in_right_offset,
            self::trim_count(in_right, in_right_offset, in_right_count));
    }

    //-------------------------------------------------------------------------
    /** @brief ��������������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = 0)
    const
    {
        if (in_offset < this->length())
        {
            auto const local_find(
                template_char_traits::find(
                    this->data() + in_offset,
                    this->length() - in_offset,
                    in_char));
            if (local_find != nullptr)
            {
                return local_find - this->data();
            }
        }
        return self::npos;
    }

    /** @brief ���������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type find(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find(&in_string[0], in_offset, template_size - 1);
    }

    /** @brief ���������������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find(
        template_string_type const& in_string,
        typename self::size_type    in_offset = 0)
    const
    {
        return this->find(in_string.data(), in_offset, in_string.length());
    }

    /** @brief ���������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        if (in_length <= 0)
        {
            return in_offset <= this->length()? in_offset: self::npos;
        }
        PSYQ_ASSERT(nullptr != in_string);

        auto local_rest_length(this->length() - in_offset);
        if (in_offset < this->length() && in_length <= local_rest_length)
        {
            local_rest_length -= in_length - 1;
            auto local_rest_string(this->data() + in_offset);
            for (;;)
            {
                // ����������̐擪�����ƍ��v����ʒu��������B
                auto const local_find(
                    template_char_traits::find(
                        local_rest_string, local_rest_length, *in_string));
                if (local_find == nullptr)
                {
                    break;
                }

                // ����������ƍ��v���邩����B
                int const local_compare(
                    template_char_traits::compare(
                        local_find, in_string, in_length));
                if (local_compare == 0)
                {
                    return local_find - this->data();
                }

                // ���̌��ցB
                local_rest_length -= local_find + 1 - local_rest_string;
                local_rest_string = local_find + 1;
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ��납�當������������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type rfind(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = self::npos)
    const
    {
        if (!this->empty())
        {
            for (auto i(self::get_pointer(*this, in_offset)); ; --i)
            {
                if (template_char_traits::eq(*i, in_char))
                {
                    return i - this->data();
                }
                if (i <= this->data())
                {
                    break;
                }
            }
        }
        return self::npos;
    }

    /** @brief ��납�當�������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type rfind(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->rfind(&in_string[0], in_offset, template_size - 1);
    }

    /** @brief ��납�當�������������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type rfind(
        template_string_type const&    in_string,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->rfind(in_string.data(), in_offset, in_string.length());
    }

    /** @brief ��납�當�������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type rfind(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        if (in_length <= 0)
        {
            return in_offset < this->length() ? in_offset: this->length();
        }
        PSYQ_ASSERT(nullptr != in_string);

        if (in_length <= this->length())
        {
            auto const local_offset(
                (std::min)(in_offset, this->length() - in_length));
            for (auto i(this->data() + local_offset); ; --i)
            {
                if (template_char_traits::eq(*i, *in_string)
                    && template_char_traits::compare(i, in_string, in_length) == 0)
                {
                    return i - this->data();
                }
                if (i <= this->data())
                {
                    break;
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ��������������B
        @param[in] in_char   �������镶���B
        @param[in] in_offset �������J�n����ʒu�B
        @return �����������������ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_first_of(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find(in_char, in_offset);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕�������������B
        @param[in] in_string ����������̐擪�ʒu�B�K��nullptr�ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type find_first_of(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find_first_of(
            &in_string[0], in_offset, template_size - 1);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕�������������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_first_of(
        template_string_type const&    in_string,
        typename self::size_type const in_offset = 0)
    const
    {
        return this->find_first_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕�������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_first_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || nullptr != in_string);
        if (0 < in_length && in_offset < this->length())
        {
            auto const local_end(this->data() + this->length());
            for (auto i(this->data() + in_offset); i < local_end; ++i)
            {
                if (template_char_traits::find(in_string, in_length, *i) != nullptr)
                {
                    return i - this->data();
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ��������납�猟������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_last_of(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = self::npos)
    const
    {
        return this->rfind(in_char, in_offset);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type find_last_of(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find_last_of(
            &in_string[0], in_offset, template_size - 1);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_last_of(
        template_string_type const&    in_string,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->find_last_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length   ����������̒����B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_last_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || nullptr != in_string);
        if (0 < in_length && 0 < this->length())
        {
            for (auto i(self::get_pointer(*this, in_offset)); ; --i)
            {
                if (template_char_traits::find(in_string, in_length, *i) != nullptr)
                {
                    return i - this->data();
                }
                if (i <= this->data())
                {
                    break;
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ���������ȊO�̕�������������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_first_not_of(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = 0)
    const
    {
        auto const local_end(this->data() + this->length());
        for (auto i(this->data() + in_offset); i < local_end; ++i)
        {
            if (!template_char_traits::eq(*i, in_char))
            {
                return i - this->data();
            }
        }
        return self::npos;
    }

    /** @brief ����������Ɋ܂܂�Ȃ���������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type find_first_not_of(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find_first_not_of(
            &in_string[0], in_offset, template_size - 1);
    }

    /** @brief ����������Ɋ܂܂�Ȃ���������������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_first_not_of(
        template_string_type const&    in_string,
        typename self::size_type const in_offset = 0)
    const
    {
        return this->find_first_not_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�Ȃ���������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return
           ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_first_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || nullptr != in_string);
        if (in_offset < this->length())
        {
            auto const local_end(this->data() + this->length());
            for (auto i(this->data() + in_offset); i < local_end; ++i)
            {
                if (template_char_traits::find(in_string, in_length, *i) == nullptr)
                {
                    return i - this->data();
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ���������ȊO�̕������A��납�猟������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_last_not_of(
        typename self::value_type const in_char,
        typename self::size_type const  in_offset = self::npos)
    const
    {
        if (!this->empty())
        {
            for (auto i(self::get_pointer(*this, in_offset)); ; --i)
            {
                if (!template_char_traits::eq(*i, in_char))
                {
                    return i - this->data();
                }
                if (i <= this->data())
                {
                    break;
                }
            }
        }
        return self::npos;
    }

    /** @brief ����������Ɋ܂܂�Ȃ���������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template <std::size_t template_size>
    typename self::size_type find_last_not_of(
        typename self::value_type const (&in_string)[template_size],
        typename self::size_type const  in_offset = 0)
    const
    {
        return this->find_last_not_of(
            &in_string[0], in_offset, template_size - 1);
    }

    /** @brief ����������Ɋ܂܂�Ȃ���������������B
        @tparam template_string_traits @copydoc string_interface
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_last_not_of(
        template_string_type const&    in_string,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->find_last_not_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�Ȃ��������A��납�猟������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return
            ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ�� self::npos ��Ԃ��B
     */
    public: typename self::size_type find_last_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || nullptr != in_string);
        if (!this->empty())
        {
            for (auto i(self::get_pointer(*this, in_offset)); ; --i)
            {
                if (template_char_traits::find(in_string, in_length, *i) == nullptr)
                {
                    return i - this->data();
                }
                else if (i <= this->data())
                {
                    break;
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ������̐擪�Ɩ����ɂ���󔒕�������菜���B
     */
    public: void trim()
    {
        this->trim_end();
        this->trim_begin();
    }

    /** @brief ������̐擪�ɂ���󔒕�������菜���B
     */
    public: void trim_begin()
    {
        auto const local_first(
            self::find_first_not_of_space(this->data(), this->length()));
        if (local_first != self::npos)
        {
            this->data_ += local_first;
            this->length_ -= local_first;
        }
        else if (this->data() != nullptr)
        {
            this->data_ += this->length();
            this->length_ = 0;
        }
    }

    /** @brief ������̖����ɂ���󔒕�������菜���B
     */
    public: void trim_end()
    {
        auto const local_last(
            self::find_last_not_of_space(this->data(), this->length()));
        this->length_ = (local_last != self::npos? local_last + 1: 0);
    }

    /** @brief �󔒂ł͂Ȃ��������ŏ��ɂ���ʒu����������B
        @param[in] in_string ������̐擪�ʒu�B
        @param[in] in_length ������̒����B
        @retval !=self::npos �󔒂ł͂Ȃ��������ŏ��ɂ���ʒu�B
        @retval ==self::npos �����񂪋󂩁A���ׂċ󔒕����������B
     */
    public: static typename self::size_type find_first_not_of_space(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        if (in_string != nullptr && 0 < in_length)
        {
            auto const local_end(in_string + in_length);
            for (auto i(in_string); i < local_end; ++i)
            {
                if (' ' < *i)
                {
                    return i - in_string;
                }
            }
        }
        return self::npos;
    }

    /** @brief �󔒂ł͂Ȃ��������Ō�ɂ���ʒu����������B
        @param[in] in_string ������̐擪�ʒu�B
        @param[in] in_length ������̒����B
        @retval !=self::npos �󔒂ł͂Ȃ��������Ō�ɂ���ʒu�B
        @retval ==self::npos �����񂪋󂩁A���ׂċ󔒕����������B
     */
    public: static typename self::size_type find_last_not_of_space(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        if (in_string != nullptr && 0 < in_length)
        {
            for (auto i(in_string + in_length - 1); in_string <= i; --i)
            {
                if (' ' < *i)
                {
                    return i - in_string;
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief �����񂩂�󕶎�����������B
        @param[in] in_string ������̐擪�ʒu�Bnullptr�̏ꍇ�́A�󕶎���Ƃ݂Ȃ��B
        @return �󕶎����������ʒu��index�ԍ��B
     */
    public: static typename self::size_type find_null(
        typename self::const_pointer const in_string)
    {
        if (in_string == nullptr)
        {
            return 0;
        }
        return template_char_traits::length(in_string);
    }

    //-------------------------------------------------------------------------
    /** @brief ���������ɂ���B
     */
    public: void clear()
    {
        this->size_ = 0;
    }

    //-------------------------------------------------------------------------
    /** @brief �����ȕ����񂩔��肷��B
        @param[in] in_right_begin  ��r���镶����̐擪�ʒu�B
        @param[in] in_right_length ��r���镶����̕������B
        @retval true  �����ȕ����񂾂����B
        @retval false �����ȕ�����ł͂Ȃ������B
     */
    protected: bool is_equal(
        typename self::const_pointer const in_right_begin,
        typename self::size_type const     in_right_length)
    const
    {
        if (this->length() != in_right_length)
        {
            return false;
        }
        if (this->data() == in_right_begin)
        {
            return true;
        }
        int const local_compare(
            template_char_traits::compare(
                this->data(), in_right_begin, in_right_length));
        return local_compare == 0;
    }

    /** @brief ����pointer���擾����B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string ������B
        @param[in] in_offset ������̊J�noffset�ʒu�B
        @return
            �J�noffset�ʒu�ɂ��镶���ւ�pointer�B
            �J�noffset�ʒu���������𒴂��Ă���ꍇ�́A�Ō�̕����ւ�pointer�B
     */
    private: template<typename template_string_type>
    static typename template_string_type::const_pointer get_pointer(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset)
    {
        auto local_offset(
            self::convert_count<template_string_type>(in_offset));
        auto const local_length(in_string.length());
        if (local_length <= local_offset)
        {
            PSYQ_ASSERT(0 < local_length);
            local_offset = local_length - 1;
        }
        return in_string.data() + local_offset;
    }

    /** @brief ��������trimming����B
        @tparam template_string_type @copydoc string_interface
        @param[in] in_string ������B
        @param[in] in_offset ������̊J�noffset�ʒu�B
        @param[in] in_count  ������̊J�noffset�ʒu����̕������B
        @return ������S�̂̕������Ɏ��܂�悤��in_count��trimming�����l�B
     */
    protected: template<typename template_string_type>
    static typename template_string_type::size_type trim_count(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset,
        typename template_string_type::size_type const in_count)
    {
        return self::trim_count(
            in_string.length(),
            self::convert_count<template_string_type>(in_offset),
            self::convert_count<template_string_type>(in_count));
    }

    /** @brief ��������trimming����B
        @param[in] in_length ������S�̂̕������B
        @param[in] in_offset ������̊J�noffset�ʒu�B
        @param[in] in_count  ������̊J�noffset�ʒu����̕������B
        @return ������S�̂̕������Ɏ��܂�悤��in_count��trimming�����l�B
     */
    protected: static typename self::size_type trim_count(
        typename self::size_type const in_length,
        typename self::size_type const in_offset,
        typename self::size_type const in_count)
    {
        return in_length < in_offset?
            0: (std::min)(in_count, in_length - in_offset);
    }

    private: template<typename template_string_type>
    static typename template_string_type::size_type convert_count(
        typename template_string_type::size_type const in_count)
    {
        if (in_count != template_string_type::npos)
        {
            return in_count;
        }
        return (std::numeric_limits<typename template_string_type::size_type>::max)();
    }

    //-------------------------------------------------------------------------
    /// �����Ȉʒu��\���B find() �ȂǂŎg����B
    public: static typename self::size_type const npos =
        static_cast<typename self::size_type>(-1);

    //-------------------------------------------------------------------------
    private: typename self::const_pointer data_;   ///< ������̐擪�ʒu�B
    private: typename self::size_type     length_; ///< ������̒����B

    /** @page string_interface

        ����������^�B������̐擪���疖���܂ł�memory�A�������K�{�B

        ������̐擪�ʒu���擾���邽�߁A�ȉ��̊֐����g���邱�ƁB
        @code
        psyq::basic_reference_string::const_pointer template_string_type::data() const
        @endcode

        ������̒������擾���邽�߁A�ȉ��̊֐����g���邱�ƁB
        @code
        template_string_type::size_type template_string_type::length() const
        @endcode
     */
};

//-----------------------------------------------------------------------------
/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� == �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator==(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right.operator==(in_left);
}

/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� != �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator!=(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right.operator!=(in_left);
}

/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� < �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator<(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right.operator>(in_left);
}

/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� <= �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator<=(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right.operator>=(in_left);
}

/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� > �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator>(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right.operator<(in_left);
}

/** @brief ��������r����B
    @tparam template_string_type @copydoc string_interface
    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� >= �E��
 */
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator>=(
    template_string_type const& in_left,
    psyq::basic_reference_string<
        template_char_type, template_char_traits> const&
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
        @param[in] in_left  �������镶����B
        @param[in] in_right �������镶����B
     */
    template<typename template_char_type, typename template_char_traits>
    void swap(
        psyq::basic_reference_string<template_char_type, template_char_traits>&
            io_left,
        psyq::basic_reference_string<template_char_type, template_char_traits>&
            io_right)
    {
        io_left.swap(io_right);
    }
};

#endif // !PSYQ_REFERENCE_STRING_HPP_
