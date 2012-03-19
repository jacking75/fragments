#ifndef PSYQ_FIXED_MEMORY_HPP_
#define PSYQ_FIXED_MEMORY_HPP_

#ifndef PSYQ_FIXED_MEMORY_POLICY_CHUNK_SIZE_DEFAULT
#define PSYQ_FIXED_MEMORY_POLICY_CHUNK_SIZE_DEFAULT 4096
#endif // !PSYQ_FIXED_MEMORY_POLICY_CHUNK_SIZE_DEFAULT

namespace psyq
{
	template< typename > class fixed_memory_pool;

	template< std::size_t, std::size_t, std::size_t, std::size_t, typename >
		class fixed_memory_policy;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory-pool�B
    @tparam t_memory_policy memory����policy�B
 */
template< typename t_memory_policy = PSYQ_MEMORY_POLICY_DEFAULT >
class psyq::fixed_memory_pool:
	private boost::noncopyable
{
	typedef psyq::fixed_memory_pool< t_memory_policy > this_type;

//.............................................................................
public:
	typedef t_memory_policy memory_policy;

	//-------------------------------------------------------------------------
	~fixed_memory_pool()
	{
		if (NULL != this->chunk_container)
		{
			// ��chunk��j������B
			PSYQ_ASSERT(this->chunk_container->next == this->chunk_container);
			this->destroy_chunk(*this->chunk_container);
		}
	}

	//-------------------------------------------------------------------------
	/** @param[in] i_block_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_chunk_alignment memory-chunk�̔z�u���E�l�B
	    @param[in] i_chunk_offset    memory-chunk�̔z�uoffset�l�B
	    @param[in] i_chunk_size      memory-chunk�̍ő�l�Bbyte�P�ʁB
	 */
	fixed_memory_pool(
		std::size_t const i_block_size,
		std::size_t const i_chunk_alignment,
		std::size_t const i_chunk_offset,
		std::size_t const i_chunk_size):
	chunk_container(NULL),
	allocator_chunk(NULL),
	deallocator_chunk(NULL),
	empty_chunk(NULL),
	block_size(i_block_size),
	chunk_alignment(i_chunk_alignment),
	chunk_offset(i_chunk_offset)
	{
		PSYQ_ASSERT(0 < i_block_size);
		std::size_t const a_alignment(
			boost::alignment_of< typename this_type::chunk >::value);
		std::size_t a_chunk_size(
			i_chunk_offset - sizeof(typename this_type::chunk)
				+ a_alignment * (
					(i_chunk_size - i_chunk_offset) / a_alignment));
		PSYQ_ASSERT(a_chunk_size <= i_chunk_size);

		// chunk������block�̐�������B
		std::size_t const a_max_blocks(a_chunk_size / this->block_size);
		PSYQ_ASSERT(0 < a_max_blocks);
		this->max_blocks = static_cast< boost::uint8_t >(
			a_max_blocks <= 0xff? a_max_blocks: 0xff);

		// chunk�̑傫��������B
		a_chunk_size = this->max_blocks * i_block_size;
		PSYQ_ASSERT(i_chunk_offset < a_chunk_size);
		this->chunk_size = i_chunk_offset + a_alignment * (
			(a_chunk_size - i_chunk_offset + a_alignment - 1) / a_alignment);
		PSYQ_ASSERT(
			this->chunk_size <= i_chunk_size
				- sizeof(typename this_type::chunk));
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	void* allocate(char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT)
	{
		// memory�m��chunk������B
		if (NULL != this->allocator_chunk)
		{
			if (this->empty_chunk == this->allocator_chunk)
			{
				// �������memory���m�ۂ����ċ�chunk�ł͂Ȃ��Ȃ�̂ŁB
				this->empty_chunk = NULL;
			}
		}
		else if (NULL != this->empty_chunk)
		{
			// ��chunk������Ȃ�Amemory�m��chunk�ɐ؂�ւ���B
			this->allocator_chunk = this->empty_chunk;
			this->empty_chunk = NULL;
		}
		else if (!this->find_allocator() && !this->create_chunk(i_name))
		{
			return NULL;
		}
		PSYQ_ASSERT(NULL != this->allocator_chunk);
		typename this_type::chunk& a_chunk(*this->allocator_chunk);
		PSYQ_ASSERT(0 < a_chunk.num_blocks);

		// ��block-list����擪��block�����o���B
		boost::uint8_t* const a_block(
			this->get_chunk_begin(a_chunk)
				+ a_chunk.first_block * this->block_size);
		a_chunk.first_block = *a_block;
		--a_chunk.num_blocks;

		// ��block���Ȃ��Ȃ�����Amemory�m��chunk�𖳌��ɂ���B
		if (a_chunk.num_blocks <= 0)
		{
			this->allocator_chunk = NULL;
		}
		return a_block;
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	bool deallocate(void* const i_memory)
	{
		if (NULL == i_memory)
		{
			return true;
		}

		// memory���chunk������B
		if (!this->find_deallocator(i_memory))
		{
			PSYQ_ASSERT(false);
			return false;
		}
		PSYQ_ASSERT(NULL != this->deallocator_chunk);
		typename this_type::chunk& a_chunk(*this->deallocator_chunk);
		PSYQ_ASSERT(!this->find_empty_block(a_chunk, i_memory));
		PSYQ_ASSERT(a_chunk.num_blocks < this->max_blocks);

		// �������block��index�ԍ����擾�B
		boost::uint8_t* const a_block(static_cast< boost::uint8_t* >(i_memory));
		std::size_t const a_distance(a_block - this->get_chunk_begin(a_chunk));
		PSYQ_ASSERT(a_distance % this->block_size == 0);
		boost::uint8_t const a_index(
			static_cast< boost::uint8_t >(a_distance / this->block_size));
		PSYQ_ASSERT(a_distance / this->block_size == a_index);
		PSYQ_ASSERT(0 == a_chunk.num_blocks || a_index != a_chunk.first_block);

		// �������block����block-list�̐擪�ɑ}���B
		*a_block = a_chunk.first_block;
		a_chunk.first_block = a_index;
		++a_chunk.num_blocks;

		// memory���chunk����ɂȂ�����A��chunk�ɓ]�p����B
		if (&a_chunk != this->empty_chunk
			&& this->max_blocks <= a_chunk.num_blocks)
		{
			this->destroy_empty_chunk();
			this->empty_chunk = &a_chunk;
		}
		return true;
	}

	//-------------------------------------------------------------------------
	std::size_t get_block_size() const
	{
		return this->block_size;
	}

	std::size_t get_chunk_alignment() const
	{
		return this->chunk_alignment;
	}

	std::size_t get_chunk_offset() const
	{
		return this->chunk_offset;
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	struct chunk
	{
		chunk*         next;
		chunk*         prev;
		boost::uint8_t num_blocks;
		boost::uint8_t first_block;
	};

	//-------------------------------------------------------------------------
	/** @brief ��block�̂���chunk��T���B
	 */
	bool find_allocator()
	{
		if (NULL == this->chunk_container)
		{
			return false;
		}

		// chunk-container�����ׂđ������ĒT���B
		typename this_type::chunk* const a_first(
			NULL != this->deallocator_chunk?
				this->deallocator_chunk: this->chunk_container);
		for (typename this_type::chunk* i = a_first;;)
		{
			if (0 < i->num_blocks)
			{
				this->allocator_chunk = i;
				return true;
			}
			i = i->next;
			if (a_first == i)
			{
				return false;
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief memory���chunk��T���B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	bool find_deallocator(void const* const i_memory)
	{
		if (NULL == this->chunk_container)
		{
			return false;
		}

		// �������memory���܂�chunk���Achunk-container����o�����ɒT���B
		typename this_type::chunk* a_next(
			NULL != this->deallocator_chunk?
				this->deallocator_chunk: this->chunk_container);
		typename this_type::chunk* a_prev(a_next->prev);
		for (;;)
		{
			// �������Ɍ����B
			if (this->has_block(*a_next, i_memory))
			{
				this->deallocator_chunk = a_next;
				return true;
			}
			else if (a_next == a_prev)
			{
				return false;
			}
			a_next = a_next->next;

			// �t�����Ɍ����B
			if (this->has_block(*a_prev, i_memory))
			{
				this->deallocator_chunk = a_prev;
				return true;
			}
			else if (a_prev == a_next)
			{
				return false;
			}
			a_prev = a_prev->prev;
		}
	}

	//-------------------------------------------------------------------------
	/** @brief ��chunk��j������B
	 */
	void destroy_empty_chunk()
	{
		if (NULL != this->empty_chunk)
		{
			if (this->empty_chunk == this->chunk_container)
			{
				this->chunk_container = this->chunk_container->next;
			}
			if (this->empty_chunk == this->allocator_chunk)
			{
				this->allocator_chunk = this->deallocator_chunk;
			}

			// ��chunk��chunk-container����؂藣���A�j������B
			this->empty_chunk->prev->next = this->empty_chunk->next;
			this->empty_chunk->next->prev = this->empty_chunk->prev;
			this->destroy_chunk(*this->empty_chunk);
		}
	}

	//-------------------------------------------------------------------------
	bool create_chunk(char const* const i_name)
	{
		// chunk�Ɏg��memory���m�ہB
		void* const a_memory(
			t_memory_policy::allocate(
				this->chunk_size + sizeof(typename this_type::chunk),
				this->chunk_alignment,
				this->chunk_offset,
				i_name));
		if (NULL == a_memory)
		{
			PSYQ_ASSERT(false);
			return false;
		}

		// chunk���\�z�B
		boost::uint8_t* a_block(static_cast< boost::uint8_t* >(a_memory));
		typename this_type::chunk& a_chunk(
			*static_cast< typename this_type::chunk* >(
				static_cast< void* >(a_block + this->chunk_size)));
		PSYQ_ASSERT(
			0 == reinterpret_cast< std::size_t >(&a_chunk)
				% boost::alignment_of< chunk >::value);
		a_chunk.first_block = 0;
		a_chunk.num_blocks = static_cast< boost::uint8_t >(this->max_blocks);

		// ��block-list���\�z�B
		std::size_t const a_max_blocks(this->max_blocks);
		std::size_t const a_block_size(this->block_size);
		for (boost::uint8_t i = 0; i < a_max_blocks; a_block += a_block_size)
		{
			++i;
			*a_block = i;
		}

		// chunk-container�̐擪�ɑ}���B
		if (NULL != this->chunk_container)
		{
			a_chunk.next = this->chunk_container;
			a_chunk.prev = this->chunk_container->prev;
			this->chunk_container->prev->next = &a_chunk;
			this->chunk_container->prev = &a_chunk;
		}
		else
		{
			a_chunk.next = &a_chunk;
			a_chunk.prev = &a_chunk;
		}
		this->chunk_container = &a_chunk;
		this->allocator_chunk = &a_chunk;
		return true;
	}

	//-------------------------------------------------------------------------
	void destroy_chunk(typename this_type::chunk& i_chunk)
	{
		PSYQ_ASSERT(this->max_blocks <= i_chunk.num_blocks);
		t_memory_policy::deallocate(
			reinterpret_cast< boost::uint8_t* >(&i_chunk) - this->chunk_size,
			this->chunk_size + sizeof(typename this_type::chunk));
	}

	//-------------------------------------------------------------------------
	/** @brief memory-chunk�Ɋ܂܂�Ă���memory-block������B
	 */
	bool has_block(
		typename this_type::chunk& i_chunk,
		void const* const          i_block)
	const
	{
		return this->get_chunk_begin(i_chunk) <= i_block && i_block < &i_chunk;
	}

	/** @brief memory-chunk�Ɋ܂܂�Ă����memory-block������B
	 */
	bool find_empty_block(
		typename this_type::chunk& i_chunk,
		void const* const          i_block)
	const
	{
		std::size_t a_index(i_chunk.first_block);
		for (std::size_t i = i_chunk.num_blocks; 0 < i; --i)
		{
			boost::uint8_t const* const a_block(
				this->get_chunk_begin(i_chunk) + this->block_size * a_index);
			if (i_block != a_block)
			{
				a_index = *a_block;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	/** @brief memory-chunk�̐擪�ʒu���擾�B
	 */
	boost::uint8_t* get_chunk_begin(typename this_type::chunk& i_chunk) const
	{
		return reinterpret_cast< boost::uint8_t* >(&i_chunk) - this->chunk_size;
	}

	//-------------------------------------------------------------------------
	typename this_type::chunk* chunk_container;
	typename this_type::chunk* allocator_chunk;
	typename this_type::chunk* deallocator_chunk;
	typename this_type::chunk* empty_chunk;
	std::size_t                block_size;
	std::size_t                max_blocks;
	std::size_t                chunk_alignment;
	std::size_t                chunk_offset;
	std::size_t                chunk_size;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory����policy�B
    @tparam t_block_size      ���蓖�Ă�memory�̑傫���Bbyte�P�ʁB
    @tparam t_chunk_alignment memory-chunk�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_chunk_offset    memory-cnunk�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size      memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_memory_policy   ���ۂɎg��memory����policy�B
 */
template<
	std::size_t t_block_size,
	std::size_t t_chunk_alignment = sizeof(void*),
	std::size_t t_chunk_offset = 0,
	std::size_t t_chunk_size = PSYQ_FIXED_MEMORY_POLICY_CHUNK_SIZE_DEFAULT,
	typename    t_memory_policy = PSYQ_MEMORY_POLICY_DEFAULT >
class psyq::fixed_memory_policy:
	private boost::noncopyable
{
	typedef fixed_memory_policy<
		t_block_size,
		t_chunk_alignment,
		t_chunk_offset,
		t_chunk_size,
		t_memory_policy >
			this_type;

	// memory�z�u���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 == (t_chunk_alignment & (t_chunk_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_chunk_alignment);

	// ���蓖�Ă�memory��chunk�Ɏ��܂邩�m�F�B
	BOOST_STATIC_ASSERT(t_chunk_offset < t_chunk_size);
#if 0
	BOOST_STATIC_ASSERT(
		t_block_size <= t_chunk_size
			- sizeof(typename this_type::pool::chunk));
#endif // 0

//.............................................................................
public:
	typedef t_memory_policy memory_policy;
	typedef psyq::fixed_memory_pool< t_memory_policy > pool;

	//-------------------------------------------------------------------------
	static std::size_t const block_size = t_block_size;
	static std::size_t const chunk_alignment = t_chunk_alignment;
	static std::size_t const chunk_offset = t_chunk_offset;
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
		std::size_t const i_alignment =
			t_block_size < sizeof(void*)? t_block_size: sizeof(void*),
		std::size_t const i_offset = t_chunk_offset,
		char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT)
	{
		return i_size <= t_block_size
			&& 0 < i_size
			&& 0 < i_alignment
			&& 0 == t_chunk_alignment % i_alignment
			&& 0 == t_block_size % i_alignment
			&& t_chunk_offset == i_offset?
				this_type::allocate(i_name): NULL;
	}

	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* allocate(char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT)
	{
		return this_type::get_pool()->allocate(i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	static void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		if (0 < i_size)
		{
			PSYQ_ASSERT(i_size <= t_block_size);
			this_type::deallocate(i_memory);
		}
		else
		{
			PSYQ_ASSERT(NULL == i_memory);
		}
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	static void deallocate(void* const i_memory)
	{
		this_type::get_pool()->deallocate(i_memory);
	}

	//-------------------------------------------------------------------------
	/** @brief ��x�Ɋm�ۂł���memory�̍ő�size���擾�B
	 */
	static std::size_t max_size()
	{
		return t_block_size;
	}

	//-------------------------------------------------------------------------
	/** @brief memory�Ǘ��Ɏg���Ă���singleton-pool���擾�B
	 */
	static typename this_type::pool* get_pool()
	{
		typedef psyq::singleton< typename this_type::pool, this_type >
			singleton;
		return singleton::construct(
			boost::in_place(
				t_block_size,
				t_chunk_alignment,
				t_chunk_offset,
				t_chunk_size));
	}

//.............................................................................
private:
	fixed_memory_policy();
};

#endif // PSYQ_FIXED_MEMORY_HPP_
