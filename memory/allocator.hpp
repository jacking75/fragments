#ifndef PSYQ_ALLOCATOR_HPP_
#define PSYQ_ALLOCATOR_HPP_

#include <memory>
#include <boost/type_traits/alignment_of.hpp>
//#include <psyq/memory/arena.hpp>

/// @cond
namespace psyq
{
	template< typename, std::size_t, std::size_t > class _allocator_base;
	template< typename, std::size_t, std::size_t, typename > class allocator;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief interface�� std::allocator �݊���memory�����q�̊��^�B
    @tparam t_value     @copydoc _allocator_base::value_type
    @tparam t_alignment @copydoc _allocator_base::ALIGNMENT
    @tparam t_offset    @copydoc _allocator_base::OFFSET
 */
template<
	typename    t_value,
	std::size_t t_alignment,
	std::size_t t_offset >
class psyq::_allocator_base:
	public std::allocator< t_value >
{
	/// ����object�̌^�B
	public: typedef psyq::_allocator_base< t_value, t_alignment, t_offset >
		this_type;

	/// ����object�̊��^�B
	public: typedef std::allocator< t_value > super_type;

	// �z�u���E�l��2�ׂ̂��悩�m�F�B
	/// @cond
	BOOST_STATIC_ASSERT(0 == (t_alignment & (t_alignment - 1)));
	BOOST_STATIC_ASSERT(0 < t_alignment);
	/// @endcond

	//-------------------------------------------------------------------------
	/// �����Ă�object�̌^�B
	public: typedef super_type::value_type value_type;

	/// �����Ă�object��memory�z�u���E�l�Bbyte�P�ʁB
	public: static std::size_t const ALIGNMENT = t_alignment;

	/// �����Ă�object��memory�z�uoffset�l�Bbyte�P�ʁB
	public: static std::size_t const OFFSET = t_offset;

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	protected: explicit _allocator_base(char const* const i_name):
	super_type(),
	name_(i_name)
	{
		// pass
	}

	/** @param[in] i_source copy��instance�B
	 */
	protected: _allocator_base(this_type const& i_source):
	super_type(i_source),
	name_(i_source.get_name())
	{
		// pass
	}

	/** @param[in] i_source copy��instance�B
	 */
	protected: template<
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

	//-------------------------------------------------------------------------
	//public: this_type& operator=(this_type const&) = default;

	//-------------------------------------------------------------------------
	/** @brief memory���ʖ����擾�B
	 */
	public: char const* get_name() const
	{
		return this->name_;
	}

	/** @brief memory���ʖ���ݒ�B
	    @param[in] i_name �ݒ肷��memory���ʖ��̕�����B
	 */
	public: void set_name(char const* const i_name)
	{
		this->name_ = i_name;
	}

	//-------------------------------------------------------------------------
	private: void allocate();   ///< �g�p�֎~�B
	private: void deallocate(); ///< �g�p�֎~�B

	//-------------------------------------------------------------------------
	private: char const* name_; ///< debug�Ŏg�����߂�memory���ʖ��B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief interface�� std::allocator �݊���memory�����q�B
    @tparam t_value     @copydoc _allocator_base::value_type
    @tparam t_alignment @copydoc _allocator_base::ALIGNMENT
    @tparam t_offset    @copydoc _allocator_base::OFFSET
    @tparam t_arena     @copydoc allocator::arena
 */
template<
	typename    t_value,
	std::size_t t_alignment = boost::alignment_of< t_value >::value,
	std::size_t t_offset = 0,
	typename    t_arena = PSYQ_ARENA_DEFAULT >
class psyq::allocator:
	public psyq::_allocator_base< t_value, t_alignment, t_offset >
{
	/// ����object�̌^�B
	typedef psyq::allocator< t_value, t_alignment, t_offset, t_arena >
		this_type;

	/// ����object�̊��^�B
	typedef psyq::_allocator_base< t_value, t_alignment, t_offset >
		super_type;

	//-------------------------------------------------------------------------
	/// memory����policy�̌^�B
	public: typedef t_arena arena;

	//-------------------------------------------------------------------------
	/** @brief psyq::allocator �̍Ē�`policy�B
	    @tparam t_other_type      @copydoc _allocator_base::value_type
	    @tparam t_other_alignment @copydoc _allocator_base::ALIGNMENT
	    @tparam t_other_offset    @copydoc _allocator_base::OFFSET
	    @tparam t_other_arena     @copydoc allocator::arena
	 */
	public: template<
		typename    t_other_type,
		std::size_t t_other_alignment =
			boost::alignment_of< t_other_type >::value,
		std::size_t t_other_offset = t_offset,
		typename    t_other_arena = t_arena >
	struct rebind
	{
		/// �Ē�`���� allocator �̌^�B
		typedef psyq::allocator<
			t_other_type, t_other_alignment, t_other_offset, t_other_arena >
				other;
	};

	//-------------------------------------------------------------------------
	/** @param[in] i_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	public: explicit allocator(char const* const i_name = PSYQ_ARENA_NAME_DEFAULT):
	super_type(i_name)
	{
		// pass
	}

	//allocator(this_type const&) = default;

	/** @param[in] i_source copy��instance�B
	 */
	public: template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_arena >
	allocator(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_arena > const&
				i_source):
	super_type(i_source)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	//public: this_type& operator=(this_type const&) = default;

	//-------------------------------------------------------------------------
	public: template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset >
	bool operator==(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_arena > const&)
	const
	{
		return true;
	}

	public: template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_arena >
	bool operator==(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_arena > const&)
	const
	{
		return false;
	}

	public: template<
		typename    t_other_type,
		std::size_t t_other_alignment,
		std::size_t t_other_offset,
		typename    t_other_arena >
	bool operator!=(
		psyq::allocator<
			t_other_type,
			t_other_alignment,
			t_other_offset,
			t_other_arena > const&
				i_right)
	const
	{
		return !this->operator==(i_right);
	}

	//-------------------------------------------------------------------------
	/** @brief instance�Ɏg��memory���m�ۂ���B
	    @param[in] i_num  �m�ۂ���instance�̐��B
	    @param[in] i_hint �œK���̂��߂�hint�B
	    @return �m�ۂ���memory�̐擪�ʒu�B������NULL�̏ꍇ�͎��s�B
	 */
	public: typename this_type::pointer allocate(
		typename this_type::size_type const i_num,
		void const* const                   i_hint = NULL)
	{
		(void)i_hint;
		void* const a_memory(
			(t_arena::malloc)(
				i_num * sizeof(t_value),
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
	public: void deallocate(
		typename this_type::pointer const   i_memory,
		typename this_type::size_type const i_num)
	{
		(t_arena::free)(i_memory, i_num * sizeof(t_value));
	}
};

#endif // !PSYQ_ALLOCATOR_HPP_
