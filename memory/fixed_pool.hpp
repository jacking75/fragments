#ifndef PSYQ_FIXED_POOL_HPP_
#define PSYQ_FIXED_POOL_HPP_

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>

namespace psyq
{
	template< typename, typename > class fixed_pool;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Œ�size��memory-pool�B
    @tparam t_arena memory����policy�B
    @tparam t_mutex multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template< typename t_arena, typename t_mutex >
class psyq::fixed_pool:
	private boost::noncopyable
{
	public: typedef psyq::fixed_pool< t_arena, t_mutex > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_arena arena;

	//-------------------------------------------------------------------------
	private: struct chunk
	{
		chunk*         next;
		chunk*         prev;
		boost::uint8_t num_blocks;
		boost::uint8_t first_block;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_block_size �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment  memory�̔z�u���E�l�B
	    @param[in] i_offset     memory�̔z�uoffset�l�B
	    @param[in] i_chunk_size memory-chunk�̍ő�l�Bbyte�P�ʁB
	 */
	public: fixed_pool(
		std::size_t const i_block_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		std::size_t const i_chunk_size):
	chunk_container_(NULL),
	allocator_chunk_(NULL),
	deallocator_chunk_(NULL),
	empty_chunk_(NULL),
	block_size_(i_block_size),
	alignment_(i_alignment),
	offset_(i_offset)
	{
		PSYQ_ASSERT(0 == (i_alignment & (i_alignment - 1)));
		PSYQ_ASSERT(0 < i_alignment);
		PSYQ_ASSERT(i_block_size % i_alignment == 0);

		// chunk������block�̐�������B
		PSYQ_ASSERT(0 < i_block_size);
		PSYQ_ASSERT(i_block_size <= i_chunk_size);
		std::size_t const a_max_blocks(i_chunk_size / i_block_size);
		this->max_blocks_ = static_cast< boost::uint8_t >(
			a_max_blocks <= 0xff? a_max_blocks: 0xff);

		// chunk�̑傫��������B
		std::size_t const a_alignment(
			boost::alignment_of< typename this_type::chunk >::value);
		PSYQ_ASSERT(
			i_alignment % a_alignment == 0 || a_alignment % i_alignment == 0);
		this->chunk_size_ = i_offset + a_alignment * (
			(this->max_blocks_ * i_block_size - i_offset + a_alignment - 1)
			/ a_alignment);
	}

	//-------------------------------------------------------------------------
	public: ~fixed_pool()
	{
		if (NULL != this->chunk_container_)
		{
			// ��chunk��j������B
			PSYQ_ASSERT(this->chunk_container_->next == this->chunk_container_);
			this->destroy_chunk(*this->chunk_container_);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	public: void* allocate(char const* const i_name)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);

		// memory�m��chunk������B
		if (NULL != this->allocator_chunk_)
		{
			if (this->empty_chunk_ == this->allocator_chunk_)
			{
				// �������memory���m�ۂ����ċ�chunk�ł͂Ȃ��Ȃ�̂ŁB
				this->empty_chunk_ = NULL;
			}
		}
		else if (NULL != this->empty_chunk_)
		{
			// ��chunk������Ȃ�Amemory�m��chunk�ɐ؂�ւ���B
			this->allocator_chunk_ = this->empty_chunk_;
			this->empty_chunk_ = NULL;
		}
		else if (!this->find_allocator() && !this->create_chunk(i_name))
		{
			return NULL;
		}
		PSYQ_ASSERT(NULL != this->allocator_chunk_);
		typename this_type::chunk& a_chunk(*this->allocator_chunk_);
		PSYQ_ASSERT(0 < a_chunk.num_blocks);

		// ��block-list����擪��block�����o���B
		boost::uint8_t* const a_block(
			this->get_chunk_begin(a_chunk)
				+ a_chunk.first_block * this->block_size_);
		a_chunk.first_block = *a_block;
		--a_chunk.num_blocks;

		// ��block���Ȃ��Ȃ�����Amemory�m��chunk�𖳌��ɂ���B
		if (a_chunk.num_blocks <= 0)
		{
			this->allocator_chunk_ = NULL;
		}
		return a_block;
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	 */
	public: bool deallocate(void* const i_memory)
	{
		if (NULL == i_memory)
		{
			return true;
		}
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);

		// memory���chunk������B
		if (!this->find_deallocator(i_memory))
		{
			PSYQ_ASSERT(false);
			return false;
		}
		PSYQ_ASSERT(NULL != this->deallocator_chunk_);
		typename this_type::chunk& a_chunk(*this->deallocator_chunk_);
		PSYQ_ASSERT(!this->find_empty_block(a_chunk, i_memory));
		PSYQ_ASSERT(a_chunk.num_blocks < this->max_blocks_);

		// �������block��index�ԍ����擾�B
		boost::uint8_t* const a_block(
			static_cast< boost::uint8_t* >(i_memory));
		std::size_t const a_distance(
			a_block - this->get_chunk_begin(a_chunk));
		PSYQ_ASSERT(a_distance % this->block_size_ == 0);
		boost::uint8_t const a_index(
			static_cast< boost::uint8_t >(a_distance / this->block_size_));
		PSYQ_ASSERT(a_distance / this->block_size_ == a_index);
		PSYQ_ASSERT(0 == a_chunk.num_blocks || a_index != a_chunk.first_block);

		// �������block����block-list�̐擪�ɑ}���B
		*a_block = a_chunk.first_block;
		a_chunk.first_block = a_index;
		++a_chunk.num_blocks;

		// memory���chunk����ɂȂ�����A��chunk�ɓ]�p����B
		if (&a_chunk != this->empty_chunk_
			&& this->max_blocks_ <= a_chunk.num_blocks)
		{
			this->destroy_empty_chunk();
			this->empty_chunk_ = &a_chunk;
		}
		return true;
	}

	//-------------------------------------------------------------------------
	/** @brief �m�ۂ���memory�̑傫�����擾�B
	    @return �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	 */
	public: std::size_t get_block_size() const
	{
		return this->block_size_;
	}

	/** @brief �m�ۂ���memory�̔z�u���E�l���擾�B
	    @return �m�ۂ���memory�̔z�u���E�l�Bbyte�P�ʁB
	 */
	public: std::size_t get_alignment() const
	{
		return this->alignment_;
	}

	/** @brief �m�ۂ���memory�̔z�uoffset�l���擾�B
	    @return �m�ۂ���memory�̔z�uoffset�l�Bbyte�P�ʁB
	 */
	public: std::size_t get_offset() const
	{
		return this->offset_;
	}

	//-------------------------------------------------------------------------
	/** @brief ��block�̂���chunk��T���B
	 */
	private: bool find_allocator()
	{
		if (NULL == this->chunk_container_)
		{
			return false;
		}

		// chunk-container�����ׂđ������ĒT���B
		typename this_type::chunk* const a_first(
			NULL != this->deallocator_chunk_?
				this->deallocator_chunk_: this->chunk_container_);
		for (typename this_type::chunk* i = a_first;;)
		{
			if (0 < i->num_blocks)
			{
				this->allocator_chunk_ = i;
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
	private: bool find_deallocator(void const* const i_memory)
	{
		if (NULL == this->chunk_container_)
		{
			return false;
		}

		// �������memory���܂�chunk���Achunk-container����o�����ɒT���B
		typename this_type::chunk* a_next(
			NULL != this->deallocator_chunk_?
				this->deallocator_chunk_: this->chunk_container_);
		typename this_type::chunk* a_prev(a_next->prev);
		for (;;)
		{
			// �������Ɍ����B
			if (this->has_block(*a_next, i_memory))
			{
				this->deallocator_chunk_ = a_next;
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
				this->deallocator_chunk_ = a_prev;
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
	private: void destroy_empty_chunk()
	{
		if (NULL != this->empty_chunk_)
		{
			if (this->empty_chunk_ == this->chunk_container_)
			{
				this->chunk_container_ = this->chunk_container_->next;
			}
			if (this->empty_chunk_ == this->allocator_chunk_)
			{
				this->allocator_chunk_ = this->deallocator_chunk_;
			}

			// ��chunk��chunk-container����؂藣���A�j������B
			this->empty_chunk_->prev->next = this->empty_chunk_->next;
			this->empty_chunk_->next->prev = this->empty_chunk_->prev;
			this->destroy_chunk(*this->empty_chunk_);
		}
	}

	//-------------------------------------------------------------------------
	private: bool create_chunk(char const* const i_name)
	{
		// chunk�Ɏg��memory���m�ہB
		std::size_t const a_alignment(
			boost::alignment_of< typename this_type::chunk >::value);
		void* const a_memory(
			(t_arena::malloc)(
				this->chunk_size_ + sizeof(typename this_type::chunk),
				this->alignment_ < a_alignment? a_alignment: this->alignment_,
				this->offset_,
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
				static_cast< void* >(a_block + this->chunk_size_)));
		PSYQ_ASSERT(
			0 == reinterpret_cast< std::size_t >(&a_chunk)
				% boost::alignment_of< chunk >::value);
		a_chunk.first_block = 0;
		a_chunk.num_blocks = static_cast< boost::uint8_t >(this->max_blocks_);

		// ��block-list���\�z�B
		std::size_t const a_max_blocks(this->max_blocks_);
		std::size_t const a_block_size(this->block_size_);
		for (boost::uint8_t i = 0; i < a_max_blocks; a_block += a_block_size)
		{
			++i;
			*a_block = i;
		}

		// chunk-container�̐擪�ɑ}���B
		if (NULL != this->chunk_container_)
		{
			a_chunk.next = this->chunk_container_;
			a_chunk.prev = this->chunk_container_->prev;
			this->chunk_container_->prev->next = &a_chunk;
			this->chunk_container_->prev = &a_chunk;
		}
		else
		{
			a_chunk.next = &a_chunk;
			a_chunk.prev = &a_chunk;
		}
		this->chunk_container_ = &a_chunk;
		this->allocator_chunk_ = &a_chunk;
		return true;
	}

	//-------------------------------------------------------------------------
	private: void destroy_chunk(typename this_type::chunk& i_chunk)
	{
		PSYQ_ASSERT(this->max_blocks_ <= i_chunk.num_blocks);
		(t_arena::free)(
			reinterpret_cast< boost::uint8_t* >(&i_chunk) - this->chunk_size_,
			this->chunk_size_ + sizeof(typename this_type::chunk));
	}

	//-------------------------------------------------------------------------
	/** @brief memory-chunk�Ɋ܂܂�Ă���memory-block������B
	 */
	private: bool has_block(
		typename this_type::chunk& i_chunk,
		void const* const          i_block)
	const
	{
		return this->get_chunk_begin(i_chunk) <= i_block && i_block < &i_chunk;
	}

	/** @brief memory-chunk�Ɋ܂܂�Ă����memory-block������B
	 */
	private: bool find_empty_block(
		typename this_type::chunk& i_chunk,
		void const* const          i_block)
	const
	{
		std::size_t a_index(i_chunk.first_block);
		for (std::size_t i = i_chunk.num_blocks; 0 < i; --i)
		{
			boost::uint8_t const* const a_block(
				this->get_chunk_begin(i_chunk) + this->block_size_ * a_index);
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
	private: boost::uint8_t* get_chunk_begin(typename this_type::chunk& i_chunk) const
	{
		return reinterpret_cast< boost::uint8_t* >(&i_chunk) - this->chunk_size_;
	}

	//-------------------------------------------------------------------------
	private: typename this_type::chunk* chunk_container_;
	private: typename this_type::chunk* allocator_chunk_;
	private: typename this_type::chunk* deallocator_chunk_;
	private: typename this_type::chunk* empty_chunk_;
	private: std::size_t                block_size_;
	private: std::size_t                max_blocks_;
	private: std::size_t                alignment_;
	private: std::size_t                offset_;
	private: std::size_t                chunk_size_;
	private: t_mutex                    mutex_;
};

#endif // !PSYQ_FIXED_POOL_HPP_
