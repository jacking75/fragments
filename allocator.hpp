#ifndef PSYQ_ALLOCATOR_HPP_
#define PSYQ_ALLOCATOR_HPP_

#ifndef PSYQ_ALLOCATOR_POLICY_DEFAULT
#define PSYQ_ALLOCATOR_POLICY_DEFAULT psyq::allocator_policy
#endif // !PSYQ_ALLOCATOR_POLICY_DEFAULT

#ifndef PSYQ_ALLOCATOR_NAME_DEFAULT
#define PSYQ_ALLOCATOR_NAME_DEFAULT "PSYQ"
#endif // !PSYQ_ALLOCATOR_NAME_DEFAULT

namespace psyq
{
	class allocator_policy;
	template< typename, std::size_t, std::size_t, typename > class allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief memory����policy�B
 */
class psyq::allocator_policy:
	private boost::noncopyable
{
	typedef psyq::allocator_policy this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���memory�̋��Eoffset�l�Bbyte�P�ʁB
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	static void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		(void)i_name;

		// memory���E�l��2�ׂ̂��悩�m�F�B
		PSYQ_ASSERT(0 < i_alignment);
		PSYQ_ASSERT(0 == (i_alignment & (i_alignment - 1)));

		// size��0�Ȃ�memory�m�ۂ��Ȃ��B
		if (i_size <= 0)
		{
			return NULL;
		}

#ifdef _WIN32
		// win32���ł�memory�m�ہB
		return _aligned_offset_malloc(i_size, i_alignment, i_offset);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix���ł�memory�m�ہB
		PSYQ_ASSERT(0 == i_offset);
		void* a_memory(NULL);
		int const a_result(
			posix_memalign(
				&a_memory,
				sizeof(void*) <= i_alignment? i_alignment: sizeof(void*),
				i_size));
		return 0 == a_result? a_memory: NULL;
#else
		// ���̑��̊��ł�memory�m�ہB
		PSYQ_ASSERT(0 == i_offset);
		PSYQ_ASSERT(i_alignment <= sizeof(void*));
		(void)i_alignment;
		return std::malloc(i_size);
#endif // _WIN32
	}

	//-------------------------------------------------------------------------
	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	static void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		(void)i_size;

#ifdef _WIN32
		// win32���ł�memory����B
		_aligned_free(i_memory);
#elif 200112L <= _POSIX_C_SOURCE || 600 <= _XOPEN_SOURCE
		// posix���ł�memory����B
		std::free(i_memory);
#else
		// ���̑��̊��ł�memory����B
		std::free(i_memory)
#endif
	}

	//-------------------------------------------------------------------------
	/** @brief ��x�Ɋm�ۂł���memory�̍ő�size���擾�Bbyte�P�ʁB
	 */
	static std::size_t max_size()
	{
		return (std::numeric_limits< std::size_t >::max)();
	}

//.............................................................................
private:
	allocator_policy();
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::allocator�݊���instance�����q�B
    @tparam t_value_type       ���蓖�Ă�instance�̌^�B
    @tparam t_alignment        instance�̔z�u���E�l�Bbyte�P�ʁB
    @tparam t_offset           instance�̔z�uoffset�l�Bbyte�P�ʁB
    @tparam t_allocator_policy memory����policy�B
 */
template<
	typename    t_value_type,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0,
	typename    t_allocator_policy = PSYQ_ALLOCATOR_POLICY_DEFAULT >
class psyq::allocator
{
	typedef psyq::allocator<
		t_value_type, t_alignment, t_offset, t_allocator_policy >
			this_type;

	// �z�u���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);

//.............................................................................
public:
	typedef t_allocator_policy  allocator_policy;
	typedef std::size_t         size_type;
	typedef std::ptrdiff_t      difference_type;
	typedef t_value_type*       pointer;
	typedef const t_value_type* const_pointer;
	typedef t_value_type&       reference;
	typedef const t_value_type& const_reference;
	typedef t_value_type        value_type;

	static std::size_t const alignment = t_alignment;
	static std::size_t const offset = t_offset;

	//-------------------------------------------------------------------------
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		typename    t_other_policy = t_allocator_policy >
	struct rebind
	{
		typedef psyq::allocator<
			t_other_type, t_other_alignment, t_other_offset, t_other_policy >
				other;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit allocator(char const* const i_name = PSYQ_ALLOCATOR_NAME_DEFAULT):
	name(i_name)
	{
		// pass
	}

	//allocator(this_type const&) = default;

	/** @param[in] i_source copy��instance�B
	 */
	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_policy >
	allocator(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_policy > const&
				i_source):
	name(i_source.get_name())
	{
		// pass
	}

	//-------------------------------------------------------------------------
	//this_type& operator=(this_type const&) = default;

	//-------------------------------------------------------------------------
	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset >
	bool operator==(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_allocator_policy > const&)
	const
	{
		return true;
	}

	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_policy >
	bool operator==(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_policy > const&)
	const
	{
		return false;
	}

	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_policy >
	bool operator!=(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_policy > const&
				i_right)
	const
	{
		return !this->operator==(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_num       �m�ۂ���instance�̐��B
	    @param[in] i_alignment �m�ۂ���instance�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���instance�̋��Eoffset�l�Bbyte�P�ʁB
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	typename this_type::pointer allocate(
		typename this_type::size_type const i_num,
		void const* const                   i_hint = NULL)
	{
		(void)i_hint;
		return static_cast< typename this_type::pointer >(
			t_allocator_policy::allocate(
				i_num * sizeof(t_value_type),
				t_alignment,
				t_offset,
				this->get_name()));
	}

	/** @brief instance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	void deallocate(
		typename this_type::pointer const   i_instance,
		typename this_type::size_type const i_num)
	{
		t_allocator_policy::deallocate(
			i_instance, i_num * sizeof(t_value_type));
	}

	//-------------------------------------------------------------------------
	static typename this_type::pointer address(
		typename this_type::reference i_value)
	{
		return &i_value;
	}

	static typename this_type::const_pointer address(
		typename this_type::const_reference i_value)
	{
		return &i_value;
	}

	static void construct(
		typename this_type::pointer const   i_pointer,
		typename this_type::const_reference i_value)
    {
		new (i_pointer) t_value_type(i_value);
	}

	static void destroy(
		typename this_type::pointer const i_pointer)
    {
		i_pointer->~t_value_type();
		(void)i_pointer;
	}

	static typename this_type::size_type max_size()
	{
		return t_allocator_policy::max_size() / sizeof(t_value_type);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���ʖ����擾�B
	 */
	char const* get_name() const
	{
		return this->name;
	}

	/** @brief memory���ʖ���ݒ�B
	 */
	char const* set_name(char const* const i_name)
	{
		this->name = i_name;
		return i_name;
	}

//.............................................................................
private:
	char const* name; ///< debug�Ŏg�����߂�memory���ʖ��B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class std_file:
	private boost::noncopyable
{
	typedef std_file this_type;

public:
	typedef boost::shared_ptr< this_type > holder;
	typedef boost::weak_ptr< this_type > observer;

	enum flag
	{
		flag_READ   = 1 << 0,
		flag_WRITE  = 1 << 1,
		flag_BINARY = 1 << 2,
	};

	~std_file()
	{
		if (NULL != this->handle)
		{
			std::fclose(this->handle);
		}
	}

	explicit std_file(
		std::FILE* const i_handle):
	handle(i_handle)
	{
		// pass
	}

	bool read(
		void* const       o_buffer,
		std::size_t const i_size,
		std::size_t const i_offset)
	{
		if (NULL == o_buffer || NULL == this->handle)
		{
			return false;
		}

		std::fseek(this->handle, i_offset, SEEK_SET);
		std::size_t const a_count(1);
		return a_count == std::fread(o_buffer, i_size, a_count, this->handle);
	}

	std::size_t get_size() const
	{
		return NULL != this->handle
			&& 0 == std::fseek(this->handle, 0, SEEK_END)?
				std::ftell(this->handle): 0;
	}

	template< typename t_allocator >
	static this_type::holder create(
		t_allocator&       io_allocator,
		char const* const  i_path,
		unsigned const     i_flags)
	{
		if (NULL != i_path)
		{
			char a_flags[4];
			std::size_t a_num_flags(0);
			if (0 != (i_flags & this_type::flag_READ))
			{
				PSYQ_ASSERT(a_num_flags < sizeof(a_flags));
				a_flags[a_num_flags] = 'r';
				++a_num_flags;
			}
			if (0 != (i_flags & this_type::flag_WRITE))
			{
				PSYQ_ASSERT(a_num_flags < sizeof(a_flags));
				a_flags[a_num_flags] = 'w';
				++a_num_flags;
			}
			if (0 != (i_flags & this_type::flag_BINARY))
			{
				PSYQ_ASSERT(a_num_flags < sizeof(a_flags));
				a_flags[a_num_flags] = 'b';
				++a_num_flags;
			}
			a_flags[a_num_flags] = 0;

			// file���J���B
			std::FILE* const a_handle(std::fopen(i_path, a_flags));
			if (NULL != a_handle)
			{
				return boost::allocate_shared< this_type >(
					io_allocator, a_handle);
			}
		}
		return this_type::holder();
	}

private:
	std::FILE* handle;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
#if 0
class file_task
{
	typedef file_task this_type;

public:
	typedef std::tr1::shared_ptr< this_type > holder;
	typedef std::tr1::weak_ptr< this_type > observer;

	enum state
	{
		state_EXECUTING = 0,
		state_SUCCESS = 1,
	};

	virtual ~file_task()
	{
		// pass
	}

	virtual int execute() = 0;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class file_read_task:
	public file_task
{
public:
	typedef std::tr1::shared_ptr< this_type > holder;
	typedef std::tr1::weak_ptr< this_type > observer;

	enum state
	{
		state_READ_ERROR = -0x80000000,
	};

	virtual ~file_read_task()
	{
 		std::size_t const a_unit_size(sizeof(typename t_allocator::value_type));
		this->allocator.deallocate(
			this->buffer, (this->size + a_unit_size - 1) / a_unit_size);
	}

	template< typename t_other_allocator >
	static this_type::holder create(
		t_other_allocator&      io_allocator,
		std_file::holder const& i_file,
		std::size_t const       i_read_size,
		std::size_t const       i_read_offset,
		t_allocator const&      i_buffer_allocator = io_allocator)
	{
		std_file* const a_file(i_file.get());
		if (NULL != a_file)
		{
			return boost::allocate_shared(
				io_allocator,
				i_file,
				i_read_size,
				i_read_offset,
				i_buffer_allocator);
		}
		return this_type::holder();
	}

	template< typename t_other_allocator >
	static this_type::holder create(
		t_other_allocator&      io_allocator,
		std_file::holder const& i_file,
		t_allocator const&      i_buffer_allocator = io_allocator)
	{
		std_file* const a_file(i_file.get());
		if (NULL != a_file)
		{
			return this_type::create(
				io_allocator,
				i_file,
				a_file->get_size(),
				0,
				i_buffer_allocator);
		}
		return this_type::holder();
	}

	file_read_task(
		std_file::holder const& i_file,
		std::size_t const       i_size,
		std::size_t const       i_offset,
		t_allocator const&      i_allocator):
	file(i_file),
	size(i_size),
	offset(i_offset),
	allocator(i_allocator)
	{
		std::size_t const a_unit_size(sizeof(typename t_allocator::value_type));
		this->buffer = this->allocator.allocate(
			(i_size + a_unit_size - 1) / a_unit_size);
		PSYQ_ASSERT(NULL != this->buffer);
	}

	virtual int execute()
	{
		return this->file->read(this->buffer, this->size, this->offset)?
			this_type::state_SUCCESS: this_type::state_READ_ERROR;
	}

private:
	std_file::holder file;
	std::size_t      size;
	std::size_t      offset;
	t_allocator      allocator;
	void*            buffer;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class file_server
{
public:
	file_task::holder read(
		std_file::holder const& i_file,
		std::size_t const       i_size,
		std::size_t const       i_offset)
	{
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class tcpip_file
{
	typedef tcpip_file this_type;

public:
	typedef std::tr1::shared_ptr< this_type > holder;
	typedef std::tr1::weak_ptr< this_type > observer;

	static this_type::holder create(
		boost::asio::io_service&        io_service,
		boost::asio::ip::tcp::endpoint& i_endpoint)
	{
	}

private:
	boost::asio::ip::tcp::socket socket;
};

class windows_file
{
	typedef windows_file this_type;

public:
	typedef std::tr1::shared_ptr< this_type > holder;
	typedef std::tr1::weak_ptr< this_type > observer;

	~windows_file()
	{
		::CloseHandle(this->handle);
	}

	HANDLE get_handle() const
	{
		return this->handle;
	}

private:
	HANDLE handle;
};
#endif // 0

#endif // PSYQ_ALLOCATOR_HPP_
