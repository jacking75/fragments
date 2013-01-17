#ifndef PSYQ_ASYNC_FILE_HPP_
#define PSYQ_ASYNC_FILE_HPP_

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
//#include <psyq/file_buffer.hpp>

/// @cond
namespace psyq
{
	template< typename, typename > class _async_file_task;
	template< typename, typename, typename > class async_file_reader;
	template< typename, typename > class async_file_writer;
	template< typename > class async_file_mapper;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�file�����̊��^�B
    @ingroup async-file-processing
    @tparam t_handle @copydoc _async_file_task::file
    @tparam t_mutex  @copydoc _async_file_task::mutex
 */
template< typename t_handle, typename t_mutex >
class psyq::_async_file_task:
	public psyq::lockable_async_task< t_mutex >
{
	/// ����object�̌^�B
	public: typedef psyq::_async_file_task< t_handle, t_mutex > this_type;

	/// ����object�̊��^�B
	public: typedef psyq::lockable_async_task< t_mutex > super_type;

	//-------------------------------------------------------------------------
	/// ����instance���ێ�����file-handle�̌^�B
	public: typedef t_handle file;

	/// ����instance���ێ�����mutex�̌^�B
	protected: typedef t_mutex mutex;

	/// ����instance�̕ێ��q�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;

	/// ����instance�̊Ď��q�B
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	/** @brief file-handle���擾�B
	 */
	public: typename t_handle::shared_ptr const& get_handle() const
	{
		return this->handle_;
	}

	/** @brief file-buffer���擾�B
	 */
	public: psyq::file_buffer const* get_buffer() const
	{
		return super_type::state_BUSY != this->get_state()?
			&this->buffer_: NULL;
	}

	/** @brief file-buffer���擾�B
	 */
	public: psyq::file_buffer* get_buffer()
	{
		return const_cast< psyq::file_buffer* >(
			const_cast< this_type const* >(this)->get_buffer());
	}

	/** @brief error�ԍ����擾�B
	    @return error�ԍ��Berror���Ȃ��ꍇ��0�B
	 */
	public: int get_error() const
	{
		return super_type::state_BUSY != this->get_state()?
			this->error_: 0;
	}

	//-------------------------------------------------------------------------
	/** @param[in] i_handle file-handle�̏����l�B
	 */
	protected: explicit _async_file_task(
		typename t_handle::shared_ptr const& i_handle):
	handle_(i_handle),
	error_(0)
	{
		PSYQ_ASSERT(NULL != i_handle.get());
		PSYQ_ASSERT(i_handle->is_open());
	}

	//-------------------------------------------------------------------------
	/// �ێ����Ă���file-handle�B
	protected: typename t_handle::shared_ptr handle_;

	/// �ێ����Ă���file-buffer�B
	protected: psyq::file_buffer buffer_;

	/// file�����Ŕ�������error�ԍ��B
	protected: int error_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�file�ǂݍ��ݏ���object�B
    @ingroup async-file-processing
    @tparam t_handle @copydoc _async_file_task::file
    @tparam t_arena  @copydoc async_file_reader::arena
    @tparam t_mutex  @copydoc _async_file_task::mutex
 */
template<
	typename t_handle,
	typename t_arena = PSYQ_ARENA_DEFAULT,
	typename t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::async_file_reader:
	public psyq::_async_file_task< t_handle, t_mutex >
{
	/// ����object�̌^�B
	public: typedef psyq::async_file_reader< t_handle, t_arena, t_mutex >
		this_type;

	/// ����object�̊��^�B
	public: typedef psyq::_async_file_task< t_handle, t_mutex > super_type;

	//-------------------------------------------------------------------------
	/// ����instance���g�p����memory-arena�̌^�B
	public: typedef t_arena arena;

	//-------------------------------------------------------------------------
	/** @param[in] i_handle      �ǂݍ���file�B
	    @param[in] i_read_offset �ǂݍ��݊J�n�ʒu�B
	    @param[in] i_read_size   �ǂݍ���byte���B
	    @param[in] i_buffer_alignment
	        �ǂݍ���buffer��memory�z�u���E�l�B
	        ������file�̘_��block-size�̂ق����傫���ꍇ�́A
	        file�̘_��block-size��memory�z�u���E�l�ƂȂ�B
	    @param[in] i_buffer_name
	        �ǂݍ���buffer��memory���ʖ��Bdebug�ł̂ݎg���B
	 */
	public: explicit async_file_reader(
		typename t_handle::shared_ptr const& i_handle,
		psyq::file_buffer::offset const      i_read_offset = 0,
		std::size_t const                    i_read_size
			= (std::numeric_limits< std::size_t >::max)(),
		std::size_t const                    i_buffer_alignment = 0,
		const char* const                    i_buffer_name
			= PSYQ_ARENA_NAME_DEFAULT):
	super_type(i_handle),
	buffer_alignment_(i_buffer_alignment),
	buffer_name_(i_buffer_name),
	read_offset_(i_read_offset),
	read_size_(i_read_size)
	{
		// pass
	}

	private: virtual boost::uint32_t run()
	{
		this->error_ = this->get_handle()->template read< t_arena >(
			this->buffer_,
			this->read_offset_,
			this->read_size_,
			this->buffer_alignment_,
			this->buffer_name_);
		return super_type::state_FINISHED;
	}

	//-------------------------------------------------------------------------
	/// �ǂݍ���buffer��memory�z�u���E�l�B
	private: std::size_t buffer_alignment_;

	/// �ǂݍ���buffer��memory���ʖ��Bdebug�ł̂ݎg���B
	private: char const* buffer_name_;

	/// file�̓ǂݍ��݊J�n�ʒu�B
	private: psyq::file_buffer::offset read_offset_;

	/// file����ǂݍ���byte���B
	private: std::size_t read_size_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�file�����o������object�B
    @ingroup async-file-processing
    @tparam t_handle ����instance���ێ�����file-handle�̌^�B
    @tparam t_mutex  ����instance���ێ�����mutex�̌^�B
 */
template< typename t_handle, typename t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::async_file_writer:
	public psyq::_async_file_task< t_handle, t_mutex >
{
	/// ����object�̌^�B
	public: typedef psyq::async_file_writer< t_handle > this_type;

	/// ����object�̊��^�B
	public: typedef psyq::_async_file_task< t_handle, t_mutex > super_type;

	//-------------------------------------------------------------------------
	public: async_file_writer(
		typename t_handle::shared_ptr const& i_handle,
		psyq::file_buffer&                   io_buffer):
	super_type(i_handle),
	write_size_(0)
	{
		this->buffer_.swap(io_buffer);
	}

	//-------------------------------------------------------------------------
	private: virtual boost::uint32_t run()
	{
		this->error_ = this->get_handle()->write(
			this->write_size_, this->buffer_);
		return super_type::state_FINISHED;
	}

	//-------------------------------------------------------------------------
	private: std::size_t write_size_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�file-mapping-object�B
    @ingroup async-file-processing
    @tparam t_mutex ����instance���ێ�����mutex�̌^�B
 */
template< typename t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::async_file_mapper:
	public psyq::lockable_async_task< t_mutex >
{
	public: typedef psyq::async_file_mapper< t_mutex > this_type;
	public: typedef psyq::lockable_async_task< t_mutex > super_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< boost::interprocess::file_mapping >
		handle_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< boost::interprocess::file_mapping >
		handle_weak_ptr;

	//-------------------------------------------------------------------------
	public: async_file_mapper(
		typename this_type::handle_shared_ptr const& i_handle,
		boost::interprocess::mode_t const            i_mode,
		boost::interprocess::offset_t const          i_offset = 0,
		std::size_t const                            i_size = 0,
		void const* const                            i_address = NULL):
	handle_(i_handle),
	mode_(i_mode),
	offset_(i_offset),
	size_(i_size),
	address_(i_address)
	{
		PSYQ_ASSERT(NULL != i_handle.get());
	}

	public: typename this_type::handle_shared_ptr const& get_handle() const
	{
		return this->handle_;
	}

	public: boost::interprocess::mapped_region const* get_region() const
	{
		return super_type::state_BUSY != this->get_state()?
			&this->region_: NULL;
	}

	public: boost::interprocess::mapped_region* get_region()
	{
		return const_cast< boost::interprocess::mapped_region* >(
			const_cast< this_type const* >(this)->get_region());
	}

	//-------------------------------------------------------------------------
	private: virtual boost::uint32_t run()
	{
		boost::interprocess::mapped_region(
			*this->get_handle(),
			this->mode_,
			this->offset_,
			this->size_,
			this->address_).swap(this->region_);
		return super_type::state_FINISHED;
	}

	//-------------------------------------------------------------------------
	private: typename this_type::handle_shared_ptr handle_;
	private: boost::interprocess::mapped_region    region_;
	private: boost::interprocess::mode_t           mode_;
	private: boost::interprocess::offset_t         offset_;
	private: std::size_t                           size_;
	private: void const*                           address_;
};

#endif // !PSYQ_ASYNC_FILE_HPP_
