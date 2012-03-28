#ifndef PSYQ_SINGLE_ALLOCATOR_HPP_
#define PSYQ_SINGLE_ALLOCATOR_HPP_

#ifndef PSYQ_FIXED_ALLOCATOR_POLICY_CHUNK_SIZE_DEFAULT
#define PSYQ_FIXED_ALLOCATOR_POLICY_CHUNK_SIZE_DEFAULT 4096
#endif // !PSYQ_FIXED_ALLOCATOR_POLICY_CHUNK_SIZE_DEFAULT

namespace psyq
{
	template< std::size_t, std::size_t, std::size_t, std::size_t, typename >
		class fixed_allocator_policy;
	template< typename, std::size_t, std::size_t, std::size_t, typename >
		class single_allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory����policy�B
    @tparam t_block_size       ���蓖�Ă�memory�̑傫���Bbyte�P�ʁB
    @tparam t_alignment        memory�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset           memory�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size       memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_allocator_policy ���ۂɎg��memory����policy�B
 */
template<
	std::size_t t_block_size,
	std::size_t t_alignment = sizeof(void*),
	std::size_t t_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_ALLOCATOR_POLICY_CHUNK_SIZE_DEFAULT,
	typename    t_allocator_policy = PSYQ_ALLOCATOR_POLICY_DEFAULT >
class psyq::fixed_allocator_policy:
	private boost::noncopyable
{
	typedef fixed_allocator_policy<
		t_block_size,
		t_alignment,
		t_offset,
		t_chunk_size,
		t_allocator_policy >
			this_type;

	// memory�z�u���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);
	BOOST_STATIC_ASSERT(t_block_size % t_alignment == 0);

	// ���蓖�Ă�memory��chunk�Ɏ��܂邩�m�F�B
	BOOST_STATIC_ASSERT(0 < t_block_size);
	BOOST_STATIC_ASSERT(t_offset < t_chunk_size);
#if 0
	BOOST_STATIC_ASSERT(
		t_block_size <= t_chunk_size
			- sizeof(psyq::fixed_memory_pool< t_allocator_policy >::chunk));
#endif // 0

//.............................................................................
public:
	typedef t_allocator_policy allocator_policy;

	//-------------------------------------------------------------------------
	static std::size_t const block_size = t_block_size;
	static std::size_t const alignment  = t_alignment;
	static std::size_t const offset     = t_offset;
	static std::size_t const chunk_size = t_chunk_size;

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���memory�̋��Eoffset�l�Bbyte�P�ʁB
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		return 0 < i_size
			&& 0 < i_alignment
			&& t_offset == i_offset
			&& i_size <= t_block_size
			&& t_alignment % i_alignment == 0
			&& t_block_size % i_alignment == 0?
				this_type::allocate(i_name): NULL;
	}

	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* allocate(
		char const* const i_name)
	{
		return this_type::get_pool()->allocate(i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	static void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		if (0 < i_size && i_size <= t_block_size)
		{
			this_type::deallocate(i_memory);
		}
		else
		{
			PSYQ_ASSERT(0 == i_size && NULL == i_memory);
		}
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	static void deallocate(
		void* const i_memory)
	{
		this_type::get_pool()->deallocate(i_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief ��x�Ɋm�ۂł���memory�̍ő�size���擾�Bbyte�P�ʁB
	 */
	static std::size_t max_size()
	{
		return t_block_size;
	}

	//-------------------------------------------------------------------------
	/** @brief memory�Ǘ��Ɏg���Ă���singleton-pool���擾�B
	 */
	static psyq::fixed_memory_pool< t_allocator_policy >* get_pool()
	{
		typedef psyq::singleton<
			psyq::fixed_memory_pool< t_allocator_policy >, this_type >
				singleton;
		return singleton::construct(
			boost::in_place(
				t_block_size, t_alignment, t_offset, t_chunk_size));
	}

//.............................................................................
private:
	fixed_allocator_policy();
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ��x�ɂЂƂ�instance���m�ۂ���Astd::allocator�݊��̊����q�B
        �z��͊m�ۂł��Ȃ��B
    @tparam t_value_type    �m�ۂ���instance�̌^�B
    @tparam t_alignment     instance�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset        instance�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size    memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_allocator_policy ���ۂɎg��memory����policy�B
 */
template<
	typename    t_value_type,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_ALLOCATOR_POLICY_CHUNK_SIZE_DEFAULT,
	typename    t_allocator_policy = PSYQ_ALLOCATOR_POLICY_DEFAULT >
class psyq::single_allocator:
	public psyq::allocator<
		t_value_type,
		t_alignment,
		t_offset,
		psyq::fixed_allocator_policy<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_allocator_policy > >
{
	typedef psyq::single_allocator<
		t_value_type, t_alignment, t_offset, t_chunk_size, t_allocator_policy >
			this_type;
	typedef psyq::allocator<
		t_value_type,
		t_alignment,
		t_offset,
		psyq::fixed_allocator_policy<
			((sizeof(t_value_type) + t_alignment - 1) / t_alignment)
				* t_alignment,
			t_alignment,
			t_offset,
			t_chunk_size,
			t_allocator_policy > >
				super_type;

//.............................................................................
public:
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		std::size_t t_other_chunk = t_chunk_size,
		typename    t_other_policy = t_allocator_policy >
	struct rebind
	{
		typedef psyq::single_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_chunk,
			t_other_policy >
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
			t_allocator_policy > const&
				i_source):
	super_type(i_source)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	//this_type& operator=(this_type const&) = default;

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_num �m�ۂ���instance�̐��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate(
		typename super_type::size_type const i_num)
	{
		return this->super_type::allocate(i_num);
	}

	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename super_type::pointer allocate()
	{
		return static_cast< typename super_type::pointer >(
			super_type::allocator_policy::allocate(this->get_name()));
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
		this->super_type::deallocate(i_instance, i_num);
	}

	void deallocate(
		typename super_type::pointer const i_instance)
	{
		super_type::allocator_policy::get_pool()->deallocate(i_instance);
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
		return t_allocator_policy::max_size() / sizeof(t_value_type);
	}
#endif // _MSC_VER
};

#endif // PSYQ_SINGLE_ALLOCATOR_HPP_
