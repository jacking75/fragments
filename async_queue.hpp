#ifndef PSYQ_ASYNC_QUEUE_HPP_
#define PSYQ_ASYNC_QUEUE_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
//#include <psyq/async_task.hpp>
//#include <psyq/memory/arena.hpp>
//#include <psyq/memory/dynamic_storage.hpp>

namespace psyq
{
	class async_queue;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�����task-queue�B
 */
class psyq::async_queue:
	private boost::noncopyable
{
	typedef psyq::async_queue this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~async_queue()
	{
		this->stop(true);
		this_type::abort_tasks(this->reserve_storage_);
	}

	explicit async_queue():
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
		return this->reserve_storage_.get_size() <= 0
			&& this->running_size_ <= 0;
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�����task��o�^�B
	    @param[in] i_task �o�^����񓯊�����task���w��smart-pointer�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_arena >
	std::size_t add(
		psyq::async_task::shared_ptr const& i_task,
		char const* const                   i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		return this->template add< t_arena >(&i_task, &i_task + 1, i_name);
	}

	/** @brief container�����񓯊�����task���܂Ƃ߂ēo�^�B
	    @tparam t_iterator shared_ptr�^��container��iterator�^�B
	    @param[in] i_begin container�̐擪�ʒu�B
	    @param[in] i_end   container�̖����ʒu�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_arena, typename t_iterator >
	std::size_t add(
		t_iterator const  i_begin,
		t_iterator const  i_end,
		char const* const i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		// �\��queue�����o���B
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		psyq::dynamic_storage a_storage;
		a_storage.swap(this->reserve_storage_);
		std::size_t const a_last_size(
			0 < a_storage.get_size()?
				a_storage.get_size() / sizeof(this_type::task_ptr):
				this->running_size_);

		// �V����queue���\�z�B
		std::size_t const a_new_capacity(
			a_last_size + std::distance(i_begin, i_end));
		this_type::task_ptr* const a_new_tasks(
			this_type::resize_tasks< t_arena >(
				a_storage, a_last_size, a_new_capacity, i_name));

		// container�����񓯊�����task���A�V����queue��copy�B
		std::size_t a_count(0);
		t_iterator a_iterator(i_begin);
		for (std::size_t i = a_last_size; i < a_new_capacity; ++i, ++a_iterator)
		{
			psyq::async_task* const a_task(a_iterator->get());
			if (NULL != a_task
				&& a_task->set_locked_state(psyq::async_task::state_BUSY))
			{
				// busy��Ԃł͂Ȃ��񓯊�����task�������o�^�ł���B
				new(&a_new_tasks[i]) this_type::task_ptr(*a_iterator);
				++a_count;
			}
			else
			{
				new(&a_new_tasks[i]) this_type::task_ptr();
			}
		}

		// �V����queue��\��queue�ɍ����ւ��Ă���ʒm�B
		this->reserve_storage_.swap(a_storage);
		this->condition_.notify_all();
		return a_count;
	}

	/** @brief queue�̑傫����K�v�Œ���ɂ���B
	 */
	template< typename t_arena >
	void shrink(char const* const i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		this->template add< t_arena >(
			static_cast< psyq::async_task::shared_ptr const* >(NULL),
			static_cast< psyq::async_task::shared_ptr const* >(NULL),
			i_name);
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
		psyq::dynamic_storage a_storage;
		std::size_t a_size(0);
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		while (!this->stop_)
		{
			// ���squeue���X�V�B
			if (0 < this->reserve_storage_.get_size())
			{
				// �\��queue���A�V�������squeue�Ƃ��Ď��o���B
				psyq::dynamic_storage a_last_storage;
				a_last_storage.swap(a_storage);
				this->reserve_storage_.swap(a_storage);
				std::size_t const a_last_size(a_size);
				a_size = a_storage.get_size() / sizeof(this_type::task_ptr);
				PSYQ_ASSERT(this->running_size_ <= a_size);
				PSYQ_ASSERT(a_last_size <= a_size);
				this->running_size_
					= a_last_size + a_size - this->running_size_;
				a_lock.unlock();

				// ���̎��squeue������task���A�V�������squeue�Ɉړ��B
				this_type::task_ptr* const a_new_tasks(
					static_cast< this_type::task_ptr* >(
						a_storage.get_address()));
				this_type::task_ptr* const a_last_tasks(
					static_cast< this_type::task_ptr* >(
						a_last_storage.get_address()));
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_new_tasks[i].expired());
					a_new_tasks[i].swap(a_last_tasks[i]);
				}

				// ���̎��squeue��j���B
				this_type::destroy_tasks(a_last_storage);
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
			a_size = this_type::run_tasks(
				static_cast< this_type::task_ptr* >(a_storage.get_address()),
				a_size);
			if (a_size <= 0)
			{
				// ���squeue����ɂȂ����̂Ŕj���B
				this_type::destroy_tasks(a_storage);
			}
			a_lock.lock();
		}
		this_type::abort_tasks(a_storage);
	}

	//-------------------------------------------------------------------------
	/** @brief queue�����񓯊�����task�����s����B
	    @param[in] io_tasks queue�̐擪�ʒu�B
	    @param[in] i_size   queue�����񓯊�����task�̐��B
	    @return queue�����񓯊�����task�̐��B
	 */
	static std::size_t run_tasks(
		this_type::task_ptr* const io_tasks,
		std::size_t const          i_size)
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
	    @param[in,out] io_storage queue�Ƃ��Ďg���Ă���̈�B
	    @param[in] i_last_size    ����queue�̗v�f���B
	    @param[in] i_new_capacity �V����queue�̍ő�v�f���B
		@return �V����queue�̐擪�ʒu�B
	 */
	template< typename t_arena >
	static this_type::task_ptr* resize_tasks(
		psyq::dynamic_storage& io_storage,
		std::size_t const      i_last_size,
		std::size_t const      i_new_capacity,
		char const* const      i_memory_name)
	{
		// �V����storage���m�ہB
		psyq::dynamic_storage a_storage(
			boost::type< t_arena >(),
			i_new_capacity * sizeof(this_type::task_ptr),
			boost::alignment_of< this_type::task_ptr >::value,
			0,
			i_memory_name);
		this_type::task_ptr* const a_new_tasks(
			static_cast< this_type::task_ptr* >(a_storage.get_address()));
		if (NULL != a_new_tasks)
		{
			PSYQ_ASSERT(i_last_size <= i_new_capacity);

			// ����queue�����񓯊�����task���A�V����queue�Ɉړ��B
			this_type::task_ptr* const a_last_tasks(
				static_cast< this_type::task_ptr* >(io_storage.get_address()));
			for (std::size_t i = 0; i < i_last_size; ++i)
			{
				new(&a_new_tasks[i]) typename this_type::task_ptr();
				if (NULL != a_last_tasks)
				{
					a_new_tasks[i].swap(a_last_tasks[i]);
				}
			}
		}
		else
		{
			PSYQ_ASSERT(i_new_capacity <= 0);
		}
		io_storage.swap(a_storage);
		this_type::destroy_tasks(a_storage);
		return a_new_tasks;
	}

	/** @brief �񓯊�����task�s���r���I������B
	    @param[in,out] io_storage queue�Ƃ��Ďg���Ă���̈�B
	 */
	static void abort_tasks(psyq::dynamic_storage& io_storage)
	{
		this_type::task_ptr* const a_tasks(
			static_cast< this_type::task_ptr* >(io_storage.get_address()));
		std::size_t const a_capacity(
			io_storage.get_size() / sizeof(this_type::task_ptr));
		for (std::size_t i = 0; i < a_capacity; ++i)
		{
			psyq::async_task::shared_ptr const a_holder(a_tasks[i].lock());
			psyq::async_task* const a_task(a_holder.get());
			if (NULL != a_task
				&& psyq::async_task::state_BUSY == a_task->get_state())
			{
				a_task->set_unlocked_state(
					psyq::async_task::state_ABORTED);
			}
		}
		this_type::destroy_tasks(io_storage);
	}

	/** @brief �񓯊�����task�s���j������B
	    @param[in,out] io_storage queue�Ƃ��Ďg���Ă���̈�B
	 */
	static void destroy_tasks(psyq::dynamic_storage& io_storage)
	{
		PSYQ_ASSERT(
			NULL != io_storage.get_address() || io_storage.get_size() <= 0);
		this_type::task_ptr* const a_tasks(
			static_cast< this_type::task_ptr* >(io_storage.get_address()));
		std::size_t const a_capacity(
			io_storage.get_size() / sizeof(this_type::task_ptr));
		for (std::size_t i = 0; i < a_capacity; ++i)
		{
			a_tasks[i].~task_ptr();
		}
		psyq::dynamic_storage().swap(io_storage);
	}

//.............................................................................
private:
	boost::thread         thread_;
	boost::condition      condition_;
	boost::mutex          mutex_;
	psyq::dynamic_storage reserve_storage_;
	std::size_t           running_size_; ///< ���squeue�̗v�f���B
	bool                  stop_;
};

#endif // !PSYQ_ASYNC_QUEUE_HPP_
