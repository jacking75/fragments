#ifndef PSYQ_EVENT_ARCHIVE_HPP_
#define PSYQ_EVENT_ARCHIVE_HPP_

namespace psyq
{
	template< typename > class event_archive;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_hash >
class psyq::event_archive
{
	typedef psyq::event_archive< t_hash > this_type;

//.............................................................................
public:
	typedef t_hash hash;
	typedef typename t_hash::value_type offset;

	//-------------------------------------------------------------------------
	struct item
	{
		typename t_hash::value_type name;   ///< item�̖��O�B
		typename t_hash::value_type type;   ///< item�̌^���B
		typename this_type::offset  offset; ///< item�̏��ɓ�offset�l�B
	};

	//-------------------------------------------------------------------------
	/** @brief ���ɓ��ɑ��݂���instance�ւ�pointer���擾�B
		@tparam t_value      instance�̌^�B
	    @param[in] i_archive ���ɁB
	    @param[in] i_offset  ���ɐ擪�ʒu�����offset�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	template< typename t_value >
	static t_value const* get_address(
		psyq::file_buffer const&         i_archive,
		typename this_type::offset const i_offset)
	{
		return 0 < i_offset && i_offset < i_archive.get_region_size()?
			static_cast< t_value const* >(
				static_cast< void const* >(
					i_offset + static_cast< char const* >(
						i_archive.get_region_address()))):
			NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief ���ɂ���item�������B
	    @param[in] i_archive ���ɁB
	    @param[in] i_name    ��������item�̖��Ohash�l�B
	    @retval !=NULL ������item�ւ�pointer�B
	    @retval ==NULL �Y������item�͌�����Ȃ������B
	 */
	static typename this_type::item const* find_item(
		psyq::file_buffer const&          i_archive,
		typename t_hash::value_type const i_name)
	{
		// item�z��̐擪�ʒu���擾�B
		std::size_t const a_offset(
			*static_cast< typename this_type::offset const* >(
				i_archive.get_region_address()));
		if (sizeof(typename this_type::offset) <= a_offset)
		{
			typename this_type::item const* const a_begin(
				this_type::get_address< typename this_type::item >(
					i_archive, a_offset));
			if (NULL != a_begin)
			{
				// item�z��̖����ʒu���擾�B
				typename this_type::item const* const a_end(
					static_cast< typename this_type::item const* >(
						static_cast< void const* >(
							i_archive.get_region_size() +
							static_cast< char const* >(
								i_archive.get_region_address()))));

				// item�z�񂩂�A���O�ɍ��v������̂������B
				typename this_type::item a_key;
				a_key.name = i_name;
				typename this_type::item const* const a_position(
					std::lower_bound(
						a_begin,
						a_end,
						a_key,
						this_type::item_compare_by_name()));
				if (a_end != a_position && a_position->name == i_name)
				{
					return a_position;
				}
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief �������'('��')'�Ɉ͂܂ꂽ�P���u�����Ahash�l���Z�o�B
	    @param[in] i_map   �u������P���key�Ƃ��鎫���B
	    @param[in] i_begin �u�����镶����̐擪�ʒu�B
	    @param[in] i_end   �u�����镶����̖����ʒu�B
	 */
	template< typename t_map >
	static typename t_hash::value_type make_hash(
		t_map const&                                     i_map,
		typename t_map::mapped_type::const_pointer const i_begin,
		typename t_map::mapped_type::const_pointer const i_end)
	{
		typedef typename t_map::mapped_type::value_type char_type;
		char_type const* a_last_end(i_begin);
		std::basic_string< char_type > a_string;
		for (;;)
		{
			std::pair< char_type const*, char_type const* > const a_range(
				this_type::find_word(a_last_end, i_end));
			if (a_range.first == a_range.second)
			{
				a_string.append(a_last_end, i_end);
				return t_hash::generate(a_string.begin(), a_string.end());
			}

			// ��������u����������B
			typename t_map::const_iterator a_position(
				i_map.find(
					typename t_map::mapped_type(
						a_range.first + 1,
						a_range.second - a_range.first - 2)));
			if (i_map.end() != a_position)
			{
				// �����ɂ���P��Œu������B
				a_string.append(a_last_end, a_range.first);
				a_string.append(
					a_position->second.data(),
					a_position->second.length());
			}
			else
			{
				// �u����ł͂Ȃ������̂ŁA���̕�����̂܂܂ɂ��Ă����B
				a_string.append(a_last_end, a_range.second - a_last_end);
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
			typename this_type::item const& i_left,
			typename this_type::item const& i_right)
		const
		{
			return i_left.name < i_right.name;
		}
	};

	//-------------------------------------------------------------------------
	/** @biref �����񂩂�'('��')'�ň͂܂ꂽ�P��������B
	 */
	template< typename t_char >
	static std::pair< t_char const*, t_char const* > find_word(
		t_char const* const i_begin,
		t_char const* const i_end)
	{
		t_char const* a_word_begin(i_end);
		for (t_char* i = i_begin; i_end != i; ++i)
		{
			switch (*i)
			{
			case t_char('('):
				a_word_begin = i;
				break;
			case t_char(')'):
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

#endif // !PSYQ_EVENT_ARCHIVE_HPP_
