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
		this->reserve_queue_.abort();
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
		return this->reserve_queue_.is_empty() && this->running_size_ <= 0;
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�����task��o�^�B
	    @param[in] i_task �o�^����񓯊�����task���w��smart-pointer�B
	    @param[in] i_name �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
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
	    @param[in] i_name  �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_arena, typename t_iterator >
	std::size_t add(
		t_iterator const  i_begin,
		t_iterator const  i_end,
		char const* const i_name = PSYQ_ARENA_NAME_DEFAULT)
	{
		// �\��queue�����o���B
		this_type::task_queue a_queue;
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		this->reserve_queue_.swap(a_queue);
		std::size_t const a_last_size(
			a_queue.is_empty()? this->running_size_: a_queue.get_size());

		// �V����queue���\�z�B
		std::size_t const a_new_capacity(
			a_last_size + std::distance(i_begin, i_end));
		this_type::task_ptr* const a_new_tasks(
			a_queue.template resize< t_arena >(
				a_last_size, a_new_capacity, i_name));

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
		this->reserve_queue_.swap(a_queue);
		this->condition_.notify_all();
		return a_count;
	}

	/** @brief queue�̑傫����K�v�Œ���ɂ���B
	    @param[in] i_name �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
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
	template< typename t_value >
	class container:
		private psyq::dynamic_storage
	{
		typedef container this_type;
		typedef psyq::dynamic_storage super_type;

	public:
		~container()
		{
			t_value* const a_tasks(this->get_address());
			std::size_t const a_size(this->get_size());
			for (std::size_t i = 0; i < a_size; ++i)
			{
				a_tasks[i].~t_value();
			}
		}

		container():
		super_type()
		{
			// pass
		}

		void swap(this_type& io_target)
		{
			this->super_type::swap(io_target);
		}

		t_value* get_address()
		{
			return static_cast< t_value* >(this->super_type::get_address());
		}

		std::size_t get_size() const
		{
			return this->super_type::get_size() / sizeof(t_value);
		}

		bool is_empty() const
		{
			return this->super_type::get_size() <= 0;
		}

		/** @brief container�̑傫����ύX����B
		    @param[in] i_last_size    ����container�̗v�f���B
		    @param[in] i_new_capacity �V����container�̍ő�v�f���B
			@param[in] i_memory_name  �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
		    @return �V����container�̐擪�ʒu�B
		 */
		template< typename t_arena >
		t_value* resize(
			std::size_t const i_last_size,
			std::size_t const i_new_capacity,
			char const* const i_memory_name)
		{
			PSYQ_ASSERT(i_last_size <= i_new_capacity);

			// �V����container���m�ہB
			this_type a_container(
				boost::type< t_arena >(),
				i_new_capacity * sizeof(t_value),
				boost::alignment_of< t_value >::value,
				0,
				i_memory_name);
			t_value* const a_new_tasks(a_container.get_address());
			if (NULL != a_new_tasks)
			{
				// ����container�����񓯊�����task���A�V����container�Ɉړ��B
				t_value* const a_last_tasks(this->get_address());
				for (std::size_t i = 0; i < i_last_size; ++i)
				{
					new(&a_new_tasks[i]) typename t_value();
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
			this->swap(a_container);
			return a_new_tasks;
		}

		/** @brief container�����񓯊�����task�����s����B
		    @param[in] i_size container�����񓯊�����task�̐��B
		    @return container�����񓯊�����task�̐��B
		 */
		std::size_t run(std::size_t const i_size)
		{
			t_value* const a_tasks(this->get_address());
			std::size_t a_size(0);
			for (std::size_t i = 0; i < i_size; ++i)
			{
				psyq::async_task::shared_ptr const a_holder(a_tasks[i].lock());
				psyq::async_task* const a_task(a_holder.get());
				if (NULL != a_task
					&& psyq::async_task::state_BUSY == a_task->get_state())
				{
					boost::int32_t const a_state(a_task->run());
					if (psyq::async_task::state_BUSY == a_state)
					{
						a_tasks[a_size].swap(a_tasks[i]);
						++a_size;
						continue;
					}
					a_task->set_unlocked_state(a_state);
				}
				a_tasks[i].reset();
			}
			return a_size;
		}

		/** @brief container�����񓯊�����task��r���I������B
		 */
		void abort()
		{
			t_value* const a_tasks(this->get_address());
			std::size_t const a_size(this->get_size());
			for (std::size_t i = 0; i < a_size; ++i)
			{
				psyq::async_task::shared_ptr const a_holder(a_tasks[i].lock());
				psyq::async_task* const a_task(a_holder.get());
				if (NULL != a_task
					&& psyq::async_task::state_BUSY == a_task->get_state())
				{
					a_task->set_unlocked_state(psyq::async_task::state_ABORTED);
				}
			}
		}

	private:
		template< typename t_arena >
		container(
			boost::type< t_arena > const& i_type,
			std::size_t const             i_size,
			std::size_t const             i_alignment,
			std::size_t const             i_offset,
			char const* const             i_name):
		super_type(i_type, i_size, i_alignment, i_offset, i_name)
		{
			// pass
		}
	};
	typedef this_type::container< this_type::task_ptr > task_queue;

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
		this_type::task_queue a_queue;
		std::size_t a_size(0);
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		while (!this->stop_)
		{
			// ���squeue���X�V�B
			if (!this->reserve_queue_.is_empty())
			{
				// �\��queue���A�V�������squeue�Ƃ��Ď��o���B
				this_type::task_queue a_last_queue;
				a_last_queue.swap(a_queue);
				this->reserve_queue_.swap(a_queue);
				std::size_t const a_last_size(a_size);
				a_size = a_queue.get_size();
				PSYQ_ASSERT(this->running_size_ <= a_size);
				PSYQ_ASSERT(a_last_size <= a_size);
				this->running_size_
					= a_last_size + a_size - this->running_size_;
				a_lock.unlock();

				// ���̎��squeue������task���A�V�������squeue�Ɉړ��B
				this_type::task_ptr* const a_new_tasks(
					static_cast< this_type::task_ptr* >(
						a_queue.get_address()));
				this_type::task_ptr* const a_last_tasks(
					static_cast< this_type::task_ptr* >(
						a_last_queue.get_address()));
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_new_tasks[i].expired());
					a_new_tasks[i].swap(a_last_tasks[i]);
				}
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
			a_size = a_queue.run(a_size);
			if (a_size <= 0)
			{
				// ���squeue����ɂȂ����̂Ŕj���B
				this_type::task_queue().swap(a_queue);
			}
			a_lock.lock();
		}
		a_queue.abort();
	}

//.............................................................................
private:
	boost::thread         thread_;
	boost::condition      condition_;
	boost::mutex          mutex_;
	this_type::task_queue reserve_queue_; ///< �\��task-queue�B
	std::size_t           running_size_;  ///< ���stask-queue�̗v�f���B
	bool                  stop_;
};

#endif // !PSYQ_ASYNC_QUEUE_HPP_
