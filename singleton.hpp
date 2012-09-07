#ifndef PSYQ_SINGLETON_HPP_
#define PSYQ_SINGLETON_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/bind/bind.hpp>

namespace psyq
{
	template< typename, typename, typename > class singleton;

	template< typename > class _singleton_ordered_destructor;
	template< typename, typename > class _singleton_ordered_holder;
}

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

	//-------------------------------------------------------------------------
	protected: typedef void (*function)(this_type* const); ///< �j���֐��̌^�B

	//-------------------------------------------------------------------------
	protected: explicit _singleton_ordered_destructor(
		typename this_type::function i_destructor):
	destructor_(i_destructor),
	priority_(0)
	{
		this->next_ = this;
	}

	//-------------------------------------------------------------------------
	public: ~_singleton_ordered_destructor()
	{
		// list�̐擪node��؂藣���B
		this_type* const a_node(this_type::first_node());
		PSYQ_ASSERT(NULL != a_node);
		this_type::first_node() = a_node->next_;

		// �؂藣����node��j������Bthis�̔j���ł͂Ȃ����Ƃɒ��ӁI
		typename this_type::function const a_destructor(a_node->destructor_);
		a_node->next_ = a_node;
		a_node->destructor_ = NULL;
		(*a_destructor)(a_node);
	}

	//-------------------------------------------------------------------------
	/** @brief �j���̗D�揇�ʂ��擾����B
	    @return �j���̗D�揇�ʁB�j���͏����ɍs����B
	 */
	public: int get_priority() const
	{
		return this->priority_;
	}

	//-------------------------------------------------------------------------
	/** @brief �j���֐�list�ɓo�^����B
	    @param[in] i_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	 */
	private: void join(int const i_priority)
	{
		PSYQ_ASSERT(this == this->next_);

		// �D�揇�ʂ��X�V���Alist�ɑ}������B
		this->priority_ = i_priority;
		this_type* a_node(this_type::first_node());
		if (this_type::less_equal(i_priority, a_node))
		{
			PSYQ_ASSERT(this != a_node);

			// �D�揇�ʂ��ŏ��Ȃ̂ŁA�擪�ɑ}������B
			this->next_ = a_node;
			this_type::first_node() = this;
		}
		else
		{
			// �}���ʒu���������Ă���}������B
			for (;;)
			{
				PSYQ_ASSERT(this != a_node);
				this_type* const a_next(a_node->next_);
				if (this_type::less_equal(i_priority, a_next))
				{
					a_node->next_ = this;
					this->next_ = a_next;
					break;
				}
				a_node = a_next;
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �j���֐�list���番������B
	 */
	private: void unjoin()
	{
		this_type* a_node(this_type::first_node());
		if (this == a_node)
		{
			// �擪����؂藣���B
			this_type::first_node() = this->next_;
		}
		else if (this != this->next_)
		{
			for (;;)
			{
				PSYQ_ASSERT(NULL != a_node);
				PSYQ_ASSERT(this != a_node);
				this_type* const a_next(a_node->next_);
				if (a_next == this)
				{
					a_node->next_ = this->next_;
					break;
				}
				a_node = a_next;
			}
		}
		this->next_ = this;
	}

	//-------------------------------------------------------------------------
	/** @brief �D�揇�ʂ��r�B
	    @param[in] i_priority ���ӂ̗D�揇�ʁB
	    @param[in] i_node     �E�ӂ�node�B
	 */
	private: static bool less_equal(
		int const              i_priority,
		this_type const* const i_node)
	{
		return NULL == i_node || i_priority <= i_node->priority_;
	}

	/** @brief �j���֐�list�̐擪node�ւ�pointer���Q�Ƃ���B
	 */
	private: static this_type*& first_node()
	{
		static this_type* s_first_node(NULL);
		return s_first_node;
	}

	/** @brief singleton�Ŏg��mutex���Q�Ƃ���B
	 */
	private: static t_mutex& class_mutex()
	{
		static t_mutex s_mutex;
		return s_mutex;
	}

	//-------------------------------------------------------------------------
	private: this_type*                   next_;       ///< ����node�B
	private: typename this_type::function destructor_; ///< �j�����ɌĂяo���֐��B
	private: int                          priority_;   ///< �j���̗D�揇�ʁB�����ɔj�������B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief singleton-instance�̈�̒P�����A��list�B
    @tparam t_value singleton-instance�̌^�B
    @tparam t_mutex      multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template< typename t_value, typename t_mutex >
class psyq::_singleton_ordered_holder:
	public psyq::_singleton_ordered_destructor< t_mutex >
{
	typedef psyq::_singleton_ordered_holder< t_value, t_mutex > this_type;
	typedef psyq::_singleton_ordered_destructor< t_mutex > super_type;
	template< typename, typename, typename > friend class psyq::singleton;

	//-------------------------------------------------------------------------
	public: typedef t_value value_type;

	//-------------------------------------------------------------------------
	private: _singleton_ordered_holder():
	super_type(&this_type::destruct),
	pointer_(NULL)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���̈��instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	 */
	private: template< typename t_constructor >
	void construct(
		t_value*&            io_pointer,
		t_constructor const& i_constructor)
	{
		PSYQ_ASSERT(NULL == io_pointer);
		PSYQ_ASSERT(NULL == this->pointer_);
		i_constructor.template apply< t_value >(&this->storage_);
		io_pointer = reinterpret_cast< t_value* >(&this->storage_);
		this->pointer_ = &io_pointer;
	}

	//-------------------------------------------------------------------------
	/** @brief �ێ����Ă���̈��instance��j������B
	 */
	private: static void destruct(super_type* const i_instance)
	{
		this_type* const a_instance(static_cast< this_type* >(i_instance));
		PSYQ_ASSERT(NULL != a_instance->pointer_);
		t_value* const a_pointer(*a_instance->pointer_);
		PSYQ_ASSERT(
			static_cast< void* >(&a_instance->storage_) == a_pointer);
		*a_instance->pointer_ = NULL;
		a_instance->pointer_ = NULL;
		a_pointer->~t_value();
	}

	//-------------------------------------------------------------------------
	/// singleton-instance���i�[����̈�B
	private: typename boost::aligned_storage<
		sizeof(t_value), boost::alignment_of< t_value >::value >::type
			storage_;

	/// singleton-instance�ւ�pointer�̊i�[�ꏊ�B
	private: t_value** pointer_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �j�����Ԃ��ς�singleton�Ǘ�class�B
    @tparam t_value singleton-instance�̌^�B
    @tparam t_tag   �����^��singleton-instance�ŁA��ʂ��K�v�ȏꍇ�Ɏg���B
    @tparam t_mutex multi-thread�Ή��Ɏg��mutex�̌^�B
 */
template<
	typename t_value,
	typename t_tag = t_value,
	typename t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::singleton:
	private boost::noncopyable
{
	typedef psyq::singleton< t_value, t_tag, t_mutex > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_value value_type;
	public: typedef t_tag tag;
	public: typedef t_mutex mutex;
	private: typedef psyq::_singleton_ordered_holder< t_value, t_mutex >
		instance_holder;

	//-------------------------------------------------------------------------
	private: singleton();

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance���擾����B
	    @return singleton-instance�ւ�pointer�B
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́ANULL��Ԃ��B
	 */
	public: static t_value* get()
	{
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance��default-constructor�ō\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ�pointer�B
	 */
	public: static t_value* construct(int const i_destruct_priority = 0)
	{
		return this_type::construct(boost::in_place(), i_destruct_priority);
	}

	/** @brief sigleton-instance���\�z����B
	        ���ł�singleton-instance������Ȃ�A�����s�킸�Ɋ����̂��̂�Ԃ��B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	    @param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ�pointer�B
	 */
	public: template< typename t_constructor >
	static t_value* construct(
		t_constructor const& i_constructor,
		int const            i_destruct_priority = 0)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		PSYQ_CALL_ONCE(
			this_type::construct_flag(),
			boost::bind(
				&construct_instance< t_constructor >,
				&i_constructor,
				i_destruct_priority));

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != this_type::pointer());
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief �j���̗D�揇�ʂ��擾����B
	    @return singleton-instance�̔j���̗D�揇�ʁB�j���͏����ɍs����B
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́A0��Ԃ��B
	 */
	public: static int get_destruct_priority()
	{
		if (NULL != this_type::pointer())
		{
			return this_type::instance().get_priority();
		}
		return 0;
	}

	/** @brief �j���̗D�揇�ʂ�ݒ肷��B
	        ������singleton-instance���܂��\�z���ĂȂ��ꍇ�́A�����s��Ȃ��B
	    @param[in] i_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return i_priority�����̂܂ܕԂ��B
	 */
	public: static int set_destruct_priority(int const i_priority)
	{
		if (NULL != this_type::pointer())
		{
			// lock���Ă���D�揇�ʂ�ύX����B
			PSYQ_LOCK_GUARD< t_mutex > const a_lock(
				psyq::_singleton_ordered_destructor< t_mutex >::class_mutex());

			// �قȂ�D�揇�ʂ��ݒ肳�ꂽ�ꍇ�ɂ̂ݕύX����B
			typename this_type::instance_holder& a_instance(
				this_type::instance());
			if (a_instance.get_priority() != i_priority)
			{
				// �j���֐�list������O������ŁA�ēx�o�^����B
				a_instance.unjoin();
				a_instance.join(i_priority);
			}
		}
		return i_priority;
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	    @param[in] i_priority �j���̗D�揇�ʁB
	 */
	private: template< typename t_constructor >
	static void construct_instance(
		t_constructor const* const i_constructor,
		int const                  i_priority)
	{
		// mutex���\�z����B
		psyq::_singleton_ordered_destructor< t_mutex >::class_mutex();

		// instance��j���֐�list�ɓo�^���Ă���\�z����B
		typename this_type::instance_holder& a_instance(this_type::instance());
		a_instance.join(i_priority);
		a_instance.construct(this_type::pointer(), *i_constructor);
	}

	/** @brief singleton-instance���\�z�������ǂ�����flag���Q�Ƃ���B
	 */
	private: static PSYQ_ONCE_FLAG& construct_flag()
	{
		PSYQ_ONCE_FLAG_INIT(s_construct_flag);
		return s_construct_flag;
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instance�ւ�pointer���Q�Ƃ���B
	 */
	private: static t_value*& pointer()
	{
		static t_value* s_pointer(NULL);
		return s_pointer;
	}

	/** @brief singleton-instance�̈���Q�Ƃ���B
	        �ÓI�Ǐ��ϐ��Ȃ̂ŁA�\�z�͍ŏ��ɂ��̊֐����Ă΂ꂽ���_�ōs����B
	        �ŏ��͕K��construct_instance()����Ă΂��B
	 */
	private: static typename this_type::instance_holder& instance()
	{
		static typename this_type::instance_holder s_instance;
		return s_instance;
	}
};

#endif // PSYQ_SINGLETON_HPP_
