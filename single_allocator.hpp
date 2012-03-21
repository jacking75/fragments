#ifndef PSYQ_SINGLE_ALLOCATOR_HPP_
#define PSYQ_SINGLE_ALLOCATOR_HPP_

namespace psyq
{
	template< typename, std::size_t, std::size_t, std::size_t, typename >
		class single_allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ��x�ɂЂƂ�instance���m�ۂ���Astd::allocator�݊��̊����q�B
        �z��͊m�ۂł��Ȃ��B
    @tparam t_value_type    �m�ۂ���instance�̌^�B
    @tparam t_alignment     instance�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset        instance�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size    memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_memory_policy ���ۂɎg��memory����policy�B
 */
template<
	typename    t_value_type,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_MEMORY_POLICY_CHUNK_SIZE_DEFAULT,
	typename    t_memory_policy = PSYQ_MEMORY_POLICY_DEFAULT >
class psyq::single_allocator:
	public psyq::allocator<
		t_value_type,
		psyq::fixed_memory_policy<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_memory_policy > >
{
	typedef psyq::single_allocator<
		t_value_type, t_alignment, t_offset, t_chunk_size, t_memory_policy >
			this_type;
	typedef psyq::allocator<
		t_value_type,
		psyq::fixed_memory_policy<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_memory_policy > >
				super_type;

//.............................................................................
public:
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		std::size_t t_other_chunk = t_chunk_size,
		typename    t_other_memory = t_memory_policy >
	struct rebind
	{
		typedef psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_other_memory >
				other;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit single_allocator(
		char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT):
	super_type(i_name),
	pool(super_type::memory_policy::get_pool())
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
			t_memory_policy > const&
				i_source):
	super_type(i_source.get_name()),
	pool(
		i_source.get_pool()->get_chunk_alignment() % t_alignment == 0
		&& sizeof(t_value_type) <= i_source.get_pool()->get_block_size()?
			const_cast< psyq::fixed_memory_pool< t_memory_policy >* >(
				i_source.get_pool()):
			super_type::memory_policy::get_pool())
	{
		// pass
	}

	//-------------------------------------------------------------------------
	this_type& operator=(this_type const& i_source)
	{
		PSYQ_ASSERT (*this == i_source);
		this->super_type::operator=(i_source);
		return *this;
	}

	template< typename t_other_type, std::size_t t_other_alignment >
	this_type& operator=(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_offset,
			t_chunk_size,
			t_memory_policy > const&
				i_source)
	{
		PSYQ_ASSERT (*this == i_source);
		this->super_type::operator=(i_source);
		return *this;
	}

	//-------------------------------------------------------------------------
	template< typename t_other_type, std::size_t t_other_alignment >
	bool operator==(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_offset,
			t_chunk_size,
			t_memory_policy > const&
				i_right)
	const
	{
		return this->super_type::operator==(i_right)
			&& this->get_pool() == i_right.get_pool();
	}

	template< typename t_other_type, std::size_t t_other_alignment >
	bool operator!=(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_offset,
			t_chunk_size,
			t_memory_policy > const&
				i_right)
	const
	{
		return !this->operator==(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_num       �m�ۂ���instance�̐��B
	    @param[in] i_alignment �m�ۂ���instance�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���instance�̋��Eoffset�l�Bbyte�P�ʁB
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate(
		typename super_type::size_type const i_num,
		std::size_t const                    i_alignment = t_alignment,
		std::size_t const                    i_offset = t_offset)
	{
		return static_cast< typename super_type::pointer >(
			this->pool->allocate(
				i_num * sizeof(t_value_type),
				i_alignment,
				i_offset,
				this->get_name()));
	}

	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate()
	{
		return static_cast< typename super_type::pointer >(
			this->pool->allocate(this->get_name()));
	}

	//-------------------------------------------------------------------------
	/** @brief intance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	void deallocate(
		typename super_type::pointer const   i_instance,
		typename super_type::size_type const i_num)
	{
		this->pool->deallocate(i_instance, i_num * sizeof(t_value_type));
	}

	void deallocate(
		typename super_type::pointer const i_instance)
	{
		this->pool->deallocate(i_instance);
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
		return (std::numeric_limits< std::size_t >::max)()
			/ sizeof(t_value_type);
	}
#endif // _MSC_VER

	//-------------------------------------------------------------------------
	psyq::fixed_memory_pool< t_memory_policy >* get_pool()
	{
		return this->pool;
	}

	psyq::fixed_memory_pool< t_memory_policy > const* get_pool() const
	{
		return this->pool;
	}

//.............................................................................
private:
	psyq::fixed_memory_pool< t_memory_policy >* const pool;
};

#endif // PSYQ_SINGLE_ALLOCATOR_HPP_
