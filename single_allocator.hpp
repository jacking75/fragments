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
    @tparam t_alignment     instance�z�u���E�l�B
    @tparam t_offset        instance�z�uoffset�l�B
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
	public psyq::allocator< t_value_type, t_memory_policy >
{
	typedef psyq::single_allocator<
		t_value_type, t_alignment, t_offset, t_chunk_size, t_memory_policy >
			this_type;
	typedef psyq::allocator< t_value_type, t_memory_policy > super_type;

//.............................................................................
public:
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		std::size_t t_other_chunk = t_chunk_size >
	struct rebind
	{
		typedef single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_memory_policy >
				other;
	};

	typedef psyq::fixed_memory_policy<
		((sizeof(t_value_type) + t_alignment - 1) / t_alignment) * t_alignment,
		t_alignment,
		t_offset,
		t_chunk_size,
		t_memory_policy >
			memory_policy;

	//-------------------------------------------------------------------------
	explicit single_allocator(
		char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT):
	super_type(i_name)
	{
		// pass
	}

	single_allocator(this_type const& i_source):
	super_type(i_source)
	{
		// pass
	}

	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		std::size_t t_other_chunk >
	single_allocator(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_memory_policy > const&
				i_source):
	super_type(i_source)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	this_type& operator=(this_type const& i_source)
	{
		this->super_type::operator=(i_source);
		return *this;
	}

	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		std::size_t t_other_chunk >
	this_type& operator=(
		psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_memory_policy > const&
				i_source)
	{
		this->super_type::operator=(i_source);
		return *this;
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
		return 1 == i_num
			&& 0 < i_alignment
			&& 0 == t_alignment % i_alignment
			&& 0 == this_type::memory_policy::block_size % i_alignment
			&& t_offset == i_offset?
				this_type::allocate(): NULL;
	}

	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate()
	{
		return static_cast< typename super_type::pointer >(
			this_type::memory_policy::allocate(this->get_name()));
	}

	//-------------------------------------------------------------------------
	/** @brief intance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	void deallocate(
		typename super_type::pointer const   i_instance,
		typename super_type::size_type const i_num = 1)
	{
		if (1 == i_num)
		{
			this_type::memory_policy::deallocate(i_instance);
		}
		else
		{
			PSYQ_ASSERT(0 == i_num && NULL == i_instance);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �m�ۂł���instance�̍ő吔��Ԃ��B
	    @warning
	      max_size()�̕Ԃ�l�́Aallocate()�Ŏw��ł���instance���̍ő�l��
	      C++�̎d�l�Ō��߂��Ă���B
	      �Ƃ��낪VC++�ɓY�t����Ă�STL�̎����͂��̂悤�ɂȂ��Ă��炸�A
	      memory��ɑ��݂ł���instance�̍ő吔��Ԃ��悤�Ɏ�������Ă���B
	      ���̂���VC++�̏ꍇ�́Asuper_type:::max_size()���g�����Ƃɂ���B
	      http://msdn.microsoft.com/en-us/library/h36se6sf.aspx
	 */
#ifndef _MSC_VER
	static typename super_type::size_type max_size()
	{
		return 1;
	}
#endif // !_MSC_VER

	//-------------------------------------------------------------------------
	/** @brief memory�Ǘ��Ɏg���Ă���singleton-pool���擾�B
	 */
	static typename this_type::memory_policy::pool& get_pool()
	{
		return this_type::memory_policy::get_pool();
	}
};

#endif // PSYQ_SINGLE_ALLOCATOR_HPP_
