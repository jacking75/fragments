/// @file
#ifndef PSYQ_ASYNC_TASK_HPP_
#define PSYQ_ASYNC_TASK_HPP_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
//#include <psyq/thread.hpp>

/// @cond
namespace psyq
{
	class async_task;
	template< typename > class lockable_async_task;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�task�̊��^�B

    - �񓯊�task�̕ێ��q�� psyq::async_queue::insert() �̈����ɓn�����ƂŁA
      �񓯊�task��񓯊�task���squeue�ɗ\��ł���B
      �\�񂳂ꂽ�񓯊�task�͗\��busy��ԂƂȂ�A���s���I�����邩
      abort() ���Ȃ��ƁA���̔񓯊�task���squeue�ɗ\��ł��Ȃ��B

    - psyq::async_queue::flush() �Ŕ񓯊�task���squeue���X�V�����ƁA
      �\�񂳂ꂽ�񓯊�task�͎��sbusy��ԂƂȂ�B

    - ���sbusy��ԂƂȂ����񓯊�task�́A�K���Ȏ��_�Ŕ񓯊�task���sthread����
      run() ���Ăяo�����B
      ���̕Ԃ�l�� state_BUSY �ȊO�Ȃ�񓯊�task�͎��s�I�����A
      �񓯊�task���squeue����|���o�����B
      �Ԃ�l�� state_BUSY �Ȃ�񓯊�task�̎��s�͌p�����A���΂炭���čĂ�
      run() ���Ăяo�����B

    @sa async_queue
 */
class psyq::async_task:
	private boost::noncopyable
{
	/// ����object�̌^�B
	public: typedef psyq::async_task this_type;

	template< typename, typename, typename, typename >
	friend class async_queue;

	//-------------------------------------------------------------------------
	/// ����instance�̕ێ��q�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;

	/// ����insrtance�̊Ď��q�B
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	/// @cond
	private: template< typename, typename > class function_wrapper;
	/// @endcond

	//-------------------------------------------------------------------------
	/// task�̎��s��Ԓl�B
	public: enum state
	{
		state_BUSY,     ///< busy��ԁB���s�����A���s���\�񂳂�Ă���B
		state_FINISHED, ///< ����I���B
		state_ABORTED,  ///< �r���I���B
	};

	//-------------------------------------------------------------------------
	protected: async_task(): state_(this_type::state_FINISHED)
	{
		// pass
	}

	public: virtual ~async_task()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �֐�object���Ăяo���񓯊�task�𐶐��B
	    @param[in] i_allocator memory�����Ɏg��allocator�B
	    @param[in] i_functor   �Ăяo���֐�object�B
	 */
	public: template< typename t_allocator, typename t_functor >
	static typename this_type::shared_ptr create(
		t_allocator const& i_allocator,
		t_functor const&   i_functor)
	{
		return this_type::create< PSYQ_MUTEX_DEFAULT >(i_allocator, i_functor);
	}

	/** @brief �֐�object���Ăяo���񓯊�task�𐶐��B
	    @param[in] i_allocator memory�����Ɏg��allocator�B
	    @param[in] i_functor   �Ăяo���֐�object�B
	 */
	public: template<
		typename t_mutex,
		typename t_allocator,
		typename t_functor >
	static typename this_type::shared_ptr create(
		t_allocator const& i_allocator,
		t_functor const&   i_functor)
	{
		typedef this_type::function_wrapper< t_functor, t_mutex > _task;
		return PSYQ_ALLOCATE_SHARED< _task >(i_allocator, i_functor);
	}

	//-------------------------------------------------------------------------
	/** @brief task�̏�Ԓl���擾�B
	 */
	public: int get_state() const
	{
		return this->state_;
	}

	//-------------------------------------------------------------------------
	/** @brief task�̎��s��r���I���B
	    @param[in] i_state �V���ɐݒ肷�� state_BUSY �ȊO�̎��s��Ԓl�B
	 */
	public: virtual void abort(
		int const i_state = this_type::state_ABORTED) = 0;

	/** @brief ���s��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷����s��Ԓl�B
	    @retval !=false ���s��Ԓl��ݒ肵���B
	    @retval ==false busy��Ԃ������̂ŁA���s��Ԓl��ݒ�ł��Ȃ������B
	 */
	protected: virtual bool set_state(int const i_state) = 0;

	/** @brief ���s��Ԓl��task�I����Ԃ�ݒ�B
	    @param[in] i_state �ݒ肷�� state_BUSY �ȊO�̎��s��Ԓl�B
	 */
	protected: void set_finish_state(int const i_state)
	{
		if (this_type::state_BUSY != i_state &&
			this_type::state_BUSY == this->get_state())
		{
			this->state_ = i_state;
		}
	}

	/** @brief idle��Ԃ�task�Ɏ��s��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷����s��Ԓl�B
	    @retval !=false ���s��Ԓl��ݒ肵���B
	    @retval ==false busy��Ԃ������̂ŁA���s��Ԓl��ݒ�ł��Ȃ������B
	 */
	protected: bool set_idle_state(int const i_state)
	{
		if (this_type::state_BUSY != this->get_state())
		{
			this->state_ = i_state;
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------------------
	/** @brief task�����s�B
	    @return ���s��ԁB async_task::state_BUSY �ȊO���Ɣ񓯊�task�̎��s���I�����A�V���ɂ��̒l�����s��Ԓl�ɐݒ肳���B
	 */
	protected: virtual int run() = 0;

	//-------------------------------------------------------------------------
	private: boost::int32_t state_; ///< task�̎��s��Ԓl�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief mutex�����Ɏg���񓯊�task�̊��^�B
    @tparam t_mutex @copydoc lockable_async_task::mutex
 */
template< typename t_mutex >
class psyq::lockable_async_task:
	public psyq::async_task
{
	/// ����object�̌^�B
	public: typedef psyq::lockable_async_task< t_mutex > this_type;

	/// ����object�̊��^�B
	public: typedef psyq::async_task super_type;

	//-------------------------------------------------------------------------
	/// lock�Ɏg��mutex�̌^�B
	public: typedef t_mutex mutex;

	//-------------------------------------------------------------------------
	protected: lockable_async_task(): super_type()
	{
		// pass
	}

	public: virtual void abort(int const i_state = super_type::state_ABORTED)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		this->set_finish_state(i_state);
	}

	protected: virtual bool set_state(int const i_state)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		return this->set_idle_state(i_state);
	}

	//-------------------------------------------------------------------------
	private: t_mutex mutex_; ///< lock�Ɏg��mutex�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �֐�object���Ăяo���񓯊�task�B
    @tparam t_functor �Ăяo���֐�object�̌^�B
    @tparam t_mutex   ����object�Ŏg��mutex�̌^�B
 */
template< typename t_functor, typename t_mutex >
class psyq::async_task::function_wrapper:
	public psyq::lockable_async_task< t_mutex >
{
	/// ����object�̌^�B
	public: typedef psyq::async_task::function_wrapper< t_functor, t_mutex >
		this_type;

	/// ����object�̊��^�B
	public: typedef psyq::lockable_async_task< t_mutex > super_type;

	//-------------------------------------------------------------------------
	/** @param[in] i_functor �Ăяo���֐�object�̏����l�B
	 */
	public: explicit function_wrapper(t_functor const& i_functor):
	psyq::lockable_async_task< t_mutex >(),
	functor_(i_functor)
	{
		// pass
	}

	protected: virtual int run()
	{
		return this->functor_();
	}

	//-------------------------------------------------------------------------
	private: t_functor functor_; ///< �Ăяo���֐�object�B
};

#endif // !PSYQ_ASYNC_TASK_HPP_
