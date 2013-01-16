#ifndef PSYQ_ASYNC_TASK_HPP_
#define PSYQ_ASYNC_TASK_HPP_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
//#include <psyq/thread.hpp>

namespace psyq
{
	class async_task;
	template< typename > class lockable_async_task;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�����task���class�B
 */
class psyq::async_task:
	private boost::noncopyable
{
	public: typedef psyq::async_task this_type;
	template< typename, typename, typename > friend class async_queue;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	private: template< typename, typename > class function_wrapper;

	//-------------------------------------------------------------------------
	/// ��Ԓl�B
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
	/** @brief ��Ԓl���擾�B
	 */
	public: boost::uint32_t get_state() const
	{
		return this->state_;
	}

	//-------------------------------------------------------------------------
	/** @brief ��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷���Ԓl�B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	protected: virtual bool set_lockable_state(boost::uint32_t const i_state) = 0;

	/** @brief ��Ԓl��ݒ�B
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
	private: boost::uint32_t state_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief mutex�����񓯊�����task���class�B
 */
template< typename t_mutex >
class psyq::lockable_async_task:
	public psyq::async_task
{
	public: typedef psyq::lockable_async_task< t_mutex > this_type;
	public: typedef psyq::async_task super_type;

	//-------------------------------------------------------------------------
	public: typedef t_mutex mutex;

	//-------------------------------------------------------------------------
	protected: lockable_async_task(): super_type()
	{
		// pass
	}

	/** @brief ��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷���Ԓl�B
	    @return false�ȊO�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	protected: virtual bool set_lockable_state(boost::uint32_t const i_state)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		return this->set_unlockable_state(i_state);
	}

	//-------------------------------------------------------------------------
	private: t_mutex mutex_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �֐�object���Ăяo���񓯊�����task�B
 */
template< typename t_functor, typename t_mutex >
class psyq::async_task::function_wrapper:
	public psyq::lockable_async_task< t_mutex >
{
	public: typedef psyq::async_task::function_wrapper< t_functor, t_mutex >
		this_type;
	public: typedef psyq::lockable_async_task< t_mutex > super_type;

	//-------------------------------------------------------------------------
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
	private: t_functor functor_;
};

#endif // !PSYQ_ASYNC_TASK_HPP_
