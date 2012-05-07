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
/** @brief 非同期処理task-queue。
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

	/** @param[in] i_memory_name debugで使うためのmemory識別名。
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
	/** @brief 非同期処理taskを登録。
	    @param[in] i_task 登録する非同期処理taskを指すsmart-pointer。
	    @return 登録した非同期処理taskの数。
	 */
	std::size_t add(psyq::async_task::shared_ptr const& i_task)
	{
		return this->add(&i_task, &i_task + 1);
	}

	/** @brief containerが持つ非同期処理taskをまとめて登録。
	    @tparam t_iterator shared_ptr型のcontainerのiterator型。
	    @param[in] i_begin containerの先頭位置。
	    @param[in] i_end   containerの末尾位置。
	    @return 登録した非同期処理taskの数。
	 */
	template< typename t_iterator >
	std::size_t add(t_iterator const i_begin, t_iterator const i_end)
	{
		// 予約queueを取り出す。
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		typename this_type::task_ptr* const a_last_tasks(this->reserve_tasks_);
		std::size_t const a_last_capacity(this->reserve_capacity_);
		std::size_t const a_last_size(
			0 < a_last_capacity? a_last_capacity: this->running_size_);

		// 新しいqueueを構築。
		std::size_t const a_capacity(
			a_last_size + std::distance(i_begin, i_end));
		typename this_type::task_ptr* const a_tasks(
			this_type::resize_tasks(
				a_capacity,
				a_last_tasks,
				a_last_size,
				a_last_capacity,
				this->arena_name_));

		// containerが持つ非同期処理taskを、新しいqueueにcopy。
		std::size_t a_count(0);
		t_iterator a_iterator(i_begin);
		for (std::size_t i = a_last_size; i < a_capacity; ++i, ++a_iterator)
		{
			psyq::async_task* const a_task(a_iterator->get());
			if (NULL != a_task
				&& a_task->set_locked_state(psyq::async_task::state_BUSY))
			{
				// busy状態ではない非同期処理taskだけが登録できる。
				new(&a_tasks[i]) typename this_type::task_ptr(*a_iterator);
				++a_count;
			}
			else
			{
				new(&a_tasks[i]) typename this_type::task_ptr();
			}
		}

		// 新しいqueueを予約queueに差し替えてから通知。
		this->reserve_tasks_ = a_tasks;
		this->reserve_capacity_ = a_capacity;
		this->condition_.notify_all();
		return a_count;
	}

	/** @brief queueの大きさを必要最低限にする。
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
		// 終了flagを有効化。
		this->stop_ = true;
		if (i_sync)
		{
			// threadの終了まで待つ。
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
			// 実行queueを更新。
			if (0 < this->reserve_capacity_)
			{
				// 予約queueを、新しい実行queueとして取り出す。
				typename this_type::task_ptr* const a_last_tasks(a_tasks);
				std::size_t const a_last_capacity(a_capacity);
				a_tasks = this->reserve_tasks_;
				a_capacity = this->reserve_capacity_;
				this->running_size_ = this->reserve_capacity_;
				this->reserve_tasks_ = NULL;
				this->reserve_capacity_ = 0;
				a_lock.unlock();

				// 元の実行queueが持つtaskを、新しい実行queueに移動。
				std::size_t const a_last_size(a_size);
				PSYQ_ASSERT(a_last_size <= a_capacity);
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_tasks[i].expired());
					a_tasks[i].swap(a_last_tasks[i]);
				}
				a_size = a_capacity;

				// 元の実行queueを破棄。
				this_type::destroy_tasks(a_last_tasks, a_last_capacity);
			}
			else if (0 < a_size)
			{
				this->running_size_ = a_size;
				a_lock.unlock();
			}
			else
			{
				// 予約queueと実行queueが空になったので待機。
				this->running_size_ = 0;
				this->condition_.wait(a_lock);
				continue;
			}

			// 実行queueのtaskを呼び出す。
			a_size = this_type::run_tasks(a_tasks, a_size);
			if (a_size <= 0)
			{
				// 実行queueが空になったので破棄。
				this_type::destroy_tasks(a_tasks, a_capacity);
				a_tasks = NULL;
				a_capacity = 0;
			}
			a_lock.lock();
		}
		this_type::abort_tasks(a_tasks, a_capacity);
	}

	//-------------------------------------------------------------------------
	/** @brief queueが持つ非同期処理taskを実行する。
	    @param[in] io_tasks queueの先頭位置。
	    @param[in] i_size   queueが持つ非同期処理taskの数。
	    @return queueが持つ非同期処理taskの数。
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

	/** @brief queueの大きさを変更する。
	    @param[in] i_capacity        新しいqueueの最大要素数。
	    @param[in,out] io_last_tasks 元のqueueの先頭位置。
	    @param[in] i_last_size       元のqueueの要素数。
	    @param[in] i_last_capacity   元のqueueの最大要素数。
		@return 新しいqueueの先頭位置。
	 */
	static typename this_type::task_ptr* resize_tasks(
		std::size_t const                   i_capacity,
		typename this_type::task_ptr* const io_last_tasks,
		std::size_t const                   i_last_size,
		std::size_t const                   i_last_capacity,
		char const* const                   i_memory_name)
	{
		// 新しいqueueを確保。
		typename this_type::task_ptr* const a_tasks(
			static_cast< typename this_type::task_ptr* >(
				(t_arena::malloc)(
					i_capacity * sizeof(typename this_type::task_ptr),
					boost::alignment_of< typename this_type::task_ptr >::value,
					0,
					i_memory_name)));
		if (NULL != a_tasks)
		{
			// 元のqueueが持つ非同期処理taskを、新しいqueueに移動。
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

		// 元のqueueを破棄。
		this_type::destroy_tasks(io_last_tasks, i_last_capacity);
		return a_tasks;
	}

	/** @brief 非同期処理task行列を途中終了する。
	    @param[in] io_tasks   queueの先頭位置。
	    @param[in] i_capacity queueが持つ非同期処理taskの数。
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

	/** @brief 非同期処理task行列を破棄する。
	    @param[in] io_tasks   queueの先頭位置。
	    @param[in] i_capacity queueが持つ非同期処理taskの数。
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
	typename this_type::task_ptr* reserve_tasks_;    ///< 予約queueの先頭位置。
	std::size_t                   reserve_capacity_; ///< 予約queueの最大容量。
	std::size_t                   running_size_;     ///< 実行queueの要素数。
	bool                          stop_;
};

#endif // !PSYQ_ASYNC_QUEUE_HPP_
