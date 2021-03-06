#ifndef PSYQ_FIXED_ARENA_HPP_
#define PSYQ_FIXED_ARENA_HPP_

//#include <psyq/singleton.hpp>
//#include <psyq/memory/fixed_pool.hpp>
//#include <psyq/memory/arena.hpp>

#ifndef PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT
#define PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT 4096
#endif // !PSYQ_FIXED_ARENA_CHUNK_SIZE_DEFAULT

/// @cond
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
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 固定sizeのmemory割当policy。
    @tparam t_max_size   割り当てるmemoryの大きさ。byte単位。
    @tparam t_alignment  memoryの配置境界値。byte単位。
    @tparam t_offset     memoryの配置offset値。byte単位。
    @tparam t_chunk_size memory-chunkの最大size。byte単位。
    @tparam t_arena      実際に使うmemory割当policy。
    @tparam t_mutex      multi-thread対応に使うmutexの型。
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
	public: typedef fixed_arena<
		t_max_size,
		t_alignment,
		t_offset,
		t_chunk_size,
		t_arena,
		t_mutex >
			this_type;
	public: typedef psyq::arena super_type;

	// memory配置境界値が2のべき乗か確認。
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);
	BOOST_STATIC_ASSERT(t_max_size % t_alignment == 0);

	// 割り当てるmemoryがchunkに収まるか確認。
	BOOST_STATIC_ASSERT(0 < t_max_size);
	BOOST_STATIC_ASSERT(t_offset < t_chunk_size);
#if 0
	BOOST_STATIC_ASSERT(
		t_max_size <= t_chunk_size
			- sizeof(psyq::fixed_pool< t_arena, t_mutex >::chunk));
#endif // 0

	//-------------------------------------------------------------------------
	public: typedef t_arena arena;

	//-------------------------------------------------------------------------
	/** @brief singletonとして使う、固定sizeのmemory-pool。
	 */
	public: class pool:
		public psyq::fixed_pool< t_arena, t_mutex >
	{
		public: typedef psyq::singleton< pool, t_mutex > singleton;

		public: pool():
		psyq::fixed_pool< t_arena, t_mutex >(
			t_max_size, t_alignment, t_offset, t_chunk_size)
		{
			// pass
		}
	};

	//-------------------------------------------------------------------------
	public: static std::size_t const MAX_SIZE   = t_max_size;
	public: static std::size_t const ALIGNMENT  = t_alignment;
	public: static std::size_t const OFFSET     = t_offset;
	public: static std::size_t const CHUNK_SIZE = t_chunk_size;

	//-------------------------------------------------------------------------
	public: explicit fixed_arena(
		char const* const i_name = PSYQ_ARENA_NAME_DEFAULT):
	super_type(i_name)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief memoryを確保する。
	    @param[in] i_size      確保するmemoryの大きさ。byte単位。
	    @param[in] i_alignment 確保するmemoryの境界値。byte単位。
	    @param[in] i_offset    確保するmemoryの境界offset値。byte単位。
	    @param[in] i_name      debugで使うためのmemory識別名。
	    @return 確保したmemoryの先頭位置。ただしNULLの場合は失敗。
	 */
	public: static void* (malloc)(
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

	/** @brief memoryを確保する。
	    @param[in] i_name debugで使うためのmemory識別名。
	    @return 確保したmemoryの先頭位置。ただしNULLの場合は失敗。
	 */
	public: static void* (malloc)(char const* const i_name)
	{
		return this_type::pool::singleton::construct()->allocate(i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief memoryを解放する。
	    @param[in] i_memory 解放するmemoryの先頭位置。
	    @param[in] i_size   解放するmemoryの大きさ。byte単位。
	 */
	public: static void (free)(void* const i_memory, std::size_t const i_size)
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

	/** @brief memoryを解放する。
	    @param[in] i_memory 解放するmemoryの先頭位置。
	 */
	public: static void (free)(void* const i_memory)
	{
		this_type::pool::singleton::construct()->deallocate(i_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief malloc()に指定できるmemoryの最大sizeを取得。
	    @return malloc()に指定できる確保できるmemoryの最大size。byte単位。
	 */
	public: virtual std::size_t get_max_size() const
	{
		return this_type::MAX_SIZE;
	}

	//-------------------------------------------------------------------------
	protected: virtual typename super_type::malloc_function get_malloc() const
	{
		return &this_type::malloc;
	}

	protected: virtual typename super_type::free_function get_free() const
	{
		return &this_type::free;
	}
};

#endif // !PSYQ_FIXED_ARENA_HPP_
