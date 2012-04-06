#ifndef PSYQ_SINGLE_ALLOCATOR_HPP_
#define PSYQ_SINGLE_ALLOCATOR_HPP_

//#include <psyq/memory/allocator.hpp>
//#include <psyq/memory/fixed_arena.hpp>

namespace psyq
{
	template<
		typename, std::size_t, std::size_t, std::size_t, typename, typename >
			class single_allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ��x�ɂЂƂ�instance���m�ۂ���Astd::allocator�݊��̊����q�B
        �z��͊m�ۂł��Ȃ��B
    @tparam t_value_type �m�ۂ���instance�̌^�B
    @tparam t_alignment  instance�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset     instance�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_arena      ���ۂɎg��memory����policy�B
	@tparam t_mutex      multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template<
	typename    t_value_type,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT,
	typename    t_arena = PSYQ_ARENA_DEFAULT,
	typename    t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::single_allocator:
	public psyq::allocator<
		t_value_type,
		t_alignment,
		t_offset,
		psyq::fixed_arena<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_arena,
			t_mutex > >
{
	typedef psyq::single_allocator<
		t_value_type,
		t_alignment,
		t_offset,
		t_chunk_size,
		t_arena,
		t_mutex >
			this_type;
	typedef psyq::allocator<
		t_value_type,
		t_alignment,
		t_offset,
		psyq::fixed_arena<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_arena,
			t_mutex > >
				super_type;

//.............................................................................
public:
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		std::size_t t_other_chunk = t_chunk_size,
		typename    t_other_arena = t_arena,
		typename    t_other_mutex = t_mutex >
	struct rebind
	{
		typedef psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_other_arena,
			t_other_mutex >
				other;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit single_allocator(
		char const* const i_name = PSYQ_ALLOCATOR_NAME_DEFAULT):
	super_type(i_name)
	{
		// pass
	}

	//single_allocator(this_type const&) = default;

	/** @param[in] i_source copy��instance�B
	 */
	template< typename t_other_type, std::size_t t_other_alignment >
	single_allocator(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_offset,
			t_chunk_size,
			t_arena,
			t_mutex > const&
				i_source):
	super_type(i_source)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	//this_type& operator=(this_type const&) = default;

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_num  �m�ۂ���instance�̐��B
	    @param[in] i_hint �œK���̂��߂�hint�B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate(
		typename super_type::size_type const i_num,
		void const* const                    i_hint = NULL)
	{
		return this->super_type::allocate(i_num, i_hint);
	}

	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate()
	{
		void* const a_memory((super_type::arena::malloc)(this->get_name()));
		PSYQ_ASSERT(NULL != a_memory);
		return static_cast< typename super_type::pointer >(a_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg���Ă���memory���������B
	    @param[in] i_memory �������instance�̐擪�ʒu�B
	    @param[in] i_num    �������instance�̐��B
	 */
	void deallocate(
		typename super_type::pointer const   i_memory,
		typename super_type::size_type const i_num)
	{
		this->super_type::deallocate(i_memory, i_num);
	}

	void deallocate(
		typename super_type::pointer const i_memory)
	{
		(super_type::arena::free)(i_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief �m�ۂł���instance�̍ő吔��Ԃ��B
	    @warning
	      max_size()�̕Ԃ�l�́Aallocate()�Ŏw��ł���instance���̍ő�l��
	      C++�̎d�l�Ō��߂��Ă���B
	      �Ƃ��낪VC++�ɓY�t����Ă�STL�̎����͂��̂悤�ɂȂ��Ă��炸�A
	      memory��ɑ��݂ł���instance�̍ő吔��Ԃ��悤�Ɏ�������Ă���B
	      ���̂���VC++�̏ꍇ�́A��p��max_size()���g�����Ƃɂ���B
	      http://msdn.microsoft.com/en-us/library/h36se6sf.aspx
	 */
#ifdef _MSC_VER
	static typename super_type::size_type max_size()
	{
		return t_arena::max_size / sizeof(t_value_type);
	}
#endif // _MSC_VER
};

#endif // !PSYQ_SINGLE_ALLOCATOR_HPP_
