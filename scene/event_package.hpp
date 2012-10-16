#ifndef PSYQ_SCENE_EVENT_PACKAGE_HPP_
#define PSYQ_SCENE_EVENT_PACKAGE_HPP_

namespace psyq
{
	template< typename > class event_package;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event�Ŏg��resource���܂Ƃ߂�package�B
    @tparam t_hash event-package���Ŏg���Ă���hash�֐��B
 */
template< typename t_hash >
class psyq::event_package:
	private psyq::file_buffer
{
	public: typedef psyq::event_package< t_hash > this_type;
	private: typedef psyq::file_buffer super_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef typename t_hash::value offset;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	//-------------------------------------------------------------------------
	private: struct item
	{
		bool operator<(item const& i_right) const
		{
			return this->name < i_right.name;
		}

		typename t_hash::value name;     ///< item�̖��Ohash�l�B
		typename t_hash::value position; ///< item�̐擪�ʒu��event-package��offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename this_type::const_shared_ptr make(
		PSYQ_SHARED_PTR< psyq::file_buffer const > const& i_package)
	{
		return PSYQ_STATIC_POINTER_CAST< this_type >(i_package);
	}

	private: event_package();

	//-------------------------------------------------------------------------
	/** @brief event-package���ɑ��݂���instance�ւ�pointer���擾�B
	    @param[in] i_package event-package�B
	    @param[in] i_offset  event-package�擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: void const* get_address(typename this_type::offset const i_offset)
	const
	{
		return 0 < i_offset && i_offset < this->get_region_size()?
			i_offset + static_cast< char const* >(this->get_region_address()):
			NULL;
	}

	/** @brief event-package���ɑ��݂���instance�ւ�pointer���擾�B
		@tparam t_value      instance�̌^�B
	    @param[in] i_package �ΏۂƂȂ�event-package�B
	    @param[in] i_offset  event-package�擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: template< typename t_value >
	t_value const* get_value(typename this_type::offset const i_offset) const
	{
		void const* const a_address(this->get_address(i_offset));
		std::size_t const a_alignment(boost::alignment_of< t_value >::value);
		if (0 != reinterpret_cast< std::size_t >(a_address) % a_alignment)
		{
			PSYQ_ASSERT(false);
			return NULL;
		}
		return static_cast< t_value const* >(a_address);
	}

	/** @brief event-package���ɑ��݂���instance�ւ�pointer���擾�B
		@tparam t_value      instance�̌^�B
	    @param[in] i_package �ΏۂƂȂ�event-package�B
	    @param[in] i_name    instance�̖��Ohash�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: template< typename t_value >
	t_value const* find_value(typename t_hash::value const i_name) const
	{
		// event-package����event���ڂ��擾�B
		typename this_type::item const* const a_item(this->find_item(i_name));
		if (NULL != a_item)
		{
			return this->get_value< t_value >(a_item->position);
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief �������'('��')'�Ɉ͂܂ꂽ�P���u�������A������𐶐��B
		@tparam t_string �o�͂��镶����̌^�B
	    @tparam t_map
	        std::map�݊��̌^�Bt_map::key_type�ɂ�t_hash::value�^�A
	        t_map::mapped_type�ɂ�std::basic_string�݊��̌^�ł���K�v������B
	    @param[in] i_dictionary �u������P���hash�l��key�Ƃ��鎫���B
	    @param[in] i_begin      �u�����ƂȂ镶����̊J�n�����ʒu�B
	    @param[in] i_end        �u�����ƂȂ镶����̖��������ʒu�B
		@return �u����̕�����B
	 */
	public: template< typename t_string, typename t_map, typename t_iterator >
	static t_string replace_string(
		t_map const&     i_dictionary,
		t_iterator const i_begin,
		t_iterator const i_end)
	{
		return this_type::template replace_string< t_string >(
			i_dictionary, i_begin, i_end, i_dictionary.get_allocator());
	}

	/** @brief �������'('��')'�Ɉ͂܂ꂽ�P���u�������A������𐶐��B
		@tparam t_string �o�͂��镶����̌^�B
	    @tparam t_map
	        std::map�݊��̌^�Bt_map::key_type�ɂ�t_hash::value�^�A
	        t_map::mapped_type�ɂ�std::basic_string�݊��̌^�ł���K�v������B
	    @param[in] i_dictionary �u������P���hash�l��key�Ƃ��鎫���B
	    @param[in] i_begin      �u�����ƂȂ镶����̊J�n�����ʒu�B
	    @param[in] i_end        �u�����ƂȂ镶����̖��������ʒu�B
	    @param[in] i_allocator  �o�͕������memory�����q�B
		@return �u����̕�����B
	 */
	public: template< typename t_string, typename t_map, typename t_iterator >
	static t_string replace_string(
		t_map const&                             i_dictionary,
		t_iterator const                         i_begin,
		t_iterator const                         i_end,
		typename t_string::allocator_type const& i_allocator)
	{
		t_iterator a_last_end(i_begin);
		t_string a_string(i_allocator);
		for (;;)
		{
			// �����񂩂�'('��')'�ň͂܂ꂽ�͈͂������B 
			std::pair< t_iterator, t_iterator > const a_range(
				this_type::find_word(a_last_end, i_end));
			if (a_range.first == a_range.second)
			{
				// ���ׂĂ̒P���u�������B
				a_string.append(a_last_end, i_end);
				return a_string;
			}

			// ��������u����������B
			typename t_map::const_iterator const a_position(
				i_dictionary.find(
					t_hash::generate(a_range.first + 1, a_range.second - 1)));
			if (i_dictionary.end() != a_position)
			{
				// �����ɂ���P��Œu������B
				a_string.append(a_last_end, a_range.first);
				a_string.append(
					a_position->second.begin(),
					a_position->second.end());
			}
			else
			{
				// �u����ł͂Ȃ������̂ŁA���̂܂܂ɂ��Ă����B
				a_string.append(a_last_end, a_range.second);
			}
			a_last_end = a_range.second;
		}
	}

	//-------------------------------------------------------------------------
	/** @brief event-package����event-item�������B
	    @param[in] i_package event-package�B
	    @param[in] i_name    ��������item�̖��Ohash�l�B
	    @retval !=NULL ������item�ւ�pointer�B
	    @retval ==NULL �Y������item�͌�����Ȃ������B
	 */
	private: typename this_type::item const* find_item(
		typename t_hash::value const i_name)
	const
	{
		// item�z��̐擪�ʒu���擾�B
		std::size_t const a_offset(
			*static_cast< typename this_type::offset const* >(
				this->get_region_address()));
		if (sizeof(typename this_type::offset) <= a_offset)
		{
			typename this_type::item const* const a_begin(
				this->get_value< typename this_type::item >(a_offset));
			if (NULL != a_begin)
			{
				// item�z��̖����ʒu���擾�B
				typename this_type::item const* const a_end(
					static_cast< typename this_type::item const* >(
						static_cast< void const* >(
							this->get_region_size() +
							static_cast< char const* >(
								this->get_region_address()))));

				// item�z�񂩂�A���O�ɍ��v������̂������B
				typename this_type::item a_key;
				a_key.name = i_name;
				typename this_type::item const* const a_position(
					std::lower_bound(a_begin, a_end, a_key));
				if (a_end != a_position && a_position->name == i_name)
				{
					return a_position;
				}
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @biref �����񂩂�'('��')'�ň͂܂ꂽ�P��������B
	 */
	private: template< typename t_iterator >
	static std::pair< t_iterator, t_iterator > find_word(
		t_iterator const i_begin,
		t_iterator const i_end)
	{
		t_iterator a_word_begin(i_end);
		for (t_iterator i = i_begin; i_end != i; ++i)
		{
			switch (*i)
			{
				case '(':
				a_word_begin = i;
				break;

				case ')':
				// �Ή�����'('������΁A�P��͈̔͂�Ԃ��B
				if (i_end != a_word_begin)
				{
					return std::make_pair(a_word_begin, i + 1);
				}
				break;
			}
		}
		return std::make_pair(i_end, i_end);
	}
};

#endif // !PSYQ_SCENE_EVENT_PACKAGE_HPP_
