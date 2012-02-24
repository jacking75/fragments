#ifndef PSYQ_SINGLETON_HPP_
#define PSYQ_SINGLETON_HPP_

#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#ifndef PSYQ_SINGLETON_DISABLE_THREADS
	#include <boost/bind.hpp>
	#include <boost/thread/locks.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/once.hpp>
#endif // !PSYQ_SINGLETON_DISABLE_THREADS

namespace psyq
{
	template< typename, typename > class singleton;

	struct _singleton_default_tag {};
	class _singleteon_ordered_destructor;
	template< typename > class _singleton_ordered_storage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �j���֐��̒P�����A��list�B
 */
class psyq::_singleteon_ordered_destructor:
	public boost::noncopyable
{
	typedef psyq::_singleteon_ordered_destructor this_type;
	template< typename, typename > friend class psyq::singleton;

	//.........................................................................
	public:
	//-------------------------------------------------------------------------
	~_singleteon_ordered_destructor()
	{
		// list�̐擪node��؂藣���B
		this_type* const a_node(this_type::first_node());
		PSYQ_ASSERT(NULL != a_node);
		this_type::first_node() = a_node->next;

		// �؂藣����node��j������Bthis�̔j���ł͂Ȃ����Ƃɒ��ӁI
		this_type::function const a_destructor(a_node->destructor);
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

	//.........................................................................
	protected:
	typedef void (*function)(this_type* const); ///< �j���֐��̌^�B

	//-------------------------------------------------------------------------
	explicit _singleteon_ordered_destructor(
		this_type::function i_destructor):
		destructor(i_destructor),
		priority(0)
	{
		this->next = this;
	}

	//.........................................................................
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
	#ifndef PSYQ_SINGLETON_DISABLE_THREADS
		static boost::mutex& class_mutex()
		{
			static boost::mutex s_mutex;
			return s_mutex;
		}
	#endif // !PSYQ_SINGLETON_DISABLE_THREADS

	//.........................................................................
	private:
	this_type*          next;       ///< ����node�B
	this_type::function destructor; ///< �j�����ɌĂяo���֐��B
	int                 priority;   ///< �j���̗D�揇�ʁB�����ɔj�������B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief singleton-instance�̈�̒P�����A��list�B
 */
template< typename t_value_type >
class psyq::_singleton_ordered_storage:
	public psyq::_singleteon_ordered_destructor
{
	typedef psyq::_singleton_ordered_storage< t_value_type > this_type;
	typedef psyq::_singleteon_ordered_destructor super_type;
	template< typename, typename > friend class psyq::singleton;

	//.........................................................................
	public:
	typedef t_value_type value_type;

	//.........................................................................
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

	//.........................................................................
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
    @tparam t_value_type singleton�̌^�B
    @tparam t_tag �����^��singleton�ŋ�ʂ��K�v�ȏꍇ�Ɏg��tag�B
 */
template<
	typename t_value_type,
	typename t_tag = psyq::_singleton_default_tag >
class psyq::singleton:
	private boost::noncopyable
{
	typedef psyq::singleton< t_value_type, t_tag > this_type;

	//.........................................................................
	public:
	typedef t_value_type value_type;
	typedef t_tag tag;

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance���Q�Ƃ���B
	        �܂�singleton-instance���Ȃ��Ȃ�Adefault-constructor�ō\�z����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	static t_value_type& get()
	{
		return this_type::construct();
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance��default-constructor�ō\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	static t_value_type& construct(
		int const i_destruct_priority = 0)
	{
		return this_type::construct(boost::in_place(), i_destruct_priority);
	}

	/** @brief sigleton-instance���\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	template< typename t_constructor >
	static t_value_type& construct(
		t_constructor const& i_constructor,
		int const            i_destruct_priority = 0)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		#ifndef PSYQ_SINGLETON_DISABLE_THREADS
			boost::call_once(
				this_type::is_constructed(),
				boost::bind(
					&construct_instance< t_constructor >,
					&i_constructor,
					i_destruct_priority));
		#else
			if (!this_type::is_constructed())
			{
				construct_instance(&i_constructor, i_destruct_priority);
				this_type::is_constructed() = true;
			}
		#endif // !PSYQ_SINGLETON_NO_THREAD_SAFE

		// singleton-instance���擾�B
		typename this_type::storage& a_instance(this_type::instance());
		PSYQ_ASSERT(NULL != a_instance.get_pointer());
		return *a_instance.get_pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief �j���̗D�揇�ʂ��擾����B
	 */
	static int get_destruct_priority()
	{
		// �܂�singleton-instance���Ȃ��ꍇ�́A�����ō\�z���Ă����B
		this_type::construct();
		return this_type::instance().get_priority();
	}

	/** @brief �j���̗D�揇�ʂ�ݒ肷��B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	 */
	static void set_destruct_priority(
		int const i_destruct_priority)
	{
		// �܂�singleton-instance���Ȃ��ꍇ�́A�����ō\�z���Ă����B
		this_type::construct(i_destruct_priority);

		// singleton-instance���Q�Ƃ���O�ɁAlock���Ă����B
		#ifndef PSYQ_SINGLETON_DISABLE_THREADS
			boost::lock_guard< boost::mutex > const a_lock(
				psyq::_singleteon_ordered_destructor::class_mutex());
		#endif // !PSYQ_SINGLETON_DISABLE_THREADS

		// �قȂ�D�揇�ʂ��ݒ肳�ꂽ�ꍇ�ɂ̂ݕύX����B
		typename this_type::storage& a_instance(this_type::instance());
		if (a_instance.get_priority() != i_destruct_priority)
		{
			// �j���֐�list������O������ŁA�o�^����B
			a_instance.unjoin();
			a_instance.join(i_destruct_priority);
		}
	}

	//.........................................................................
	private:
	typedef psyq::_singleton_ordered_storage< t_value_type > storage;

	//-------------------------------------------------------------------------
	/** @brief singleton-instance�̈���Q�Ƃ���B
	 */
	static typename this_type::storage& instance()
	{
		static typename this_type::storage s_instance;
		return s_instance;
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
		#ifndef PSYQ_SINGLETON_DISABLE_THREADS
			// mutex���\�z����B
			psyq::_singleteon_ordered_destructor::class_mutex();
		#endif // !PSYQ_SINGLETON_DISABLE_THREADS

		// instance���\�z���A�j���֐�list�ɓo�^����B
		typename this_type::storage& a_instance(this_type::instance());
		a_instance.construct(*i_constructor);
		a_instance.join(i_destruct_priority);
	}

	/** @brief singleton-instance���\�z�������ǂ�����flag���Q�Ƃ���B
	 */
	#ifndef PSYQ_SINGLETON_DISABLE_THREADS
		static boost::once_flag& is_constructed()
		{
			static boost::once_flag s_constructed = BOOST_ONCE_INIT;
			return s_constructed;
		}
	#else
		static bool& is_constructed()
		{
			static bool s_constructed(false);
			return s_constructed;
		}
	#endif // !PSYQ_SINGLETON_DISABLE_THREADS

	//-------------------------------------------------------------------------
	singleton();
};

#endif // PSYQ_SINGLETON_HPP_
