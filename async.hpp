#ifndef PSYQ_ASYNC_HPP_
#define PSYQ_ASYNC_HPP_

#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
//#include <psyq/memory/arena.hpp>

namespace psyq
{
	class async_client;
	class async_server;
	class async_functor;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::async_client:
	private boost::noncopyable
{
	typedef psyq::async_client this_type;

	friend class psyq::async_server;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > shared_ptr;
	typedef boost::weak_ptr< this_type > weak_ptr;

	enum state
	{
		state_BUSY,
		state_FINISHED,
		state_ABORTED,
		state_end
	};

	virtual ~async_client()
	{
		// pass
	}

	boost::int32_t get_state() const
	{
		return this->state_;
	}

//.............................................................................
protected:
	async_client():
	state_(this_type::state_FINISHED)
	{
		// pass
	}

	virtual boost::int32_t run() = 0;

//.............................................................................
private:
	bool set_state(boost::int32_t const i_state)
	{
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		if (this_type::state_BUSY != this->state_)
		{
			this->state_ = i_state;
			return true;
		}
		return false;
	}

	void set_state_unlocked(boost::int32_t const i_state)
	{
		this->state_ = i_state;
	}

//.............................................................................
private:
	boost::mutex   mutex_;
	boost::int32_t state_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::async_server:
	private boost::noncopyable
{
	typedef psyq::async_server this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~async_server()
	{
		this->stop(true);
		this_type::destroy_queue(
			this->queue_begin_, this->queue_capacity_, *this->arena_);
	}

	explicit async_server(psyq::arena::shared_ptr const& i_arena):
	arena_(i_arena),
	queue_begin_(NULL),
	queue_capacity_(0),
	queue_size_(0),
	stop_(false)
	{
		PSYQ_ASSERT(NULL != i_arena.get());
		this->start();
	}

	//-------------------------------------------------------------------------
	/** @brief �񓯊�����client��o�^�B
	    @param[in] i_client �o�^����񓯊�����client�B
	    @return �o�^�����񓯊�����client�̐��B
	 */
	std::size_t add(psyq::async_client::shared_ptr const& i_client)
	{
		return this->add(&i_client, &i_client + 1);
	}

	/** @brief container�����񓯊�����client���܂Ƃ߂ēo�^�B
	    @param[in] i_begin container�̐擪�ʒu�B
	    @param[in] i_end   container�̖����ʒu�B
	    @return �o�^�����񓯊�����client�̐��B
	 */
	template< typename t_iterator >
	std::size_t add(t_iterator const i_begin, t_iterator const i_end)
	{
		// ���݂�queue�����o���B
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		psyq::async_client::weak_ptr* const a_last_queue(this->queue_begin_);
		std::size_t const a_last_capacity(
			NULL != a_last_queue? this->queue_capacity_: this->queue_size_);
		this->queue_begin_ = NULL;
		this->queue_capacity_ = 0;
		a_lock.unlock();

		// �V����queue���m�ہB
		std::size_t const a_capacity(
			a_last_capacity + std::distance(i_begin, i_end));
		psyq::async_client::weak_ptr* const a_queue(
			static_cast< psyq::async_client::weak_ptr* >(
				this->arena_->allocate(
					a_capacity * sizeof(psyq::async_client::weak_ptr),
					boost::alignment_of< psyq::async_client::weak_ptr >::value,
					0)));

		// ���݂�queue���g���̈���������B
		for (std::size_t i = 0; i < a_last_capacity; ++i)
		{
			new(&a_queue[i]) psyq::async_client::weak_ptr();
		}
		if (NULL != a_last_queue)
		{
			// ���݂�queue��V����queue�Ɉړ����Ă���A���݂�queue��j���B
			for (std::size_t i = 0; i < a_last_capacity; ++i)
			{
				a_last_queue[i].swap(a_queue[i]);
			}
			this_type::destroy_queue(
				a_last_queue, a_last_capacity, *this->arena_);
		}

		// container��V����queue��copy�B
		std::size_t a_count(0);
		t_iterator a_iterator(i_begin);
		for (std::size_t i = a_last_capacity; i < a_capacity; ++i, ++a_iterator)
		{
			psyq::async_client::shared_ptr const& a_shared_ptr(*a_iterator);
			psyq::async_client* const a_client(a_shared_ptr.get());
			if (NULL != a_client
				&& a_client->set_state(psyq::async_client::state_BUSY))
			{
				// busy��Ԃł͂Ȃ��񓯊�����client�������o�^�ł���B
				new(&a_queue[i]) psyq::async_client::weak_ptr(a_shared_ptr);
				++a_count;
			}
			else
			{
				new(&a_queue[i]) psyq::async_client::weak_ptr();
			}
		}

		// �V����queue�ɍ����ւ��Ă���ʒm�B
		a_lock.lock();
		this->queue_begin_ = a_queue;
		this->queue_capacity_ = a_capacity;
		this->condition_.notify_all();
		return a_count;
	}

	//-------------------------------------------------------------------------
	psyq::arena::shared_ptr const& get_arena() const
	{
		return this->arena_;
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	void start()
	{
		if (!this->thread_.joinable())
		{
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
		psyq::async_client::weak_ptr* a_queue(NULL);
		std::size_t a_size(0);
		std::size_t a_capacity(0);
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		while (!this->stop_)
		{
			// queue�̑傫�����X�V�B
			this->queue_size_ = a_size;
			if (a_size <= 0)
			{
				// queue����ɂȂ�����Aqueue��j�����Ă���ҋ@�B
				this_type::destroy_queue(a_queue, a_capacity, *this->arena_);
				a_queue = NULL;
				a_capacity = 0;
				this->condition_.wait(a_lock);
			}

			// queue���X�V�B
			if (NULL != this->queue_begin_)
			{
				// ���݂�queue��V����queue�Ɉړ��B
				PSYQ_ASSERT(a_size <= this->queue_capacity_);
				for (std::size_t i = 0; i < a_size; ++i)
				{
					PSYQ_ASSERT(this->queue_begin_[i].expired());
					this->queue_begin_[i].swap(a_queue[i]);
				}
				this_type::destroy_queue(a_queue, a_capacity, *this->arena_);
				a_size = this->queue_capacity_;

				// �V����queue�����o���B
				a_queue = this->queue_begin_;
				a_capacity = this->queue_capacity_;
				this->queue_begin_ = NULL;
				this->queue_capacity_ = 0;
			}

			// mutex��unlock���Ă���Aqueue�����s�B
			a_lock.unlock();
			a_size = this_type::run_queue(a_queue, a_size);
			a_lock.lock();
		}
		this_type::destroy_queue(a_queue, a_capacity, *this->arena_);
	}

	//-------------------------------------------------------------------------
	/** @brief queue�����񓯊�����client�����s����B
	    @param[in] io_queue queue�̐擪�ʒu�B
	    @param[in] i_size   queue�����񓯊�����client�̐��B
	    @return queue�����񓯊�����client�̐��B
	 */
	static std::size_t run_queue(
		psyq::async_client::weak_ptr* const io_queue,
		std::size_t const                   i_size)
	{
		std::size_t a_size(0);
		for (std::size_t i = 0; i < i_size; ++i)
		{
			psyq::async_client::shared_ptr const a_shared_ptr(io_queue[i]);
			psyq::async_client* const a_client(a_shared_ptr.get());
			if (NULL != a_client
				&& psyq::async_client::state_BUSY == a_client->get_state())
			{
				boost::int32_t const a_state(a_client->run());
				if (psyq::async_client::state_BUSY == a_state)
				{
					io_queue[a_size].swap(io_queue[i]);
					++a_size;
					continue;
				}
				a_client->set_state_unlocked(a_state);
			}
			io_queue[i].reset();
		}
		return a_size;
	}

	/** @brief �񓯊�����client�s���j������B
	    @param[in] io_queue queue�̐擪�ʒu�B
	    @param[in] i_size   queue�����񓯊�����client�̐��B
	    @param[in] i_arena  �j���Ɏg��memory-arena�B
	 */
	static void destroy_queue(
		psyq::async_client::weak_ptr* const io_queue,
		std::size_t const                   i_size,
		psyq::arena&                        i_arena)
	{
		PSYQ_ASSERT(NULL != io_queue || i_size <= 0);
		for (std::size_t i = 0; i < i_size; ++i)
		{
			io_queue[i].~weak_ptr();
		}
		i_arena.deallocate(
			io_queue, sizeof(psyq::async_client::weak_ptr) * i_size);
	}

//.............................................................................
private:
	boost::thread                 thread_;
	boost::condition              condition_;
	boost::mutex                  mutex_;
	psyq::arena::shared_ptr       arena_;
	psyq::async_client::weak_ptr* queue_begin_;
	std::size_t                   queue_capacity_;
	std::size_t                   queue_size_;
	bool                          stop_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::async_functor:
	private boost::noncopyable
{
public:
	template< typename t_value_type, typename t_allocator >
	static psyq::async_client::shared_ptr create(
		t_allocator const&  i_allocator,
		t_value_type const& i_functor)
	{
		return boost::allocate_shared< wrapper< t_value_type > >(
			i_allocator, i_functor);
	}

private:
	template< typename t_value_type >
	class wrapper:
		public psyq::async_client
	{
	public:
		explicit wrapper(
			t_value_type const& i_functor):
		psyq::async_client(),
		functor_(i_functor)
		{
			// pass
		}

		virtual boost::int32_t run()
		{
			return this->functor_();
		}

	private:
		t_value_type functor_;
	};
};

#endif // !PSYQ_ASYNC_HPP_
