#ifndef PSYQ_CONST_STRING_HPP_
#define PSYQ_CONST_STRING_HPP_

#include <iosfwd>
#include <iterator>

/// @cond
namespace psyq
{
    template<
        typename template_char_type,
        typename template_char_traits = std::char_traits<template_char_type>>
            class basic_const_string;
    typedef psyq::basic_const_string<char> const_string;
    typedef psyq::basic_const_string<wchar_t> const_wstring;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string �ɏ�������������萔�B
    @param template_char_type   @copydoc basic_const_string::value_type
    @param template_char_traits @copydoc basic_const_string::traits_type
 */
template<typename template_char_type, typename template_char_traits>
class psyq::basic_const_string
{
    /// this�̎w���l�̌^�B
    public: typedef psyq::basic_const_string<
        template_char_type, template_char_traits> self;

    //-------------------------------------------------------------------------
    /// �����̌^�B
    public: typedef template_char_type value_type;

    /// ���������̌^�B
    public: typedef template_char_traits traits_type;

    /// �������̌^�B
    public: typedef std::size_t size_type;

    /// �����萔�ւ�pointer�B
    public: typedef template_char_type const* const_pointer;

    /// �����萔�ւ�pointer�B
    public: typedef typename self::const_pointer pointer;

    /// �����萔�ւ̎Q�ƁB
    public: typedef template_char_type const& const_reference;

    /// �����萔�ւ̎Q�ƁB
    public: typedef typename self::const_reference reference;

    /// �����萔�̔����q�B
    public: typedef typename self::const_pointer const_iterator;

    /// �����萔�̔����q�B
    public: typedef typename self::const_iterator iterator;

    /// �����萔�̋t�����q�B
    public: typedef std::reverse_iterator<const_iterator>
        const_reverse_iterator;

    /// �����萔�̋t�����q�B
    public: typedef typename self::const_reverse_iterator
        reverse_iterator;

    //-------------------------------------------------------------------------
    public: basic_const_string():
    data_(NULL),
    length_(0)
    {
        // pass
    }

    public: basic_const_string(template_char_type const (&in_string)[1])
    {
        PSYQ_ASSERT(0 == in_string[0]);
        new(this) self();
    }

    /** @tparam    t_length ������literal�̗v�f���B
        @param[in] in_string ������literal�B
     */
    public: template <std::size_t t_length>
    basic_const_string(template_char_type const (&in_string)[t_length])
    {
        PSYQ_ASSERT(0 < t_length && 0 == in_string[t_length - 1]);
        this->data_ = &in_string[0];
        this->length_ = t_length - 1;
    }

    /** @param[in] in_string ���蓖�Ă镶����̐擪�ʒu�B
        @param[in] in_length ���蓖�Ă镶����̒����B
     */
    public: basic_const_string(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length):
    data_(in_string),
    length_(NULL != in_string? in_length: 0)
    {
        PSYQ_ASSERT(NULL != in_string || 0 == in_length);
    }

    /** @param[in] in_begin ���蓖�Ă镶����̐擪�ʒu�B
        @param[in] in_end   ���蓖�Ă镶����̖����ʒu�B
     */
    public: basic_const_string(
        typename self::const_pointer const in_begin,
        typename self::const_pointer const in_end)
    {
        PSYQ_ASSERT(in_begin <= in_end);
        new(this) self(in_begin, std::distance(in_begin, in_end));
    }

    /** @param[in] in_string ���蓖�Ă镶����B
        @param[in] in_offset ������̊J�n�ʒu�B
        @param[in] in_count  �������B
     */
    public: basic_const_string(
        self const&                    in_string,
        typename self::size_type const in_offset = 0,
        typename self::size_type const in_count = self::npos):
    data_(in_string.data() + in_offset),
    length_(in_string.trim_length(in_offset, in_count))
    {
        // pass
    }

    //-------------------------------------------------------------------------
    /** @brief ������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_target)
    {
        std::swap(this->data_, io_target.data_);
        std::swap(this->length_, io_target.length_);
    }

    //-------------------------------------------------------------------------
    /** @brief ������̐擪�ʒu���擾�B
        @return ������̐擪�ʒu�B
     */
    public: typename self::const_pointer data() const
    {
        return this->data_;
    }

    /** @brief ������̐擪�ʒu���擾�B
        @return ������̐擪�ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator begin() const
    {
        return this->data();
    }

    /** @brief ������̖����ʒu���擾�B
        @return ������̖����ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator end() const
    {
        return this->begin() + this->length();
    }

    /** @brief ������̐擪�ʒu���擾�B
        @return ������̐擪�ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator cbegin() const
    {
        return this->begin();
    }

    /** @brief ������̖����ʒu���擾�B
        @return ������̖����ʒu�ւ̔����q�B
     */
    public: typename self::const_iterator cend() const
    {
        return this->end();
    }

    /** @brief ������̖����ʒu���擾�B
        @return ������̖����ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator rbegin() const
    {
        return typename self::const_reverse_iterator(this->end());
    }

    /** @brief ������̐擪�ʒu���擾�B
        @return ������̐擪�ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator rend() const
    {
        return typename self::const_reverse_iterator(this->begin());
    }

    /** @brief ������̖����ʒu���擾�B
        @return ������̖����ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator crbegin() const
    {
        return this->rbegin();
    }

    /** @brief ������̐擪�ʒu���擾�B
        @return ������̐擪�ʒu�ւ̋t�����q�B
     */
    public: typename self::const_reverse_iterator crend() const
    {
        return this->rend();
    }

    /** @brief ������̐擪�������Q�ƁB
        @return ������̐擪�����ւ̎Q�ƁB
     */
    typename self::const_reference front() const
    {
        return (*this)[0];
    }

    /** @brief ������̖����������Q�ƁB
        @return ������̖��������ւ̎Q�ƁB
     */
    public: typename self::const_reference back() const
    {
        return (*this)[this->length() - 1];
    }

    /** @brief ������̒������擾�B
        @return ������̒����B
     */
    public: typename self::size_type length() const
    {
        return this->length_;
    }

    /** @brief ������̒������擾�B
        @return ������̒����B
     */
    public: typename self::size_type size() const
    {
        return this->length();
    }

    /** @brief ������̍ő咷���擾�B
        @return ������̍ő咷�B
            ������̉��H���ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
     */
    public: typename self::size_type max_size() const
    {
        return this->length();
    }

    /** @brief ������̗e�ʂ��擾�B
        @return ������̗e�ʁB
           ������̉��H���ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
     */
    public: typename self::size_type capacity() const
    {
        return this->length();
    }

    /** @brief �����񂪋󂩔���B
        @retval true  ������͋�B
        @retval false ������͋�ł͂Ȃ��B
     */
    public: bool empty() const
    {
        return this->length() <= 0;
    }

    //-------------------------------------------------------------------------
    /** @brief �����񂪎��������Q�ƁB
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

    /** @brief �����񂪎��������Q�ƁB
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
    public: bool operator==(self const& in_right) const
    {
        return this->operator==<self>(in_right);
    }

    public: template<typename template_string_type>
    bool operator==(template_string_type const& in_right) const
    {
        return this->length() == in_right.length()
            && 0 == template_char_traits::compare(
                this->data(), in_right.data(), this->length());
    }

    public: bool operator!=(self const& in_right) const
    {
        return !this->operator==(in_right);
    }

    public: template<typename template_string_type>
    bool operator!=(template_string_type const& in_right) const
    {
        return !this->operator==(in_right);
    }

    public: bool operator<(self const& in_right) const
    {
        return this->compare(in_right) < 0;
    }

    public: template<typename template_string_type>
    bool operator<(template_string_type const& in_right) const
    {
        return this->compare(in_right) < 0;
    }

    public: bool operator<=(self const& in_right) const
    {
        return this->compare(in_right) <= 0;
    }

    public: template<typename template_string_type>
    bool operator<=(template_string_type const& in_right) const
    {
        return this->compare(in_right) <= 0;
    }

    public: bool operator>(self const& in_right) const
    {
        return 0 < this->compare(in_right);
    }

    public: template<typename template_string_type>
    bool operator>(template_string_type const& in_right) const
    {
        return 0 < this->compare(in_right);
    }

    public: bool operator>=(self const& in_right) const
    {
        return 0 <= this->compare(in_right);
    }

    public: template<typename template_string_type>
    bool operator>=(template_string_type const& in_right) const
    {
        return 0 <= this->compare(in_right);
    }

    //-------------------------------------------------------------------------
    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����̐擪�ʒu�B�K��NULL�����ŏI���B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(typename self::const_pointer const in_right) const
    {
        return this->compare(0, this->length(), in_right);
    }

    /** @brief ��������r�B
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

    /** @brief ��������r�B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right       �E�ӂ̕�����̐擪�ʒu�B�K��NULL�����ŏI���B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        typename self::size_type const     in_left_offset,
        typename self::size_type const     in_left_count,
        typename self::const_pointer const in_right)
    const
    {
        return this->compare(
            in_left_offset,
            in_left_count,
            in_right,
            self::find_null(in_right));
    }

    /** @brief ��������r�B
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
        template_string_type const&                     in_right)
    const
    {
        return this->compare(
            in_left_offset, in_left_count, in_right.data(), in_right.length());
    }

    /** @brief ��������r�B
        @param[in] in_left_offset  ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count   ���ӂ̕�����̕������B
        @param[in] in_right        �E�ӂ̕�����̐擪�ʒu�B
        @param[in] in_right_length �E�ӂ̕�����̒����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        typename self::size_type const     in_left_offset,
        typename self::size_type const     in_left_count,
        typename self::const_pointer const in_right,
        typename self::size_type const     in_right_length)
    const
    {
        std::size_t const a_left_length(
            this->trim_length(in_left_offset, in_left_count));
        bool const a_less(a_left_length < in_right_length);
        int const a_compare(
            template_char_traits::compare(
                this->data() + in_left_offset,
                in_right,
                a_less? a_left_length: in_right_length));
        if (0 != a_compare)
        {
            return a_compare;
        }
        if (a_less)
        {
            return -1;
        }
        return in_right_length < a_left_length? 1: 0;
    }

    /** @brief ��������r�B
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
        template_string_type const&                     in_right,
        typename self::size_type const in_right_offset,
        typename self::size_type const in_right_count)
    const
    {
        return this->compare(
            in_left_offset,
            in_left_count,
            in_right.data() + in_right_offset,
            in_right.trim_length(in_right_offset, in_right_count));
    }

    //-------------------------------------------------------------------------
    /** @brief �����������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = 0)
    const
    {
        if (in_offset < this->length())
        {
            typename self::const_pointer const a_find(
                template_char_traits::find(
                    this->data() + in_offset,
                    this->length() - in_offset,
                    in_char));
            if (NULL != a_find)
            {
                return a_find - this->data();
            }
        }
        return self::npos;
    }

    /** @brief ������������B
        @param[in] in_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = 0)
    const
    {
        return this->find(in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ������������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find(
        template_string_type const&               in_string,
        typename self::size_type in_offset = 0)
    const
    {
        return this->find(in_string.data(), in_offset, in_string.length());
    }

    /** @brief ������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
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
        PSYQ_ASSERT(NULL != in_string);

        typename self::size_type a_rest_length(
            this->length() - in_offset);
        if (in_offset < this->length() && in_length <= a_rest_length)
        {
            a_rest_length -= in_length - 1;
            typename self::const_pointer a_rest_string(
                this->data() + in_offset);
            for (;;)
            {
                // ����������̐擪�����ƍ��v����ʒu��������B
                typename self::const_pointer const a_find(
                    template_char_traits::find(a_rest_string, a_rest_length, *in_string));
                if (NULL == a_find)
                {
                    break;
                }

                // ����������ƍ��v���邩����B
                if (0 == template_char_traits::compare(a_find, in_string, in_length))
                {
                    return a_find - this->data();
                }

                // ���̌��ցB
                a_rest_length -= a_find + 1 - a_rest_string;
                a_rest_string = a_find + 1;
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ��납�當���������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type rfind(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = self::npos)
    const
    {
        if (!this->empty())
        {
            typename self::const_pointer i(this->trim_pointer(in_offset));
            for (;; --i)
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

    /** @brief ��납�當����������B
        @param[in] in_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type rfind(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = self::npos)
    const
    {
        return this->rfind(in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ��납�當����������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type rfind(
        template_string_type const&                     in_string,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->rfind(in_string.data(), in_offset, in_string.length());
    }

    /** @brief ��납�當����������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
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
        PSYQ_ASSERT(NULL != in_string);

        if (in_length <= this->length())
        {
            typename self::size_type const a_rest(
                this->length() - in_length);
            typename self::const_pointer i(
                this->data() + (in_offset < a_rest? in_offset: a_rest));
            for (;; --i)
            {
                if (template_char_traits::eq(*i, *in_string)
                    && 0 == template_char_traits::compare(i, in_string, in_length))
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
    /** @brief �����������B
        @param[in] in_char   �������镶���B
        @param[in] in_offset �������J�n����ʒu�B
        @return �����������������ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_of(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = 0)
    const
    {
        return this->find(in_char, in_offset);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
        @param[in] in_string ����������̐擪�ʒu�B�K��NULL�ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = 0)
    const
    {
        return this->find_first_of(
            in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_first_of(
        template_string_type const&                     in_string,
        typename self::size_type const in_offset = 0)
    const
    {
        return this->find_first_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || NULL != in_string);
        if (0 < in_length && in_offset < this->length())
        {
            typename self::const_pointer const a_end(
                this->data() + this->length());
            typename self::const_pointer i(this->data() + in_offset);
            for (; i < a_end; ++i)
            {
                if (NULL != template_char_traits::find(in_string, in_length, *i))
                {
                    return i - this->data();
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ��������납�猟���B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_of(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->rfind(in_char, in_offset);
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
        @param[in] in_string ����������B�K��NULL�����ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = self::npos)
    const
    {
        return this->find_last_of(
            in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: template<typename template_string_type>
    typename self::size_type find_last_of(
        template_string_type const&                     in_string,
        typename self::size_type const in_offset = self::npos)
    const
    {
        return this->find_last_of(
            in_string.data(), in_offset, in_string.length());
    }

    /** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || NULL != in_string);
        if (0 < in_length && 0 < this->length())
        {
            typename self::const_pointer i(this->trim_pointer(in_offset));
            for (;; --i)
            {
                if (NULL != template_char_traits::find(in_string, in_length, *i))
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
    /** @brief ���������ȊO�̕����������B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_not_of(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = 0)
    const
    {
        typename self::const_pointer const a_end(
            this->data() + this->length());
        typename self::const_pointer i(this->data() + in_offset);
        for (; i < a_end; ++i)
        {
            if (!template_char_traits::eq(*i, in_char))
            {
                return i - this->data();
            }
        }
        return self::npos;
    }

    /** @brief ����������Ɋ܂܂�Ȃ������������B
        @param[in] in_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = 0)
    const
    {
        return this->find_first_not_of(
            in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ����������Ɋ܂܂�Ȃ������������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
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

    /** @brief ����������Ɋ܂܂�Ȃ������������B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_first_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || NULL != in_string);
        if (in_offset < this->length())
        {
            typename self::const_pointer const a_end(
                this->data() + this->length());
            typename self::const_pointer i(this->data() + in_offset);
            for (; i < a_end; ++i)
            {
                if (NULL == template_char_traits::find(in_string, in_length, *i))
                {
                    return i - this->data();
                }
            }
        }
        return self::npos;
    }

    //-------------------------------------------------------------------------
    /** @brief ���������ȊO�̕������A��납�猟���B
        @param[in] in_char   ���������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_not_of(
        template_char_type const                       in_char,
        typename self::size_type const in_offset = self::npos)
    const
    {
        if (!this->empty())
        {
            typename self::const_pointer i(this->trim_pointer(in_offset));
            for (;; --i)
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

    /** @brief ����������Ɋ܂܂�Ȃ������������B
        @param[in] in_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset = self::npos)
    const
    {
        return this->find_last_not_of(
            in_string, in_offset, self::find_null(in_string));
    }

    /** @brief ����������Ɋ܂܂�Ȃ������������B
        @param[in] in_string ����������B
        @param[in] in_offset �������J�n����ʒu�B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
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

    /** @brief ����������Ɋ܂܂�Ȃ��������A��납�猟���B
        @param[in] in_string ����������̐擪�ʒu�B
        @param[in] in_offset �������J�n����ʒu�B
        @param[in] in_length ����������̒����B
        @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
     */
    public: typename self::size_type find_last_not_of(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_offset,
        typename self::size_type const     in_length)
    const
    {
        PSYQ_ASSERT(in_length <= 0 || NULL != in_string);
        if (!this->empty())
        {
            typename self::const_pointer i(this->trim_pointer(in_offset));
            for (;; --i)
            {
                if (NULL == template_char_traits::find(in_string, in_length, *i))
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
    /** @brief ����������蓖�Ă�B
        @param[in] in_string ���蓖�Ă镶����̐擪�ʒu�B�K��NULL�����ŏI���B
     */
    public: self& assign(typename self::const_pointer const in_string)
    {
        return *new(this) self(in_string);
    }

    /** @brief ����������蓖�Ă�B
        @param[in] in_string ���蓖�Ă镶����̐擪�ʒu�B
        @param[in] in_length ���蓖�Ă镶����̒����B
     */
    public: self& assign(
        typename self::const_pointer const in_string,
        typename self::size_type const     in_length)
    {
        return *new(this) self(in_string, in_length);
    }

    /** @brief ����������蓖�Ă�B
        @param[in] in_begin ���蓖�Ă镶����̐擪�ʒu�B
        @param[in] in_end   ���蓖�Ă镶����̖����ʒu�B
     */
    public: self& assign(
        typename self::const_pointer const in_begin,
        typename self::const_pointer const in_end)
    {
        return *new(this) self(in_begin, in_end);
    }

    /** @brief ����������蓖�Ă�B
        @param[in] in_string ���蓖�Ă镶����B
        @param[in] in_offset ������̊J�n�ʒu�B
        @param[in] in_count  �������B
     */
    public: self& assign(
        self const&                    in_string,
        typename self::size_type const in_offset = 0,
        typename self::size_type const in_count = self::npos)
    {
        return *new(this) self(in_string, in_offset, in_count);
    }

    //-------------------------------------------------------------------------
    /** @brief ������������\�z�B
        @param[in] in_offset ������̊J�n�ʒu�B
        @param[in] in_count  �������B
        @return �\�z��������������B
     */
    public: self substr(
        typename self::size_type const in_offset = 0,
        typename self::size_type const in_count = self::npos)
    const
    {
        return this->substr<self>(in_offset, in_count);
    }

    /** @brief ������������\�z�B
        @tparam template_string_type �\�z���镶����̌^�B
        @param[in] in_offset ������̊J�n�ʒu�B
        @param[in] in_count  �������B
        @return �\�z��������������B
     */
    public: template<typename template_string_type>
    template_string_type substr(
        typename self::size_type const in_offset = 0,
        typename self::size_type const in_count = self::npos)
    const
    {
        return template_string_type(
            this->data() + in_offset, this->trim_length(in_offset, in_count));
    }

    //-------------------------------------------------------------------------
    /** @brief ���������ɂ���B
     */
    public: void clear()
    {
        new(this) self();
    }

    //-------------------------------------------------------------------------
    private: typename self::const_pointer trim_pointer(
        typename self::size_type const in_offset)
    const
    {
        return this->data() + (
            in_offset < this->length()? in_offset: this->length() - 1);
    }

    private: typename self::size_type trim_length(
        typename self::size_type const in_offset,
        typename self::size_type const in_count)
    const
    {
        if (this->length() < in_offset)
        {
            PSYQ_ASSERT(false);
            return 0;
        }
        typename self::size_type const a_limit(this->length() - in_offset);
        return in_count < a_limit? in_count: a_limit;
    }

    private: static typename self::size_type find_null(
        typename self::const_pointer const in_string)
    {
        return NULL != in_string? template_char_traits::length(in_string): 0;
    }

    //-------------------------------------------------------------------------
    public: static typename self::size_type const npos =
        static_cast<typename self::size_type>(-1);

    //-------------------------------------------------------------------------
    private: template_char_type const*                data_;   ///< ������̐擪�ʒu�B
    private: typename self::size_type length_; ///< �������B
};

//.............................................................................
template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator==(
    template_string_type const& in_left,
    psyq::basic_const_string<
        template_char_type, template_char_traits> const&
            in_right)
{
    return in_right == in_left;
}

template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator!=(
    template_string_type const& in_left,
    psyq::basic_const_string<template_char_type, template_char_traits> const&
        in_right)
{
    return in_right != in_left;
}

template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator<(
    template_string_type const& in_left,
    psyq::basic_const_string<template_char_type, template_char_traits> const&
        in_right)
{
    return in_right > in_left;
}

template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator<=(
    template_string_type const& in_left,
    psyq::basic_const_string<template_char_type, template_char_traits> const&
        in_right)
{
    return in_right >= in_left;
}

template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator>(
    template_string_type const& in_left,
    psyq::basic_const_string<template_char_type, template_char_traits> const&
        in_right)
{
    return in_right < in_left;
}

template<
    typename template_string_type,
    typename template_char_type,
    typename template_char_traits>
bool operator>=(
    template_string_type const& in_left,
    psyq::basic_const_string<template_char_type, template_char_traits> const&
        in_right)
{
    return in_right <= in_left;
}

namespace std
{
    template<typename template_char_type, typename template_char_traits>
    void swap(
        psyq::basic_const_string<template_char_type, template_char_traits>&
            io_left,
        psyq::basic_const_string<template_char_type, template_char_traits>&
            io_right)
    {
        io_left.swap(io_right);
    }
};

#endif // PSYQ_CONST_STRING_HPP_
