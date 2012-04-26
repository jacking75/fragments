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
/** @brief std::basic_stringに準拠した文字列定数。
    @param t_value  文字の型。
    @param t_traits 文字特性の型。
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

	//-------------------------------------------------------------------------
	/** @param[in] i_string NULL終端文字列の先頭位置。
	 */
	basic_const_string(
		typename this_type::const_pointer const i_string = NULL):
	data_(i_string),
	length_(NULL != i_string? t_traits::length(i_string): 0)
	{
		// pass
	}

	/** @param[in] i_string 基準となる文字列。
	    @param[in] i_offset 文字列の開始位置。
	 */
	basic_const_string(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset = 0)
	{
		new(this) this_type(i_string, i_offset, i_string.length() - i_offset);
	}

	/** @param[in] i_string NULL終端文字列の先頭位置。
	    @param[in] i_length 文字数。
	 */
	basic_const_string(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length):
	data_(i_string),
	length_(this_type::trim_length(i_string, i_length))
	{
		// pass
	}

	/** @param[in] i_begin 文字列の先頭位置。
	    @param[in] i_end   文字列の末尾位置。
	 */
	basic_const_string(
		typename this_type::const_iterator const i_begin,
		typename this_type::const_iterator const i_end):
	data_(i_begin),
	length_(std::distance(i_begin, i_end))
	{
		// pass
	}

	/** @param[in] i_string 基準となる文字列。
	    @param[in] i_offset 文字列の開始位置。
	    @param[in] i_count  文字数。
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
	typename this_type::const_pointer data() const
	{
		return this->data_;
	}

	typename this_type::const_iterator begin() const
	{
		return this->data();
	}

	typename this_type::const_iterator end() const
	{
		return this->begin() + this->length();
	}

	typename this_type::const_iterator cbegin() const
	{
		return this->begin();
	}

	typename this_type::const_iterator cend() const
	{
		return this->end();
	}

	typename this_type::const_reference front() const
	{
		return (*this)[0];
	}

	typename this_type::const_reference back() const
	{
		return (*this)[this->length() - 1];
	}

	typename this_type::size_type length() const
	{
		return this->length_;
	}

	typename this_type::size_type size() const
	{
		return this->length();
	}

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
			//throw std::out_of_range; // 例外は使いたくない。
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
	int compare(typename this_type::const_pointer const i_right_string) const
	{
		return this->compare(0, this->length(), i_right_string);
	}

	template< typename t_string >
	int compare(t_string const& i_right) const
	{
		return this->compare(
			0, this->length(), i_right.data(), i_right.length());
	}

	int compare(
		typename this_type::size_type const     i_left_offset,
		typename this_type::size_type const     i_left_count,
		typename this_type::const_pointer const i_right_string)
	const
	{
		return this->compare(
			i_left_offset,
			i_left_count,
			i_right_string,
			NULL != i_right_string?
				t_traits::length(i_right_string): 0);
	}

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
	this_type& assign(typename this_type::const_pointer const i_string)
	{
		return *new(this) this_type(i_string);
	}

	this_type& assign(this_type const& i_string)
	{
		return *new(this) this_type(i_string);
	}

	this_type& assign(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length)
	{
		return *new(this) this_type(i_string, i_length);
	}

	this_type& assign(
		typename this_type::const_iterator const i_begin,
		typename this_type::const_iterator const i_end)
	{
		return *new(this) this_type(i_begin, i_end);
	}

	this_type& assign(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	{
		return *new(this) this_type(i_string, i_offset, i_count);
	}

	//-------------------------------------------------------------------------
	this_type substr(typename this_type::size_type const i_offset = 0) const
	{
		return this->substr< this_type >(i_offset);
	}

	this_type substr(
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	const
	{
		return this->substr< this_type >(i_offset, i_count);
	}

	template< typename t_string >
	t_string substr(typename this_type::size_type const i_offset = 0) const
	{
		return this->substr< t_string >(i_offset, this->length() - i_offset);
	}

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
