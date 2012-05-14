#ifndef PSYQ_FILE_HANDLE_HPP_
#define PSYQ_FILE_HANDLE_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
//#include <psyq/file_buffer.hpp>
//#include <psyq/posix_file_descriptor.hpp>

namespace psyq
{
	template< typename, typename > class basic_file_handle;
	typedef basic_file_handle< posix_file_descriptor, PSYQ_MUTEX_DEFAULT >
		file_handle;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_descriptor, typename t_mutex >
class psyq::basic_file_handle:
	private boost::noncopyable
{
	typedef psyq::basic_file_handle< t_descriptor, t_mutex > this_type;

//.............................................................................
public:
	typedef t_descriptor descriptor;
	typedef t_mutex mutex;
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	basic_file_handle()
	{
		// pass
	}

	/** @brief file���J���B
	    @param[in] i_path  �J��file��path���B�K��NULL�����ŏI���B
	    @param[in] i_flags �����鑀��Bt_descriptor::open_flag�̘_���a�B
	 */
	basic_file_handle(char const* const i_path, int const i_flags)
	{
		int const a_error(this->descriptor_.open(i_path, i_flags));
		if (0 != a_error)
		{
			PSYQ_ASSERT(false);
		}
	}

	/** @brief file���J���B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_path   �J��file��path���B�K��NULL�����ŏI���B
	    @param[in] i_flags  �����鑀��Bt_descriptor::open_flag�̘_���a�B
	 */
	basic_file_handle(
		int&              o_error,
		char const* const i_path,
		int const         i_flags)
	{
		o_error = this->descriptor_.open(i_path, i_flags);
	}

	//-------------------------------------------------------------------------
	/** @brief handle�������B
	    @param[in,out] io_target ��������handle�B
	 */
	void swap(this_type& io_target)
	{
		if (&io_target != this)
		{
			boost::unique_lock< t_mutex > a_this_lock(
				this->mutex_, boost::defer_lock);
			boost::unique_lock< t_mutex > a_target_lock(
				io_target.mutex_, boost::defer_lock);
			boost::lock(a_this_lock, a_target_lock);
			this->descriptor_.swap(io_target.descriptor_);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief file���J���Ă��邩����B
	    @retval true  file���J���Ă���B
	    @retval false file���J���ĂȂ��B
	 */
	bool is_open()
	{
		boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		return this->descriptor_.is_open();
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @return file��byte�P�ʂ̑傫���B
	 */
	psyq::file_buffer::offset get_size()
	{
		int a_error;
		psyq::file_buffer::offset const a_size(this->get_size(a_error));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @return file��byte�P�ʂ̑傫���B
	 */
	psyq::file_buffer::offset get_size(int& o_error)
	{
		boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		return this->descriptor_.get_size(o_error);
	}

	//-------------------------------------------------------------------------
	/** @brief file�̘_��block-size��byte�P�ʂŎ擾�B
	 */
	std::size_t get_block_size() const
	{
		int a_error;
		std::size_t const a_size(this->descriptor_.get_block_size(a_error));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief file�̘_��block-size��byte�P�ʂŎ擾�B
	 */
	std::size_t get_block_size(int& o_error) const
	{
		return this->descriptor_.get_block_size(o_error);
	}

	//-------------------------------------------------------------------------
	/** @brief file��ǂݍ��ށB
	    @param[out] o_buffer   ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_allocator memory�m�ۂɎg�������q�B
	    @param[in] i_offset    file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size      �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_allocator >
	int read(
		psyq::file_buffer&              o_buffer,
		t_allocator const&              i_allocator,
		psyq::file_buffer::offset const i_offset = 0,
		std::size_t const               i_size
			= (std::numeric_limits< std::size_t >::max)())
	{
		return this->read< typename t_allocator::arena >(
			o_buffer, i_offset, i_size, i_allocator.get_name());
	}

	/** @brief file��ǂݍ��ށB
	    @param[out] o_buffer   ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_allocator memory�m�ۂɎg�������q�B
	    @param[in] i_offset    file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size      �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_alignment �ǂݍ���buffer��memory�z�u���E�l�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_allocator >
	int read(
		psyq::file_buffer&              o_buffer,
		t_allocator const&              i_allocator,
		psyq::file_buffer::offset const i_offset,
		std::size_t const               i_size,
		std::size_t const               i_alignment)
	{
		return this->read< typename t_allocator::arena >(
			o_buffer, i_offset, i_size, i_alignment, i_allocator.get_name());
	}

	/** @brief file��ǂݍ��ށB
		@tparam t_arena      memory�m�ۂɎg��memory-arena�̌^�B
	    @param[out] o_buffer ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_offset  file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size    �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_arena >
	int read(
		psyq::file_buffer&              o_buffer,
		psyq::file_buffer::offset const i_offset = 0,
		std::size_t const               i_size
			= (std::numeric_limits< std::size_t >::max)(),
		char const* const               i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		return this->read< t_arena >(
			o_buffer, i_offset, i_size, this->get_block_size(), i_name);
	}

	/** @brief file��ǂݍ��ށB
		@tparam t_arena        memory�m�ۂɎg��memory-arena�̌^�B
	    @param[out] o_buffer   ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_offset    file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size      �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_alignment �ǂݍ���buffer��memory�z�u���E�l�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_arena >
	int read(
		psyq::file_buffer&              o_buffer,
		psyq::file_buffer::offset const i_offset,
		std::size_t const               i_size,
		std::size_t const               i_alignment,
		char const* const               i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int a_error;
		psyq::file_buffer::offset const a_file_size(
			this->descriptor_.get_size(a_error));
		if (0 == a_error)
		{
			psyq::file_buffer::offset const a_read_offset(
				(std::min)(i_offset, a_file_size));
			std::size_t const a_rest_size(
				static_cast< std::size_t >(a_file_size - a_read_offset));
			PSYQ_ASSERT(a_file_size - a_read_offset == a_rest_size);
			std::size_t const a_region_size((std::min)(i_size, a_rest_size));
			std::size_t const a_alignment(
				(std::max)(i_alignment, this->get_block_size(a_error)));
			if (0 == a_error)
			{
				psyq::file_buffer::offset const a_mapped_offset(
					a_alignment * (a_read_offset / a_alignment));
				std::size_t const a_region_offset(
					static_cast< std::size_t >(
						a_read_offset - a_mapped_offset));
				PSYQ_ASSERT(
					a_read_offset - a_mapped_offset == a_region_offset);
				std::size_t const a_temp_size(
					a_region_offset + a_region_size + a_alignment - 1);
				PSYQ_ASSERT(
					a_temp_size == (
						static_cast< psyq::file_buffer::offset >(-1) +
						a_region_offset + a_region_size + a_alignment));

				// �_��block-size�P�ʂŁA�ǂݍ���buffer���m�ہB
				psyq::file_buffer a_buffer(
					boost::type< t_arena >(),
					a_mapped_offset,
					a_alignment * (a_temp_size / a_alignment),
					a_alignment,
					0,
					i_name);

				// �_��block-size�P�ʂŁAfile��ǂݍ��ށB
				std::size_t const a_read_size(
					this->descriptor_.read(
						a_error,
						a_buffer.get_mapped_offset(),
						a_buffer.get_mapped_size(),
						a_buffer.get_mapped_address()));
				if (0 == a_error)
				{
					a_buffer.set_region(
						a_region_offset,
						(std::min)(
							a_region_size, a_read_size - a_region_offset));
					a_buffer.swap(o_buffer);
				}
			}
		}
		return a_error;
	}

	//-------------------------------------------------------------------------
	/** @brief file�ɏ����o���B
	    @param[in] i_buffer �����o��buffer�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int write(psyq::file_buffer const& i_buffer)
	{
		std::size_t a_size;
		return this->write(a_size, i_buffer);
	}

	/** @brief file�ɏ����o���B
	    @param[out] o_size  �����o����byte���̊i�[��B
	    @param[in] i_buffer �����o��buffer�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int write(std::size_t& o_size, psyq::file_buffer const& i_buffer)
	{
		PSYQ_ASSERT(
			0 == i_buffer.get_mapped_offset() % this->get_block_size());
		PSYQ_ASSERT(
			0 == i_buffer.get_mapped_size() % this->get_block_size());

		boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int a_error;
		psyq::file_buffer::offset const a_file_size(
			this->descriptor_.get_size(a_error));
		if (0 == a_error)
		{
			// �_��block-size�P�ʂŁAfile�ɏ����o���B
			o_size = this->descriptor_.write(
				a_error,
				i_buffer.get_mapped_offset(),
				i_buffer.get_mapped_size(),
				i_buffer.get_mapped_address());

			// file�̑傫���𒲐��B
			psyq::file_buffer::offset const a_mapped_end(
				i_buffer.get_mapped_offset() + i_buffer.get_mapped_size());
			if (0 == a_error && a_file_size < a_mapped_end)
			{
				a_error = this->descriptor_.resize(
					i_buffer.get_mapped_offset() +
					i_buffer.get_region_offset() +
					i_buffer.get_region_size());
			}
		}
		return a_error;
	}

//.............................................................................
private:
	t_mutex      mutex_;
	t_descriptor descriptor_;
};

#endif // !PSYQ_FILE_HANDLE_HPP_
