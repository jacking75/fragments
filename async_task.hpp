#ifndef PSYQ_ASYNC_TASK_HPP_
#define PSYQ_ASYNC_TASK_HPP_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/locks.hpp>

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
	typedef psyq::async_task this_type;

	template< typename > friend class async_queue;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	enum state
	{
		state_BUSY,     ///< �ғ����B
		state_FINISHED, ///< ����I���B
		state_ABORTED,  ///< �r���I���B
		state_end
	};

	//-------------------------------------------------------------------------
	/** @brief �֐�object���Ăяo���񓯊�����task�𐶐��B
	    @param[in] i_allocator memory�����Ɏg��allocator�B
	    @param[in] i_functor   �Ăяo���֐�object�B
	 */
	template< typename t_allocator, typename t_functor >
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
	template< typename t_mutex, typename t_allocator, typename t_functor >
	static typename this_type::shared_ptr create(
		t_allocator const& i_allocator,
		t_functor const&   i_functor)
	{
		typedef this_type::function_wrapper< t_functor, t_mutex > _task;
		return boost::allocate_shared< _task >(i_allocator, i_functor);
	}

	//-------------------------------------------------------------------------
	virtual ~async_task()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief ��Ԓl���擾�B
	 */
	boost::uint32_t get_state() const
	{
		return this->state_;
	}

	/** @brief ��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷��state_BUSY�ȊO�̏�Ԓl�B
	    @return true�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	bool set_state(boost::uint32_t const i_state)
	{
		// busy��Ԃɂ͐ݒ�ł��Ȃ��B
		return this_type::state_BUSY != i_state?
			this->set_locked_state(i_state): false;
	}

//.............................................................................
protected:
	async_task():
	state_(this_type::state_FINISHED)
	{
		// pass
	}

	virtual boost::uint32_t run() = 0;
	virtual bool set_locked_state(boost::uint32_t const i_state) = 0;

	void set_unlocked_state(boost::uint32_t const i_state)
	{
		this->state_ = i_state;
	}

//.............................................................................
private:
	template< typename, typename > class function_wrapper;

//.............................................................................
private:
	boost::uint32_t state_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief mutex�����񓯊�����task���class�B
 */
template< typename t_mutex >
class psyq::lockable_async_task:
	public psyq::async_task
{
	typedef psyq::lockable_async_task< t_mutex > this_type;
	typedef psyq::async_task super_type;

//.............................................................................
public:
	typedef t_mutex mutex;

//.............................................................................
protected:
	lockable_async_task():
	super_type()
	{
		// pass
	}

	virtual bool set_locked_state(boost::uint32_t const i_state)
	{
		boost::lock_guard< t_mutex > const a_lock(this->mutex_);
		if (super_type::state_BUSY != this->get_state())
		{
			this->set_unlocked_state(i_state);
			return true;
		}
		return false;
	}

//.............................................................................
private:
	t_mutex mutex_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �֐�object���Ăяo���񓯊�����task�B
 */
template< typename t_functor, typename t_mutex >
class psyq::async_task::function_wrapper:
	public psyq::lockable_async_task< t_mutex >
{
//.............................................................................
public:
	explicit function_wrapper(t_functor const& i_functor):
	psyq::lockable_async_task< t_mutex >(),
	functor_(i_functor)
	{
		// pass
	}

//.............................................................................
protected:
	virtual boost::uint32_t run()
	{
		return this->functor_();
	}

//.............................................................................
private:
	t_functor functor_;
};

#endif // !PSYQ_ASYNC_TASK_HPP_
