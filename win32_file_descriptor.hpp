#ifndef PSYQ_WIN32_FILE_DESCRIPTOR_HPP_
#define PSYQ_WIN32_FILE_DESCRIPTOR_HPP_

//#include <psyq/file_buffer.hpp>

#ifndef PSYQ_WIN32_FILE_BLOCK_SIZE
//#define PSYQ_WIN32_FILE_BLOCK_SIZE 4096
#endif // !PSYQ_WIN32_FILE_BLOCK_SIZE

namespace psyq
{
	class win32_file_descriptor;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::win32_file_descriptor:
	private boost::noncopyable
{
	typedef psyq::win32_file_descriptor this_type;

//.............................................................................
public:
	enum open_flag
	{
		open_READ     = 1 << 0,
		open_WRITE    = 1 << 1,
		open_CREATE   = 1 << 2,
		open_TRUNCATE = 1 << 3,
	};

	//-------------------------------------------------------------------------
	~win32_file_descriptor()
	{
		int const a_error(this->close());
		if (0 != a_error)
		{
			PSYQ_ASSERT(false);
		}
	}

	//-------------------------------------------------------------------------
	win32_file_descriptor():
	handle_(INVALID_HANDLE_VALUE)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief file�L�q�q�������B
	    @param[in,out] io_target ��������file�L�q�q�B
	 */
	void swap(this_type& io_target)
	{
		std::swap(this->handle_, io_target.handle_);
	}

	//-------------------------------------------------------------------------
	/** @brief file���J���Ă��邩����B
	    @retval true  file�͊J���Ă���B
	    @retval false file�͊J���ĂȂ��B
	 */
	bool is_open() const
	{
		return INVALID_HANDLE_VALUE != this->handle_;
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

		DWORD a_access(0);
		DWORD a_share(0);
		DWORD a_creation(OPEN_EXISTING);
		DWORD a_attributes((i_flags & 0xfff80000) | FILE_ATTRIBUTE_NORMAL);
		if (0 != (this_type::open_READ & i_flags))
		{
			a_access |= GENERIC_READ;
			a_share = FILE_SHARE_READ;
		}
		if (0 != ((this_type::open_WRITE | this_type::open_CREATE) & i_flags))
		{
			a_access |= GENERIC_WRITE;
			a_share = 0;
			if (0 == (this_type::open_CREATE & i_flags))
			{
				if (0 != (this_type::open_TRUNCATE & i_flags))
				{
					// file������΋�ɂ���B�Ȃ���Ύ��s�B
					a_creation = TRUNCATE_EXISTING;
				}
			}
			else if (0 == (this_type::open_WRITE & i_flags))
			{
				// file������Ύ��s�B�Ȃ���΍��B
				a_creation = CREATE_NEW;
			}
			else if (0 == (this_type::open_TRUNCATE & i_flags))
			{
				// file������ΊJ���B�Ȃ���΍��B
				a_creation = OPEN_ALWAYS;
			}
			else
			{
				// file������΋�ɂ���B�Ȃ���΍��B
				a_creation = CREATE_ALWAYS;
			}
		}
		//a_attributes |= FILE_FLAG_NO_BUFFERING;
		this->handle_ = ::CreateFileA(
			i_path, a_access, a_share, NULL, a_creation, a_attributes, NULL);
		return INVALID_HANDLE_VALUE != this->handle_? 0: ::GetLastError();
	}

	//-------------------------------------------------------------------------
	int close()
	{
		if (INVALID_HANDLE_VALUE != this->handle_)
		{
			if (0 == ::CloseHandle(this->handle_))
			{
				return ::GetLastError();
			}
			this->handle_ = INVALID_HANDLE_VALUE;
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
		int a_error;
		this->seek(a_error, i_offset, this_type::seek_BEGIN);
		if (0 == a_error)
		{
			DWORD a_read_size;
			bool const a_success(
				0 != ::ReadFile(
					this->handle_, i_buffer, i_size, &a_read_size, NULL));
			if (a_success)
			{
				o_error = 0;
				return a_read_size;
			}
			o_error = ::GetLastError();
		}
		else
		{
			o_error = a_error;
		}
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
		int a_error;
		this->seek(a_error, i_offset, this_type::seek_BEGIN);
		if (0 == a_error)
		{
			DWORD a_write_size;
			bool const a_success(
				0 != ::WriteFile(
					this->handle_, i_buffer, i_size, &a_write_size, NULL));
			if (a_success)
			{
				o_error = 0;
				return a_write_size;
			}
			o_error = ::GetLastError();
		}
		else
		{
			o_error = a_error;
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����ύX�B
	    @param[in] i_size file��byte�P�ʂ̑傫���B
	    @return ���ʂ�error�ԍ��B0�Ȃ琬���B
	 */
	int resize(psyq::file_buffer::offset const i_size) const
	{
		int a_error;
		this->seek(a_error, i_size, this_type::seek_BEGIN);
		if (0 != a_error)
		{
			return a_error;
		}
		if (0 == ::SetEndOfFile(this->handle_))
		{
			return ::GetLastError();
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	/** @brief file�̑傫����byte�P�ʂŎ擾�B
	    @param[out] o_error ���ʂ�error�ԍ��B0�Ȃ琬���B
	    @return file��byte�P�ʂ̑傫���B
	 */
	psyq::file_buffer::offset get_size(int& o_error) const
	{
		::LARGE_INTEGER a_size;
		if (0 != ::GetFileSizeEx(this->handle_, &a_size))
		{
			o_error = 0;
			return static_cast< psyq::file_buffer::offset >(a_size.QuadPart);
		}
		o_error = ::GetLastError();
		return 0;
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
#ifdef PSYQ_WIN32_FILE_BLOCK_SIZE
		o_error = 0;
		return PSYQ_WIN32_FILE_BLOCK_SIZE;
#else
		o_error = 0;
		return psyq::file_buffer::_get_page_size();
#endif // PSYQ_WIN32_FILE_BLOCK_SIZE
	}

//.............................................................................
private:
	enum seek_origin
	{
		seek_BEGIN   = FILE_BEGIN,
		seek_END     = FILE_END,
		seek_CURRENT = FILE_CURRENT,
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
		::LARGE_INTEGER a_offset;
		a_offset.QuadPart = i_offset;
		::LARGE_INTEGER a_position;
		bool const a_success(
			0 != ::SetFilePointerEx(
				this->handle_, a_offset, &a_position, i_origin));
		if (a_success)
		{
			o_error = 0;
			return static_cast< psyq::file_buffer::offset >(
				a_position.QuadPart);
		}
		o_error = ::GetLastError();
		return 0;
	}

//.............................................................................
private:
	HANDLE handle_;
};

#endif // !PSYQ_WIN32_FILE_DESCRIPTOR_HPP_
