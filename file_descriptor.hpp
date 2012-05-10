#ifndef PSYQ_FILE_DESCRIPTOR_HPP_
#define PSYQ_FILE_DESCRIPTOR_HPP_

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#endif // _WIN32

namespace psyq
{
	class file_descriptor;
}

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
		std::size_t a_size(0);
		o_error = this->seek_file(a_size, 0, SEEK_END);
		return a_size;
	}

	//-------------------------------------------------------------------------
	/** @brief file��ǂݍ��ށB
	    @param[in] i_offset file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size   �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer �ǂݍ���buffer�̐擪�ʒu�B
	    @return �ǂݍ���byte���B
	 */
	std::size_t read(
		std::size_t const i_offset,
		std::size_t const i_size,
		void* const       i_buffer)
	{
		int a_error;
		std::size_t const a_size(
			this->read(a_error, i_offset, i_size, i_buffer));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief file��ǂݍ��ށB
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset file�̓ǂݍ��݊J�n�ʒu�B
	    @param[in] i_size   �ǂݍ���buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer �ǂݍ���buffer�̐擪�ʒu�B
	    @return �ǂݍ���byte���B
	 */
	std::size_t read(
		int&              o_error,
		std::size_t const i_offset,
		std::size_t const i_size,
		void* const       i_buffer)
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
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
			int const a_error(this->seek_file(i_offset, SEEK_SET));
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
	    @param[in] i_offset file�̏������݊J�n�ʒu�B
	    @param[in] i_size   ��������buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer ��������buffer�̐擪�ʒu�B
	    @return ��������byte���B
	 */
	std::size_t write(
		std::size_t const i_offset,
		std::size_t const i_size,
		void* const       i_buffer)
	{
		int a_error;
		std::size_t const a_size(
			this->write(a_error, i_offset, i_size, i_buffer));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief file�ɏ������ށB
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset file�̏������݊J�n�ʒu�B
	    @param[in] i_size   ��������buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer ��������buffer�̐擪�ʒu�B
	    @return ��������byte���B
	 */
	std::size_t write(
		int&              o_error,
		std::size_t const i_offset,
		std::size_t const i_size,
		void const* const i_buffer)
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int a_error(this->seek_file(i_offset, SEEK_END));
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
	int truncate(std::size_t const i_size) const
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
	int seek_file(
		std::size_t const i_offset,
		int const         i_origin)
	const
	{
		std::size_t a_position;
		return this->seek_file(a_position, i_offset, i_origin);
	}

	int seek_file(
		std::size_t&      o_position,
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
			return errno;
		}
		else if ((std::numeric_limits< std::size_t >::max)() < a_position)
		{
			return EFBIG;
		}
		o_position = static_cast< std::size_t >(a_position);
		return 0;
	}

//.............................................................................
private:
	int descriptor_;
};

#endif // !PSYQ_FILE_DESCRIPTOR_HPP_
