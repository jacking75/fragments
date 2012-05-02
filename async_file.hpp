#ifndef PSYQ_ASYNC_FILE_HPP_
#define PSYQ_ASYNC_FILE_HPP_

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#endif // _WIN32

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class file_descriptor:
	private boost::noncopyable
{
	typedef file_descriptor this_type;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	enum open_flag
	{
		open_REWRITE = 1 << 0, ///< fileがあれば上書き。なければ失敗。
		open_CREATE  = 2 << 0, ///< fileがあれば失敗。なければ作る。
		open_WRITE   = 3 << 0, ///< fileがあれば空にする。なければ作る。
		open_READ    = 1 << 2, ///< fileがあれば読み込み。なければ失敗。
	};

	//-------------------------------------------------------------------------
	~file_descriptor()
	{
		int const a_error(this->close());
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

	//-------------------------------------------------------------------------
	/** @brief fileを開く。
	    @param[in] i_path  fileのpath名。必ずNULL文字で終わる。
	    @param[in] i_flags 許可する操作。this_type::open_flagの論理和。
	    @return 結果のerror番号。0なら成功。
	 */
	int open(
		char const* const i_path,
		int const         i_flags)
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int const a_error(this->close_file());
		if (0 != a_error)
		{
			return a_error;
		}

#ifdef _WIN32
		int a_mode;
		int a_share;
		int a_flags(_O_BINARY);
		if (0 == (i_flags & this_type::open_WRITE))
		{
			a_mode = _S_IREAD;
			a_share = _SH_DENYWR;
			a_flags |= _O_RDONLY;
		}
		else
		{
			a_mode = _S_IWRITE;
			if (0 != (i_flags & this_type::open_READ))
			{
				a_mode |= _S_IREAD;
				a_share = _SH_DENYRW;
				a_flags |= _O_RDWR;
			}
			else
			{
				a_share = _SH_DENYRD;
				a_flags |= _O_WRONLY;
			}
			if (0 != (i_flags & this_type::open_CREATE))
			{
				// fileがなければ作る。
				a_flags |= _O_CREAT | _O_TRUNC;
				if (0 == (i_flags & this_type::open_REWRITE))
				{
					// fileがあれば失敗。
					a_flags |= _O_EXCL;
				}
			}
		}
		::_sopen_s(&this->descriptor_, i_path, a_flags, a_share, a_mode);
#else
		int a_flags(0);
		if (0 == (i_flags & this_type::open_WRITE))
		{
			a_flags |= O_RDONLY;
		}
		else
		{
			if (0 != (i_flags & this_type::open_READ))
			{
				a_flags |= O_RDWR;
			}
			else
			{
				a_flags |= O_WRONLY;
			}
			if (0 != (i_flags & this_type::open_CREATE))
			{
				// fileがなければ作る。
				a_flags |= O_CREAT | O_TRUNC;
				if (0 == (i_flags & this_type::open_REWRITE))
				{
					// fileがあれば失敗。
					a_flags |= O_EXCL;
				}
			}
		}
		this->descriptor_ = ::open(i_path, a_flags);
#endif // _WIN32

		return this->is_open()? 0: errno;
	}

	/** @brief fileを閉じる。fileを開いてないなら、何も行わない。
	    @return 結果のerror番号。0なら成功。
	 */
	int close()
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		int const a_error(this->close_file());
		if (0 == a_error)
		{
			this->descriptor_ = -1;
		}
		return a_error;
	}

	/** @brief fileを開いているか判定。
	    @retval true  fileを開いている。
	    @retval false fileを開いてない。
	 */
	bool is_open() const
	{
		return 0 <= this->descriptor_;
	}

	//-------------------------------------------------------------------------
	/** @brief fileの大きさをbyte単位で取得。
	    @return fileのbyte単位の大きさ。
	 */
	std::size_t get_size() const
	{
		int a_error;
		std::size_t const a_size(this->get_size(a_error));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief fileの大きさをbyte単位で取得。
	    @param[out] o_error 結果のerror番号。0なら成功。
	    @return fileのbyte単位の大きさ。
	 */
	std::size_t get_size(int& o_error) const
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		std::size_t a_size(0);
		o_error = this->seek_file(a_size, 0, SEEK_END);
		return a_size;
	}

	//-------------------------------------------------------------------------
	/** @brief fileを読み込む。
	    @param[in] i_buffer 読み込みbufferの先頭位置。
	    @param[in] i_size   読み込みbufferのbyte単位の大きさ。
	    @param[in] i_offset fileの読み込み開始位置。
	    @return 読み込んだbyte数。
	 */
	std::size_t read(
		void* const       i_buffer,
		std::size_t const i_size,
		std::size_t const i_offset)
	const
	{
		int a_error;
		std::size_t const a_size(
			this->read(a_error, i_buffer, i_size, i_offset));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief fileを読み込む。
	    @param[out] o_error 結果のerror番号。0なら成功。
	    @param[in] i_buffer 読み込みbufferの先頭位置。
	    @param[in] i_size   読み込みbufferのbyte単位の大きさ。
	    @param[in] i_offset fileの読み込み開始位置。
	    @return 読み込んだbyte数。
	 */
	std::size_t read(
		int&              o_error,
		void* const       i_buffer,
		std::size_t const i_size,
		std::size_t const i_offset)
	const
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
	/** @brief fileに書き込む。
	    @param[in] i_buffer 書き込みbufferの先頭位置。
	    @param[in] i_size   書き込みbufferのbyte単位の大きさ。
	    @param[in] i_offset fileの書き込み開始位置。
	    @return 書き込んだbyte数。
	 */
	std::size_t write(
		void* const       i_buffer,
		std::size_t const i_size,
		std::size_t const i_offset =
			(std::numeric_limits< std::size_t >::max)())
	const
	{
		int a_error;
		std::size_t const a_size(
			this->write(a_error, i_buffer, i_size, i_offset));
		PSYQ_ASSERT(0 == a_error);
		return a_size;
	}

	/** @brief fileに書き込む。
	    @param[out] o_error 結果のerror番号。0なら成功。
	    @param[in] i_buffer 書き込みbufferの先頭位置。
	    @param[in] i_size   書き込みbufferのbyte単位の大きさ。
	    @param[in] i_offset fileの書き込み開始位置。
	    @return 書き込んだbyte数。
	 */
	std::size_t write(
		int&              o_error,
		void const* const i_buffer,
		std::size_t const i_size,
		std::size_t const i_offset =
			(std::numeric_limits< std::size_t >::max)())
	const
	{
		//boost::lock_guard< t_mutex > const a_lock(this->mutex_);

		std::size_t a_file_size(0);
		int a_error(this->seek_file(a_file_size, 0, SEEK_END));
		if (0 == a_error && i_offset < a_file_size)
		{
			a_error = this->seek_file(i_offset, SEEK_SET);
		}
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
	void swap(this_type& io_target)
	{
		std::swap(this->descriptor_, io_target.descriptor_);
	}

//.............................................................................
private:
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

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename    t_file,
	typename    t_arena = psyq::heap_arena,
	std::size_t t_alignment = sizeof(void*),
	std::size_t t_offset = 0 >
class async_file_reader:
	public psyq::async_task
{
	typedef async_file_reader this_type;
	typedef psyq::async_task super_type;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	typedef t_file file;
	typedef t_arena arena;

	static std::size_t const BUFFER_ALIGNMENT = t_alignment;
	static std::size_t const BUFFER_OFFSET = t_offset;

	virtual ~async_file_reader()
	{
		if (NULL != this->buffer_begin_)
		{
			(t_arena::free)(this->buffer_begin_, this->buffer_size_);
		}
	}

	explicit async_file_reader(
		typename t_file::shared_ptr const& i_file,
		std::size_t const                  i_size =
			(std::numeric_limits< std::size_t >::max)(),
		std::size_t const                  i_read_offset = 0,
		const char* const                  i_arena_name =
			PSYQ_ARENA_NAME_DEFAULT):
	file_(i_file),
	arena_name_(i_arena_name),
	buffer_begin_(NULL),
	buffer_size_(i_size),
	read_size_(0),
	read_offset_(i_read_offset)
	{
		PSYQ_ASSERT(NULL != i_file.get());
		PSYQ_ASSERT(i_file->is_open());
	}

	void* get_buffer() const
	{
		return super_type::state_BUSY != this->get_state()?
			this->buffer_begin_: NULL;
	}

	std::size_t get_size() const
	{
		return super_type::state_BUSY != this->get_state()?
			this->read_size_: 0;
	}

	int get_error() const
	{
		return super_type::state_BUSY != this->get_state()?
			this->error_: 0;
	}

	void swap(this_type& io_target)
	{
		if (super_type::state_BUSY != this->get_state()
			&& super_type::state_BUSY != io_target.get_state())
		{
			this->file_->swap(io_target.file_);
			std::swap(this->arena_name_, io_target.arena_name_);
			std::swap(this->buffer_begin_, io_target.buffer_begin_);
			std::swap(this->buffer_size_, io_target.buffer_size_);
			std::swap(this->read_size_, io_target.read_size_);
			std::swap(this->read_offset_, io_target.read_offset_);
			std::swap(this->error_, io_target.error_);
		}
		else
		{
			PSYQ_ASSERT(false);
		}
	}

//.............................................................................
private:
	virtual boost::int32_t run()
	{
		std::size_t const a_file_size(this->file_->get_size(this->error_));
		if (0 == this->error_)
		{
			if (NULL != this->buffer_begin_)
			{
				(t_arena::free)(this->buffer_begin_, this->buffer_size_);
			}
			std::size_t const a_size(
				this->read_offset_ < a_file_size?
					a_file_size - this->read_offset_: 0);
			if (a_size < this->buffer_size_)
			{
				this->buffer_size_ = a_size;
			}
			this->buffer_begin_ = (t_arena::malloc)(
				this->buffer_size_, t_alignment, t_offset, this->arena_name_);
			this->read_size_ = this->file_->read(
				this->error_,
				this->buffer_begin_,
				this->buffer_size_,
				this->read_offset_);
		}
		return super_type::state_FINISHED;
	}

//.............................................................................
private:
	typename t_file::shared_ptr file_;
	char const*                 arena_name_;
	void*                       buffer_begin_;
	std::size_t                 buffer_size_;
	std::size_t                 read_size_;
	std::size_t const           read_offset_;
	int                         error_;
};

#endif // !PSYQ_ASYNC_FILE_HPP_
