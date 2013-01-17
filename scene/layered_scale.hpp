#ifndef PSYQ_LAYERED_SCALE_HPP_
#define PSYQ_LAYERED_SCALE_HPP_

//#include <psyq/memory/arena.hpp>
//#include <psyq/lerp.hpp>

/// @cond
namespace psyq
{
	template< typename, typename, typename > class layered_scale;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename t_value,
	typename t_count = boost::uint32_t,
	typename t_tag = void* >
class psyq::layered_scale
{
	typedef psyq::layered_scale< t_value, t_count, t_tag > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_value value;
	public: typedef t_count count;
	public: typedef t_tag tag;
	public: typedef psyq::lerp< t_value, t_count > lerp;
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	//-------------------------------------------------------------------------
	/** @param[in] i_scale ���`���scale�l�B
	    @param[in] i_super ��ʊK�wscale�B�ŏ�ʂ̏ꍇ�͋���w�肷��B
	 */
	public: explicit layered_scale(
		typename this_type::lerp const&       i_scale = lerp(1),
		typename this_type::shared_ptr const& i_super = shared_ptr()):
	super_scale_(i_super),
	lerp_scale_(i_scale),
	last_scale_(i_scale.current()),
	last_count_(this_type::counter())
	{
		this_type* const a_super(i_super.get());
		if (NULL != a_super)
		{
			this->last_scale_ *= a_super->get_current();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief scale�l��ݒ�B
	    @param[in] i_scale �V���Ȑ��`���scale�l�B
	 */
	public: void reset(typename this_type::lerp const& i_scale)
	{
		this->lerp_scale_ = i_scale;
		this->last_count_ = this_type::counter();
		this->last_scale_ = i_scale.current();
		this_type* const a_super(this->super_scale_.get());
		if (NULL != a_super)
		{
			this->last_scale_ *= a_super->get_current();
		}
	}

	/** @brief ��ʊK�wscale��ݒ�B
	    @param[in] i_super �V���ȏ�ʊK�wscale�B�ŏ�ʂ̏ꍇ�͋���w�肷��B
	 */
	public: bool reset(typename this_type::shared_ptr const& i_super)
	{
		this_type const* const a_super(i_super.get());
		if (NULL != a_super && a_super->find_super(*this))
		{
			return false;
		}
		this->super_scale_ = i_super;
		return true;
	}

	/** @brief scale�l��ݒ�B
	    @param[in] i_scale �V���Ȑ��`���scale�l�B
	    @param[in] i_super �V���ȏ�ʊK�wscale�B�ŏ�ʂ̏ꍇ�͋���w�肷��B
	 */
	public: bool reset(
		typename this_type::lerp const&       i_scale,
		typename this_type::shared_ptr const& i_super)
	{
		this_type* const a_super(i_super.get());
		if (NULL == a_super)
		{
			this->last_scale_ = i_scale.current();
		}
		else if (a_super->find_super(*this))
		{
			return false;
		}
		else
		{
			this->last_scale_ = i_scale.current() * a_super->get_current();
		}
		this->super_scale_ = i_super;
		this->lerp_scale_ = i_scale;
		this->last_count_ = this_type::counter();
		return true;
	}

	//-------------------------------------------------------------------------
	public: static t_value get_current(
		typename this_type::shared_ptr const& i_scale,
		t_value const                         i_base = 1)
	{
		this_type* const a_scale(i_scale.get());
		return NULL != a_scale? a_scale->get_current() * i_base: i_base;
	}

	/** @brief ���݂�scale�l���擾�B
	 */
	public: t_value get_current()
	{
		t_count const a_count(this_type::counter());
		if (a_count != this->last_count_)
		{
			// count�l���قȂ��Ă�����X�V����B
			this->lerp_scale_.update(a_count - this->last_count_);
			this->last_count_ = a_count;
			this->last_scale_ = this_type::get_current(
				this->super_scale_, this->lerp_scale_.current());
		}
		return this->last_scale_;
	}

	//-------------------------------------------------------------------------
	/** @brief ���݂�count�l���擾�B
	 */
	public: static t_count get_count()
	{
		return this_type::counter();
	}

	/** @brief count�l���X�V�B
	 */
	public: static t_count update_count(t_count const i_count = 1)
	{
		return this_type::counter() += i_count;
	}

	//-------------------------------------------------------------------------
	private: bool find_super(this_type const& i_timer) const
	{
		if (&i_timer != this)
		{
			this_type const* const a_super(this->super_scale_.get());
			return NULL != a_super? a_super->find_super(i_timer): false;
		}
		return true;
	}

	private: static t_count& counter()
	{
		static t_count s_count(0);
		return s_count;
	}

	//-------------------------------------------------------------------------
	private: typename this_type::shared_ptr super_scale_; ///< ��ʊK�w��scale�B
	private: typename this_type::lerp       lerp_scale_;  ///< ���`���scale�l�B
	private: t_value                        last_scale_;  ///< �Ō�ɍX�V����scale�l�B
	private: t_count                        last_count_;  ///< �Ō�ɍX�V����count�l�B
};

#endif // !PSYQ_LAYERED_SCALE_HPP_
