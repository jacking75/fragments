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
	bool set_locked_state(boost::int32_t const i_state)
	{
		boost::lock_guard< boost::mutex > const a_lock(this->mutex_);
		if (this_type::state_BUSY != this->state_)
		{
			this->state_ = i_state;
			return true;
		}
		return false;
	}

	void set_unlocked_state(boost::int32_t const i_state)
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
			*this->arena_, this->queue_begin_, this->queue_capacity_);
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
	    @param[in] i_client �o�^����񓯊�����client���w��smart-pointer�B
	    @return �o�^�����񓯊�����client�̐��B
	 */
	template< typename t_smart_ptr >
	std::size_t add(t_smart_ptr const& i_client)
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
		this_type::client_ptr* const a_last_queue(this->queue_begin_);
		std::size_t const a_last_capacity(this->queue_capacity_);
		std::size_t const a_last_size(this->queue_size_);
		this->queue_begin_ = NULL;
		this->queue_capacity_ = 0;
		a_lock.unlock();

		// �V����queue���\�z�B
		std::size_t const a_capacity(
			a_last_size + std::distance(i_begin, i_end));
		this_type::client_ptr* const a_queue(
			this_type::resize_queue(
				*this->arena_,
				a_capacity,
				a_last_queue,
				a_last_size,
				a_last_capacity));

		// container��V����queue��copy�B
		std::size_t a_count(0);
		t_iterator a_iterator(i_begin);
		for (std::size_t i = a_last_size; i < a_capacity; ++i, ++a_iterator)
		{
			psyq::async_client::shared_ptr const a_holder(*a_iterator);
			psyq::async_client* const a_client(a_holder.get());
			if (NULL != a_client
				&& a_client->set_locked_state(psyq::async_client::state_BUSY))
			{
				// busy��Ԃł͂Ȃ��񓯊�����client�������o�^�ł���B
				new(&a_queue[i]) this_type::client_ptr(a_holder);
				++a_count;
			}
			else
			{
				new(&a_queue[i]) this_type::client_ptr();
			}
		}

		// �V����queue�ɍ����ւ��Ă���ʒm�B
		a_lock.lock();
		this->queue_begin_ = a_queue;
		this->queue_capacity_ = a_capacity;
		this->condition_.notify_all();
		return a_count;
	}

	/** @brief queue�̑傫����K�v�Œ���ɂ���B
	 */
	void shrink_queue()
	{
		this->add(
			static_cast< psyq::async_client::shared_ptr* >(NULL),
			static_cast< psyq::async_client::shared_ptr* >(NULL));
	}

	//-------------------------------------------------------------------------
	psyq::arena::shared_ptr const& get_arena() const
	{
		return this->arena_;
	}

//.............................................................................
private:
	typedef psyq::async_client::weak_ptr client_ptr;

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
		this_type::client_ptr* a_queue(NULL);
		std::size_t a_size(0);
		std::size_t a_capacity(0);
		boost::unique_lock< boost::mutex > a_lock(this->mutex_);
		while (!this->stop_)
		{
			// queue�̑傫�����X�V�B
			this->queue_size_ = a_size;
			if (a_size <= 0 && NULL == this->queue_begin_)
			{
				// queue����ɂȂ�����ҋ@�B
				this->condition_.wait(a_lock);
			}

			// queue���X�V�B
			if (NULL != this->queue_begin_)
			{
				// �V����queue�����o���B
				this_type::client_ptr* const a_last_queue(a_queue);
				std::size_t const a_last_capacity(a_capacity);
				a_queue = this->queue_begin_;
				a_capacity = this->queue_capacity_;
				this->queue_begin_ = NULL;
				this->queue_capacity_ = 0;

				// ����queue�����񓯊�����client���A�V����queue�Ɉړ��B
				std::size_t const a_last_size(a_size);
				PSYQ_ASSERT(a_last_size <= a_capacity);
				for (std::size_t i = 0; i < a_last_size; ++i)
				{
					PSYQ_ASSERT(a_queue[i].expired());
					a_queue[i].swap(a_last_queue[i]);
				}
				a_size = a_capacity;

				// ����queue��j���B
				this_type::destroy_queue(
					*this->arena_, a_last_queue, a_last_capacity);
			}

			// mutex��unlock���Ă���Aqueue�����s�B
			a_lock.unlock();
			a_size = this_type::run_queue(a_queue, a_size);
			if (a_size <= 0)
			{
				// queue����ɂȂ����̂Ŕj���B
				this_type::destroy_queue(*this->arena_, a_queue, a_capacity);
				a_queue = NULL;
				a_capacity = 0;
			}
			a_lock.lock();
		}
		this_type::destroy_queue(*this->arena_, a_queue, a_capacity);
	}

	//-------------------------------------------------------------------------
	/** @brief queue�����񓯊�����client�����s����B
	    @param[in] io_queue queue�̐擪�ʒu�B
	    @param[in] i_size   queue�����񓯊�����client�̐��B
	    @return queue�����񓯊�����client�̐��B
	 */
	static std::size_t run_queue(
		this_type::client_ptr* const io_queue,
		std::size_t const            i_size)
	{
		std::size_t a_size(0);
		for (std::size_t i = 0; i < i_size; ++i)
		{
			psyq::async_client::shared_ptr const a_holder(io_queue[i]);
			psyq::async_client* const a_client(a_holder.get());
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
				a_client->set_unlocked_state(a_state);
			}
			io_queue[i].reset();
		}
		return a_size;
	}

	/** @brief queue�̑傫����ύX����B
	    @param[in] i_arena           memory�����Ɏg��arena�B
	    @param[in] i_capacity        �V����queue�̍ő�v�f���B
	    @param[in,out] io_last_queue ����queue�̐擪�ʒu�B
	    @param[in] i_last_size       ����queue�̗v�f���B
	    @param[in] i_last_capacity   ����queue�̍ő�v�f���B
		@return �V����queue�̐擪�ʒu�B
	 */
	static this_type::client_ptr* resize_queue(
		psyq::arena&                 i_arena,
		std::size_t const            i_capacity,
		this_type::client_ptr* const io_last_queue,
		std::size_t const            i_last_size,
		std::size_t const            i_last_capacity)
	{
		// �V����queue���m�ہB
		this_type::client_ptr* const a_queue(
			static_cast< this_type::client_ptr* >(
				i_arena.allocate(
					i_capacity * sizeof(this_type::client_ptr),
					boost::alignment_of< this_type::client_ptr >::value,
					0)));
		if (NULL != a_queue)
		{
			// ����queue�����񓯊�����client���A�V����queue�Ɉړ��B
			PSYQ_ASSERT(i_last_size <= i_capacity);
			for (std::size_t i = 0; i < i_last_size; ++i)
			{
				new(&a_queue[i]) this_type::client_ptr();
				if (NULL != io_last_queue)
				{
					a_queue[i].swap(io_last_queue[i]);
				}
			}
		}
		else
		{
			PSYQ_ASSERT(i_capacity <= 0);
		}

		// ����queue��j���B
		this_type::destroy_queue(i_arena, io_last_queue, i_last_capacity);
		return a_queue;
	}

	/** @brief �񓯊�����client�s���j������B
	    @param[in] i_arena    �j���Ɏg��memory-arena�B
	    @param[in] io_queue   queue�̐擪�ʒu�B
	    @param[in] i_capacity queue�����񓯊�����client�̐��B
	 */
	static void destroy_queue(
		psyq::arena&                 i_arena,
		this_type::client_ptr* const io_queue,
		std::size_t const            i_capacity)
	{
		PSYQ_ASSERT(NULL != io_queue || i_capacity <= 0);
		for (std::size_t i = 0; i < i_capacity; ++i)
		{
			io_queue[i].~weak_ptr();
		}
		i_arena.deallocate(
			io_queue, sizeof(this_type::client_ptr) * i_capacity);
	}

//.............................................................................
private:
	boost::thread           thread_;
	boost::condition        condition_;
	boost::mutex            mutex_;
	psyq::arena::shared_ptr arena_;
	this_type::client_ptr*  queue_begin_;
	std::size_t             queue_capacity_;
	std::size_t             queue_size_;
	bool                    stop_;
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
