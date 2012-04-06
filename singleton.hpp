#ifndef PSYQ_SINGLETON_HPP_
#define PSYQ_SINGLETON_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>

#ifdef PSYQ_DISABLE_THREADS
	#undef PSYQ_MUTEX_DEFAULT
	#define PSYQ_MUTEX_DEFAULT psyq::_dummy_mutex
#elif !defined(PSYQ_MUTEX_DEFAULT)
	#define PSYQ_MUTEX_DEFAULT boost::mutex
#endif // PSYQ_DISABLE_THREADS

namespace psyq
{
	template< typename, typename, typename > class singleton;

	template< typename > class _singleton_ordered_destructor;
	template< typename, typename > class _singleton_ordered_storage;
	class _dummy_mutex;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::_dummy_mutex:
	private boost::noncopyable
{
public:
	void lock() const {}
	void unlock() const {}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �j���֐��̒P�����A��list�B
    @tparam t_mutex multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template< typename t_mutex >
class psyq::_singleton_ordered_destructor:
	public boost::noncopyable
{
	typedef psyq::_singleton_ordered_destructor< t_mutex > this_type;
	template< typename, typename, typename > friend class psyq::singleton;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	~_singleton_ordered_destructor()
	{
		// list�̐擪node��؂藣���B
		this_type* const a_node(this_type::first_node());
		PSYQ_ASSERT(NULL != a_node);
		this_type::first_node() = a_node->next;

		// �؂藣����node��j������Bthis�̔j���ł͂Ȃ����Ƃɒ��ӁI
		typename this_type::function const a_destructor(a_node->destructor);
		a_node->next = a_node;
		a_node->destructor = NULL;
		(*a_destructor)(a_node);
	}

	//-------------------------------------------------------------------------
	/** @brief �j���̗D�揇�ʂ��擾����B
	 */
	int get_priority() const
	{
		return this->priority;
	}

//.............................................................................
protected:
	typedef void (*function)(this_type* const); ///< �j���֐��̌^�B

	//-------------------------------------------------------------------------
	explicit _singleton_ordered_destructor(
		typename this_type::function i_destructor):
		destructor(i_destructor),
		priority(0)
	{
		this->next = this;
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	/** @brief �j���֐�list�ɓo�^����B
	    @param[in] i_priority �j���̗D�揇�ʁB
	 */
	void join(
		int const i_priority)
	{
		PSYQ_ASSERT(this == this->next);

		// �D�揇�ʂ��X�V���Alist�ɑ}������B
		this->priority = i_priority;
		this_type* a_node(this_type::first_node());
		if (this_type::less_equal(i_priority, a_node))
		{
			PSYQ_ASSERT(this != a_node);

			// �D�揇�ʂ��ŏ��Ȃ̂ŁA�擪�ɑ}������B
			this->next = a_node;
			this_type::first_node() = this;
		}
		else
		{
			// �}���ʒu���������Ă���}������B
			for (;;)
			{
				PSYQ_ASSERT(this != a_node);
				this_type* const a_next(a_node->next);
				if (this_type::less_equal(i_priority, a_next))
				{
					a_node->next = this;
					this->next = a_next;
					break;
				}
				a_node = a_next;
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �j���֐�list���番������B
	 */
	void unjoin()
	{
		this_type* a_node(this_type::first_node());
		if (this == a_node)
		{
			// �擪����؂藣���B
			this_type::first_node() = this->next;
		}
		else if (this != this->next)
		{
			for (;;)
			{
				PSYQ_ASSERT(NULL != a_node);
				PSYQ_ASSERT(this != a_node);
				this_type* const a_next(a_node->next);
				if (a_next == this)
				{
					a_node->next = this->next;
					break;
				}
				a_node = a_next;
			}
		}
		this->next = this;
	}

	//-------------------------------------------------------------------------
	/** @brief �D�揇�ʂ��r�B
	 */
	static bool less_equal(
		int const              i_priority,
		this_type const* const i_node)
	{
		return NULL == i_node || i_priority <= i_node->priority;
	}

	/** @brief �j���֐�list�̐擪node�ւ�pointer���Q�Ƃ���B
	 */
	static this_type*& first_node()
	{
		static this_type* s_first_node(NULL);
		return s_first_node;
	}

	/** @brief singleton�Ŏg��mutex���Q�Ƃ���B
	 */
	static t_mutex& class_mutex()
	{
		static t_mutex s_mutex;
		return s_mutex;
	}

//.............................................................................
private:
	this_type*                   next;       ///< ����node�B
	typename this_type::function destructor; ///< �j�����ɌĂяo���֐��B
	int                          priority;   ///< �j���̗D�揇�ʁB�����ɔj�������B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief singleton-instance�̈�̒P�����A��list�B
    @tparam t_value_type singleton-instance�̌^�B
    @tparam t_mutex      multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template< typename t_value_type, typename t_mutex >
class psyq::_singleton_ordered_storage:
	public psyq::_singleton_ordered_destructor< t_mutex >
{
	typedef psyq::_singleton_ordered_storage< t_value_type, t_mutex > this_type;
	typedef psyq::_singleton_ordered_destructor< t_mutex > super_type;
	template< typename, typename, typename > friend class psyq::singleton;

//.............................................................................
public:
	typedef t_value_type value_type;

//.............................................................................
	private:
	//-------------------------------------------------------------------------
	_singleton_ordered_storage():
		super_type(&this_type::destruct),
		pointer(NULL)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	t_value_type* get_pointer() const
	{
		return this->pointer;
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���̈��instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	 */
	template< typename t_constructor >
	void construct(
		t_constructor const& i_constructor)
	{
		PSYQ_ASSERT(NULL == this->get_pointer());
		i_constructor.template apply< t_value_type >(&this->storage);
		this->pointer = reinterpret_cast< t_value_type* >(&this->storage);
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���̈��instance��j������B
	 */
	static void destruct(
		super_type* const i_instance)
	{
		this_type* const a_instance(static_cast< this_type* >(i_instance));
		t_value_type* const a_pointer(a_instance->get_pointer());
		PSYQ_ASSERT(static_cast< void* >(&a_instance->storage) == a_pointer);
		a_instance->pointer = NULL;
		a_pointer->~t_value_type();
	}

//.............................................................................
	private:
	/// singleton-instance�ւ�pointer�B
	t_value_type* pointer;

	/// singleton-instance���i�[����̈�B
	typename boost::aligned_storage<
		sizeof(t_value_type),
		boost::alignment_of< t_value_type >::value >::type
			storage;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief singleton�Ǘ�class�B
    @tparam t_value_type singleton-instance�̌^�B
    @tparam t_mutex      multi-thread�Ή��Ɏg��mutex�̌^�B
    @tparam t_tag        �����^��singleton-instance�ŁA��ʂ��K�v�ȏꍇ�Ɏg���B
 */
template<
	typename t_value_type,
	typename t_mutex = PSYQ_MUTEX_DEFAULT,
	typename t_tag = t_value_type >
class psyq::singleton:
	private boost::noncopyable
{
	typedef psyq::singleton< t_value_type, t_mutex, t_tag > this_type;

//.............................................................................
public:
	typedef t_value_type value_type;
	typedef t_tag tag;

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance���擾����B
	    @return singleton-instance�ւ�pointer�B
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́ANULL��Ԃ��B
	 */
	static t_value_type* get()
	{
		return this_type::get(boost::type< t_mutex >());
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance��default-constructor�ō\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ�pointer�B
	 */
	static t_value_type* construct(
		int const i_destruct_priority = 0)
	{
		return this_type::construct(boost::in_place(), i_destruct_priority);
	}

	/** @brief sigleton-instance���\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ�pointer�B
	 */
	template< typename t_constructor >
	static t_value_type* construct(
		t_constructor const& i_constructor,
		int const            i_destruct_priority = 0)
	{
		return this_type::construct_once(
			i_constructor, i_destruct_priority, boost::type< t_mutex >());
	}

	//-------------------------------------------------------------------------
	/** @brief �j���̗D�揇�ʂ��擾����B
	    @return singleton-instance�̔j���̗D�揇�ʁB
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́A0��Ԃ��B
	 */
	static int get_destruct_priority()
	{
		if (NULL != this_type::get())
		{
			return this_type::instance().get_priority();
		}
		return 0;
	}

	/** @brief �j���̗D�揇�ʂ�ݒ肷��B
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́A�����s��Ȃ��B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	 */
	static void set_destruct_priority(
		int const i_destruct_priority)
	{
		if (NULL != this_type::get())
		{
			this_type::set_destruct_priority(
				i_destruct_priority, boost::type< t_mutex >());
		}
	}

//.............................................................................
private:
	typedef psyq::_singleton_ordered_storage< t_value_type, t_mutex > storage;

	//-------------------------------------------------------------------------
	template< typename t_mutex_policy >
	static t_value_type* get(boost::type< t_mutex_policy > const&)
	{
		if (0 < this_type::construct_flag().count)
		{
			return this_type::instance().get_pointer();
		}
		return NULL;
	}

	static t_value_type* get(boost::type< psyq::_dummy_mutex > const&)
	{
		return this_type::instance().get_pointer();
	}

	//-------------------------------------------------------------------------
	template< typename t_constructor, typename t_mutex_policy >
	static t_value_type* construct_once(
		t_constructor const& i_constructor,
		int const            i_destruct_priority,
		boost::type< t_mutex_policy > const&)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		boost::call_once(
			this_type::construct_flag(),
			boost::bind(
				&construct_instance< t_constructor >,
				&i_constructor,
				i_destruct_priority));

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != this_type::instance().get_pointer());
		return  this_type::instance().get_pointer();
	}

	template< typename t_constructor >
	static t_value_type* construct_once(
		t_constructor const& i_constructor,
		int const            i_destruct_priority,
		boost::type< psyq::_dummy_mutex > const&)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		typename this_type::storage& a_instance(this_type::instance());
		if (NULL == a_instance.get_pointer())
		{
			// instance��j���֐�list�ɓo�^���Ă���\�z����B
			a_instance.join(i_destruct_priority);
			a_instance.construct(i_constructor);
		}

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != a_instance.get_pointer());
		return a_instance.get_pointer();
	}

	/** @brief singleton-instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB
	 */
	template< typename t_constructor >
	static void construct_instance(
		t_constructor const* const i_constructor,
		int const                  i_destruct_priority)
	{
		// mutex���\�z����B
		psyq::_singleton_ordered_destructor< t_mutex >::class_mutex();

		// instance��j���֐�list�ɓo�^���Ă���\�z����B
		typename this_type::storage& a_instance(this_type::instance());
		a_instance.join(i_destruct_priority);
		a_instance.construct(*i_constructor);
	}

	/** @brief singleton-instance���\�z�������ǂ�����flag���Q�Ƃ���B
	 */
	static boost::once_flag& construct_flag()
	{
		static boost::once_flag s_constructed = BOOST_ONCE_INIT;
		return s_constructed;
	}

	//-------------------------------------------------------------------------
	template< typename t_mutex_policy >
	static void set_destruct_priority(
		int const i_destruct_priority,
		boost::type< t_mutex_policy > const&)
	{
		// lock���Ă���D�揇�ʂ�ύX����B
		boost::lock_guard< t_mutex > const a_lock(
			this_type::storage::class_mutex());
		this_type::set_destruct_priority(
			i_destruct_priority, boost::type< psyq::_dummy_mutex >());
	}

	static void set_destruct_priority(
		int const i_destruct_priority,
		boost::type< psyq::_dummy_mutex > const&)
	{
		// �قȂ�D�揇�ʂ��ݒ肳�ꂽ�ꍇ�ɂ̂ݕύX����B
		typename this_type::storage& a_instance(this_type::instance());
		if (a_instance.get_priority() != i_destruct_priority)
		{
			// �j���֐�list������O������ŁA�o�^����B
			a_instance.unjoin();
			a_instance.join(i_destruct_priority);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instance�̈���Q�Ƃ���B
	 */
	static typename this_type::storage& instance()
	{
		static typename this_type::storage s_instance;
		return s_instance;
	}

	//-------------------------------------------------------------------------
	singleton();
};

#endif // PSYQ_SINGLETON_HPP_
