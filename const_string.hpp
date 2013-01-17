#ifndef PSYQ_CONST_STRING_HPP_
#define PSYQ_CONST_STRING_HPP_

#include <iosfwd>
#include <iterator>

/// @cond
namespace psyq
{
	template<
		typename t_value, typename t_traits = std::char_traits< t_value > >
			class basic_const_string;
	typedef psyq::basic_const_string< char > const_string;
	typedef psyq::basic_const_string< wchar_t > const_wstring;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string �ɏ�������������萔�B
    @param t_value  @copydoc basic_const_string::value_type
    @param t_traits @copydoc basic_const_string::traits_type
 */
template< typename t_value, typename t_traits >
class psyq::basic_const_string
{
	/// ����object�̌^�B
	public: typedef psyq::basic_const_string< t_value, t_traits > this_type;

	//-------------------------------------------------------------------------
	/// �����̌^�B
	public: typedef t_value value_type;

	/// ���������̌^�B
	public: typedef t_traits traits_type;

	/// �������̌^�B
	public: typedef std::size_t size_type;

	/// �����萔�ւ�pointer�B
	public: typedef t_value const* const_pointer;

	/// �����萔�ւ�pointer�B
	public: typedef typename this_type::const_pointer pointer;

	/// �����萔�ւ̎Q�ƁB
	public: typedef t_value const& const_reference;

	/// �����萔�ւ̎Q�ƁB
	public: typedef typename this_type::const_reference reference;

	/// �����萔�̔����q�B
	public: typedef typename this_type::const_pointer const_iterator;

	/// �����萔�̔����q�B
	public: typedef typename this_type::const_iterator iterator;

	/// �����萔�̋t�����q�B
	public: typedef std::reverse_iterator< const_iterator >
		const_reverse_iterator;

	/// �����萔�̋t�����q�B
	public: typedef typename this_type::const_reverse_iterator
		reverse_iterator;

	//-------------------------------------------------------------------------
	public: basic_const_string():
	data_(NULL),
	length_(0)
	{
		// pass
	}

	public: basic_const_string(t_value const (&i_string)[1])
	{
		PSYQ_ASSERT(0 == i_string[0]);
		new(this) this_type();
	}

	/** @tparam    t_length ������literal�̗v�f���B
	    @param[in] i_string ������literal�B
	 */
	public: template < std::size_t t_length >
	basic_const_string(t_value const (&i_string)[t_length])
	{
		PSYQ_ASSERT(0 < t_length && 0 == i_string[t_length - 1]);
		this->data_ = &i_string[0];
		this->length_ = t_length - 1;
	}

	/** @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_length ���蓖�Ă镶����̒����B
	 */
	public: basic_const_string(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length):
	data_(i_string),
	length_(NULL != i_string? i_length: 0)
	{
		PSYQ_ASSERT(NULL != i_string || 0 == i_length);
	}

	/** @param[in] i_begin ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_end   ���蓖�Ă镶����̖����ʒu�B
	 */
	public: basic_const_string(
		typename this_type::const_pointer const i_begin,
		typename this_type::const_pointer const i_end)
	{
		PSYQ_ASSERT(i_begin <= i_end);
		new(this) this_type(i_begin, std::distance(i_begin, i_end));
	}

	/** @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	 */
	public: basic_const_string(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset = 0,
		typename this_type::size_type const i_count = this_type::npos):
	data_(i_string.data() + i_offset),
	length_(i_string.trim_length(i_offset, i_count))
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief ������������B
		@param[in,out] io_target �������镶����B
	 */
	public: void swap(this_type& io_target)
	{
		std::swap(this->data_, io_target.data_);
		std::swap(this->length_, io_target.length_);
	}

	//-------------------------------------------------------------------------
	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�B
	 */
	public: typename this_type::const_pointer data() const
	{
		return this->data_;
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�ւ̔����q�B
	 */
	public: typename this_type::const_iterator begin() const
	{
		return this->data();
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�ւ̔����q�B
	 */
	public: typename this_type::const_iterator end() const
	{
		return this->begin() + this->length();
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�ւ̔����q�B
	 */
	public: typename this_type::const_iterator cbegin() const
	{
		return this->begin();
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�ւ̔����q�B
	 */
	public: typename this_type::const_iterator cend() const
	{
		return this->end();
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�ւ̋t�����q�B
	 */
	public: typename this_type::const_reverse_iterator rbegin() const
	{
		return typename this_type::const_reverse_iterator(this->end());
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�ւ̋t�����q�B
	 */
	public: typename this_type::const_reverse_iterator rend() const
	{
		return typename this_type::const_reverse_iterator(this->begin());
	}

	/** @brief ������̖����ʒu���擾�B
	    @return ������̖����ʒu�ւ̋t�����q�B
	 */
	public: typename this_type::const_reverse_iterator crbegin() const
	{
		return this->rbegin();
	}

	/** @brief ������̐擪�ʒu���擾�B
	    @return ������̐擪�ʒu�ւ̋t�����q�B
	 */
	public: typename this_type::const_reverse_iterator crend() const
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
	public: typename this_type::const_reference back() const
	{
		return (*this)[this->length() - 1];
	}

	/** @brief ������̒������擾�B
	    @return ������̒����B
	 */
	public: typename this_type::size_type length() const
	{
		return this->length_;
	}

	/** @brief ������̒������擾�B
	    @return ������̒����B
	 */
	public: typename this_type::size_type size() const
	{
		return this->length();
	}

	/** @brief ������̍ő咷���擾�B
	    @return ������̍ő咷�B
		    ������̉��H���ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
	 */
	public: typename this_type::size_type max_size() const
	{
		return this->length();
	}

	/** @brief ������̗e�ʂ��擾�B
	    @return ������̗e�ʁB
		   ������̉��H���ł��Ȃ��̂ŁA������̒����Ɠ����l�ɂȂ�B
	 */
	public: typename this_type::size_type capacity() const
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
	    @param[in] i_index ������index�ԍ��B
	    @return �����ւ̎Q�ƁB
	 */
	public: typename this_type::const_reference at(
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

	/** @brief �����񂪎��������Q�ƁB
	    @param[in] i_index ������index�ԍ��B
	    @return �����ւ̎Q�ƁB
	 */
   public: typename this_type::const_reference operator[](
		typename this_type::size_type const i_index)
	const
	{
		PSYQ_ASSERT(i_index < this->length());
		return *(this->data() + i_index);
	}

	//-------------------------------------------------------------------------
	public: bool operator==(this_type const& i_right) const
	{
		return this->operator==< this_type >(i_right);
	}

	public: template< typename t_string >
	bool operator==(t_string const& i_right) const
	{
		return this->length() == i_right.length()
			&& 0 == t_traits::compare(
				this->data(), i_right.data(), this->length());
	}

	public: bool operator!=(this_type const& i_right) const
	{
		return !this->operator==(i_right);
	}

	public: template< typename t_string >
	bool operator!=(t_string const& i_right) const
	{
		return !this->operator==(i_right);
	}

	public: bool operator<(this_type const& i_right) const
	{
		return this->compare(i_right) < 0;
	}

	public: template< typename t_string >
	bool operator<(t_string const& i_right) const
	{
		return this->compare(i_right) < 0;
	}

	public: bool operator<=(this_type const& i_right) const
	{
		return this->compare(i_right) <= 0;
	}

	public: template< typename t_string >
	bool operator<=(t_string const& i_right) const
	{
		return this->compare(i_right) <= 0;
	}

	public: bool operator>(this_type const& i_right) const
	{
		return 0 < this->compare(i_right);
	}

	public: template< typename t_string >
	bool operator>(t_string const& i_right) const
	{
		return 0 < this->compare(i_right);
	}

	public: bool operator>=(this_type const& i_right) const
	{
		return 0 <= this->compare(i_right);
	}

	public: template< typename t_string >
	bool operator>=(t_string const& i_right) const
	{
		return 0 <= this->compare(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief ��������r�B
	    @param[in] i_right �E�ӂ̕�����̐擪�ʒu�B�K��NULL�����ŏI���B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: int compare(typename this_type::const_pointer const i_right) const
	{
		return this->compare(0, this->length(), i_right);
	}

	/** @brief ��������r�B
	    @param[in] i_right �E�ӂ̕�����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: template< typename t_string >
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
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: int compare(
		typename this_type::size_type const     i_left_offset,
		typename this_type::size_type const     i_left_count,
		typename this_type::const_pointer const i_right)
	const
	{
		return this->compare(
			i_left_offset,
			i_left_count,
			i_right,
			this_type::find_null(i_right));
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count  ���ӂ̕�����̕������B
	    @param[in] i_right       �E�ӂ̕�����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: template< typename t_string >
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
	    @param[in] i_right        �E�ӂ̕�����̐擪�ʒu�B
	    @param[in] i_right_length �E�ӂ̕�����̒����B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: int compare(
		typename this_type::size_type const     i_left_offset,
		typename this_type::size_type const     i_left_count,
		typename this_type::const_pointer const i_right,
		typename this_type::size_type const     i_right_length)
	const
	{
		std::size_t const a_left_length(
			this->trim_length(i_left_offset, i_left_count));
		bool const a_less(a_left_length < i_right_length);
		int const a_compare(
			t_traits::compare(
				this->data() + i_left_offset,
				i_right,
				a_less? a_left_length: i_right_length));
		if (0 != a_compare)
		{
			return a_compare;
		}
		if (a_less)
		{
			return -1;
		}
		return i_right_length < a_left_length? 1: 0;
	}

	/** @brief ��������r�B
	    @param[in] i_left_offset  ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_left_count   ���ӂ̕�����̕������B
	    @param[in] i_right        �E�ӂ̕�����B
	    @param[in] i_right_offset ���ӂ̕�����̊J�n�ʒu�B
	    @param[in] i_right_count  �E�ӂ̕�����̕������B
	    @retval �� �E�ӂ̂ق����傫���B
	    @retval �� ���ӂ̂ق����傫���B
	    @retval 0  ���ӂƉE�ӂ͓����B
	 */
	public: template< typename t_string >
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
			i_right.trim_length(i_right_offset, i_right_count));
	}

	//-------------------------------------------------------------------------
	/** @brief �����������B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = 0)
	const
	{
		if (i_offset < this->length())
		{
			typename this_type::const_pointer const a_find(
				t_traits::find(
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
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find(i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ������������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type find(
		t_string const&               i_string,
		typename this_type::size_type i_offset = 0)
	const
	{
		return this->find(i_string.data(), i_offset, i_string.length());
	}

	/** @brief ������������B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		if (i_length <= 0)
		{
			return i_offset <= this->length()? i_offset: this_type::npos;
		}
		PSYQ_ASSERT(NULL != i_string);

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
					t_traits::find(a_rest_string, a_rest_length, *i_string));
				if (NULL == a_find)
				{
					break;
				}

				// ����������ƍ��v���邩����B
				if (0 == t_traits::compare(a_find, i_string, i_length))
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
	/** @brief ��납�當���������B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type rfind(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		if (!this->empty())
		{
			typename this_type::const_pointer i(this->trim_pointer(i_offset));
			for (;; --i)
			{
				if (t_traits::eq(*i, i_char))
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

	/** @brief ��납�當����������B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type rfind(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = this_type::npos)
	const
	{
		return this->rfind(i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ��납�當����������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type rfind(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->rfind(i_string.data(), i_offset, i_string.length());
	}

	/** @brief ��납�當����������B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������񂪌��ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type rfind(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		if (i_length <= 0)
		{
			return i_offset < this->length() ? i_offset: this->length();
		}
		PSYQ_ASSERT(NULL != i_string);

		if (i_length <= this->length())
		{
			typename this_type::size_type const a_rest(
				this->length() - i_length);
			typename this_type::const_pointer i(
				this->data() + (i_offset < a_rest? i_offset: a_rest));
			for (;; --i)
			{
				if (t_traits::eq(*i, *i_string)
					&& 0 == t_traits::compare(i, i_string, i_length))
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
	/** @brief �����������B
	    @param[in] i_char   �������镶���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �����������������ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_of(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = 0)
	const
	{
		return this->find(i_char, i_offset);
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find_first_of(
			i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type find_first_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = 0)
	const
	{
		return this->find_first_of(
			i_string.data(), i_offset, i_string.length());
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕����������B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_string);
		if (0 < i_length && i_offset < this->length())
		{
			typename this_type::const_pointer const a_end(
				this->data() + this->length());
			typename this_type::const_pointer i(this->data() + i_offset);
			for (; i < a_end; ++i)
			{
				if (NULL != t_traits::find(i_string, i_length, *i))
				{
					return i - this->data();
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	/** @brief ��������납�猟���B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_of(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->rfind(i_char, i_offset);
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
	    @param[in] i_string ����������B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = this_type::npos)
	const
	{
		return this->find_last_of(
			i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type find_last_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->find_last_of(
			i_string.data(), i_offset, i_string.length());
	}

	/** @brief ����������Ɋ܂܂�邢���ꂩ�̕������A��납�猟���B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return �������������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_string);
		if (0 < i_length && 0 < this->length())
		{
			typename this_type::const_pointer i(this->trim_pointer(i_offset));
			for (;; --i)
			{
				if (NULL != t_traits::find(i_string, i_length, *i))
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
	/** @brief ���������ȊO�̕����������B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_not_of(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = 0)
	const
	{
		typename this_type::const_pointer const a_end(
			this->data() + this->length());
		typename this_type::const_pointer i(this->data() + i_offset);
		for (; i < a_end; ++i)
		{
			if (!t_traits::eq(*i, i_char))
			{
				return i - this->data();
			}
		}
		return this_type::npos;
	}

	/** @brief ����������Ɋ܂܂�Ȃ������������B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = 0)
	const
	{
		return this->find_first_not_of(
			i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ����������Ɋ܂܂�Ȃ������������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type find_first_not_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = 0)
	const
	{
		return this->find_first_not_of(
			i_string.data(), i_offset, i_string.length());
	}

	/** @brief ����������Ɋ܂܂�Ȃ������������B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_first_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_string);
		if (i_offset < this->length())
		{
			typename this_type::const_pointer const a_end(
				this->data() + this->length());
			typename this_type::const_pointer i(this->data() + i_offset);
			for (; i < a_end; ++i)
			{
				if (NULL == t_traits::find(i_string, i_length, *i))
				{
					return i - this->data();
				}
			}
		}
		return this_type::npos;
	}

	//-------------------------------------------------------------------------
	/** @brief ���������ȊO�̕������A��납�猟���B
	    @param[in] i_char   ���������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_not_of(
		t_value const                       i_char,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		if (!this->empty())
		{
			typename this_type::const_pointer i(this->trim_pointer(i_offset));
			for (;; --i)
			{
				if (!t_traits::eq(*i, i_char))
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

	/** @brief ����������Ɋ܂܂�Ȃ������������B
	    @param[in] i_string ����������̐擪�ʒu�B�K��NULL�����ŏI���B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset = this_type::npos)
	const
	{
		return this->find_last_not_of(
			i_string, i_offset, this_type::find_null(i_string));
	}

	/** @brief ����������Ɋ܂܂�Ȃ������������B
	    @param[in] i_string ����������B
	    @param[in] i_offset �������J�n����ʒu�B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: template< typename t_string >
	typename this_type::size_type find_last_not_of(
		t_string const&                     i_string,
		typename this_type::size_type const i_offset = this_type::npos)
	const
	{
		return this->find_last_not_of(
			i_string.data(), i_offset, i_string.length());
	}

	/** @brief ����������Ɋ܂܂�Ȃ��������A��납�猟���B
	    @param[in] i_string ����������̐擪�ʒu�B
	    @param[in] i_offset �������J�n����ʒu�B
	    @param[in] i_length ����������̒����B
	    @return ���������ȊO�̕��������ꂽ�ʒu�B����Ȃ��ꍇ��npos��Ԃ��B
	 */
	public: typename this_type::size_type find_last_not_of(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_offset,
		typename this_type::size_type const     i_length)
	const
	{
		PSYQ_ASSERT(i_length <= 0 || NULL != i_string);
		if (!this->empty())
		{
			typename this_type::const_pointer i(this->trim_pointer(i_offset));
			for (;; --i)
			{
				if (NULL == t_traits::find(i_string, i_length, *i))
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
	public: this_type& assign(typename this_type::const_pointer const i_string)
	{
		return *new(this) this_type(i_string);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_length ���蓖�Ă镶����̒����B
	 */
	public: this_type& assign(
		typename this_type::const_pointer const i_string,
		typename this_type::size_type const     i_length)
	{
		return *new(this) this_type(i_string, i_length);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_begin ���蓖�Ă镶����̐擪�ʒu�B
	    @param[in] i_end   ���蓖�Ă镶����̖����ʒu�B
	 */
	public: this_type& assign(
		typename this_type::const_pointer const i_begin,
		typename this_type::const_pointer const i_end)
	{
		return *new(this) this_type(i_begin, i_end);
	}

	/** @brief ����������蓖�Ă�B
	    @param[in] i_string ���蓖�Ă镶����B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	 */
	public: this_type& assign(
		this_type const&                    i_string,
		typename this_type::size_type const i_offset = 0,
		typename this_type::size_type const i_count = this_type::npos)
	{
		return *new(this) this_type(i_string, i_offset, i_count);
	}

	//-------------------------------------------------------------------------
	/** @brief ������������\�z�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	    @return �\�z��������������B
	 */
	public: this_type substr(
		typename this_type::size_type const i_offset = 0,
		typename this_type::size_type const i_count = this_type::npos)
	const
	{
		return this->substr< this_type >(i_offset, i_count);
	}

	/** @brief ������������\�z�B
	    @tparam t_string �\�z���镶����̌^�B
	    @param[in] i_offset ������̊J�n�ʒu�B
	    @param[in] i_count  �������B
	    @return �\�z��������������B
	 */
	public: template< typename t_string >
	t_string substr(
		typename this_type::size_type const i_offset = 0,
		typename this_type::size_type const i_count = this_type::npos)
	const
	{
		return t_string(
			this->data() + i_offset, this->trim_length(i_offset, i_count));
	}

	//-------------------------------------------------------------------------
	/** @brief ���������ɂ���B
	 */
	public: void clear()
	{
		new(this) this_type();
	}

	//-------------------------------------------------------------------------
	private: typename this_type::const_pointer trim_pointer(
		typename this_type::size_type const i_offset)
	const
	{
		return this->data() + (
			i_offset < this->length()? i_offset: this->length() - 1);
	}

	private: typename this_type::size_type trim_length(
		typename this_type::size_type const i_offset,
		typename this_type::size_type const i_count)
	const
	{
		if (this->length() < i_offset)
		{
			PSYQ_ASSERT(false);
			return 0;
		}
		typename this_type::size_type const a_limit(this->length() - i_offset);
		return i_count < a_limit? i_count: a_limit;
	}

	private: static typename this_type::size_type find_null(
		typename this_type::const_pointer const i_string)
	{
		return NULL != i_string? t_traits::length(i_string): 0;
	}

	//-------------------------------------------------------------------------
	public: static typename this_type::size_type const npos =
		static_cast< typename this_type::size_type >(-1);

	//-------------------------------------------------------------------------
	private: t_value const*                data_;   ///< ������̐擪�ʒu�B
	private: typename this_type::size_type length_; ///< �������B
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

namespace std
{
	template< typename t_value, typename t_traits >
	void swap(
		psyq::basic_const_string< t_value, t_traits >& io_left,
		psyq::basic_const_string< t_value, t_traits >& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // PSYQ_CONST_STRING_HPP_
