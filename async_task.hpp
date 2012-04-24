#ifndef PSYQ_ASYNC_TASK_HPP_
#define PSYQ_ASYNC_TASK_HPP_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/mutex.hpp>

namespace psyq
{
	class async_task;
	class async_queue;
	class async_functor;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񓯊�����task���class�B
 */
class psyq::async_task:
	private boost::noncopyable
{
	typedef psyq::async_task this_type;

	friend class psyq::async_queue;

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
	static psyq::async_task::shared_ptr create(
		t_allocator const& i_allocator,
		t_functor const&   i_functor)
	{
		return boost::allocate_shared< function_wrapper< t_functor > >(
			i_allocator, i_functor);
	}

	//-------------------------------------------------------------------------
	virtual ~async_task()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief ��Ԓl���擾�B
	 */
	boost::int32_t get_state() const
	{
		return this->state_;
	}

	/** @brief ��Ԓl��ݒ�B
	    @param[in] i_state �ݒ肷��state_BUSY�ȊO�̏�Ԓl�B
	    @return true�Ȃ琬���Bfalse�Ȃ玸�s�B
	 */
	bool set_state(boost::int32_t const i_state)
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

	virtual boost::int32_t run() = 0;

//.............................................................................
private:
	template< typename > class function_wrapper;

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
/** @brief �֐�object���Ăяo���񓯊�����task�B
 */
template< typename t_functor >
class psyq::async_task::function_wrapper:
	public psyq::async_task
{

//.............................................................................
public:
	explicit function_wrapper(t_functor const& i_functor):
	psyq::async_task(),
	functor_(i_functor)
	{
		// pass
	}

	virtual boost::int32_t run()
	{
		return this->functor_();
	}

//.............................................................................
private:
	t_functor functor_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class fnv_hash
{
	typedef fnv_hash this_type;

public:
	static boost::uint32_t const offset32 = 0x811c9dc5;
	static boost::uint32_t const prime32 = 0x1000193;
	static boost::uint64_t const offset64 = 0xcbf29ce484222325ULL;
	static boost::uint64_t const prime64 = 0x100000001b3ULL;

	//---------------------------------------------------------------------
	/** @brief FNV(Fowler-Noll-Vo)hash�֐�
	    http://www.radiumsoftware.com/0605.html#060526
	    http://d.hatena.ne.jp/jonosuke/20100406/p1
	    @param[in] i_begin  hash������byte�z��̐擪�ʒu�B
	    @param[in] i_size   hash������byte���B
	    @param[in] i_offset hash�J�n�l�B
	    @param[in] i_prime  fnv-hash�f���B
	 */
	template< typename t_value_type>
	t_value_type make(
		void const* const  i_begin,
		std::size_t const  i_size,
		t_value_type const i_offset,
		t_value_type const i_prime)
	{
		t_value_type a_hash(i_offset);
		if (NULL != i_begin)
		{
			boost::uint8_t const* a_iterator(
				static_cast< boost::uint8_t const* >(i_begin));
			boost::uint8_t const* const a_end(a_iterator + i_size);
			while (a_iterator < a_end)
			{
				a_hash = (a_hash * i_prime) ^ *a_iterator;
				++a_iterator;
			}
		}
		return a_hash;
	}

	boost::uint32_t make32(
		void const* const     i_begin,
		std::size_t const     i_size,
		boost::uint32_t const i_offset = this_type::offset32)
	{
		return this_type::make(i_begin, i_size, i_offset, this_type::prime32);
	}

	boost::uint64_t make64(
		void const* const     i_begin,
		std::size_t const     i_size,
		boost::uint64_t const i_offset = this_type::offset64)
	{
		return this_type::make(i_begin, i_size, i_offset, this_type::prime64);
	}
};

#endif // !PSYQ_ASYNC_TASK_HPP_
