#ifndef PSYQ_FIXED_MEMORY_TABLE_HPP_
#define PSYQ_FIXED_MEMORY_TABLE_HPP_

namespace psyq
{
	template< typename > class fixed_memory_table;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_allocator_policy >
class psyq::fixed_memory_table:
	private boost::noncopyable
{
	typedef psyq::fixed_memory_table< t_allocator_policy > this_type;

//.............................................................................
public:
	typedef t_allocator_policy allocator_policy;

	//-------------------------------------------------------------------------
	virtual ~fixed_memory_table()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	void* allocate(
		std::size_t const i_size,
		char const* const i_name)
	{
		psyq::fixed_memory_pool< t_allocator_policy >* const a_pool(
			this->get_pool(this->get_pool_index(i_size)));
		if (NULL != a_pool)
		{
			// ���K��size��pool����memory���m�ہB
			return a_pool->allocate(i_name);
		}
		else if (0 < i_size)
		{
			// ���K��size���傫��memory�́At_allocator_policy����m�ہB
			return (t_allocator_policy::malloc)(
				i_size, this->get_alignment(), this->get_offset(), i_name);
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		psyq::fixed_memory_pool< t_allocator_policy >* const a_pool(
			this->get_pool(this->get_pool_index(i_size)));
		if (NULL != a_pool)
		{
			// ���K��size��pool��memory������B
			a_pool->deallocate(i_memory);
		}
		else if (0 < i_size)
		{
			// ���K��size���傫��memory�́At_allocator_policy�ŉ���B
			(t_allocator_policy::free)(i_memory, i_size);
		}
	}

	//-------------------------------------------------------------------------
	std::size_t get_pool_index(std::size_t const i_size) const
	{
		if (0 < i_size)
		{
			std::size_t const a_index((i_size - 1) / this->get_alignment());
			if (a_index < this->get_num_pools())
			{
				return a_index;
			}
		}
		return (std::numeric_limits< std::size_t >::max)();
	}

	//-------------------------------------------------------------------------
	virtual psyq::fixed_memory_pool< t_allocator_policy > const* get_pool(
		std::size_t const i_index)
	const = 0;

	psyq::fixed_memory_pool< t_allocator_policy >* get_pool(
		std::size_t const i_index)
	{
		return const_cast< psyq::fixed_memory_pool< t_allocator_policy >* >(
			const_cast< this_type const* >(this)->get_pool(i_index));
	}

	//-------------------------------------------------------------------------
	virtual std::size_t get_alignment() const = 0;
	virtual std::size_t get_offset() const = 0;
	virtual std::size_t get_num_pools() const = 0;

//.............................................................................
protected:
	fixed_memory_table()
	{
		// pass
	}
};

#endif // !PSYQ_FIXED_MEMORY_TABLE_HPP_
