#ifndef PSYQ_SCENE_EVENT_ITEM_HPP_
#define PSYQ_SCENE_EVENT_ITEM_HPP_

namespace psyq
{
	typedef psyq::file_buffer event_package;
	template< typename > struct event_item;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event-package�������ځB
    @tparam t_hash event-package�Ŏg���Ă���hash�֐��B
 */
template< typename t_hash >
struct psyq::event_item
{
	public: typedef psyq::event_item< t_hash > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef typename t_hash::value offset;

	//-------------------------------------------------------------------------
	private: struct item_compare_by_name
	{
		bool operator()(
			this_type const& i_left,
			this_type const& i_right)
		const
		{
			return i_left.name < i_right.name;
		}
	};

	//-------------------------------------------------------------------------
	/** @brief event-package����event-item�������B
	    @param[in] i_package event-package�B
	    @param[in] i_name    ��������item�̖��Ohash�l�B
	    @retval !=NULL ������item�ւ�pointer�B
	    @retval ==NULL �Y������item�͌�����Ȃ������B
	 */
	public: static this_type const* find(
		psyq::event_package const&   i_package,
		typename t_hash::value const i_name)
	{
		// item�z��̐擪�ʒu���擾�B
		std::size_t const a_offset(
			*static_cast< typename this_type::offset const* >(
				i_package.get_region_address()));
		if (sizeof(typename this_type::offset) <= a_offset)
		{
			this_type const* const a_begin(
				this_type::get_address< this_type >(i_package, a_offset));
			if (NULL != a_begin)
			{
				// item�z��̖����ʒu���擾�B
				this_type const* const a_end(
					static_cast< this_type const* >(
						static_cast< void const* >(
							i_package.get_region_size() +
							static_cast< char const* >(
								i_package.get_region_address()))));

				// item�z�񂩂�A���O�ɍ��v������̂������B
				this_type a_key;
				a_key.name = i_name;
				this_type const* const a_position(
					std::lower_bound(
						a_begin,
						a_end,
						a_key,
						item_compare_by_name()));
				if (a_end != a_position && a_position->name == i_name)
				{
					return a_position;
				}
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief event-package���ɑ��݂���instance�ւ�pointer���擾�B
	    @param[in] i_package event-package�B
	    @param[in] i_offset  event-package�擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: static void const* get_address(
		psyq::event_package const&       i_package,
		typename this_type::offset const i_offset)
	{
		return 0 < i_offset && i_offset < i_package.get_region_size()?
			i_offset + static_cast< char const* >(
				i_package.get_region_address()):
			NULL;
	}

	/** @brief event-package���ɑ��݂���instance�ւ�pointer���擾�B
		@tparam t_value      instance�̌^�B
	    @param[in] i_package event-package�B
	    @param[in] i_offset  event-package�擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: template< typename t_value >
	static t_value const* get_address(
		psyq::event_package const&       i_package,
		typename this_type::offset const i_offset)
	{
		void const* const a_address(
			this_type::get_address(i_package, i_offset));
		std::size_t const a_alignment(boost::alignment_of< t_value >::value);
		if (0 != reinterpret_cast< std::size_t >(a_address) % a_alignment)
		{
			PSYQ_ASSERT(false);
			return NULL;
		}
		return static_cast< t_value const* >(a_address);
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
		return replace_string< t_string >(
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

	//-------------------------------------------------------------------------
	public: typename t_hash::value     type;  ///< item�̌^��hash�l�B
	public: typename t_hash::value     name;  ///< item�̖��Ohash�l�B
	public: typename this_type::offset begin; ///< item�̐擪�ʒu��event-package��offset�l�B
};

#endif // !PSYQ_SCENE_EVENT_ITEM_HPP_