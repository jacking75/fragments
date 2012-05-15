#ifndef PSYQ_POSIX_FILE_DESCRIPTOR_HPP_
#define PSYQ_POSIX_FILE_DESCRIPTOR_HPP_

#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif // _WIN32
//#include <psyq/file_buffer.hpp>

#ifndef PSYQ_POSIX_FILE_BLOCK_SIZE
//#define PSYQ_POSIX_FILE_BLOCK_SIZE 4096
#endif // !PSYQ_POSIX_FILE_BLOCK_SIZE

namespace psyq
{
	class posix_file_descriptor;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief posix������file�L�q�q��p����file����B
 */
class psyq::posix_file_descriptor:
	private boost::noncopyable
{
	typedef psyq::posix_file_descriptor this_type;

//.............................................................................
public:
	enum open_flag
	{
#ifdef _WIN32
		open_READ     = _O_RDWR,   ///< ����flag�́Awin32�ƌ݊������Ȃ��B
		open_WRITE    = _O_WRONLY, ///< ����flag�́Awin32�ƌ݊������Ȃ��B
		open_CREATE   = _O_CREAT,
		open_TRUNCATE = _O_TRUNC,
#else
		open_READ     = O_RDWR,   ///< ����flag�́Aposix�ƌ݊������Ȃ��B
		open_WRITE    = O_WRONLY, ///< ����flag�́Aposix�ƌ݊������Ȃ��B
		open_CREATE   = O_CREAT,
		open_TRUNCATE = O_TRUNC,
#endif // _WIN32
	};

	//-------------------------------------------------------------------------
	~posix_file_descriptor()
	{
		int const a_error(this->close());
		if (0 != a_error)
		{
			PSYQ_ASSERT(false);
		}
	}

	//-------------------------------------------------------------------------
	posix_file_descriptor():
	descriptor_(-1)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief file�L�q�q�������B
	    @param[in,out] io_target ��������file�L�q�q�B
	 */
	void swap(this_type& io_target)
	{
		std::swap(this->descriptor_, io_target.descriptor_);
	}

	//-------------------------------------------------------------------------
	/** @brief file���J���Ă��邩����B
	    @retval true  file�͊J���Ă���B
	    @retval false file�͊J���ĂȂ��B
	 */
	bool is_open() const
	{
		return 0 <= this->descriptor_;
	}

	//-------------------------------------------------------------------------
	/** @brief file���J���B
	    @param[in] i_path  file��path���B�K��NULL�����ŏI���B
	    @param[in] i_flags �����鑀��Bthis_type::open_flag�̘_���a�B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int open(char const* const i_path, int const i_flags)
	{
		int const a_error(this->close());
		if (0 != a_error)
		{
			return a_error;
		}

		int a_flags(~(this_type::open_READ | this_type::open_WRITE) & i_flags);
#ifdef _WIN32
		int a_mode(0);
		int a_share(0);
		if (0 != (this_type::open_READ & i_flags))
		{
			a_mode = _S_IREAD;
			a_share = _SH_DENYWR;
		}
		if (0 != ((this_type::open_CREATE | this_type::open_WRITE) & i_flags))
		{
			a_mode = _S_IWRITE;
			if (0 != (this_type::open_READ & i_flags))
			{
				a_flags |= _O_RDWR;
				a_mode |= _S_IREAD;
				a_share = _SH_DENYRW;
			}
			else
			{
				a_flags |= _O_WRONLY;
				a_share = _SH_DENYRD;
			}
			if (0 == (this_type::open_WRITE & i_flags))
			{
				// file���Ȃ���΍�邪�Afile������Ύ��s�B
				a_flags |= _O_EXCL;
			}
		}

		// file���J���B
		if (0 == ((_O_TEXT | _O_WTEXT | _O_U8TEXT | _O_U16TEXT) & i_flags))
		{
			a_flags |= _O_BINARY;
		}
		::_sopen_s(&this->descriptor_, i_path, a_flags, a_share, a_mode);
#else
		if (0 != ((this_type::open_CREATE | this_type::open_WRITE) & i_flags))
		{
			if (0 != (this_type::open_READ & i_flags))
			{
				a_flags |= O_RDWR;
			}
			else
			{
				a_flags |= O_WRONLY;
			}
			if (0 == (this_type::open_WRITE & i_flags))
			{
				// file���Ȃ���΍�邪�A����Ύ��s�B
				a_flags |= O_EXCL;
			}
		}

		// file���J���B
		this->descriptor_ = ::open64(i_path, a_flags);
#endif // _WIN32
		return this->is_open()? 0: errno;
	}

	//-------------------------------------------------------------------------
	/** @brief file�����B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int close()
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
			this->descriptor_ = -1;
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
	std::size_t read(
		int&                            o_error,
		psyq::file_buffer::offset const i_offset,
		std::size_t const               i_size,
		void* const                     i_buffer)
	const
	{
#ifdef _WIN32
		int a_error;
		this->seek(a_error, i_offset, this_type::seek_BEGIN);
		if (0 == a_error)
		{
			int const a_size(::_read(this->descriptor_, i_buffer, i_size));
			if (-1 != a_size)
			{
				o_error = 0;
				return a_size;
			}
			o_error = errno;
		}
		else
		{
			o_error = a_error;
		}
#else
		::ssize_t const a_size(
			::pread64(this->descriptor_, i_buffer, i_size, i_offset));
		if (-1 != a_size)
		{
			o_error = 0;
			return a_size;
		}
		o_error = errno;
#endif // _WIN32
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file�ɏ����o���B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset file�̏����o���J�n�ʒu�B
	    @param[in] i_size   �����o��buffer��byte�P�ʂ̑傫���B
	    @param[in] i_buffer �����o��buffer�̐擪�ʒu�B
	    @return �����o����byte���B
	 */
	std::size_t write(
		int&                            o_error,
		psyq::file_buffer::offset const i_offset,
		std::size_t const               i_size,
		void const* const               i_buffer)
	const
	{
#ifdef _WIN32
		int a_error;
		this->seek(a_error, i_offset, this_type::seek_BEGIN);
		if (0 == a_error)
		{
			int const a_size(::_write(this->descriptor_, i_buffer, i_size));
			if (-1 != a_size)
			{
				o_error = 0;
				return a_size;
			}
			o_error = errno;
		}
		else
		{
			o_error = a_error;
		}
#else
		::ssize_t const a_size(
			::pwrite64(this->descriptor_, i_buffer, i_size, i_offset));
		if (-1 != a_size)
		{
			o_error = 0;
			return a_size;
		}
		o_error = errno;
#endif // _WIN32
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����ύX�B
	    @param[in] i_size file��byte�P�ʂ̑傫���B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int resize(psyq::file_buffer::offset const i_size) const
	{
#ifdef _WIN32
		return 0 == ::_chsize_s(this->descriptor_, i_size)? 0: errno;
#else
		return 0 == ::ftruncate64(this->descriptor_, i_size)? 0: errno;
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @return file��byte�P�ʂ̑傫���B
	 */
	psyq::file_buffer::offset get_size(int& o_error) const
	{
#ifdef _WIN32
		__int64 const a_size(::_filelengthi64(this->descriptor_));
		if (-1 != a_size)
		{
			o_error = 0;
			PSYQ_ASSERT(
				a_size == static_cast< __int64 >(
					static_cast< psyq::file_buffer::offset >(a_size)));
			return static_cast< psyq::file_buffer::offset >(a_size);
		}
		o_error = errno;
		return 0;
#else
		return this->seek(o_error, 0, this_type::seek_END);
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief file�̘_��block-size��byte�P�ʂŎ擾�B
	    @note 2012-05-10
	        �{����file�̘_��block-size��Ԃ��K�v�����邪�A
	        file�̑��݂���device�ɂ���Ę_��block-size���قȂ�̂ŁA
	        �ȕ։��̂��߂Ɉꗥ��page-size���g�����Ƃɂ���B
	 */
	std::size_t get_block_size(int& o_error) const
	{
#ifdef PSYQ_POSIX_FILE_BLOCK_SIZE
		o_error = 0;
		return PSYQ_POSIX_FILE_BLOCK_SIZE;
#elif defined(_WIN32)
		o_error = 0;
		return psyq::file_buffer::_get_page_size();
#else
		std::size_t const a_page_size(psyq::file_buffer::_get_page_size());
		o_error = (0 != a_page_size? 0: errno);
		return a_page_size;
#endif // PSYQ_POSIX_FILE_BLOCK_SIZE
	}

//.............................................................................
private:
	enum seek_origin
	{
		seek_BEGIN   = SEEK_SET,
		seek_END     = SEEK_END,
		seek_CURRENT = SEEK_CUR,
	};

	//-------------------------------------------------------------------------
	/** @brief file�ǂݏ���offset�ʒu��ݒ�B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @param[in] i_offset �ݒ肷��file�ǂݏ���offset�ʒu�B
	    @param[in] i_origin file�ǂݏ����̊�ʒu�B
	 */
	psyq::file_buffer::offset seek(
		int&                            o_error,
		psyq::file_buffer::offset const i_offset,
		this_type::seek_origin const    i_origin)
	const
	{
#ifdef _WIN32
		typedef __int64 _offset;
		_offset const a_position(
			::_lseeki64(this->descriptor_, i_offset, i_origin));
#else
		typedef off64_t _offset;
		_offset const a_position(
			::lseek64(this->descriptor_, i_offset, i_origin));
#endif // _WIN32
		if (-1 != a_position)
		{
			o_error = 0;
			PSYQ_ASSERT(
				a_position == static_cast< _offset >(
					static_cast< psyq::file_buffer::offset >(a_position)));
			return static_cast< psyq::file_buffer::offset >(a_position);
		}
		o_error = errno;
		return 0;
	}

//.............................................................................
private:
	int descriptor_;
};

#endif // !PSYQ_POSIX_FILE_DESCRIPTOR_HPP_
