#ifndef PSYQ_FIXED_MEMORY_HPP_
#define PSYQ_FIXED_MEMORY_HPP_

namespace psyq
{
	template< typename > class fixed_memory;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_memory = psyq::heap_memory >
class psyq::fixed_memory
{
	typedef psyq::fixed_memory< t_memory > this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~fixed_memory()
	{
		// chunk�����ׂĔj������B
		for (boost::uint8_t** i = this->chunk_begin; this->chunk_end != i; ++i)
		{
			this->destroy_chunk(*i);
		}
		t_memory::deallocate(this->chunk_begin);
	}

	//-------------------------------------------------------------------------
	/** @param[in] i_block_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_chunk_size      memory-chunk�̍ő�l�Bbyte�P�ʁB
	    @param[in] i_chunk_alignment memory-chunk�̔z�u���E�l�B
	    @param[in] i_chunk_offset    memory-chunk�̔z�uoffset�l�B
	 */
	fixed_memory(
		std::size_t const i_block_size,
		std::size_t const i_chunk_size,
		std::size_t const i_chunk_alignment,
		std::size_t const i_chunk_offset):
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
			(i_chunk_size - this_type::CHUNK_INFO_SIZE) / this->block_size);
		this->max_blocks = static_cast< boost::uint8_t >(
			a_max_blocks <= 0xff? a_max_blocks: 0xff);
	}

	//-------------------------------------------------------------------------
	std::size_t max_size() const
	{
		return this->block_size;
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

//.............................................................................
private:
	enum
	{
		CHUNK_INFO_SIZE = 2
	};

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
			if (!this->create_chunk_container(0 < a_num? a_num * 3 / 2: 4, i_name))
			{
				return false;
			}
		}

		// chunk-container�ɁA��chunk��ǉ�����B
		*this->chunk_end = this->create_chunk(i_name);
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
	bool create_chunk_container(
		std::size_t const i_capacity,
		char const* const i_name)
	{
		// �V����chunk_container�Ŏg��memory���m�ۂ���B
		PSYQ_ASSERT(0 < i_capacity);
		boost::uint8_t** const a_begin = static_cast< boost::uint8_t** >(
			t_memory::allocate(
				i_capacity * sizeof(boost::uint8_t*),
				boost::alignment_of< boost::uint8_t* >::value,
				0,
				i_name));
		if (NULL == a_begin)
		{
			return false;
		}

		// ���݂�chunk-container����ڂ��ւ���B
		std::size_t const a_num(this->chunk_end - this->chunk_begin);
		for (std::size_t i = 0; i < a_num; ++i)
		{
			a_begin[i] = this->chunk_begin[i];
		}
		t_memory::deallocate(this->chunk_begin);

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
		boost::uint8_t* const a_chunk(
			static_cast< boost::uint8_t* >(
				t_memory::allocate(
					this->block_size * this->max_blocks
						+ this_type::CHUNK_INFO_SIZE,
					this->chunk_alignment,
					this->chunk_offset,
					i_name)));

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
		t_memory::deallocate(i_chunk);
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
template<
	typename t_memory,
	std::size_t t_size,
	std::size_t t_chunk_size,
	std::size_t t_chunk_alignment,
	std::size_t t_chunk_offset >
class _fixed_memory_allocator:
	public psyq::fixed_memory< t_memory >
{
	typedef psyq::fixed_memory< t_memory > super_type;

public:
	_fixed_memory_allocator():
	super_type(t_size, t_chunk_size, t_chunk_alignment, t_chunk_offset)
	{
		// pass
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename    t_value_type,
	typename    t_memory = psyq::heap_memory,
	std::size_t t_chunk_size = 4096,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0 >
class single_object_allocator:
	public std::allocator< t_value_type >
{
	typedef single_object_allocator<
		t_value_type, t_memory, t_chunk_size, t_alignment, t_offset >
			this_type;
	typedef std::allocator< t_value_type > super_type;

//.............................................................................
public:
	typedef t_memory memory;

	template< class t_other_type >
	struct rebind
	{
		typedef single_object_allocator<
			t_other_type, t_memory, t_chunk_size, t_alignment, t_offset >
				other;
	};

	single_object_allocator():
	super_type()
	{
		// pass
	}

	static pointer allocate(
		size_type const   i_num,
		char const* const i_name = NULL)
	{
		return 1 == i_num?
			static_cast< pointer >(
				psyq::singleton< allocator >::get().allocate(i_name)):
			NULL;
	}

	static void deallocate(
		pointer const   i_object,
		size_type const i_num)
	{
		if (1 == i_num)
		{
			psyq::singleton< allocator >::get().deallocate(i_object);
		}
		else
		{
			PSYQ_ASSERT(0 == i_num && NULL == i_object);
		}
	}

	static size_type max_size()
	{
		return psyq::singleton< allocator >::get().max_size();
	}

	static std::size_t const chunk_size = t_chunk_size;
	static std::size_t const alignment = t_alignment;
	static std::size_t const offset = t_offset;

//.............................................................................
private:
	typedef _fixed_memory_allocator<
		t_memory,
		((sizeof(t_value_type) + t_alignment - 1) / t_alignment) * t_alignment,
		t_chunk_size,
		t_alignment,
		t_offset >
			allocator;
};

#endif // PSYQ_FIXED_MEMORY_HPP_
