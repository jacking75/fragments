#ifndef PSYQ_ASYNC_QUEUE_HPP_
#define PSYQ_ASYNC_QUEUE_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
//#include <psyq/async_task.hpp>
//#include <psyq/memory/arena.hpp>

namespace psyq
{
	template < typename > class async_queue;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�����task-queue�B
 */
template< typename t_arena >
class psyq::async_queue:
	private boost::noncopyable
{
	typedef psyq::async_queue< t_arena > this_type;

//.............................................................................
public:
	typedef t_arena arena;

	//-------------------------------------------------------------------------
	~async_queue()
	{
		this->stop(true);
		this_type::abort_tasks(this->reserve_tasks_, this->reserve_capacity_);
	}

	/** @param[in] i_memory_name debug�Ŏg�����߂�memory���ʖ��B
	 */
	explicit async_queue(
		char const* const i_memory_name = PSYQ_ARENA_NAME_DEFAULT):
	arena_name_(i_memory_name),
	reserve_tasks_(NULL),
	reserve_capacity_(0),
	running_size_(0),
	stop_(false)
	{
		this->start();
	}

	//-------------------------------------------------------------------------
	bool is_empty() const
	{
		boost::lock_guard< boost::mutex > const a_lock(
			const_cast< boost::mutex& >(this->mutex_));
		return this->reserve_capacity_ <= 0 && this->running_size_ <= 0;
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�����task��o�^�B
	    @param[in] i_task �o�^����񓯊�����task���w��smart-pointer�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	std::size_t add(psyq::async_task::shared_ptr const& i_task)
	{
		return this->add(&i_task, &i_task + 1);
	}

	/** @brief container�����񓯊�����task���܂Ƃ߂ēo�^�B
	    @tparam t_iterator shared_ptr�^��container��iterator�^�B
	    @param[in] i_begin container�̐擪�ʒu�B
	    @param[in] i_end   container�̖����ʒu�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_iterator >
	std::size_t add(t_iterator const i_begin, t_iterator const i_end)
	{
		// �\��queue�����o���B
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		typename this_type::task_ptr* const a_last_tasks(this->reserve_tasks_);
		std::size_t const a_last_capacity(this->reserve_capacity_);
		std::size_t const a_last_size(
			0 < a_last_capacity? a_last_capacity: this->running_size_);

		// �V����queue���\�z�B
		std::size_t const a_capacity(
			a_last_size + std::distance(i_begin, i_end));
		typename this_type::task_ptr* const a_tasks(
			this_type::resize_tasks(
				a_capacity,
				a_last_tasks,
				a_last_size,
				a_last_capacity,
				this->arena_name_));

		// container�����񓯊�����task���A�V����queue��copy�B
		std::size_t a_count(0);
		t_iterator a_iterator(i_begin);
		for (std::size_t i = a_last_size; i < a_capacity; ++i, ++a_iterator)
		{
			psyq::async_task* const a_task(a_iterator->get());
			if (NULL != a_task
				&& a_task->set_locked_state(psyq::async_task::state_BUSY))
			{
				// busy��Ԃł͂Ȃ��񓯊�����task�������o�^�ł���B
				new(&a_tasks[i]) typename this_type::task_ptr(*a_iterator);
				++a_count;
			}
			else
			{
				new(&a_tasks[i]) typename this_type::task_ptr();
			}
		}

		// �V����queue��\��queue�ɍ����ւ��Ă���ʒm�B
		this->reserve_tasks_ = a_tasks;
		this->reserve_capacity_ = a_capacity;
		this->condition_.notify_all();
		return a_count;
	}

	/** @brief queue�̑傫����K�v�Œ���ɂ���B
	 */
	void shrink()
	{
		this->add(
			static_cast< psyq::async_task::shared_ptr const* >(NULL),
			static_cast< psyq::async_task::shared_ptr const* >(NULL));
	}

//.............................................................................
private:
	typedef psyq::async_task::weak_ptr task_ptr;

	//-------------------------------------------------------------------------
	void start()
	{
		if (!this->thread_.joinable())
		{
			this->stop_ = false;
			boost::thread(boost::bind(&this_type::run, this)).swap(
				this->thread_);
		}
	}

	void stop(bool const i_sync = true)
	{
		// �I��flag��L�����B
		this->stop_ = true;
		if (i_sync)
		{
			// thread�̏I���܂ő҂B
			this->condition_.notify_all();
			this->thread_.join();
		}
	}

	void run()
	{
		typename this_type::task_ptr* a_tasks(NULL);
		std::size_t a_size(0);
		std::size_t a_capacity(0);
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		while (!this->stop_)
		{
			// ���squeue���X�V�B
			if (0 < this->reserve_capacity_)
			{
				// �\��queue���A�V�������squeue�Ƃ��Ď��o���B
				typename this_type::task_ptr* const a_last_tasks(a_tasks);
				std::size_t const a_last_capacity(a_capacity);
				a_tasks = this->reserve_tasks_;
				a_capacity = this->reserve_capacity_;
				this->running_size_ = this->reserve_capacity_;
				this->reserve_tasks_ = NULL;
				this->reserve_capacity_ = 0;
				a_lock.unlock();

				// ���̎��squeue������task���A�V�������squeue�Ɉړ��B
				std::size_t const a_last_size(a_size);
				PSYQ_ASSERT(a_last_size <= a_capacity);
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_tasks[i].expired());
					a_tasks[i].swap(a_last_tasks[i]);
				}
				a_size = a_capacity;

				// ���̎��squeue��j���B
				this_type::destroy_tasks(a_last_tasks, a_last_capacity);
			}
			else if (0 < a_size)
			{
				this->running_size_ = a_size;
				a_lock.unlock();
			}
			else
			{
				// �\��queue�Ǝ��squeue����ɂȂ����̂őҋ@�B
				this->running_size_ = 0;
				this->condition_.wait(a_lock);
				continue;
			}

			// ���squeue��task���Ăяo���B
			a_size = this_type::run_tasks(a_tasks, a_size);
			if (a_size <= 0)
			{
				// ���squeue����ɂȂ����̂Ŕj���B
				this_type::destroy_tasks(a_tasks, a_capacity);
				a_tasks = NULL;
				a_capacity = 0;
			}
			a_lock.lock();
		}
		this_type::abort_tasks(a_tasks, a_capacity);
	}

	//-------------------------------------------------------------------------
	/** @brief queue�����񓯊�����task�����s����B
	    @param[in] io_tasks queue�̐擪�ʒu�B
	    @param[in] i_size   queue�����񓯊�����task�̐��B
	    @return queue�����񓯊�����task�̐��B
	 */
	static std::size_t run_tasks(
		typename this_type::task_ptr* const io_tasks,
		std::size_t const                   i_size)
	{
		std::size_t a_size(0);
		for (std::size_t i = 0; i < i_size; ++i)
		{
			psyq::async_task::shared_ptr const a_holder(io_tasks[i].lock());
			psyq::async_task* const a_task(a_holder.get());
			if (NULL != a_task
				&& psyq::async_task::state_BUSY == a_task->get_state())
			{
				boost::int32_t const a_state(a_task->run());
				if (psyq::async_task::state_BUSY == a_state)
				{
					io_tasks[a_size].swap(io_tasks[i]);
					++a_size;
					continue;
				}
				a_task->set_unlocked_state(a_state);
			}
			io_tasks[i].reset();
		}
		return a_size;
	}

	/** @brief queue�̑傫����ύX����B
	    @param[in] i_capacity        �V����queue�̍ő�v�f���B
	    @param[in,out] io_last_tasks ����queue�̐擪�ʒu�B
	    @param[in] i_last_size       ����queue�̗v�f���B
	    @param[in] i_last_capacity   ����queue�̍ő�v�f���B
		@return �V����queue�̐擪�ʒu�B
	 */
	static typename this_type::task_ptr* resize_tasks(
		std::size_t const                   i_capacity,
		typename this_type::task_ptr* const io_last_tasks,
		std::size_t const                   i_last_size,
		std::size_t const                   i_last_capacity,
		char const* const                   i_memory_name)
	{
		// �V����queue���m�ہB
		typename this_type::task_ptr* const a_tasks(
			static_cast< typename this_type::task_ptr* >(
				(t_arena::malloc)(
					i_capacity * sizeof(typename this_type::task_ptr),
					boost::alignment_of< typename this_type::task_ptr >::value,
					0,
					i_memory_name)));
		if (NULL != a_tasks)
		{
			// ����queue�����񓯊�����task���A�V����queue�Ɉړ��B
			PSYQ_ASSERT(i_last_size <= i_capacity);
			for (std::size_t i = 0; i < i_last_size; ++i)
			{
				new(&a_tasks[i]) typename this_type::task_ptr();
				if (NULL != io_last_tasks)
				{
					a_tasks[i].swap(io_last_tasks[i]);
				}
			}
		}
		else
		{
			PSYQ_ASSERT(i_capacity <= 0);
		}

		// ����queue��j���B
		this_type::destroy_tasks(io_last_tasks, i_last_capacity);
		return a_tasks;
	}

	/** @brief �񓯊�����task�s���r���I������B
	    @param[in] io_tasks   queue�̐擪�ʒu�B
	    @param[in] i_capacity queue�����񓯊�����task�̐��B
	 */
	static void abort_tasks(
		typename this_type::task_ptr* const io_tasks,
		std::size_t const                   i_capacity)
	{
		for (std::size_t i = 0; i < i_capacity; ++i)
		{
			psyq::async_task::shared_ptr const a_holder(io_tasks[i].lock());
			psyq::async_task* const a_task(a_holder.get());
			if (NULL != a_task
				&& psyq::async_task::state_BUSY == a_task->get_state())
			{
				a_task->set_unlocked_state(
					psyq::async_task::state_ABORTED);
			}
		}
		this_type::destroy_tasks(io_tasks, i_capacity);
	}

	/** @brief �񓯊�����task�s���j������B
	    @param[in] io_tasks   queue�̐擪�ʒu�B
	    @param[in] i_capacity queue�����񓯊�����task�̐��B
	 */
	template< typename t_value_type >
	static void destroy_tasks(
		t_value_type* const io_tasks,
		std::size_t const   i_capacity)
	{
		PSYQ_ASSERT(NULL != io_tasks || i_capacity <= 0);
		for (std::size_t i = 0; i < i_capacity; ++i)
		{
			io_tasks[i].~t_value_type();
		}
		(t_arena::free)(io_tasks, sizeof(t_value_type) * i_capacity);
	}

//.............................................................................
private:
	boost::thread                 thread_;
	boost::condition              condition_;
	boost::mutex                  mutex_;
	char const*                   arena_name_;
	typename this_type::task_ptr* reserve_tasks_;    ///< �\��queue�̐擪�ʒu�B
	std::size_t                   reserve_capacity_; ///< �\��queue�̍ő�e�ʁB
	std::size_t                   running_size_;     ///< ���squeue�̗v�f���B
	bool                          stop_;
};

#endif // !PSYQ_ASYNC_QUEUE_HPP_
