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
/** @brief �񓯊�����task�̊��^�B
 */
class psyq::async_task:
	private boost::noncopyable
{
	/// ����object�̌^�B
	public: typedef psyq::async_task this_type;

	template< typename, typename, typename > friend class async_queue;

	//-------------------------------------------------------------------------
	/// ����instance�̕ێ��q�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;

	/// ����insrtance�̊Ď��q�B
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	/// @cond
	private: template< typename, typename > class function_wrapper;
	/// @endcond

	//-------------------------------------------------------------------------
	/// task�̏�Ԓl�B
	public: enum state
	{
		state_BUSY,     ///< ���s���B
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
	/** @brief �֐�object���Ăяo���񓯊�����task�𐶐��B
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

	/** @brief �֐�object���Ăяo���񓯊�����task�𐶐��B
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
	public: boost::uint32_t get_state() const
	{
		return this->state_;
	}

	//-------------------------------------------------------------------------
	/** @brief ���������̂�҂��Ă���task�̏�Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷���Ԓl�B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	protected: virtual bool set_lockable_state(boost::uint32_t const i_state) = 0;

	/** @brief ���𖳎�����task�̏�Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷���Ԓl�B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	protected: bool set_unlockable_state(boost::uint32_t const i_state)
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
	 */
	protected: virtual boost::uint32_t run() = 0;

	//-------------------------------------------------------------------------
	/// task�̏�Ԓl�B
	private: boost::uint32_t state_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief mutex�����Ɏg���񓯊�����task�̊��^�B
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
	/// ���Ɏg��mutex�̌^�B
	public: typedef t_mutex mutex;

	//-------------------------------------------------------------------------
	protected: lockable_async_task(): super_type()
	{
		// pass
	}

	protected: virtual bool set_lockable_state(boost::uint32_t const i_state)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		return this->set_unlockable_state(i_state);
	}

	//-------------------------------------------------------------------------
	/// ���Ɏg��mutex�B
	private: t_mutex mutex_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �֐�object���Ăяo���񓯊�����task�B
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

	protected: virtual boost::uint32_t run()
	{
		return this->functor_();
	}

	//-------------------------------------------------------------------------
	/// �Ăяo���֐�object��instance�B
	private: t_functor functor_;
};

#endif // !PSYQ_ASYNC_TASK_HPP_
