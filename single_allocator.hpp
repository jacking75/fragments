#ifndef PSYQ_SINGLE_ALLOCATOR_HPP_
#define PSYQ_SINGLE_ALLOCATOR_HPP_

namespace psyq
{
	template< typename, typename, std::size_t, std::size_t, std::size_t >
		class single_allocator;

	template< typename, std::size_t, std::size_t, std::size_t, std::size_t >
		class _single_allocator_memory;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename    t_memory,
	std::size_t t_max_size,
	std::size_t t_chunk_size,
	std::size_t t_chunk_alignment,
	std::size_t t_chunk_offset >
class psyq::_single_allocator_memory:
	public psyq::fixed_memory< t_memory >
{
	typedef psyq::fixed_memory< t_memory > super_type;

	// memory���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 < t_chunk_alignment);
	BOOST_STATIC_ASSERT(0 == (t_chunk_alignment & (t_chunk_alignment - 1)));

//.............................................................................
public:
	_single_allocator_memory():
	super_type(t_max_size, t_chunk_size, t_chunk_alignment, t_chunk_offset)
	{
		// pass
	}

	static std::size_t const max_size = t_max_size;
	static std::size_t const chunk_size = t_chunk_size;
	static std::size_t const chunk_alignment = t_chunk_alignment;
	static std::size_t const chunk_offset = t_chunk_offset;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ��x�ɂЂƂ�instance���m�ۂ���Astd::allocator�݊��̊����q�B
        �z��͊m�ۂł��Ȃ��B
    @tparam t_value_type �m�ۂ���instance�̌^�B
    @tparam t_memory     memory����policy�B
    @tparam t_chunk_size memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_alignment  instance�z�u���E�l�B
    @tparam t_offset     instance�z�uoffset�l�B
 */
template<
	typename    t_value_type,
	typename    t_memory = psyq::default_memory_policy,
	std::size_t t_chunk_size = 4096,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0 >
class psyq::single_allocator:
	private std::allocator< t_value_type >
{
	typedef psyq::single_allocator<
		t_value_type, t_memory, t_chunk_size, t_alignment, t_offset >
			this_type;
	typedef std::allocator< t_value_type > super_type;

//.............................................................................
public:
	using super_type::pointer;
	using super_type::const_pointer;
	using super_type::reference;
	using super_type::const_reference;
	using super_type::value_type;
	using super_type::address;
	using super_type::construct;
	using super_type::destroy;

	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value >
	struct rebind
	{
		typedef single_allocator<
			t_other_type,
			t_memory,
			t_chunk_size,
			t_other_alignment,
			t_offset >
				other;
	};

	typedef _single_allocator_memory<
		t_memory,
		((sizeof(t_value_type) + t_alignment - 1) / t_alignment) * t_alignment,
		t_chunk_size,
		t_alignment,
		t_offset >
			memory;

	//-------------------------------------------------------------------------
	single_allocator():
	super_type()
	{
		// pass
	}

	single_allocator(
		this_type const& i_source):
	super_type(i_source)
	{
		// pass
	}

	template< typename t_other_type, std::size_t t_other_alignment >
	single_allocator(
		single_allocator<
			t_other_type,
			t_memory,
			t_chunk_size,
			t_other_alignment,
			t_offset > const&
				i_source):
	super_type(i_source)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	this_type& operator=(
		this_type const& i_source)
	{
		this->super_type::operator=(i_source);
		return *this;
	}

	template< typename t_other_type, std::size_t t_other_alignment >
	this_type& operator=(
		single_allocator<
			t_other_type,
			t_memory,
			t_chunk_size,
			t_other_alignment,
			t_offset > const&
				i_source)
	{
		this->super_type::operator=(i_source);
		return *this;
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���instance�̐��B
	    @param[in] i_alignment �m�ۂ���instance�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���instance�̋��Eoffset�l�Bbyte�P�ʁB
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static typename super_type::pointer allocate(
		typename super_type::size_type const i_num,
		std::size_t const                    i_alignment = t_alignment,
		std::size_t const                    i_offset = t_offset,
		char const* const                    i_name = NULL)
	{
		return 1 == i_num
			&& 0 < i_alignment
			&& 0 == t_alignment % i_alignment
			&& t_offset == i_offset?
				typename this_type::allocate(i_name): NULL;
	}

	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static typename super_type::pointer allocate(
		char const* const i_name = NULL)
	{
		return static_cast< typename super_type::pointer >(
			psyq::singleton< typename this_type::memory >::get().allocate(
				i_name));
	}

	/** @brief intance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	static void deallocate(
		typename super_type::pointer const   i_instance,
		typename super_type::size_type const i_num = 1)
	{
		if (1 == i_num)
		{
			psyq::singleton< typename this_type::memory >::get().deallocate(
				i_instance);
		}
		else
		{
			PSYQ_ASSERT(0 == i_num && NULL == i_instance);
		}
	}

	static size_type max_size()
	{
		return 1;
	}
};

#endif // PSYQ_SINGLE_ALLOCATOR_HPP_
