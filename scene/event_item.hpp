#ifndef PSYQ_EVENT_ITEM_HPP_
#define PSYQ_EVENT_ITEM_HPP_

namespace psyq
{
	template< typename > struct event_item;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event���ɁB
    @tparam t_hash event���ɂŎg��������hash�B
 */
template< typename t_hash = psyq::fnv1_hash32 >
struct psyq::event_item
{
private:
	typedef psyq::event_item< t_hash > this_type;

//.............................................................................
public:
	typedef t_hash hash;
	typedef typename t_hash::value offset;
	typedef psyq::file_buffer archive;

	//-------------------------------------------------------------------------
	/** @brief ���ɂ���item�������B
	    @param[in] i_archive ���ɁB
	    @param[in] i_name    ��������item�̖��Ohash�l�B
	    @retval !=NULL ������item�ւ�pointer�B
	    @retval ==NULL �Y������item�͌�����Ȃ������B
	 */
	static this_type const* find(
		typename this_type::archive const& i_archive,
		typename t_hash::value const       i_name)
	{
		// item�z��̐擪�ʒu���擾�B
		std::size_t const a_offset(
			*static_cast< typename this_type::offset const* >(
				i_archive.get_region_address()));
		if (sizeof(typename this_type::offset) <= a_offset)
		{
			this_type const* const a_begin(
				this_type::get_address< this_type >(i_archive, a_offset));
			if (NULL != a_begin)
			{
				// item�z��̖����ʒu���擾�B
				this_type const* const a_end(
					static_cast< this_type const* >(
						static_cast< void const* >(
							i_archive.get_region_size() +
							static_cast< char const* >(
								i_archive.get_region_address()))));

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
	/** @brief ���ɓ��ɑ��݂���instance�ւ�pointer���擾�B
	    @param[in] i_archive ���ɁB
	    @param[in] i_offset  ���ɐ擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	static void const* get_address(
		typename this_type::archive const& i_archive,
		typename this_type::offset const   i_offset)
	{
		return 0 < i_offset && i_offset < i_archive.get_region_size()?
			i_offset + static_cast< char const* >(
				i_archive.get_region_address()):
			NULL;
	}

	/** @brief ���ɓ��ɑ��݂���instance�ւ�pointer���擾�B
		@tparam t_value      instance�̌^�B
	    @param[in] i_archive ���ɁB
	    @param[in] i_offset  ���ɐ擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	template< typename t_value >
	static t_value const* get_address(
		typename this_type::archive const& i_archive,
		typename this_type::offset const   i_offset)
	{
		void const* const a_address(this_type::get_address(i_archive, i_offset));
		std::size_t const a_alignment(boost::alignment_of< t_value >::value);
		if (0 != reinterpret_cast< std::size_t >(a_address) % a_alignment)
		{
			PSYQ_ASSERT(false);
			return NULL;
		}
		return static_cast< t_value const* >(a_address);
	}

	//-------------------------------------------------------------------------
	/** @brief �������'('��')'�Ɉ͂܂ꂽ�P���u�����A������𐶐��B
	    @tparam t_map
	        std::map�݊��̌^�Bt_map::key_type�ɂ�t_hash::value�^�A
	        t_map::mapped_type�ɂ�std::basic_string�݊��̌^�ł���K�v������B
	    @param[in] i_dictionary �u������P���hash�l��key�Ƃ��鎫���B
	    @param[in] i_source     �u�����ƂȂ镶����B
	 */
	template< typename t_out_string, typename t_map, typename t_in_string >
	static t_out_string replace_word(
		t_map const&       i_dictionary,
		t_in_string const& i_source)
	{
		return replace_word< t_out_string >(
			i_dictionary, i_source, i_dictionary.get_allocator());
	}

	/** @brief �������'('��')'�Ɉ͂܂ꂽ�P���u�����A������𐶐��B
	    @tparam t_map
	        std::map�݊��̌^�Bt_map::key_type�ɂ�t_hash::value�^�A
	        t_map::mapped_type�ɂ�std::basic_string�݊��̌^�ł���K�v������B
	    @param[in] i_dictionary �u������P���hash�l��key�Ƃ��鎫���B
	    @param[in] i_source     �u�����ƂȂ镶����B
	 */
	template< typename t_out_string, typename t_map, typename t_in_string >
	static t_out_string replace_word(
		t_map const&                                 i_dictionary,
		t_in_string const&                           i_source,
		typename t_out_string::allocator_type const& i_allocator)
	{
		typename t_in_string::const_iterator a_last_end(i_source.begin());
		t_out_string a_string(i_allocator);
		for (;;)
		{
			std::pair<
				typename t_in_string::const_iterator,
				typename t_in_string::const_iterator >
					const a_range(
						this_type::find_word(a_last_end, i_source.end()));
			if (a_range.first == a_range.second)
			{
				// ���ׂĂ̒P���u�������B
				a_string.append(a_last_end, i_source.end());
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

//.............................................................................
private:
	//-------------------------------------------------------------------------
	struct item_compare_by_name
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
	/** @biref �����񂩂�'('��')'�ň͂܂ꂽ�P��������B
	 */
	template< typename t_iterator >
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
				if (i_end != a_word_begin)
				{
					return std::make_pair(a_word_begin, i + 1);
				}
				break;
			}
		}
		return std::make_pair(i_end, i_end);
	}

//.............................................................................
public:
	typename t_hash::value     name;  ///< item�̖��O�B
	typename t_hash::value     type;  ///< item�̌^���B
	typename this_type::offset begin; ///< item�̐擪�ʒu�̏��ɓ�offset�l�B
};

#endif // !PSYQ_EVENT_ITEM_HPP_
