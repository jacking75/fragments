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
class psyq::allocator
{
	typedef psyq::allocator< t_value_type, t_memory_policy > this_type;

//.............................................................................
public:
	typedef t_memory_policy     memory_policy;
	typedef std::size_t         size_type;
	typedef std::ptrdiff_t      difference_type;
	typedef t_value_type*       pointer;
	typedef const t_value_type* const_pointer;
	typedef t_value_type&       reference;
	typedef const t_value_type& const_reference;
	typedef t_value_type        value_type;

	//-------------------------------------------------------------------------
	template<
		typename t_other_type,
		typename t_other_policy = t_memory_policy >
	struct rebind
	{
		typedef psyq::allocator< t_other_type, t_other_policy > other;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit allocator(char const* const i_name = PSYQ_MEMORY_NAME_DEFAULT):
	name(i_name)
	{
		// pass
	}

	//allocator(this_type const&) = default;

	/** @param[in] i_source copy��instance�B
	 */
	template< typename t_other_type, typename t_other_memory >
	allocator(
		psyq::allocator< t_other_type, t_other_memory > const& i_source):
	name(i_source.get_name())
	{
		// pass
	}

	//-------------------------------------------------------------------------
	//this_type& operator=(this_type const&) = default;

	template< typename t_other_type, typename t_other_memory >
	this_type& operator=(
		psyq::allocator< t_other_type, t_other_memory > const& i_source)
	{
		this->set_name(i_source.get_name());
		return *this;
	}

	//-------------------------------------------------------------------------
	bool operator==(this_type const&) const
	{
		return true;
	}

	bool operator!=(this_type const& i_right) const
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
		std::size_t const                   i_alignment =
			boost::alignment_of< t_value_type >::value,
		std::size_t const                   i_offset = 0)
	{
		return static_cast< typename this_type::pointer >(
			t_memory_policy::allocate(
				i_num * sizeof(t_value_type),
				i_alignment,
				i_offset,
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
		t_memory_policy::deallocate(i_instance, i_num * sizeof(t_value_type));
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
