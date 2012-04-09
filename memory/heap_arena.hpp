#ifndef PSYQ_HEAP_ARENA_HPP_
#define PSYQ_HEAP_ARENA_HPP_

//#include <psyq/memory/arena.hpp>

#ifndef PSYQ_ARENA_DEFAULT
#define PSYQ_ARENA_DEFAULT psyq::heap_arena
#endif // !PSYQ_ARENA_DEFAULT

namespace psyq
{
	class heap_arena;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief heap-memory����policy�B
 */
class psyq::heap_arena:
	public psyq::arena
{
	typedef psyq::heap_arena this_type;
	typedef psyq::arena super_type;

//.............................................................................
public:
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
		(void)i_name;

		// memory���E�l��2�ׂ̂��悩�m�F�B
		PSYQ_ASSERT(0 < i_alignment);
		PSYQ_ASSERT(0 == (i_alignment & (i_alignment - 1)));

		// size��0�Ȃ�memory�m�ۂ��Ȃ��B
		if (i_size <= 0)
		{
			return NULL;
		}

#ifdef _WIN32
		// win32���ł�memory�m�ہB
		return _aligned_offset_malloc(i_size, i_alignment, i_offset);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix���ł�memory�m�ہB
		PSYQ_ASSERT(0 == i_offset);
		void* a_memory(NULL);
		int const a_result(
			posix_memalign(
				&a_memory,
				sizeof(void*) <= i_alignment? i_alignment: sizeof(void*),
				i_size));
		return 0 == a_result? a_memory: NULL;
#else
		// ���̑��̊��ł�memory�m�ہB
		PSYQ_ASSERT(0 == i_offset);
		PSYQ_ASSERT(i_alignment <= sizeof(void*));
		(void)i_alignment;
		return (std::malloc)(i_size);
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	static void (free)(void* const i_memory, std::size_t const i_size)
	{
		(void)i_size;

#ifdef _WIN32
		// win32���ł�memory����B
		_aligned_free(i_memory);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix���ł�memory����B
		(std::free)(i_memory);
#else
		// ���̑��̊��ł�memory����B
		(std::free)(i_memory)
#endif
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
	virtual super_type::malloc_function get_malloc() const
	{
		return &this_type::malloc;
	}

	virtual super_type::free_function get_free() const
	{
		return &this_type::free;
	}

//.............................................................................
public:
	/// ��x�Ɋm�ۂł���memory�̍ő�size�Bbyte�P�ʁB
	static std::size_t const max_size = static_cast< std::size_t >(-1);
};

#endif // !PSYQ_HEAP_ARENA_HPP_
