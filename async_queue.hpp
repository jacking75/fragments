#ifndef PSYQ_ASYNC_QUEUE_HPP_
#define PSYQ_ASYNC_QUEUE_HPP_

#include <boost/bind/bind.hpp>
//#include <psyq/async_task.hpp>
//#include <psyq/memory/dynamic_storage.hpp>

/// @cond
namespace psyq
{
	template< typename, typename, typename, typename > class async_queue;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �Ǝ���thread������s����񓯊�task��ێ�����queue�B

    insert() �̈����ɔ񓯊�task�̕ێ��q��n���āA
    �\��task-container�ɔ񓯊�task��}������B

    flush() �Ŏ��stask-container�̍X�V���v�������ƁA
    �\��task-container�ɂ���񓯊�task���A���stask-container�Ɉړ�����B

    constructor�������� start() �ɂ���ċN�������Ǝ���thread���� main_loop()
    ���Ăяo����A���stask-container�ɂ���񓯊�task�����s����loop���N������B
    �񓯊�task�̎��s�́A async_task::run() ���Ăяo�����Ƃōs����B
    - async_task::run() �̖߂�l�� async_task::state_BUSY �ȊO�������ꍇ�A
      �񓯊�task�͏I�����A���stask-container�����菜�����B
    - async_task::run() �̖߂�l�� async_task::state_BUSY �������ꍇ�A
      �񓯊�task�͌p�����A ����loop�� async_task::run() ���ĂьĂяo���B

    @tparam t_container @copydoc async_queue::container
    @tparam t_mutex     @copydoc async_queue::mutex
    @tparam t_condition @copydoc async_queue::condition
    @tparam t_thread    @copydoc async_queue::thread
    @sa async_task
 */
template<
	typename t_container = std::vector<
		psyq::async_task::shared_ptr,
		psyq::allocator<
			psyq::async_task::shared_ptr,
			boost::alignment_of< psyq::async_task::shared_ptr >::value,
			0,
			PSYQ_ARENA_DEFAULT > >,
	typename t_mutex = PSYQ_MUTEX_DEFAULT,
	typename t_condition = PSYQ_CONDITION_DEFAULT,
	typename t_thread = PSYQ_THREAD_DEFAULT >
class psyq::async_queue:
	private boost::noncopyable
{
	/// ����object�̌^�B
	public: typedef async_queue< t_container, t_mutex, t_condition, t_thread >
		this_type;

	//-------------------------------------------------------------------------
	/// �񓯊�task-container�̌^�B std::vector �݊��B
	public: typedef t_container container;

	/// lock�Ɏg��mutex�̌^�B
	public: typedef t_mutex mutex;

	/// thread�̓����Ɏg�������ϐ��̌^�B
	public: typedef t_condition condition;

	/// �񓯊�task�����s����thread-handle�̌^�B
	public: typedef t_thread thread;

	//-------------------------------------------------------------------------
	/// @brief �񓯊�task���sthread���N������B
	public: async_queue()
	{
		this->~this_type();
		new(this) this_type(
			true, typename t_container::allocator_type());
	}

	/** @param[in] i_allocator memory�����q�̏����l�B
	    @param[in] i_start     �񓯊�task���sthread���J�n���邩�B
	 */
	private: template< typename t_allocator >
	async_queue(
		bool const         i_start,
		t_allocator const& i_allocator):
	reserve_tasks_(i_allocator),
	running_size_(0),
	stop_request_(false),
	flush_request_(false)
	{
		if (i_start)
		{
			this->start_loop();
		}
	}

	public: ~async_queue()
	{
		if (this->is_running())
		{
			this->stop(true);
		}
		else
		{
			this->clear_reserve_tasks();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief ���s���Ă���񓯊�task�̐����擾�B
	 */
	std::size_t get_running_size() const
	{
		return this->running_size_;
	}

	/** @brief �ێ����Ă���memory�����q���擾�B
	 */
	typename t_container::allocator_type get_allocator() const
	{
		return this->reserve_tasks_.get_allocator();
	}

	/** @brief �񓯊�task���sthread���N�����Ă��邩����B
	 */
	public: bool is_running() const
	{
		return this->thread_.joinable();
	}

	/** @brief �񓯊�task���sthread���N���B
	    @retval !=false �N���ɐ����B
	    @retval ==false ���łɋN�����Ă����B
	 */
	private: bool start()
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		if (!this->is_running())
		{
			this->stop_request_ = false;
			this->start_loop();
			return true;
		}
		return false;
	}

	/** @brief �񓯊�task���sthread���~�B
	    @param[in] i_block thread����~����܂�block���邩�B
	 */
	private: void stop(bool const i_block)
	{
		// �I���v�����o���B
		this->stop_request_ = true;
		if (i_block)
		{
			// thread���I������܂őҋ@�B
			{
				PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
				this->condition_.notify_all();
			}
			this->thread_.join();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�task��\��task-container�ɑ}���B
	    @param[in] i_task  �}������񓯊�task�B
	    @param[in] i_flush ���stask-container�̍X�V�����邩�B
	    @return �}�������񓯊�task�̐��B
	 */
	public: std::size_t insert(
		typename t_container::value_type const& i_task,
		bool const                              i_flush = true)
	{
		return this->insert(&i_task, &i_task + 1, i_flush);
	}

	/** @brief �񓯊�task-contaner��\��task-cotainer�ɑ}���B
	    @param[in] i_tasks �}������񓯊�task-container�B
	    @param[in] i_flush ���stask-container�̍X�V�����邩�B
	    @return �}�������񓯊�task�̐��B
	 */
	public: std::size_t insert(
		typename t_container const& i_tasks,
		bool const                  i_flush = true)
	{
		return this->insert(i_tasks.begin(), i_tasks.end(), i_flush);
	}

	/** @brief �񓯊�task-contaner��\��task-cotainer�ɑ}���B
	    @param[in] i_begin �}������񓯊�task-container�̐擪�ʒu�B
	    @param[in] i_end   �}������񓯊�task-container�̖����ʒu�B
	    @param[in] i_flush ���stask-container���X�V���邩�B
	    @return �}�������񓯊�task�̐��B
	 */
	public: template< typename t_iterator >
	std::size_t insert(
		t_iterator const& i_begin,
		t_iterator const& i_end,
		bool const        i_flush = true)
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);

		// �}������container�����񓯊�task�̂����A
		// busy��Ԃł͂Ȃ��񓯊�task�����A�\��task-container�ɑ}������B
		std::size_t const a_last_size(this->reserve_tasks_.size());
		this->reserve_tasks_.reserve(
			this->running_size_ + a_last_size + std::distance(i_begin, i_end));
		for (t_iterator i = i_begin; i_end != i; ++i)
		{
			this->reserve_tasks_.push_back(*i);
			psyq::async_task* const a_task(this->reserve_tasks_.back().get());
			if (NULL == a_task ||
				!a_task->set_state(psyq::async_task::state_BUSY))
			{
				this->reserve_tasks_.pop_back();
			}
		}

		// ���stask-container�̍X�V��v���B
		if (i_flush)
		{
			this->flush_request_ = true;
			this->condition_.notify_all();
		}

		return this->reserve_tasks_.size() - a_last_size;
	}

	/** @brief �\�񂳂ꂽ�񓯊�task�����s�J�n�B
	 */
	public: void flush()
	{
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);

		// ������memory�m�ۂ��Ă����A�񓯊�task���sthread�ł�
		// memory�m�ۂ��Ȃ��悤�ɂ���B
		if (this->reserve_tasks_.empty())
		{
			this->reserve_tasks_.reserve(this->running_size_);
		}

		// ���stask-container�̍X�V��v���B
		this->flush_request_ = true;
		this->condition_.notify_all();
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�task���sthread���N���B
	 */
	private: void start_loop()
	{
#ifdef PSYQ_USE_CLX
		this->thread_.start(boost::bind(&this_type::main_loop, this));
#else
		t_thread a_thread(boost::bind(&this_type::main_loop, this));
		this->thread_.swap(a_thread);
#endif // PSYQ_USE_CLX
	}

	/** @brief �񓯊�task���sthread��main-loop�B
	 */
	private: void main_loop()
	{
		// �I���v��������܂�loop�B
		t_container a_running_tasks; // ���stask-container�B
		while (!this->stop_request_)
		{
			if (this->update_running_tasks(a_running_tasks))
			{
				// �񓯊�task�����s�B
				typename t_container::iterator const a_end(
					a_running_tasks.end());
				a_running_tasks.erase(
					this_type::run_tasks(a_running_tasks.begin(), a_end),
					a_end);
				if (a_running_tasks.empty())
				{
					// ���scontainer����ɂȂ����̂Ŕj���B
					t_container().swap(a_running_tasks);
				}
			}
		}

		// ��n���B
		PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
		this_type::abort_tasks(
			a_running_tasks.begin(), a_running_tasks.end());
		this->clear_reserve_tasks();
		this->running_size_ = 0;
	}

	/** @brief ���stask-container���X�V�B
	    @param[in,out] io_running_tasks �X�V������stask-container�B
	    @retval !=false ���s�����o���B
	    @retval ==false ���s����O�ɂ�����x�X�V���K�v�B
	 */
	private: bool update_running_tasks(t_container& io_running_tasks)
	{
		std::size_t const a_running_size(io_running_tasks.size());
		if (this->flush_request_)
		{
			// �\��task-container��ҋ@task-container�Ɉړ��B
			t_container a_wait_tasks;
			{
				PSYQ_LOCK_GUARD< t_mutex > const a_lock(this->mutex_);
				a_wait_tasks.swap(this->reserve_tasks_);
				this->running_size_ = a_running_size + a_wait_tasks.size();
				this->flush_request_ = false;
			}

			// �ҋ@task-container������stask-container�Ɉړ��B
			// �Â����̂�����s�����悤�ɕ��т�����B
			io_running_tasks.swap(a_wait_tasks);
			this_type::move_tasks(io_running_tasks, a_wait_tasks);
		}
		else if (0 < a_running_size)
		{
			// ���stask-container�̑傫�����X�V�B
			/** @attention ���̉ӏ������Alock������
			    async_queue::running_size_ �����������Ă���B
			 */
			this->running_size_ = a_running_size;
		}
		else
		{
			// ���stask-container����ɂȂ����̂őҋ@�B
			PSYQ_UNIQUE_LOCK< t_mutex > a_lock(this->mutex_);
			this->running_size_ = 0;
			this->condition_.wait(a_lock);
			return false;
		}
		return true;
	}

	/** @brief �\��task-container����ɂ���B
	 */
	private: void clear_reserve_tasks()
	{
		this_type::abort_tasks(
			this->reserve_tasks_.begin(), this->reserve_tasks_.end());
		t_container().swap(this->reserve_tasks_);
	}

	/** @brief container���ړ����č����B

	    io_source �� io_target �̐擪�Ɉړ��}������B
	    @param[in,out] io_target �ړ����container�B
	    @param[in,out] io_source �ړ�����container�B
	 */
	private: static void move_tasks(
		t_container& io_target,
		t_container& io_source)
	{
		// memory�m�ۂ��s���Ȃ����Ƃ��m�F�B
		PSYQ_ASSERT(
			io_target.size() + io_source.size() <= io_target.capacity());

		// io_target �̐擪���󂯂�B
		io_target.insert(
			io_target.begin(),
			io_source.size(),
			typename t_container::value_type());

		// io_source �̗v�f�� io_target �̐擪�Ɉړ��B
		typename t_container::iterator a_target(io_target.begin());
		typename t_container::iterator a_source(io_source.begin());
		for (; io_source.end() != a_source; ++a_source, ++a_target)
		{
			a_target->swap(*a_source);
		}
	}

	/** @brief �񓯊�task�����s�B
	    @param[in] i_begin ���s����񓯊�task-container�̐擪�ʒu�B
	    @param[in] i_end   ���s����񓯊�task-container�̖����ʒu�B
	    @return ���stask-container�̐V���Ȗ����ʒu�B
	 */
	private: static typename t_container::iterator run_tasks(
		typename t_container::iterator const& i_begin,
		typename t_container::iterator const& i_end)
	{
		typename t_container::iterator a_end(i_begin);
		for(typename t_container::iterator i = i_begin; i_end != i; ++i)
		{
			psyq::async_task& a_task(**i);
			if (psyq::async_task::state_BUSY == a_task.get_state())
			{
				// task�����s���A���s��Ԃ��擾�B
				int const a_state(a_task.run());
				if (psyq::async_task::state_BUSY == a_state)
				{
					// task��busy��ԂȂ̂ŁA���s���p���B
					a_end->swap(*i);
					++a_end;
				}
				else
				{
					// task��busy��Ԃł͂Ȃ��Ȃ����̂ŁA���s���I���B
					a_task.state_ = a_state;
				}
			}
		}
		return a_end;
	}

	/** @brief �񓯊�task��r���I������B
	    @param[in] i_begin �r���I������񓯊�task-container�̐擪�ʒu�B
	    @param[in] i_end   �r���I������񓯊�task-container�̖����ʒu�B
	 */
	private: static void abort_tasks(
		typename t_container::iterator const& i_begin,
		typename t_container::iterator const& i_end)
	{
		for (typename t_container::iterator i = i_begin; i_end != i; ++i)
		{
			(**i).abort();
		}
	}

	//-------------------------------------------------------------------------
	/// �񓯊�task�����s����thread�B
	private: t_thread thread_;

	/// lock�Ɏg��mutex�B
	private: t_mutex mutex_;

	/// thread�̓����Ɏg�������ϐ��B
	private: t_condition condition_;

	/// �\��task-container�B
	private: t_container reserve_tasks_;

	/// ���s���Ă���task�̐��B
	private: std::size_t running_size_;

	/// ���s��~�v���B
	private: bool stop_request_;

	/// ���scontainer�̍X�V�v���B
	private: bool flush_request_;

};

#endif // !PSYQ_ASYNC_QUEUE_HPP_
