#ifndef PSYQ_ARENA_HPP_
#define PSYQ_ARENA_HPP_

#include <boost/noncopyable.hpp>

#ifndef PSYQ_ARENA_NAME_DEFAULT
#	define PSYQ_ARENA_NAME_DEFAULT "PSYQ"
#endif // !PSYQ_ALLOCATOR_NAME_DEFAULT

#ifdef PSYQ_USER_DEFINED_MEMORY
#elif defined(PSYQ_CPP11)
#	include <memory>
#	define PSYQ_SHARED_PTR std::shared_ptr
#	define PSYQ_WEAK_PTR std::weak_ptr
#	define PSYQ_ALLOCATE_SHARED std::allocate_shared
#	define PSYQ_MAKE_SHARED std::make_shared
#else
#	include <boost/smart_ptr/shared_ptr.hpp>
#	include <boost/smart_ptr/weak_ptr.hpp>
#	include <boost/smart_ptr/make_shared.hpp>
#	define PSYQ_SHARED_PTR boost::shared_ptr
#	define PSYQ_WEAK_PTR boost::weak_ptr
#	define PSYQ_ALLOCATE_SHARED boost::allocate_shared
#	define PSYQ_MAKE_SHARED boost::make_shared
#endif // PSYQ_USER_DEFINED_MEMORY

namespace psyq
{
	class arena;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief memory����policy���class�B
 */
class psyq::arena:
	private boost::noncopyable
{
	typedef psyq::arena this_type;

//.............................................................................
public:
	typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	virtual ~arena()
	{
		// pass
	}

	explicit arena(char const* const i_name = PSYQ_ARENA_NAME_DEFAULT):
	name_(i_name)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	bool operator==(this_type const& i_right) const
	{
		return this->get_malloc() == i_right.get_malloc()
			&& this->get_free() == i_right.get_free();
	}

	bool operator!=(this_type const& i_right) const
	{
		return !this->operator==(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���memory�̋��Eoffset�l�Bbyte�P�ʁB
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset)
	{
		return (*this->get_malloc())(
			i_size, i_alignment, i_offset, this->get_name());
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	void deallocate(void* const i_memory, std::size_t const i_size)
	{
		(*this->get_free())(i_memory, i_size);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���ʖ����擾�B
	 */
	char const* get_name() const
	{
		return this->name_;
	}

	/** @brief memory���ʖ���ݒ�B
	    @param[in] i_name �ݒ肷��memory���ʖ��̕�����B
	 */
	void set_name(char const* const i_name)
	{
		this->name_ = i_name;
	}

	//-------------------------------------------------------------------------
	/** @brief malloc()�Ɏw��ł���memory�̍ő�size���擾�B
	    @return malloc()�Ɏw��ł���m�ۂł���memory�̍ő�size�Bbyte�P�ʁB
	 */
	virtual std::size_t get_max_size() const = 0;

//.............................................................................
protected:
	typedef void* (*malloc_function)(
		std::size_t const,
		std::size_t const,
		std::size_t const,
		char const* const);
	typedef void (*free_function)(void* const, std::size_t const);

	virtual this_type::malloc_function get_malloc() const = 0;

	virtual this_type::free_function get_free() const = 0;

//.............................................................................
private:
	char const* name_;
};

#endif // !PSYQ_ARENA_HPP_
