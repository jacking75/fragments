#ifndef PSYQ_MEMORY_POLICY_HPP_
#define PSYQ_MEMORY_POLICY_HPP_

#ifndef PSYQ_MEMORY_NAME_DEFAULT
#define PSYQ_MEMORY_NAME_DEFAULT "PSYQ"
#endif // !PSYQ_MEMORY_NAME_DEFAULT

#ifndef PSYQ_MEMORY_POLICY_DEFAULT
#define PSYQ_MEMORY_POLICY_DEFAULT psyq::memory_policy
#endif // !PSYQ_MEMORY_POLICY_DEFAULT

namespace psyq
{
	class memory_policy;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief memory割当policy。
 */
class psyq::memory_policy:
	private boost::noncopyable
{
	typedef psyq::memory_policy this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	/** @brief memoryを確保する。
	    @param[in] i_size      確保するmemoryの大きさ。byte単位。
	    @param[in] i_alignment 確保するmemoryの境界値。byte単位。
	    @param[in] i_offset    確保するmemoryの境界offset値。byte単位。
	    @param[in] i_name      debugで使うためのmemory識別名。
	    @return 確保したmemoryの先頭位置。ただしNULLの場合は失敗。
	 */
	static void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment = sizeof(void*),
		std::size_t const i_offset = 0,
		char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT)
	{
		(void)i_name;

		// memory境界値が2のべき乗か確認。
		PSYQ_ASSERT(0 < i_alignment);
		PSYQ_ASSERT(0 == (i_alignment & (i_alignment - 1)));

		// sizeが0ならmemory確保しない。
		if (i_size <= 0)
		{
			return NULL;
		}

#ifdef _WIN32
		// win32環境でのmemory確保。
		return _aligned_offset_malloc(i_size, i_alignment, i_offset);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix環境でのmemory確保。
		PSYQ_ASSERT(0 == i_offset);
		void* a_memory(NULL);
		int const a_result(
			posix_memalign(
				&a_memory,
				sizeof(void*) <= i_alignment? i_alignment: sizeof(void*),
				i_size));
		return 0 == a_result? a_memory: NULL;
#else
		// その他の環境でのmemory確保。
		PSYQ_ASSERT(0 == i_offset);
		PSYQ_ASSERT(i_alignment <= sizeof(void*));
		(void)i_alignment;
		return std::malloc(i_size);
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief memoryを解放する。
	    @param[in] i_memory 解放するmemoryの先頭位置。
	    @param[in] i_size   解放するmemoryの大きさ。byte単位。
	 */
	static void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		(void)i_size;

#ifdef _WIN32
		// win32環境でのmemory解放。
		_aligned_free(i_memory);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix環境でのmemory解放。
		std::free(i_memory);
#else
		// その他の環境でのmemory解放。
		std::free(i_memory)
#endif
	}

	//-------------------------------------------------------------------------
	static std::size_t max_size()
	{
		return (std::numeric_limits< std::size_t >::max)();
	}

//.............................................................................
private:
	memory_policy();
};

#endif // PSYQ_MEMORY_POLICY_HPP_
