#ifndef PSYQ_SIMPLE_SINGLETON_HPP_
#define PSYQ_SIMPLE_SINGLETON_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/bind.hpp>
//#include <psyq/singleton.hpp>

namespace psyq
{
	template< typename, typename, typename > class simple_singleton;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename t_value,
	typename t_mutex = PSYQ_MUTEX_DEFAULT,
	typename t_tag = t_value >
class psyq::simple_singleton:
	private boost::noncopyable
{
	typedef simple_singleton< t_value, t_mutex, t_tag > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_value value_type;
	public: typedef t_tag tag;

	//-------------------------------------------------------------------------
	/** @brief singleton-instance��ێ�����B
	 */
	private: class instance_holder:
		private boost::noncopyable
	{
		//---------------------------------------------------------------------
		public: ~instance_holder()
		{
			// �ێ����Ă���̈��instance��j������B
			t_value* const a_pointer(*(this->pointer_));
			PSYQ_ASSERT(NULL != a_pointer);
			*(this->pointer_) = NULL;
			this->pointer_ = NULL;
			a_pointer->~t_value();
		}

		public: instance_holder():
		pointer_(NULL)
		{
			// pass
		}

		public: template< typename t_constructor >
		void construct(
			t_value*&            io_pointer,
			t_constructor const& i_constructor)
		{
			// �ێ����Ă���̈��instance���\�z����B
			PSYQ_ASSERT(NULL == this->pointer_);
			i_constructor.template apply< t_value >(&this->storage_);
			io_pointer = reinterpret_cast< t_value* >(&this->storage_);
			this->pointer_ = &io_pointer;
		}

		//---------------------------------------------------------------------
		private: t_value** pointer_;
		private: typename boost::aligned_storage<
			sizeof(t_value),
			boost::alignment_of< t_value >::value >::type
				storage_;
	};

	//-------------------------------------------------------------------------
	private: simple_singleton();

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance���Q�Ƃ���B
	        �܂�singleton-instance���Ȃ��Ȃ�Adefault-constructor�ō\�z����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	public: static t_value* get()
	{
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance��default-constructor�ō\�z����B
	        ���ł�singleton-instance������Ȃ�A�\�z�͍s�킸�����̂��̂�Ԃ��B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	public: static t_value* construct()
	{
		return this_type::construct(boost::in_place());
	}

	/** @brief sigleton-instance���\�z����B
	        ���ł�singleton-instance������Ȃ�A�\�z�͍s�킸�����̂��̂�Ԃ��B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
		@param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	public: template< typename t_constructor >
	static t_value* construct(t_constructor const& i_constructor)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		PSYQ_CALL_ONCE(
			this_type::construct_flag(),
			boost::bind(
				&construct_instance< t_constructor >, &i_constructor));

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != this_type::instance().pointer);
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	 */
	private: template< typename t_constructor >
	static void construct_instance(
		t_constructor const* const i_constructor)
	{
		this_type::instance().construct(this_type::pointer(), *i_constructor);
	}

	/** @brief singleton-instance���\�z�������ǂ�����flag���Q�Ƃ���B
	 */
	private: static boost::once_flag& construct_flag()
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

	/** @brief singleton-instance��ێ�����̈���Q�Ƃ���B
	        �ÓI�Ǐ��ϐ��Ȃ̂ŁA�\�z�͍ŏ��ɂ��̊֐����Ă΂ꂽ���_�ōs����B
	        �ŏ��͕K��construct_instance()����Ă΂��B
	        �j���́Amain()�̏I����ɁA�\�z���������̋t���Ŏ����I�ɍs����B
	 */
	private: static typename this_type::instance_holder& instance()
	{
		static typename this_type::instance_holder s_instance;
		return s_instance;
	}
};

#endif // PSYQ_SIMPLE_SINGLETON_HPP_
