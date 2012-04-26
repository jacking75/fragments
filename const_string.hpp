#ifndef PSYQ_CONST_STRING_HPP_
#define PSYQ_CONST_STRING_HPP_

#include <iosfwd>
#include <iterator>

namespace psyq
{
	template<
		typename t_value, typename t_traits = std::char_traits< t_value > >
			class basic_const_string;
	typedef psyq::basic_const_string< char > const_string;
	typedef psyq::basic_const_string< wchar_t > const_wstring;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string�ɏ�������������萔�B
    @param t_value  �����̌^�B
    @param t_traits ���������̌^�B
 */
template< typename t_value, typename t_traits >
class psyq::basic_const_string
{
	typedef psyq::basic_const_string< t_value, t_traits > this_type;

//.............................................................................
public:
	typedef t_value value_type;
	typedef t_traits traits_type;
	typedef std::size_t size_type;
	typedef t_value const* const_pointer;
	typedef typename this_type::const_pointer pointer;
	typedef t_value const& const_reference;
	typedef typename this_type::const_reference reference;
	typedef typename this_type::const_pointer const_iterator;
	typedef typename this_type::const_iterator iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
	typedef typename this_type::const_reverse_iterator reverse_iterator;

	//-------------------------------------------------------------------------
	/** @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B�K��NULL�����ŏI���B
	 */
	basic_const_string(
		typename this_type::const_pointer const i_string = NULL):
	data_(i_string),
	length_(NULL != i_string? t_traits::length(i_string): 0)
	{
		// pass
	}

	/** @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	 */
	basic_const_string(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset = 0)
	{
		new(this) this_type(i_string, i_offset, i_string.length() - i_offset);
	}

	/** @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_length �������B
	 */
	basic_const_string(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length):
	data_(i_string),
	length_(this_type::trim_length(i_string, i_length))
	{
		// pass
	}

	/** @param[in] i_begin ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_end   ���蓖�Ă镶����̖����ʒu�B
	 */
	basic_const_string(
		typename this_type::const_iterator const i_begin,
		typename this_type::const_iterator const i_end):
	data_(i_begin),
	length_(std::distance(i_begin, i_end))
	{
		PSYQ_ASSERT(i_begin <= i_end);
	}

	/** @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	 */
	basic_const_string(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count):
	data_(i_string.data() + i_offset),
	length_(this_type::trim_length(i_offset, i_count, i_string.length()))
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief ������̐擪�̕����ւ�pointer���擾�B
	    @return �擪�����ւ�pointer�B
	 */
	typename this_type::const_pointer data() const
	{
		return this->data_;
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�B
	 */
	typename this_type::const_iterator begin() const
	{
		return this->data();
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�B
	 */
	typename this_type::const_iterator end() const
	{
		return this->begin() + this->length();
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�B
	 */
	typename this_type::const_iterator cbegin() const
	{
		return this->begin();
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�B
	 */
	typename this_type::const_iterator cend() const
	{
		return this->end();
	}

	/** @brief ������̋t�擪�ʒu���擾�B
	    @return ������̋t�擪�ʒu�B
	 */
	typename this_type::const_reverse_iterator rbegin() const
	{
		return typename this_type::const_reverse_iterator(this->end());
	}

	/** @brief ������̋t�����ʒu���擾�B
	    @return ������̋t�����ʒu�B
	 */
	typename this_type::const_reverse_iterator rend() const
	{
		return typename this_type::const_reverse_iterator(this->begin());
	}

	/** @brief ������̋t�擪�ʒu���擾�B
	    @return ������̋t�擪�ʒu�B
	 */
	typename this_type::const_reverse_iterator crbegin() const
	{
		return this->rbegin();
	}

	/** @brief ������̋t�����ʒu���擾�B
	    @return ������̋t�����ʒu�B
	 */
	typename this_type::const_reverse_iterator crend() const
	{
		return this->rend();
	}

	/** @brief ������̐擪�������Q�ƁB
	    @return ������̐擪�����ւ̎Q�ƁB
	 */
	typename this_type::const_reference front() const
	{
		return (*this)[0];
	}

	/** @brief ������̖����������Q�ƁB
	    @return ������̖��������ւ̎Q�ƁB
	 */
	typename this_type::const_reference back() const
	{
		return (*this)[this->length() - 1];
	}

	/** @brief ������̒������擾�B
	    @return ������̒����B
	 */
	typename this_type::size_type length() const
	{
		return this->length_;
	}

	/** @brief ������̒������擾�B
	    @return ������̒����B
	 */
	typename this_type::size_type size() const
	{
		return this->length();
	}

	typename this_type::size_type max_size() const
	{
		return this->length();
	}

	typename this_type::size_type capacity() const
	{
		return this->length();
	}

	/** @brief �����񂪋󂩔���B
	    @retval true  ������͋�B
	    @retval false ������͋�ł͂Ȃ��B
	 */
	bool empty() const
	{
		return this->length() <= 0;
	}

	//-------------------------------------------------------------------------
	typename this_type::const_reference at(
		typename this_type::size_type const i_index)
	const
	{
		if (this->length() <= i_index)
		{
			PSYQ_ASSERT(false);
			//throw std::out_of_range; // ��O�͎g�������Ȃ��B
		}
		return *(this->data() + i_index);
	}

	typename this_type::const_reference operator[](
		typename this_type::size_type const i_index)
	const
	{
		PSYQ_ASSERT(i_index < this->length());
		return *(this->data() + i_index);
	}

	//-------------------------------------------------------------------------
	bool operator==(this_type const& i_right) const
	{
		return this->operator==< this_type >(i_right);
	}

	template< typename t_string >
	bool operator==(t_string const& i_right) const
	{
		return this->length() == i_right.length()
			&& 0 == t_traits::compare(
				this->data(), i_right.data(), this->length());
	}

	bool operator!=(this_type const& i_right) const
	{
		return !this->operator==(i_right);
	}

	template< typename t_string >
	bool operator!=(t_string const& i_right) const
	{
		return !this->operator==(i_right);
	}

	bool operator<(this_type const& i_right) const
	{
		return this->compare(i_right) < 0;
	}

	template< typename t_string >
	bool operator<(t_string const& i_right) const
	{
		return this->compare(i_right) < 0;
	}

	bool operator<=(this_type const& i_right) const
	{
		return this->compare(i_right) <= 0;
	}

	template< typename t_string >
	bool operator<=(t_string const& i_right) const
	{
		return this->compare(i_right) <= 0;
	}

	bool operator>(this_type const& i_right) const
	{
		return 0 < this->compare(i_right);
	}

	template< typename t_string >
	bool operator>(t_string const& i_right) const
	{
		return 0 < this->compare(i_right);
	}

	bool operator>=(this_type const& i_right) const
	{
		return 0 <= this->compare(i_right);
	}

	template< typename t_string >
	bool operator>=(t_string const& i_right) const
	{
		return 0 <= this->compare(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief ��������r�B
	    @param[in] i_right �E�ӂ̕�����̐擪�ʒu�B�K��NULL�����ŏI���B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	int compare(typename this_type::const_pointer const i_right) const
	{
		return this->compare(0, this->length(), i_right);
	}

	/** @brief ��������r�B
	    @param[in] i_right �E�ӂ̕�����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	template< typename t_string >
	int compare(t_string const& i_right) const
	{
		return this->compare(
			0, this->length(), i_right.data(), i_right.length());
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count  ���ӂ̕�����̕������B
	    @param[in] i_right       �E�ӂ̕�����̐擪�ʒu�B�K��NULL�����ŏI���B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	int compare(
		typename this_type::size_type const     i_left_offset,
		typename this_type::size_type const     i_left_count,
		typename this_type::const_pointer const i_right)
	const
	{
		return this->compare(
			i_left_offset,
			i_left_count,
			i_right,
			NULL != i_right? t_traits::length(i_right): 0);
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count  ���ӂ̕�����̕������B
	    @param[in] i_right       �E�ӂ̕�����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	template< typename t_string >
	int compare(
		typename this_type::size_type const i_left_offset,
		typename this_type::size_type const i_left_count,
		t_string const&                     i_right)
	const
	{
		return this->compare(
			i_left_offset, i_left_count, i_right.data(), i_right.length());
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset  ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count   ���ӂ̕�����̕������B
	    @param[in] i_right_string �E�ӂ̕�����̐擪�ʒu�B
	    @param[in] i_right_length �E�ӂ̕�����̒����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	int compare(
		typename this_type::size_type const     i_left_offset,
		typename this_type::size_type const     i_left_count,
		typename this_type::const_pointer const i_right_string,
		typename this_type::size_type const     i_right_length)
	const
	{
		std::size_t const a_left_length(
			this_type::trim_length(
				i_left_offset, i_left_count, this->length()));
		int const a_result(
			t_traits::compare(
				this->data() + i_left_offset,
				i_right_string,
				a_left_length < i_right_length?
					a_left_length: i_right_length));
		return 0 != a_result?
			a_result:
			a_left_length < i_right_length?
				-1:
				i_right_length < a_left_length? 1: 0;
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset  ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count   ���ӂ̕�����̕������B
	    @param[in] i_right        �E�ӂ̕�����B
	    @param[in] i_right_offset ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_right_count  �E�ӂ̕�����̕������B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval  0 ���ӂƉE�ӂ͓����B
	 */
	template< typename t_string >
	int compare(
		typename this_type::size_type const i_left_offset,
		typename this_type::size_type const i_left_count,
		t_string const&                     i_right,
		typename this_type::size_type const i_right_offset,
		typename this_type::size_type const i_right_count)
	const
	{
		return this->compare(
			i_left_offset,
			i_left_count,
			i_right.data() + i_right_offset,
			this_type::trim_length(
				i_right_offset, i_right_count, i_right.length()));
	}

	//-------------------------------------------------------------------------
	/** @brief �����������B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type find(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = 0)
	const
	{
		if (i_offset < this->length())
		{
			typename this_type::const_pointer const a_find(
				this_type::traits_type::find(
					this->data() + i_offset,
					this->length() - i_offset,
					i_char));
			if (NULL != a_find)
			{
				return a_find - this->data();
			}
		}
		return this_type::npos;
	}

	/** @brief ������������B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type find(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	/** @brief ������������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	template< typename t_string >
	typename this_type::size_type find(
		t_string const&               i_string,
		typename this_type::size_type i_offset = 0)
	const
	{
		return this->find(i_string.data(), i_offset, i_string.length());
	}

	/** @brief ������������B
	    @param[in] i_begin  ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type find(
		typename this_type::const_pointer const i_begin,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		if (0 < i_length)
		{
			PSYQ_ASSERT(NULL != i_begin);
		}
		else if (i_offset <= this->length())
		{
			return i_offset;
		}

		typename this_type::size_type a_rest_length(
			this->length() - i_offset);
		if (i_offset < this->length() && i_length <= a_rest_length)
		{
			a_rest_length -= i_length - 1;
			typename this_type::const_pointer a_rest_string(
				this->data() + i_offset);
			for (;;)
			{
				// ����������̐擪�����ƍ��v����ʒu��������B
				typename this_type::const_pointer const a_find(
					this_type::traits_type::find(
						a_rest_string, a_rest_length, *i_begin));
				if (NULL == a_find)
				{
					break;
				}

				// ����������ƍ��v���邩����B
				if (0 == this_type::traits_type::compare(a_find, i_begin, i_length))
				{
					return a_find - this->data();
				}

				// ���̌��ցB
				a_rest_length -= a_find + 1 - a_rest_string;
				a_rest_string = a_find + 1;
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	/** @brief ��������납�猟���B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type rfind(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = this_type::npos)
	const
	{
		if (!this->empty())
		{
			typename this_type::size_type a_back(this->length() - 1);
			typename this_type::const_pointer i(
				this->data() + (i_offset < a_back? i_offset: a_back));
			for (; this->data() <= i; --i)
			{
				if (this_type::traits_type::eq(i_char, *i))
				{
					return i - this->data();
				}
			}
		}
		return this_type::npos;
	}

	/** @brief ���������납�猟���B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type rfind(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = this_type::npos)
	const
	{
		return this->rfind(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	/** @brief ���������납�猟���B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	template< typename t_string >
	typename this_type::size_type rfind(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->rfind(i_string.data(), i_offset, i_string.length());
	}

	/** @brief ���������납�猟���B
	    @param[in] i_begin  ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������񂪌��������ʒu�B������Ȃ��ꍇ��npos��Ԃ��B
	 */
	typename this_type::size_type rfind(
		typename this_type::const_pointer const i_begin,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		if (i_length <= 0)
		{
			return i_offset < this->length() ? i_offset: this->length();
		}
		PSYQ_ASSERT(NULL != i_begin);

		if (i_length <= this->length())
		{
			typename this_type::size_type const a_rest(
				this->length() - i_length);
			typename this_type::const_pointer i(
				this->data() + (i_offset < a_rest? i_offset: a_rest));
			for (;; --i)
			{
				if (this_type::traits_type::eq(*i, *i_begin)
					&& 0 == this_type::traits_type::compare(i, i_begin, i_length))
				{
					return i - this->data();
				}
				if (i <= this->data())
				{
					break;
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	typename this_type::size_type find_first_of(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = 0)
	const
	{
		return this->find(i_char, i_offset);
	}

	typename this_type::size_type find_first_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find_first_of(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	template< typename t_string >
	typename this_type::size_type find_first_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = 0)
	const
	{
		return this->find_first_of(
			i_string.data(), i_offset, i_string.length());
	}

	typename this_type::size_type find_first_of(
		typename this_type::const_pointer const i_begin,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_begin);
		if (0 < i_length && i_offset < this->length())
		{
			typename this_type::const_pointer const a_end(
				this->data() + this->length());
			for (
				typename this_type::const_pointer i = this->data() + i_offset;
				i < a_end;
				++i)
			{
				if (NULL != this_type::traits_type::find(i_begin, i_length, *i))
				{
					return i - this->data();
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	typename this_type::size_type find_last_of(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = this_type::npos)
	const
	{
		return this->rfind(i_char, i_offset);
	}

	typename this_type::size_type find_last_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = this_type::npos)
	const
	{
		return this->find_last_of(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	template< typename t_string >
	typename this_type::size_type find_last_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->find_last_of(
			i_string.data(), i_offset, i_string.length());
	}

	typename this_type::size_type find_last_of(
		typename this_type::const_pointer const i_begin,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_begin);
		if (0 < i_length && 0 < this->length())
		{
			typename this_type::const_pointer i(
				this->data() + (
					i_offset < this->length()? i_offset: this->length() - 1));
			for (;; --i)
			{
				if (NULL != this_type::traits_type::find(i_begin, i_length, *i))
				{
					return i - this->data();
				}
				if (i <= this->data())
				{
					break;
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	typename this_type::size_type find_first_not_of(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = 0)
	const
	{
		return this->find_first_not_of(&i_char, i_offset, 1);
	}

	typename this_type::size_type find_first_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find_first_not_of(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	template< typename t_string >
	typename this_type::size_type find_first_not_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = 0)
	const
	{
		return this->find_first_not_of(
			i_string.data(), i_offset, i_string.length());
	}

	typename this_type::size_type find_first_not_of(
		typename this_type::const_pointer const i_begin,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_begin);
		if (i_offset < this->length())
		{
			typename this_type::const_pointer const a_end(
				this->data() + this->length());
			for (
				typename this_type::const_pointer i = this->data() + i_offset;
				i < a_end;
				++i)
			{
				if (NULL == this_type::traits_type::find(i_begin, i_length, *i))
				{
					return i - this->data();
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	typename this_type::size_type find_last_not_of(
		typename this_type::value_type const i_char,
		typename this_type::size_type const  i_offset = this_type::npos)
	const
	{
		return this->find_last_not_of(&i_char, i_offset, 1);
	}

	typename this_type::size_type find_last_not_of(
		typename this_type::const_pointer i_string,
		typename this_type::size_type i_offset = this_type::npos)
	const
	{
		return this->find_last_not_of(
			i_string,
			i_offset,
			NULL != i_string? this_type::traits_type::length(i_string): 0);
	}

	template< typename t_string >
	typename this_type::size_type find_last_not_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->find_last_not_of(
			i_string.data(), i_offset, i_string.length());
	}

	typename this_type::size_type find_last_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_string);
		if (0 < this->length())
		{
			typename this_type::const_pointer i(
				this->data() + (
					i_offset < this->length()? i_offset: this->length() - 1));
			for (;; --i)
			{
				if (NULL == this_type::traits_type::find(i_string, i_length, *i))
				{
					return i - this->data();
				}
				else if (i <= this->data())
				{
					break;
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B�K��NULL�����ŏI���B
	 */
	this_type& assign(typename this_type::const_pointer const i_string)
	{
		return *new(this) this_type(i_string);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	 */
	this_type& assign(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset = 0)
	{
		return *new(this) this_type(i_string, i_offset);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_length �������B
	 */
	this_type& assign(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length)
	{
		return *new(this) this_type(i_string, i_length);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_begin ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_end   ���蓖�Ă镶����̖����ʒu�B
	 */
	this_type& assign(
		typename this_type::const_iterator const i_begin,
		typename this_type::const_iterator const i_end)
	{
		return *new(this) this_type(i_begin, i_end);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	 */
	this_type& assign(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	{
		return *new(this) this_type(i_string, i_offset, i_count);
	}

	//-------------------------------------------------------------------------
	/** @brief ������������\�z�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @return �\�z��������������B
	 */
	this_type substr(typename this_type::size_type const i_offset = 0) const
	{
		return this->substr< this_type >(i_offset);
	}

	/** @brief ������������\�z�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	    @return �\�z��������������B
	 */
	this_type substr(
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	const
	{
		return this->substr< this_type >(i_offset, i_count);
	}

	/** @brief ������������\�z�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @return �\�z��������������B
	 */
	template< typename t_string >
	t_string substr(typename this_type::size_type const i_offset = 0) const
	{
		return this->substr< t_string >(i_offset, this->length() - i_offset);
	}

	/** @brief ������������\�z�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	    @return �\�z��������������B
	 */
	template< typename t_string >
	t_string substr(
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	const
	{
		return t_string(
			this->data() + i_offset,
			this_type::trim_length(i_offset, i_count, this->length()));
	}

	//-------------------------------------------------------------------------
	void clear()
	{
		new(this) this_type();
	}

	void swap(this_type& io_target)
	{
		std::swap(this->data_, io_target.data_);
		std::swap(this->length_, io_target.length_);
	}

//.............................................................................
private:
	static typename this_type::size_type trim_length(
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count,
		typename this_type::size_type const i_length)
	{
		if (i_length < i_offset)
		{
			PSYQ_ASSERT(false);
			return 0;
		}
		typename this_type::size_type const a_limit(i_length - i_offset);
		return i_count <= a_limit? i_count: a_limit;
	}

	static typename this_type::size_type trim_length(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length)
	{
		if (NULL == i_string)
		{
			return 0;
		}
		for (typename this_type::size_type i = 0; i < i_length; ++i)
		{
			if (0 == i_string[i])
			{
				return i;
			}
		}
		return i_length;
	}

//.............................................................................
public:
	static typename this_type::size_type const npos =
		static_cast< typename this_type::size_type >(-1);

private:
	t_value const*                data_;
	typename this_type::size_type length_;
};

//.............................................................................
template< typename t_string, typename t_value, typename t_traits >
bool operator==(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right == i_left;
}

template< typename t_string, typename t_value, typename t_traits >
bool operator!=(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right != i_left;
}

template< typename t_string, typename t_value, typename t_traits >
bool operator<(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right > i_left;
}

template< typename t_string, typename t_value, typename t_traits >
bool operator<=(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right >= i_left;
}

template< typename t_string, typename t_value, typename t_traits >
bool operator>(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right < i_left;
}

template< typename t_string, typename t_value, typename t_traits >
bool operator>=(
	t_string const&                                      i_left,
	psyq::basic_const_string< t_value, t_traits > const& i_right)
{
	return i_right <= i_left;
}

#endif // PSYQ_CONST_STRING_HPP_
