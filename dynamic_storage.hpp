#ifndef PSYQ_DYNAMIC_STORAGE_HPP_
#define PSYQ_DYNAMIC_STORAGE_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type.hpp>

namespace psyq
{
	class dynamic_storage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ���I�Ɋm�ۂ���memory���Ǘ�����B
 */
class psyq::dynamic_storage:
	private boost::noncopyable
{
	public: typedef psyq::dynamic_storage this_type;

	//-------------------------------------------------------------------------
	public: dynamic_storage():
	deallocator_(NULL),
	address_(NULL),
	size_(0)
	{
		// pass
	}

	/** @param[in] i_allocator memory�̊m�ۂɎg�������q�B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	 */
	public: template< typename t_allocator >
	dynamic_storage(
		t_allocator const& i_allocator,
		std::size_t const  i_size)
	{
		new(this) this_type(
			boost::type< typename t_allocator::arena >(),
			i_size,
			t_allocator::ALIGNMENT,
			t_allocator::OFFSET,
			i_allocator.get_name());
	}

	/** @param[in] i_size	   �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̔z�u���E�l�B
	    @param[in] i_offset    �m�ۂ���memory�̔z�uoffset�l�B
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	 */
	public: template< typename t_arena >
	dynamic_storage(
		boost::type< t_arena > const&,
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		if (0 < i_size)
		{
			this->address_ = (t_arena::malloc)(
				i_size, i_alignment, i_offset, i_name);
			if (NULL != this->get_address())
			{
				this->size_ = i_size;
				this->deallocator_ = &t_arena::free;
				return;
			}
			PSYQ_ASSERT(false);
		}
		this->deallocator_ = NULL;
		this->address_ = NULL;
		this->size_ = 0;
	}

	//-------------------------------------------------------------------------
	public: ~dynamic_storage()
	{
		// �ێ����Ă���memory������B
		if (NULL != this->deallocator_)
		{
			(*this->deallocator_)(this->get_address(), this->get_size());
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���memory�������B
	    @param[in,out] io_target ��������ΏہB
	 */
	public: void swap(this_type& io_target)
	{
		std::swap(this->deallocator_, io_target.deallocator_);
		std::swap(this->address_, io_target.address_);
		std::swap(this->size_, io_target.size_);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ��āA�ێ�����B
	    @param[in] i_allocator memory�̊m�ۂɎg�������q�B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	public: template< typename t_allocator >
	void* allocate(
		t_allocator const& i_allocator,
		std::size_t const  i_size)
	{
		return this->allocate< typename t_allocator::arena >(
			i_size,
			t_allocator::ALIGNMENT,
			t_allocator::OFFSET,
			i_allocator.get_name());
	}

	/** @brief memory���m�ۂ��āA�ێ�����B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̔z�u���E�l�B
	    @param[in] i_offset    �m�ۂ���memory�̔z�uoffset�l�B
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	public: template< typename t_arena >
	void* allocate(
		std::size_t       i_size,
		std::size_t       i_alignment,
		std::size_t       i_offset,
		char const* const i_name)
	{
		if (0 < i_size)
		{
			this_type a_storage(
				i_size,
				i_alignment,
				i_offset,
				i_name,
				boost::type< t_arena >());
			if (NULL != a_storage.get_address())
			{
				this->swap(a_storage);
				return this->get_address();
			}
		}
		else
		{
			this->deallocate();
		}
		return NULL;
	}

	/** @brief �ێ����Ă���memory������B
	 */
	public: void deallocate()
	{
		this_type().swap(*this);
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���memory�̑傫����byte�P�ʂŎ擾�B
	    @return �ێ����Ă���memory�̑傫���Bbyte�P�ʁB
	 */
	public: std::size_t get_size() const
	{
		return this->size_;
	}

	/** @brief �ێ����Ă���memory�̐擪�ʒu���擾�B
	    @return �ێ����Ă���memory�̐擪�ʒu�B
	 */
	public: void const* get_address() const
	{
		return this->address_;
	}

	/** @brief �ێ����Ă���memory�̐擪�ʒu���擾�B
	    @return �ێ����Ă���memory�̐擪�ʒu�B
	 */
	public: void* get_address()
	{
		return const_cast< void* >(
			const_cast< this_type const* >(this)->get_address());
	}

	//-------------------------------------------------------------------------
	private: void        (*deallocator_)(void* const, std::size_t const);
	private: void*       address_;
	private: std::size_t size_;
};

namespace std
{
	void swap(psyq::dynamic_storage& io_left, psyq::dynamic_storage& io_right)
	{
		io_left.swap(io_right);
	}
}

#endif // PSYQ_DYNAMIC_STORAGE_HPP_
