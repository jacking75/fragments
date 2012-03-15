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

	static std::size_t const _fixed_memory_pool_chunk_info_size = 2;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory����pool�B
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
		// chunk�����ׂĔj������B
		for (boost::uint8_t** i = this->chunk_begin; this->chunk_end != i; ++i)
		{
			this->destroy_chunk(*i);
		}
		t_memory_policy::deallocate(this->chunk_begin);
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
	chunk_begin(NULL),
	chunk_end(NULL),
	chunk_limit(NULL),
	chunk_allocator(NULL),
	chunk_deallocator(NULL),
	empty_chunk(NULL),
	block_size(i_block_size),
	chunk_alignment(i_chunk_alignment),
	chunk_offset(i_chunk_offset)
	{
		std::size_t const a_max_blocks(
			(i_chunk_size - psyq::_fixed_memory_pool_chunk_info_size)
				/ this->block_size);
		this->max_blocks = static_cast< boost::uint8_t >(
			a_max_blocks <= 0xff? a_max_blocks: 0xff);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	void* allocate(
		char const* const i_name = NULL)
	{
		if (NULL != this->chunk_allocator
			&& 0 < this->get_num_blocks(*this->chunk_allocator))
		{
			if (this->chunk_allocator == this->empty_chunk)
			{
				// �������memory���m�ۂ����ċ�chunk�ł͂Ȃ��Ȃ�̂ŁB
				this->empty_chunk = NULL;
			}
		}
		else if (NULL != this->empty_chunk)
		{
			// ��chunk������Ȃ�Amemory�m��chunk�ɐ؂�ւ���B
			this->chunk_allocator = this->empty_chunk;
			this->empty_chunk = NULL;
		}
		else if (!this->find_allocator(i_name))
		{
			// ��chunk�̊m�ۂɎ��s�����B
			return NULL;
		}
		return this->allocate_block();
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_hint
	 */
	bool deallocate(
		void* const i_memory)
	{
		if (NULL != i_memory)
		{
			// memory���chunk��T���Amemory���������B
			if (!this->find_deallocator(i_memory))
			{
				PSYQ_ASSERT(false);
				return false;
			}
			this->deallocate_block(i_memory);

			// memory���chunk����ɂȂ�����A��chunk�ɐ؂�ւ���B
			PSYQ_ASSERT(NULL != this->chunk_deallocator);
			if (this->max_blocks <= this->get_num_blocks(*this->chunk_deallocator))
			{
				this->destroy_empty_chunk();
			}
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
	/** @brief memory-block���Amemory�m��chunk����m�ۂ���B
	 */
	void* allocate_block() const
	{
		PSYQ_ASSERT(NULL != this->chunk_allocator);
		boost::uint8_t* const a_chunk(*this->chunk_allocator);
		boost::uint8_t& a_first_block(this->get_first_block(a_chunk));
		boost::uint8_t& a_num_blocks(this->get_num_blocks(a_chunk));
		PSYQ_ASSERT(0 < a_num_blocks);

		// ��block-list����擪��block�����o���B
		boost::uint8_t* const a_block(
			a_chunk + a_first_block * this->block_size);
		a_first_block = *a_block;
		--a_num_blocks;
		return a_block;
	}

	//-------------------------------------------------------------------------
	/** @brief memory-block���Amemory���chunk�ɖ߂��B
	 */
	void deallocate_block(
		void* const i_block)
	const
	{
		PSYQ_ASSERT(NULL != this->chunk_deallocator);
		boost::uint8_t* a_chunk(*this->chunk_deallocator);
		boost::uint8_t& a_first_block(this->get_first_block(a_chunk));
		boost::uint8_t& a_num_blocks(this->get_num_blocks(a_chunk));
		PSYQ_ASSERT(this->has_block(a_chunk, i_block));
		PSYQ_ASSERT(!this->find_empty_block(a_chunk, i_block));
		PSYQ_ASSERT(a_num_blocks < this->max_blocks);

		// �������block��index�ԍ����擾�B
		boost::uint8_t* const a_block(static_cast< boost::uint8_t* >(i_block));
		std::size_t const a_distance(a_block - a_chunk);
		PSYQ_ASSERT(a_distance % this->block_size == 0);
		boost::uint8_t const a_index(
			static_cast< boost::uint8_t >(a_distance / this->block_size));
		PSYQ_ASSERT(a_distance / this->block_size == a_index);
		PSYQ_ASSERT(0 == a_num_blocks || a_index != a_first_block);

		// �������block����block-list�̐擪�ɑ}���B
		*a_block = a_first_block;
		a_first_block = a_index;
		++a_num_blocks;
	}

	//-------------------------------------------------------------------------
	/** @brief ��block�̂���chunk��T���B
	 */
	bool find_allocator(
		char const* const i_name)
	{
		// ��block�̂���chunk��T���B
		for (boost::uint8_t** i = this->chunk_begin; this->chunk_end != i; ++i)
		{
			if (0 < this->get_num_blocks(*i))
			{
				this->chunk_allocator = i;
				return true;
			}
		}

		// chunk-container�������ς��Ȃ�A�g������B
		if (this->chunk_limit == this->chunk_end)
		{
			std::size_t const a_num(this->chunk_end - this->chunk_begin);
			std::size_t const a_capacity(0 < a_num? a_num * 3 / 2: 4);
			if (!this->create_chunk_container(a_capacity, i_name))
			{
				return false;
			}
		}

		// chunk-container�ɁA��chunk��ǉ�����B
		*this->chunk_end = this->create_chunk(i_name);
		if (NULL == *this->chunk_end)
		{
			return false;
		}
		this->chunk_allocator = this->chunk_end;
		this->chunk_deallocator = this->chunk_begin;
		++this->chunk_end;
		return true;
	}

	/** @brief memory���chunk��T���B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	bool find_deallocator(
		void const* const i_memory)
	{
		PSYQ_ASSERT(NULL != this->chunk_deallocator);
		if (this->has_block(*this->chunk_deallocator, i_memory))
		{
			return true;
		}

		// �������memory���܂�chunk���Achunk-container����T���B
		for (boost::uint8_t** i = this->chunk_begin; this->chunk_end != i; ++i)
		{
			if (this->has_block(*i, i_memory))
			{
				this->chunk_deallocator = i;
				return true;
			}
		}
		return false;
	}

	//-------------------------------------------------------------------------
	/** @brief chunk-container�����B
	    @param[in] i_capacity chunk-container�̍ő�e�ʁB
	    @param[in] i_name     debug�Ŏg�����߂�memory���ʖ��B
	 */
	bool create_chunk_container(
		std::size_t const i_capacity,
		char const* const i_name)
	{
		// �V����chunk_container�Ŏg��memory���m�ۂ���B
		PSYQ_ASSERT(0 < i_capacity);
		boost::uint8_t** const a_begin = static_cast< boost::uint8_t** >(
			t_memory_policy::allocate(
				i_capacity * sizeof(boost::uint8_t*),
				boost::alignment_of< boost::uint8_t* >::value,
				0,
				i_name));
		if (NULL == a_begin)
		{
			PSYQ_ASSERT(false);
			return false;
		}

		// ���݂�chunk-container����ڂ��ւ���B
		std::size_t const a_num(this->chunk_end - this->chunk_begin);
		for (std::size_t i = 0; i < a_num; ++i)
		{
			a_begin[i] = this->chunk_begin[i];
		}
		t_memory_policy::deallocate(this->chunk_begin);

		// �V����chunk-containre���\�z����B
		std::size_t const a_distance(a_begin - this->chunk_begin);
		if (NULL != this->chunk_allocator)
		{
			this->chunk_allocator += a_distance;
		}
		if (NULL != this->chunk_deallocator)
		{
			this->chunk_deallocator += a_distance;
		}
		if (NULL != this->empty_chunk)
		{
			this->empty_chunk += a_distance;
		}
		this->chunk_begin = a_begin;
		this->chunk_end = a_begin + a_num;
		this->chunk_limit = a_begin + i_capacity;
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief ��chunk��j������B
	 */
	void destroy_empty_chunk()
	{
		PSYQ_ASSERT(this->empty_chunk != this->chunk_deallocator);
		if (NULL != this->empty_chunk)
		{
			// ��chunk��chunk-container�̖����Ɉړ��B
			PSYQ_ASSERT(NULL != this->chunk_end);
			boost::uint8_t** const a_last_chunk = this->chunk_end - 1;
			if (a_last_chunk == this->chunk_deallocator)
			{
				this->chunk_deallocator = this->empty_chunk;
			}
			else if (a_last_chunk != this->empty_chunk)
			{
				std::swap(*this->empty_chunk, *a_last_chunk);
			}

			// chunk-container�̖�����j���B
			this->destroy_chunk(*a_last_chunk);
			--this->chunk_end;
			PSYQ_ASSERT(NULL != this->chunk_allocator);
			if (a_last_chunk == this->chunk_allocator
				|| this->get_num_blocks(*this->chunk_allocator) <= 0)
			{
				this->chunk_allocator = this->chunk_deallocator;
			}
		}
		this->empty_chunk = this->chunk_deallocator;
	}

	//-------------------------------------------------------------------------
	boost::uint8_t* create_chunk(
		char const* const i_name) const
	{
		// chunk�Ɏg��memory���m�ہB
		boost::uint8_t* const a_chunk(
			static_cast< boost::uint8_t* >(
				t_memory_policy::allocate(
					this->block_size * this->max_blocks
						+ psyq::_fixed_memory_pool_chunk_info_size,
					this->chunk_alignment,
					this->chunk_offset,
					i_name)));
		if (NULL == a_chunk)
		{
			PSYQ_ASSERT(false);
			return NULL;
		}

		// ��block-list���\�z�B
		this->get_first_block(a_chunk) = 0;
		this->get_num_blocks(a_chunk) =
			static_cast< boost::uint8_t >(this->max_blocks);
		boost::uint8_t* a_block(a_chunk);
		for (
			boost::uint8_t i = 0;
			i < this->max_blocks;
			a_block += this->block_size)
		{
			++i;
			*a_block = i;
		}
		return a_chunk;
	}

	void destroy_chunk(
		boost::uint8_t* const i_chunk)
	const
	{
		PSYQ_ASSERT(this->max_blocks <= this->get_num_blocks(i_chunk));
		t_memory_policy::deallocate(i_chunk);
	}

	//-------------------------------------------------------------------------
	boost::uint8_t& get_num_blocks(
		boost::uint8_t* const i_chunk)
	const
	{
		PSYQ_ASSERT(NULL != i_chunk);
		return i_chunk[this->block_size * this->max_blocks + 0];
	}

	boost::uint8_t& get_first_block(
		boost::uint8_t* const i_chunk)
	const
	{
		PSYQ_ASSERT(NULL != i_chunk);
		return i_chunk[this->block_size * this->max_blocks + 1];
	}

	bool has_block(
		boost::uint8_t* const i_chunk,
		void const* const     i_block)
	const
	{
		PSYQ_ASSERT(NULL != i_chunk);
		return i_chunk <= i_block
			&& i_block < i_chunk + this->block_size * this->max_blocks;
	}

	bool find_empty_block(
		boost::uint8_t* const i_chunk,
		void const* const     i_block)
	const
	{
		std::size_t a_index(this->get_first_block(i_chunk));
		for (std::size_t i = this->get_num_blocks(i_chunk); 0 < i; --i)
		{
			boost::uint8_t const* const a_block(
				i_chunk + this->block_size * a_index);
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

	//-------------------------------------------------------------------------
	boost::uint8_t** chunk_begin;       ///< chunk_container�̐擪�ʒu�B
	boost::uint8_t** chunk_end;         ///< chunk_container�̖����ʒu�B
	boost::uint8_t** chunk_limit;       ///< chunk_container�̌��E�ʒu�B
	boost::uint8_t** chunk_allocator;   ///< �Ō��block���m�ۂ���chunk�B
	boost::uint8_t** chunk_deallocator; ///< �Ō��block���������chunk�B
	boost::uint8_t** empty_chunk;       ///< �Sblock����ɂȂ��Ă���chunk�B
	std::size_t      block_size;        ///< block�̑傫���Bbyte�P�ʁB
	std::size_t      max_blocks;        ///< chunk������block�̍ő吔�B
	std::size_t      chunk_alignment;
	std::size_t      chunk_offset;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory����policy�B
    @tparam t_block_size      ���蓖�Ă�memory�̑傫���Bbyte�P�ʁB
    @tparam t_chunk_alignment memory-chunk�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_chunk_offset    memory-cnunk�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_chunk_size      memory-chunk�̍ő�size�Bbyte�P�ʁB
    @tparam t_memory_policy   memory����policy�B
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
	BOOST_STATIC_ASSERT(
		t_block_size <= t_chunk_size
			- psyq::_fixed_memory_pool_chunk_info_size);

//.............................................................................
public:
	typedef t_memory_policy memory_policy;

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
		std::size_t const i_alignment = sizeof(void*),
		std::size_t const i_offset = t_chunk_offset,
		char const* const i_name = NULL)
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
	static void* allocate(
		char const* const i_name = NULL)
	{
		return this_type::get_pool().allocate(i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	static void deallocate(
		void* const i_memory)
	{
		this_type::get_pool().deallocate(i_memory);
	}

	//-------------------------------------------------------------------------
	static std::size_t max_size()
	{
		return t_block_size;
	}

//.............................................................................
private:
	typedef psyq::fixed_memory_pool< t_memory_policy > memory_pool;

	static typename this_type::memory_pool& get_pool()
	{
		typedef psyq::singleton< typename this_type::memory_pool, this_type >
			singleton;
		return *singleton::construct(
			boost::in_place(
				t_block_size,
				t_chunk_alignment,
				t_chunk_offset,
				t_chunk_size));
	}

	fixed_memory_policy();
};

#endif // PSYQ_FIXED_MEMORY_HPP_
