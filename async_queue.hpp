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

	std::size_t get_size() const
	{
		return this->running_size_;
	}

	std::size_t get_capacity() const
	{
		boost::lock_guard< boost::mutex > const a_lock(
			const_cast< boost::mutex& >(this->mutex_));
		return (std::max)(
			this->reserve_queue_.get_size(), this->running_size_);
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�����task��o�^�B
	    @param[in] i_allocator memory�m�ۂɎg�������q�B
	    @param[in] i_task      �o�^����񓯊�����task���w��smart-pointer�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_allocator >
	std::size_t add(
		t_allocator const&                  i_allocator,
		psyq::async_task::shared_ptr const& i_task)
	{
		return this->add< typename t_allocator::arena >(
			i_task, i_allocator.get_name());
	}

	/** @brief �񓯊�����task��o�^�B
	    @tparam t_arena   memory�m�ۂɎg��memory-arena�̌^�B
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
		@param[in] i_allocator memory�m�ۂɎg�������q�B
	    @param[in] i_begin     container�̐擪�ʒu�B
	    @param[in] i_end       container�̖����ʒu�B
	    @return �o�^�����񓯊�����task�̐��B
	 */
	template< typename t_allocator, typename t_iterator >
	std::size_t add(
		t_allocator const& i_allocator,
		t_iterator const   i_begin,
		t_iterator const   i_end)
	{
		return this->add< typename t_allocator::arena >(
			i_begin, i_end, i_allocator.get_name());
	}

	/** @brief container�����񓯊�����task���܂Ƃ߂ēo�^�B
	    @tparam t_arena    memory�m�ۂɎg��memory-arena�̌^�B
	    @tparam t_iterator shared_ptr�^container��iterator�^�B
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
		this_type::task_ptr* a_new_task(
			a_last_size + a_queue.resize< t_arena >(
				a_last_size,
				a_last_size + std::distance(i_begin, i_end),
				i_name));
		std::size_t a_count(0);
		for (t_iterator i = i_begin; i_end != i; ++i, ++a_new_task)
		{
			// container�����񓯊�����task�̂����A
			// busy��Ԃł͂Ȃ��񓯊�����task������V����queue�ɓo�^����B
			psyq::async_task* const a_task(i->get());
			if (NULL != a_task
				&& a_task->set_locked_state(psyq::async_task::state_BUSY))
			{
				new(a_new_task) this_type::task_ptr(*i);
				++a_count;
			}
			else
			{
				new(a_new_task) this_type::task_ptr();
			}
		}

		// �V����queue��\��queue�ɍ����ւ��Ă���ʒm�B
		this->reserve_queue_.swap(a_queue);
		this->condition_.notify_all();
		return a_count;
	}

	//-------------------------------------------------------------------------
	/** @brief queue�̑傫����K�v�Œ���ɂ���B
	    @param[in] i_allocator memory�m�ۂɎg�������q�B
	 */
	template< typename t_allocator >
	void shrink(t_allocator const& i_allocator)
	{
		this->shrink< typename t_allocator::arena >(i_allocator.get_name());
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
	class array:
		private psyq::dynamic_storage
	{
		typedef array this_type;
		typedef psyq::dynamic_storage super_type;

	public:
		~array()
		{
			t_value* const a_tasks(this->get_address());
			std::size_t const a_size(this->get_size());
			for (std::size_t i = 0; i < a_size; ++i)
			{
				a_tasks[i].~t_value();
			}
		}

		array():
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

		/** @brief array�̑傫����ύX����B
		    @param[in] i_last_size   ����array�̗v�f���B
		    @param[in] i_new_size    �V����array�̗v�f���B
			@param[in] i_memory_name �m�ۂ���memory�̎��ʖ��Bdebug�ł̂ݎg���B
		    @return �V����array�̐擪�ʒu�B
		 */
		template< typename t_arena >
		t_value* resize(
			std::size_t const i_last_size,
			std::size_t const i_new_size,
			char const* const i_memory_name)
		{
			PSYQ_ASSERT(i_last_size <= i_new_size);

			// �V����array���m�ہB
			this_type a_array(
				boost::type< t_arena >(),
				i_new_size,
				boost::alignment_of< t_value >::value,
				0,
				i_memory_name);
			t_value* const a_new_tasks(a_array.get_address());
			if (NULL != a_new_tasks)
			{
				// ����array�����񓯊�����task���A�V����array�Ɉړ��B
				t_value* const a_last_tasks(this->get_address());
				for (std::size_t i = 0; i < i_last_size; ++i)
				{
					new(&a_new_tasks[i]) t_value();
					if (NULL != a_last_tasks)
					{
						a_new_tasks[i].swap(a_last_tasks[i]);
					}
				}
			}
			else
			{
				PSYQ_ASSERT(i_new_size <= 0);
			}
			this->swap(a_array);
			return a_new_tasks;
		}

		/** @brief array�����񓯊�����task�����s����B
		    @param[in] i_size array�����񓯊�����task�̐��B
		    @return array�����񓯊�����task�̐��B
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

		/** @brief array�����񓯊�����task��r���I������B
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
		array(
			boost::type< t_arena > const& i_type,
			std::size_t const             i_size,
			std::size_t const             i_alignment,
			std::size_t const             i_offset,
			char const* const             i_name):
		super_type(
			i_type, i_size * sizeof(t_value), i_alignment, i_offset, i_name)
		{
			// pass
		}
	};
	typedef this_type::array< this_type::task_ptr > task_queue;

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
				a_queue.swap(this->reserve_queue_);
				std::size_t const a_last_size(a_size);
				a_size = a_queue.get_size();
				PSYQ_ASSERT(this->running_size_ <= a_size);
				PSYQ_ASSERT(a_last_size <= a_size);
				this->running_size_	=
					a_last_size + a_size - this->running_size_;
				a_lock.unlock();

				// ���̎��squeue������task���A�V�������squeue�Ɉړ��B
				this_type::task_ptr* const a_last_tasks(
					static_cast< this_type::task_ptr* >(
						a_last_queue.get_address()));
				this_type::task_ptr* const a_tasks(
					static_cast< this_type::task_ptr* >(
						a_queue.get_address()));
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_tasks[i].expired());
					a_tasks[i].swap(a_last_tasks[i]);
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
