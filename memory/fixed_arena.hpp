#ifndef PSYQ_FIXED_ARENA_HPP_
#define PSYQ_FIXED_ARENA_HPP_

//#include <psyq/singleton.hpp>
//#include <psyq/memory/fixed_pool.hpp>
//#include <psyq/memory/arena.hpp>

#ifndef PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT
#define PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT 4096
#endif // !PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT

namespace psyq
{
	template<
		std::size_t,
		std::size_t,
		std::size_t,
		std::size_t,
		typename,
		typename >
			class fixed_arena;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory����policy�B
    @tparam t_max_size   ���蓖�Ă�memory�̑傫���Bbyte�P�ʁB
    @tparam t_alignment  memory�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset     memory�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_arena      ���ۂɎg��memory����policy�B
    @tparam t_mutex      multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template<
	std::size_t t_max_size,
	std::size_t t_alignment = sizeof(void*),
	std::size_t t_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT,
	typename    t_arena = PSYQ_ARENA_DEFAULT,
	typename    t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::fixed_arena:
	public psyq::arena
{
	typedef fixed_arena<
		t_max_size,
		t_alignment,
		t_offset,
		t_chunk_size,
		t_arena,
		t_mutex >
			this_type;
	typedef psyq::arena super_type;

	// memory�z�u���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);
	BOOST_STATIC_ASSERT(t_max_size % t_alignment == 0);

	// ���蓖�Ă�memory��chunk�Ɏ��܂邩�m�F�B
	BOOST_STATIC_ASSERT(0 < t_max_size);
	BOOST_STATIC_ASSERT(t_offset < t_chunk_size);
#if 0
	BOOST_STATIC_ASSERT(
		t_max_size <= t_chunk_size
			- sizeof(psyq::fixed_pool< t_arena, t_mutex >::chunk));
#endif // 0

//.............................................................................
public:
	typedef t_arena arena;

	//-------------------------------------------------------------------------
	class pool:
		public psyq::fixed_pool< t_arena, t_mutex >
	{
	public:
		typedef psyq::singleton< pool, t_mutex > singleton;

		pool():
		psyq::fixed_pool< t_arena, t_mutex >(
			t_max_size, t_alignment, t_offset, t_chunk_size)
		{
			// pass
		}
	};

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���memory�̋��Eoffset�l�Bbyte�P�ʁB
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* (malloc)(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		return 0 < i_size
			&& 0 < i_alignment
			&& t_offset == i_offset
			&& i_size <= t_max_size
			&& t_alignment % i_alignment == 0
			&& t_max_size % i_alignment == 0?
				(this_type::malloc)(i_name): NULL;
	}

	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* (malloc)(
		char const* const i_name)
	{
		return this_type::pool::singleton::construct()->allocate(i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	static void (free)(
		void* const       i_memory,
		std::size_t const i_size)
	{
		if (0 < i_size && i_size <= t_max_size)
		{
			(this_type::free)(i_memory);
		}
		else
		{
			PSYQ_ASSERT(0 == i_size && NULL == i_memory);
		}
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	static void (free)(
		void* const i_memory)
	{
		this_type::pool::singleton::construct()->deallocate(i_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief malloc()�Ɏw��ł���memory�̍ő�size���擾�B
	    @return malloc()�Ɏw��ł���m�ۂł���memory�̍ő�size�Bbyte�P�ʁB
	 */
	virtual std::size_t get_max_size() const
	{
		return this_type::max_size;
	}

//.............................................................................
protected:
	virtual typename super_type::malloc_function get_malloc() const
	{
		return &this_type::malloc;
	}

	virtual typename super_type::free_function get_free() const
	{
		return &this_type::free;
	}

//.............................................................................
public:
	static std::size_t const max_size   = t_max_size;
	static std::size_t const alignment  = t_alignment;
	static std::size_t const offset     = t_offset;
	static std::size_t const chunk_size = t_chunk_size;
};

#endif // !PSYQ_FIXED_ARENA_HPP_
