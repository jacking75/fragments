#ifndef PSYQ_FILE_HPP_
#define PSYQ_FILE_HPP_

#include <boost/bind/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class file_task
{
	typedef file_task this_type;

	friend class file_server;

//.............................................................................
public:
	typedef boost::shared_ptr< this_type > holder;
	typedef boost::weak_ptr< this_type > observer;

//.............................................................................
protected:
	file_task():
	next_(NULL)
	{
		// pass
	}

	virtual void run() = 0;

//.............................................................................
private:
	this_type::holder lock_;
	this_type*        next_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class file_server
{
	typedef file_server this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~file_server()
	{
		this->stop(true);
	}

	file_server():
	queue_(NULL),
	stop_(false)
	{
		static boost::once_flag s_constructed = BOOST_ONCE_INIT;
		boost::call_once(&this_type::construct_class_mutex, s_constructed);
		this->start();
	}

	//-------------------------------------------------------------------------
	void start()
	{
		if (!this->thread_.joinable())
		{
			boost::thread(boost::bind(&this_type::run, this)).swap(this->thread_);
		}
	}

	void stop(bool const i_sync = true)
	{
		this->stop_ = true;
		if (i_sync)
		{
			this->condition_.notify_all();
			this->thread_.join();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief file-task��ǉ��B
	    @param[in] i_task �ǉ�����file-task�B
	 */
	void add(file_task::holder const& i_task)
	{
		file_task* const a_task(i_task.get());
		if (NULL != a_task && NULL == a_task->lock_.get())
		{
			boost::lock_guard< boost::mutex > const a_lock(
				this_type::class_mutex());

			// file-task��lock����B
			a_task->lock_ = i_task;

			// file-task��҂��s��̖����ɑ}���B
			file_task* const a_back(this->queue_);
			this->queue_ = a_task;
			if (NULL != a_back)
			{
				a_task->next_ = a_back->next_;
				a_back->next_ = a_task;
			}
			else
			{
				// �҂��s�񂪋󂾂����̂ŁAthread���N���B
				a_task->next_ = a_task;
				this->condition_.notify_all();
			}
		}
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	void run()
	{
		while (!this->stop_)
		{
			file_task::holder const a_task(this->pop_front());
			if (!a_task.unique() && NULL != a_task.get())
			{
				a_task->run();
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �҂��s��̐擪�����o���B
	 */
	file_task::holder pop_front()
	{
		boost::unique_lock< boost::mutex > a_lock(this_type::class_mutex());
		if (NULL == this->queue_)
		{
			// �҂��s�񂪋󂾂����̂ŁA��Ԃ��ς��܂őҋ@�B
			this->condition_.wait(a_lock);
			if (NULL == this->queue_)
			{
				return file_task::holder();
			}
		}

		// �҂��s��̐擪�����o���B
		file_task* const a_task(this->queue_->next_);
		if (a_task != a_task->next_)
		{
			this->queue_->next_ = a_task->next_;
			a_task->next_ = a_task;
		}
		else
		{
			this->queue_ = NULL;
		}

		// file-task��lock�������B
		file_task::holder a_holder;
		a_holder.swap(a_task->lock_);
		return a_holder;
	}

	//-------------------------------------------------------------------------
	static boost::mutex& class_mutex()
	{
		static boost::mutex s_mutex;
		return s_mutex;
	}

	static void construct_class_mutex()
	{
		this_type::class_mutex();
	}

//.............................................................................
private:
	boost::thread    thread_;
	boost::condition condition_;
	file_task*       queue_;
	bool             stop_;
};

#endif // !PSYQ_FILE_HPP_
