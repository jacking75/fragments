#ifndef PSYQ_ALLOCATOR_POLICY_HPP_
#define PSYQ_ALLOCATOR_POLICY_HPP_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace psyq
{
	class allocator_policy;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief memory����policy���class�B
 */
class psyq::allocator_policy:
	private boost::noncopyable
{
	typedef psyq::allocator_policy this_type;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > holder;
	typedef boost::weak_ptr< this_type > observer;

	//-------------------------------------------------------------------------
	virtual ~allocator_policy()
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
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		return (*this->get_malloc())(i_size, i_alignment, i_offset, i_name);
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		(*this->get_free())(i_memory, i_size);
	}

	//-------------------------------------------------------------------------
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
};

#endif // !PSYQ_ALLOCATOR_POLICY_HPP_
