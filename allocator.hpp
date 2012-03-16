#ifndef PSYQ_ALLOCATOR_HPP_
#define PSYQ_ALLOCATOR_HPP_

namespace psyq
{
	template< typename, typename > class allocator;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::allocator�݊���instance�����q�B
    @tparam t_value_type    ���蓖�Ă�instance�̌^�B
    @tparam t_memory_policy memory����policy�B
 */
template<
	typename t_value_type,
	typename t_memory_policy = PSYQ_MEMORY_POLICY_DEFAULT >
class psyq::allocator:
	public std::allocator< t_value_type >
{
	typedef psyq::allocator< t_value_type, t_memory_policy > this_type;
	typedef std::allocator< t_value_type > super_type;

//.............................................................................
public:
	typedef t_memory_policy memory_policy;

	template< typename t_other_type >
	struct rebind
	{
		typedef psyq::allocator< t_other_type, t_memory_policy > other;
	};

	//-------------------------------------------------------------------------
	explicit allocator(char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT):
	super_type(),
	name(i_name)
	{
		// pass
	}

	allocator(this_type const& i_source):
	super_type(i_source),
	name(i_source.get_name())
	{
		// pass
	}

	template< typename t_other_type >
	allocator(
		psyq::allocator< t_other_type, t_memory_policy > const& i_source):
	super_type(i_source),
	name(i_source.get_name())
	{
		// pass
	}

	//-------------------------------------------------------------------------
	this_type& operator=(this_type const& i_source)
	{
		this->name = i_source.get_name();
		this->super_type::operator=(i_source);
		return *this;
	}

	template< typename t_other_type >
	this_type& operator=(
		psyq::allocator< t_other_type, t_memory_policy > const& i_source)
	{
		this->name = i_source.get_name();
		this->super_type::operator=(i_source);
		return *this;
	}

	//-------------------------------------------------------------------------
	template< typename t_other_type >
	bool operator==(
		psyq::allocator< t_other_type, t_memory_policy > const& i_right)
	const
	{
		return *static_cast< super_type* >(this) == i_right;
	}

	template< typename t_other_type >
	bool operator!=(
		psyq::allocator< t_other_type, t_memory_policy > const& i_right)
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
	typename super_type::pointer allocate(
		typename super_type::size_type const   i_num,
		std::size_t const                      i_alignment =
			boost::alignment_of< t_value_type >::value,
		std::size_t const                      i_offset = 0)
	{
		return t_memory_policy::allocate(
			i_num * sizeof(t_value_type),
			i_alignment,
			i_offset,
			this->get_name());
	}

	/** @brief instance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	void deallocate(
		typename super_type::pointer const   i_instance,
		typename super_type::size_type const i_num)
	{
		(void)i_num;
		t_memory_policy::deallocate(i_instance);
	}

	//-------------------------------------------------------------------------
	static typename super_type::size_type max_size()
	{
		return t_memory_policy::max_size() / sizeof(t_value_type);
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

#endif // PSYQ_ALLOCATOR_HPP_
