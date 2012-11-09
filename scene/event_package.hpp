#ifndef PSYQ_SCENE_EVENT_PACKAGE_HPP_
#define PSYQ_SCENE_EVENT_PACKAGE_HPP_

//#include <psyq/memory/arena.hpp>

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
	    @param[in] i_offset event-package�擪�ʒu�����offset�l�B
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
		@tparam t_value     instance�̌^�B
	    @param[in] i_offset event-package�擪�ʒu�����offset�l�B
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
		@tparam t_value   instance�̌^�B
	    @param[in] i_name instance�̖��Ohash�l�B
	    @retval !=NULL instance�ւ�pointer�B
	    @retval ==NULL instance�͌�����Ȃ������B
	 */
	public: template< typename t_value >
	t_value const* find_value(typename t_hash::value const i_name) const
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
				typename this_type::item const* const a_item(
					std::lower_bound(a_begin, a_end, a_key));
				if (a_end != a_item && a_item->name == i_name)
				{
					return this->get_value< t_value >(a_item->position);
				}
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief container�ɗv�f��}���B
	    @param[in,out] io_container �v�f��}������container�B
	    @param[in] i_key            �}������v�f��key�B
	    @param[in] i_shared_ptr     �}������v�f�B
	    @return �}�������l�B�}���Ɏ��s�����ꍇ�͋�B
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type const& _insert_shared_ptr(
		t_container&                             io_container,
		typename t_hash::value const             i_key,
		typename t_container::mapped_type const& i_shared_ptr)
	{
		if (t_hash::EMPTY != i_key && NULL != i_shared_ptr.get())
		{
			typename t_container::mapped_type& a_shared_ptr(
				io_container[i_key]);
			a_shared_ptr = i_shared_ptr;
			return a_shared_ptr;
		}
		return psyq::_get_null_shared_ptr<
			typename t_container::mapped_type::element_type >();
	}

	/** @brief container����v�f�������B
	    @param[in] i_container �v�f����������container�B
	    @param[in] i_key       ��������v�f��key�B
	    @return ���������v�f�����l�B
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type const& _find_shared_ptr(
		t_container const&           i_container,
		typename t_hash::value const i_key)
	{
		if (t_hash::EMPTY != i_key)
		{
			typename t_container::const_iterator const a_position(
				i_container.find(i_key));
			if (i_container.end() != a_position)
			{
				return a_position->second;
			}
		}
		return psyq::_get_null_shared_ptr<
			typename t_container::mapped_type::element_type >();
	}

	/** @brief container����v�f����菜���B
	    @param[in] i_container �v�f����菜��container�B
	    @param[in] i_key       ��菜���v�f��key�B
	    @return ��菜�����v�f�������Ă����l�B
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type _remove_shared_ptr(
		t_container&                 io_container,
		typename t_hash::value const i_key)
	{
		typename t_container::mapped_type a_shared_ptr;
		if (t_hash::EMPTY != i_key)
		{
			typename t_container::iterator const a_position(
				io_container.find(i_key));
			if (io_container.end() != a_position)
			{
				a_shared_ptr.swap(a_position->second);
				io_container.erase(a_position);
			}
		}
		return a_shared_ptr;
	}

	public: template< typename t_container >
	static typename t_container::mapped_type _remove_shared_ptr(
		t_container&                             io_container,
		typename t_container::mapped_type const& i_mapped)
	{
		for (
			typename t_container::iterator i = io_container.begin();
			io_container.end() != i;)
		{
			if (i_mapped != i->second)
			{
				++i;
			}
			else
			{
				i = io_container.erase(i);
			}
		}
		return i_mapped;
	}
};

#endif // !PSYQ_SCENE_EVENT_PACKAGE_HPP_
