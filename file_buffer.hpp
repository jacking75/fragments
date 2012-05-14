#ifndef PSYQ_FILE_BUFFER_HPP_
#define PSYQ_FILE_BUFFER_HPP_

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
//#include <psyq/dynamic_storage.hpp>

namespace psyq
{
	class file_buffer;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief file����Ɏg��buffer�B
 */
class psyq::file_buffer:
	private psyq::dynamic_storage
{
	typedef psyq::file_buffer this_type;
	typedef psyq::dynamic_storage super_type;

//.............................................................................
public:
	typedef boost::uint64_t offset;

	//-------------------------------------------------------------------------
	file_buffer():
	super_type(),
	region_offset_(0),
	mapped_offset_(0),
	region_size_(0)
	{
		// pass
	}

	/** @param[in] i_offset
	        file�̐擪�ʒu�����offset�l�B
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_size
	        �m�ۂ���buffer�̑傫���Bbyte�P�ʁB
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_allocator buffer�̊m�ۂɎg�������q�B
	 */
	template< typename t_allocator >
	file_buffer(
		t_allocator const&      i_allocator,
		this_type::offset const i_offset,
		std::size_t const       i_size):
	super_type(i_allocator, i_size),
	region_offset_(0),
	mapped_offset_(i_offset),
	region_size_(0)
	{
		// pass
	}

	/** @param[in] i_offset
	        file�̐擪�ʒu�����offset�l�B
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_size
	        �m�ۂ���buffer�̑傫���Bbyte�P�ʁB
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_memory_alignment �m�ۂ���buffer��memory�z�u���E�l�B
	    @param[in] i_memory_offset    �m�ۂ���buffer��memory�z�uoffset�l�B
	    @param[in] i_memory_name      �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
	 */
	template< typename t_arena >
	file_buffer(
		boost::type< t_arena > const& i_type,
		this_type::offset const       i_offset,
		std::size_t const             i_size,
		std::size_t const             i_memory_alignment,
		std::size_t const             i_memory_offset = 0,
		char const* const             i_memory_name = PSYQ_ARENA_NAME_DEFAULT):
	super_type(
		i_type, i_size, i_memory_alignment, i_memory_offset, i_memory_name),
	region_offset_(0),
	mapped_offset_(i_offset),
	region_size_(0)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���buffer�������B
	    @param[in,out] io_target ��������buffer�B
	 */
	void swap(this_type& io_target)
	{
		this->super_type::swap(io_target);
		std::swap(this->mapped_offset_, io_target.mapped_offset_);
		std::swap(this->region_offset_, io_target.region_offset_);
		std::swap(this->region_size_, io_target.region_size_);
	}

	//-------------------------------------------------------------------------
	/** @brief buffer�擪�ʒu����region�擪�ʒu�ւ�offset�l���擾�B
	 */
	std::size_t get_region_offset() const
	{
		return this->region_offset_;
	}

	/** @brief region�̑傫����byte�P�ʂŎ擾�B
	 */
	std::size_t get_region_size() const
	{
		return this->region_size_;
	}

	/** @brief region�̐擪�ʒu���擾�B
	 */
	void const* get_region_address() const
	{
		return static_cast< char const* >(this->get_mapped_address())
			+ this->get_region_offset();
	}

	void* get_region_address()
	{
		return const_cast< void* >(
			const_cast< this_type const* >(this)->get_region_address());
	}

	/** @brief region��ݒ�B
	    @param[in] i_offset buffer�擪�ʒu����region�擪�ʒu�ւ�offset�l�B
	    @param[in] i_size   region�̑傫���Bbyte�P�ʁB
	 */
	void set_region(
		std::size_t const i_offset,
		std::size_t const i_size)
	{
		this->region_offset_ = (std::min)(i_offset, this->get_mapped_size());
		this->region_size_ = (std::min)(
			i_size, this->get_mapped_size() - this->get_region_offset());
	}

	//-------------------------------------------------------------------------
	/** @brief file�擪�ʒu����buffer�擪�ʒu�ւ�offset�l���擾�B
	 */
	this_type::offset get_mapped_offset() const
	{
		return this->mapped_offset_;
	}

	/** @brief buffer�̑傫����byte�P�ʂŎ擾�B
	 */
	std::size_t get_mapped_size() const
	{
		return this->super_type::get_size();
	}

	/** @brief buffer�̐擪�ʒu���擾�B
	 */
	void const* get_mapped_address() const
	{
		return this->super_type::get_address();
	}

	void* get_mapped_address()
	{
		return const_cast< void* >(
			const_cast< this_type const* >(this)->get_mapped_address());
	}

//.............................................................................
private:
	std::size_t       region_offset_;
	this_type::offset mapped_offset_;
	std::size_t       region_size_;
};

#endif // !PSYQ_FILE_BUFFER_HPP_
