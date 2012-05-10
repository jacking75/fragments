#ifndef PSYQ_FILE_DESCRIPTOR_HPP_
#define PSYQ_FILE_DESCRIPTOR_HPP_

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#endif // _WIN32

#ifndef PSYQ_FILE_BLOCK_SIZE
//#define PSYQ_FILE_BLOCK_SIZE 4096
#endif // !PSYQ_FILE_BLOCK_SIZE

namespace psyq
{
	class file_buffer;
	class file_descriptor;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::file_buffer:
	private boost::noncopyable
{
	typedef psyq::file_buffer this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~file_buffer()
	{
		// �ێ����Ă���memory������B
		if (NULL != this->deallocator_)
		{
			(*this->deallocator_)(
				this->get_mapped_address(), this->get_mapped_size());
		}
	}

	file_buffer():
	deallocator_(NULL),
	storage_(NULL),
	mapped_offset_(0),
	mapped_size_(0),
	region_offset_(0),
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
		t_allocator const& i_allocator,
		std::size_t const  i_offset,
		std::size_t const  i_size)
	{
		new(this) this_type(
			boost::type< typename t_allocator::arena >(),
			i_offset,
			i_size,
			t_allocator::ALIGNMENT,
			t_allocator::OFFSET,
			i_allocator.get_name());
	}

	/** @param[in] i_offset
	        file�̐擪�ʒu�����offset�l�B
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_size
	        �m�ۂ���buffer�̑傫���Bbyte�P�ʁB
	        file�̘_��block-size�̐����{�ł���K�v������B
	    @param[in] i_memory_alignment �m�ۂ���buffer��memory�z�u���E�l�B
	    @param[in] i_memory_offset    �m�ۂ���buffer��memory�z�uoffset�l�B
	    @param[in] i_memory_name      debug�Ŏg�����߂�memory���ʖ��B
	 */
	template< typename t_arena >
	file_buffer(
		boost::type< t_arena > const&,
		std::size_t const i_offset,
		std::size_t const i_size,
		std::size_t const i_memory_alignment,
		std::size_t const i_memory_offset = 0,
		char const* const i_memory_name = PSYQ_ARENA_NAME_DEFAULT):
	mapped_offset_(i_offset),
	mapped_size_(i_size),
	region_offset_(0),
	region_size_(0)
	{
		if (0 < i_size)
		{
			this->storage_ = (t_arena::malloc)(
				i_size, i_memory_alignment, i_memory_offset, i_memory_name);
			if (NULL != this->get_mapped_address())
			{
				this->deallocator_ = &t_arena::free;
				return;
			}
			PSYQ_ASSERT(false);
			this->mapped_size_ = 0;
		}
		this->deallocator_ = NULL;
		this->storage_ = NULL;
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
	void* get_region_address() const
	{
		return static_cast< char* >(this->get_mapped_address())
			+ this->get_region_offset();
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
	/** @brief buffer�̐擪�ʒu���擾�B
	 */
	void* get_mapped_address() const
	{
		return this->storage_;
	}

	/** @brief file�擪�ʒu����buffer�擪�ʒu�ւ�offset�l���擾�B
	 */
	std::size_t get_mapped_offset() const
	{
		return this->mapped_offset_;
	}

	/** @brief buffer�̑傫����byte�P�ʂŎ擾�B
	 */
	std::size_t get_mapped_size() const
	{
		return this->mapped_size_;
	}

	//-------------------------------------------------------------------------
	/** @brief �l�������B
	 */
	void swap(this_type& io_target)
	{
		std::swap(this->deallocator_, io_target.deallocator_);
		std::swap(this->storage_, io_target.storage_);
		std::swap(this->mapped_offset_, io_target.mapped_offset_);
		std::swap(this->mapped_size_, io_target.mapped_size_);
		std::swap(this->region_offset_, io_target.region_offset_);
		std::swap(this->region_size_, io_target.region_size_);
	}

//.............................................................................
private:
	void        (*deallocator_)(void* const, std::size_t const);
	void*       storage_;
	std::size_t mapped_offset_;
	std::size_t mapped_size_;
	std::size_t region_offset_;
	std::size_t region_size_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::file_descriptor:
	private boost::noncopyable
{
	typedef psyq::file_descriptor this_type;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	enum open_flag
	{
		open_READ     = 1 << 0,
		open_WRITE    = 1 << 1,
		open_CREATE   = 1 << 2,
		open_TRUNCATE = 1 << 3,
	};

	//-------------------------------------------------------------------------
	~file_descriptor()
	{
		int const a_error(this->close_file());
		if (0 != a_error)
		{
			PSYQ_ASSERT(false);
		}
	}

	//-------------------------------------------------------------------------
	file_descriptor():
	descriptor_(-1)
	{
		// pass
	}

	/** @param[in] i_path  file��path���B�K��NULL�����ŏI���B
	    @param[in] i_flags �����鑀��Bthis_type::open_flag�̘_���a�B
	 */
	file_descriptor(char const* const i_path, int const i_flags):
	descriptor_(-1)
	{
		int const a_error(this->open_file(i_path, i_flags));
		if (0 != a_error)
		{
			PSYQ_ASSERT(false);
		}
	}

	/** @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_path   file��path���B�K��NULL�����ŏI���B
	    @param[in] i_flags  �����鑀��Bthis_type::open_flag�̘_���a�B
	 */
	file_descriptor(
		int&              o_error,
		char const* const i_path,
		int const         i_flags):
	descriptor_(-1)
	{
		o_error = this->open_file(i_path, i_flags);
	}

	//-------------------------------------------------------------------------
	/** @brief file���J���Ă��邩����B
	    @retval true  file���J���Ă���B
	    @retval false file���J���ĂȂ��B
	 */
	bool is_open() const
	{
		return 0 <= this->descriptor_;
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @return file��byte�P�ʂ̑傫���B
	 */
	std::size_t get_size() const
	{
		int a_error;
		std::size_t const a_size(this->get_size(a_error));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @return file��byte�P�ʂ̑傫���B
	 */
	std::size_t get_size(int& o_error) const
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		return this->seek_file(o_error, 0, SEEK_END);
	}

	//-------------------------------------------------------------------------
	/** @brief file��ǂݍ��ށB
	    @param[out] o_buffer   ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_offset    file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size      �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_arena >
	int read(
		psyq::file_buffer& o_buffer,
		std::size_t const  i_offset,
		std::size_t const  i_size,
		char const* const  i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		return this->read(
			o_buffer, i_offset, i_size, this_type::get_block_size(), i_name);
	}

	/** @brief file��ǂݍ��ށB
	    @param[out] o_buffer   ���������ǂݍ���buffer�̊i�[��B
	    @param[in] i_offset    file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size      �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_alignment �ǂݍ���buffer��memory�z�u���E�l�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	template< typename t_arena >
	int read(
		psyq::file_buffer& o_buffer,
		std::size_t const  i_offset,
		std::size_t const  i_size,
		std::size_t const  i_alignment,
		char const* const  i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int a_error;
		std::size_t const a_file_size(this->seek_file(a_error, 0, SEEK_END));
		if (0 == a_error)
		{
			// �ǂݍ���buffer���m�ہB
			std::size_t const a_read_offset((std::min)(i_offset, a_file_size));
			std::size_t const a_region_size(
				(std::min)(i_size, a_file_size - a_read_offset));
			std::size_t const a_alignment(
				(std::max)(i_alignment, this_type::get_block_size()));
			std::size_t const a_mapped_offset(
				a_alignment * (a_read_offset / a_alignment));
			std::size_t const a_region_offset(a_read_offset - a_mapped_offset);
			std::size_t const a_temp_size(
				a_region_offset + a_region_size + a_alignment - 1);
			psyq::file_buffer a_buffer(
				boost::type< t_arena >(),
				a_mapped_offset,
				a_alignment * (a_temp_size / a_alignment),
				a_alignment,
				0,
				i_name);

			// file��ǂݍ��ށB
			std::size_t const a_read_size(
				this->read_file(
					a_error,
					a_buffer.get_mapped_offset(),
					a_buffer.get_mapped_size(),
					a_buffer.get_mapped_address()));
			if (0 == a_error)
			{
				a_buffer.set_region(
					a_region_offset,
					(std::min)(a_region_size, a_read_size - a_region_offset));
				a_buffer.swap(o_buffer);
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
	int write(
		std::size_t&             o_size,
		psyq::file_buffer const& i_buffer)
	{
		PSYQ_ASSERT(
			0 == i_buffer.get_mapped_offset() % this_type::get_block_size());
		PSYQ_ASSERT(
			0 == i_buffer.get_mapped_size() % this_type::get_block_size());

		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int a_error;
		std::size_t const a_file_size(this->seek_file(a_error, 0, SEEK_END));
		if (0 == a_error)
		{
			o_size = this->write_file(
				a_error,
				i_buffer.get_mapped_offset(),
				i_buffer.get_mapped_size(),
				i_buffer.get_mapped_address());
			std::size_t const a_mapped_end(
				i_buffer.get_mapped_offset() + i_buffer.get_mapped_size());
			if (a_file_size < a_mapped_end && 0 == a_error)
			{
				// file-size�𒲐��B
				a_error = this->truncate_file(
					i_buffer.get_mapped_offset()
					+ i_buffer.get_region_offset()
					+ i_buffer.get_region_size());
			}
		}
		return a_error;
	}

	//-------------------------------------------------------------------------
	void swap(this_type& io_target)
	{
		std::swap(this->descriptor_, io_target.descriptor_);
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	int open_file(
		char const* const i_path,
		int const         i_flags)
	{
		int a_flags(0);
#ifdef _WIN32
		int a_mode(0);
		int a_share(0);
		if (0 != (i_flags & this_type::open_READ))
		{
			a_mode = _S_IREAD;
			a_share = _SH_DENYWR;
			a_flags = _O_RDONLY;
		}
		if (0 != (i_flags & (this_type::open_CREATE | this_type::open_WRITE)))
		{
			a_mode = _S_IWRITE;
			if (0 != (i_flags & this_type::open_READ))
			{
				a_mode |= _S_IREAD;
				a_share = _SH_DENYRW;
				a_flags = _O_RDWR;
			}
			else
			{
				a_share = _SH_DENYRD;
				a_flags = _O_WRONLY;
			}
			if (0 != (i_flags & this_type::open_CREATE))
			{
				// file���Ȃ���΍��B
				a_flags |= _O_CREAT;
				if (0 == (i_flags & this_type::open_WRITE))
				{
					// file������Ύ��s�B
					a_flags |= _O_EXCL;
				}
			}
			if (0 != (i_flags & this_type::open_TRUNCATE))
			{
				a_flags |= _O_TRUNC;
			}
		}

		// file���J���B
		a_flags |= _O_BINARY;
		::_sopen_s(&this->descriptor_, i_path, a_flags, a_share, a_mode);
#else
		if (0 != (i_flags & this_type::open_READ))
		{
			a_flags = O_RDONLY;
		}
		if (0 != (i_flags & (this_type::open_CREATE | this_type::open_WRITE)))
		{
			if (0 != (i_flags & this_type::open_READ))
			{
				a_flags = O_RDWR;
			}
			else
			{
				a_flags = O_WRONLY;
			}
			if (0 != (i_flags & this_type::open_CREATE))
			{
				// file���Ȃ���΍��B
				a_flags |= O_CREAT;
				if (0 == (i_flags & this_type::open_WRITE))
				{
					// file������Ύ��s�B
					a_flags |= O_EXCL;
				}
			}
			if (0 != (i_flags & this_type::open_TRUNCATE))
			{
				a_flags |= O_TRUNC;
			}
		}

		// file���J���B
		this->descriptor_ = ::open(i_path, a_flags);
#endif // _WIN32
		return this->is_open()? 0: errno;
	}

	//-------------------------------------------------------------------------
	int close_file() const
	{
		if (this->is_open())
		{
#ifdef _WIN32
			if (0 != ::_close(this->descriptor_))
#else
			if (0 != ::close(this->descriptor_))
#endif // _WIN32
			{
				return errno;
			}
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	std::size_t seek_file(
		int&              o_error,
		std::size_t const i_offset,
		int const         i_origin)
	const
	{
#ifdef _WIN32
		__int64 const a_position(
			::_lseeki64(this->descriptor_, i_offset, i_origin));
		if (a_position < 0)
#else
		off64_t const a_position(
			::lseek64(this->descriptor_, i_offset, i_origin));
		if (-1 == a_position)
#endif // _WIN32
		{
			o_error = errno;
		}
		else if ((std::numeric_limits< std::size_t >::max)() < a_position)
		{
			o_error = EFBIG;
		}
		else
		{
			o_error = 0;
			return static_cast< std::size_t >(a_position);
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file��ǂݍ��ށB
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size   �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer �ǂݍ���buffer�̐擪�ʒu�B
	    @return �ǂݍ���byte���B
	 */
	std::size_t read_file(
		int&              o_error,
		std::size_t const i_offset,
		std::size_t const i_size,
		void* const       i_buffer)
	const
	{
		if (i_size <= 0)
		{
			o_error = 0;
		}
		else if (NULL == i_buffer)
		{
			o_error = EFAULT;
		}
#ifndef _WIN32
		else if (SSIZE_MAX < i_size)
		{
			o_error = EFBIG;
		}
#endif // !WIN32
		else
		{
			int a_error;
			this->seek_file(a_error, i_offset, SEEK_SET);
			if (0 != a_error)
			{
				o_error = a_error;
			}
			else
			{
#ifdef _WIN32
				int const a_size(::_read(this->descriptor_, i_buffer, i_size));
#else
				int const a_size(::read(this->descriptor_, i_buffer, i_size));
#endif //_WIN32
				if (-1 != a_size)
				{
					o_error = 0;
					return static_cast< std::size_t >(a_size);
				}
				o_error = errno;
			}
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file�ɏ������ށB
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset file�̏������݊J�n�ʒu�B
	    @param[in] i_size   ��������buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer ��������buffer�̐擪�ʒu�B
	    @return ��������byte���B
	 */
	std::size_t write_file(
		int&              o_error,
		std::size_t const i_offset,
		std::size_t const i_size,
		void const* const i_buffer)
	const
	{
		int a_error;
		this->seek_file(a_error, i_offset, SEEK_END);
		if (0 == a_error)
		{
#ifdef _WIN32
			int const a_size(::_write(this->descriptor_, i_buffer, i_size));
#else
			int const a_size(::write(this->descriptor_, i_buffer, i_size));
#endif // _WIN32
			if (-1 != a_size)
			{
				o_error = 0;
				return static_cast< std::size_t >(a_size);
			}
			a_error = errno;
		}
		o_error = a_error;
		return 0;
	}

	//-------------------------------------------------------------------------
	int truncate_file(std::size_t const i_size) const
	{
#ifdef _WIN32
		if (this->descriptor_ < 0)
		{
			return EBADF;
		}

		::HANDLE const a_handle(
			reinterpret_cast< HANDLE >(_get_osfhandle(this->descriptor_)));
		::LARGE_INTEGER const a_size = { i_size };
		if (0 != ::SetFilePointerEx(a_handle, a_size, NULL, FILE_BEGIN)
			&& 0 != ::SetEndOfFile(a_handle))
		{
			return 0;
		}

		int const a_error(::GetLastError());
		switch (a_error)
		{
		case ERROR_INVALID_HANDLE:
			return EBADF;
		case ERROR_USER_MAPPED_FILE:
			/** @note 2012-05-04
			    CreateFileMapping�Ŋ��蓖�Ă��̈��SetEndOfFile()�ō��Ȃ��B
			    ���̏ꍇ�̑Ώ��͂ǂ����悤�H
				http://fallabs.com/blog-ja/promenade.cgi?id=76
			 */
		default:
			return EIO;
		}
#elif _BSD_SOURCE || 500 <= _XOPEN_SOURCE || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
		return 0 == ::ftruncate(this->descriptor_, i_size)? 0: errno;
#else
		PSYQ_ASSERT(false); // ���Ή��Ȃ̂ŁB
		return EPERM;
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief file�̘_��block-size��byte�P�ʂŎ擾�B
		@note 2012-05-10
	        �{����file�̘_��block-size��Ԃ��K�v�����邪�A
	        file�̑��݂���device�ɂ���Ę_��block-size���قȂ�̂ŁA
	        �ȕ։��̂��߂Ɉꗥ��page-size���g�����Ƃɂ���B
	 */
	static std::size_t get_block_size()
	{
#ifdef PSYQ_FILE_BLOCK_SIZE
		return PSYQ_FILE_BLOCK_SIZE;
#elif defined(_WIN32)
		SYSTEM_INFO a_info;
		::GetSystemInfo(&a_info);
		return a_info.dwPageSize;
#else
		std::size_t const a_page_size(::sysconf(_SC_PAGESIZE));
		if (static_cast< std::size_t >(-1) == a_page_size)
		{
			PSYQ_ASSERT(false);
			return 0;
		}
		return a_page_size;
#endif // PSYQ_FILE_BLOCK_SIZE
	}

//.............................................................................
private:
	int descriptor_;
};

#endif // !PSYQ_FILE_DESCRIPTOR_HPP_
