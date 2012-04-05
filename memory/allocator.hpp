#ifndef PSYQ_ALLOCATOR_HPP_
#define PSYQ_ALLOCATOR_HPP_

#ifndef PSYQ_ALLOCATOR_POLICY_DEFAULT
#define PSYQ_ALLOCATOR_POLICY_DEFAULT psyq::heap_allocator_policy
#endif // !PSYQ_ALLOCATOR_POLICY_DEFAULT

#ifndef PSYQ_ALLOCATOR_NAME_DEFAULT
#define PSYQ_ALLOCATOR_NAME_DEFAULT "PSYQ"
#endif // !PSYQ_ALLOCATOR_NAME_DEFAULT

namespace psyq
{
	template< typename, std::size_t, std::size_t > class _allocator_base;
	template< typename, std::size_t, std::size_t, typename > class allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename    t_value_type,
	std::size_t t_alignment,
	std::size_t t_offset >
class psyq::_allocator_base:
	public std::allocator< t_value_type >
{
	typedef psyq::_allocator_base< t_value_type, t_alignment, t_offset >
		this_type;
	typedef std::allocator< t_value_type > super_type;

	// �z�u���E�l��2�ׂ̂��悩�m�F�B
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);

//.............................................................................
public:
	//-------------------------------------------------------------------------
	/** @brief memory���ʖ����擾�B
	 */
	char const* get_name() const
	{
		return this->name_;
	}

	/** @brief memory���ʖ���ݒ�B
	    @param[in] i_name �ݒ肷��memory���ʖ��̕�����B
	 */
	char const* set_name(char const* const i_name)
	{
		this->name_ = i_name;
		return i_name;
	}

	//this_type& operator=(this_type const&) = default;

//.............................................................................
protected:
	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit _allocator_base(char const* const i_name):
	super_type(),
	name_(i_name)
	{
		// pass
	}

	/** @param[in] i_source copy��instance�B
	 */
	_allocator_base(this_type const& i_source):
	super_type(i_source),
	name_(i_source.get_name())
	{
		// pass
	}

	/** @param[in] i_source copy��instance�B
	 */
	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset >
	explicit _allocator_base(
		psyq::_allocator_base<
			t_other_type, t_other_alignment, t_other_offset > const&
				i_source):
	super_type(i_source),
	name_(i_source.get_name())
	{
		// pass
	}

//.............................................................................
private:
	void allocate();
	void deallocate();

//.............................................................................
public:
	static std::size_t const alignment = t_alignment;
	static std::size_t const offset = t_offset;

private:
	char const* name_; ///< debug�Ŏg�����߂�memory���ʖ��B
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
class psyq::allocator:
	public psyq::_allocator_base< t_value_type, t_alignment, t_offset >
{
	typedef psyq::allocator<
		t_value_type, t_alignment, t_offset, t_allocator_policy >
			this_type;
	typedef psyq::_allocator_base< t_value_type, t_alignment, t_offset >
		super_type;

//.............................................................................
public:
	typedef t_allocator_policy allocator_policy;

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
	super_type(i_name)
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
	super_type(i_source)
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
		void* const a_memory(
			(t_allocator_policy::malloc)(
				i_num * sizeof(t_value_type),
				t_alignment,
				t_offset,
				this->get_name()));
		PSYQ_ASSERT(NULL != a_memory);
		return static_cast< typename this_type::pointer >(a_memory);
	}

	/** @brief instance�Ɏg���Ă���memory���������B
	    @param[in] i_memory �������instance�̐擪�ʒu�B
	    @param[in] i_num    �������instance�̐��B
	 */
	void deallocate(
		typename this_type::pointer const   i_memory,
		typename this_type::size_type const i_num)
	{
		(t_allocator_policy::free)(i_memory, i_num * sizeof(t_value_type));
	}
};

#endif // !PSYQ_ALLOCATOR_HPP_