#ifndef PSYQ_VIRTUAL_ALLOCATOR_HPP_
#define PSYQ_VIRTUAL_ALLOCATOR_HPP_

namespace psyq
{
	class virtual_allocator_policy;
	template< typename, std::size_t, std::size_t > class virtual_allocator;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::virtual_allocator_policy:
	private boost::noncopyable
{
	typedef psyq::virtual_allocator_policy this_type;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > holder;
	typedef boost::weak_ptr< this_type > observer;

	//-------------------------------------------------------------------------
	virtual ~virtual_allocator_policy()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	bool operator==(this_type const& i_right) const
	{
		return this->get_allocator() == i_right.get_allocator()
			&& this->get_deallocator() == i_right.get_deallocator();
	}

	bool operator!=(this_type const& i_right) const
	{
		return !this->operator==(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief memory���m�ۂ���B
	    @param[in] i_size      �m�ۂ���memory�̑傫���Bbyte�P�ʁB
	    @param[in] i_alignment �m�ۂ���memory�̋��E�l�Bbyte�P�ʁB
	    @param[in] i_offset    �m�ۂ���memory�̋��Eoffset�l�Bbyte�P�ʁB
	    @param[in] i_name      debug�Ŏg�����߂�memory���ʖ��B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	void* allocate(
		std::size_t const i_size,
		std::size_t const i_alignment,
		std::size_t const i_offset,
		char const* const i_name)
	{
		return (*this->get_allocator())(i_size, i_alignment, i_offset, i_name);
	}

	/** @brief memory���������B
	    @param[in] i_memory �������memory�̐擪�ʒu�B
	    @param[in] i_size   �������memory�̑傫���Bbyte�P�ʁB
	 */
	void deallocate(
		void* const       i_memory,
		std::size_t const i_size)
	{
		(*this->get_deallocator())(i_memory, i_size);
	}

	/** @brief ��x�Ɋm�ۂł���memory�̍ő�size���擾�Bbyte�P�ʁB
	 */
	virtual std::size_t max_size() const = 0;

	//-------------------------------------------------------------------------
	template< typename t_allocator >
	static this_type::holder create(
		t_allocator const& i_allocator)
	{
		typedef typename t_allocator::allocator_policy allocator_policy;
		return this_type::create< allocator_policy >(i_allocator);
	}

	template< typename t_allocator_policy, typename t_allocator >
	static this_type::holder create(
		t_allocator const& i_allocator)
	{
		typedef this_type::wrapper< t_allocator_policy > wrapper;
		return boost::allocate_shared< wrapper >(i_allocator);
	}

//.............................................................................
protected:
	typedef void* (*allocate_function)(
		std::size_t const,
		std::size_t const,
		std::size_t const,
		char const* const);
	typedef void (*deallocate_function)(void* const, std::size_t const);

	virtual_allocator_policy()
	{
		// pass
	};

	virtual this_type::allocate_function get_allocator() const = 0;

	virtual this_type::deallocate_function get_deallocator() const = 0;

//.............................................................................
private:
	template< typename > class wrapper;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_allocator_policy >
class psyq::virtual_allocator_policy::wrapper:
	public psyq::virtual_allocator_policy
{
	typedef psyq::virtual_allocator_policy::wrapper< t_allocator_policy >
		this_type;
	typedef psyq::virtual_allocator_policy super_type;

protected:
	virtual typename super_type::allocate_function get_allocator() const
	{
		return &t_allocator_policy::allocate;
	}

	virtual typename super_type::deallocate_function get_deallocator() const
	{
		return &t_allocator_policy::deallocate;
	}

	virtual std::size_t max_size() const
	{
		return t_allocator_policy::max_size;
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename    t_value_type,
	std::size_t t_alignment = boost::alignment_of< t_value_type >::value,
	std::size_t t_offset = 0 >
class psyq::virtual_allocator:
	public psyq::_allocator_traits< t_value_type, t_alignment, t_offset >
{
	typedef psyq::virtual_allocator< t_value_type, t_alignment, t_offset >
		this_type;
	typedef psyq::_allocator_traits< t_value_type, t_alignment, t_offset >
		super_type;

//.............................................................................
public:
	template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset >
	struct rebind
	{
		typedef psyq::allocator<
			t_other_type, t_other_alignment, t_other_offset >
				other;
	};

	//-------------------------------------------------------------------------
	explicit virtual_allocator(
		psyq::virtual_allocator_policy::holder const& i_policy,
		char const* const i_name = PSYQ_ALLOCATOR_NAME_DEFAULT):
	super_type(i_name),
	policy_(i_policy)
	{
		PSYQ_ASSERT(NULL != i_policy.get());
	}

	/** @param[in] i_source copy��instance�B
	 */
	template< typename t_other_type, std::size_t t_other_alignment >
	virtual_allocator(
		psyq::virtual_allocator< t_other_type, t_other_alignment > const&
			i_source):
	super_type(i_source),
	policy_(i_source.get_policy())
	{
		BOOST_STATIC_ASSERT(t_other_alignment % t_alignment == 0);
		PSYQ_ASSERT(sizeof(t_value_type) <= this->get_policy()->max_size());
	}

	//-------------------------------------------------------------------------
	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset >
	bool operator==(
		psyq::virtual_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset > const& i_right)
	const
	{
		return *this->get_policy() == *i_right.get_policy();
	}

	template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset >
	bool operator!=(
		psyq::virtual_allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset > const&
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
	typename super_type::pointer allocate(
		typename super_type::size_type const i_num,
		void const* const                    i_hint = NULL)
	{
		return this->policy_->allcoate(
			i_num * sizeof(t_value_type),
			t_alignment,
			t_offset,
			this->get_name());
	}

	/** @brief intance�Ɏg���Ă���memory���������B
	    @param[in] i_instance �������instance�̐擪�ʒu�B
	    @param[in] i_num      �������instance�̐��B
	 */
	void deallocate(
		typename super_type::pointer const   i_instance,
		typename super_type::size_type const i_num)
	{
		return this->policy_->deallocate(
			i_instance, i_num * sizeof(t_value_type));
	}

	/** @brief ��x�Ɋm�ۂł���instance�̍ő吔���擾�B
	 */
	typename super_type::size_type max_size() const
	{
		return this->policy_->max_size() / sizeof(t_value_type);
	}

	//-------------------------------------------------------------------------
	psyq::virtual_allocator_policy::holder const& get_policy() const
	{
		return this->policy_;
	}

//.............................................................................
private:
	psyq::virtual_allocator_policy::holder policy_;
};

#endif // PSYQ_VIRTUAL_ALLOCATOR_HPP_
