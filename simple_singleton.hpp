#ifndef PSYQ_SIMPLE_SINGLETON_HPP_
#define PSYQ_SIMPLE_SINGLETON_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/bind.hpp>
#include <boost/thread/once.hpp>
//#include <psyq/singleton.hpp>

namespace psyq
{
	template< typename, typename, typename > class simple_singleton;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
	typename t_value_type,
	typename t_tag = psyq::_singleton_default_tag,
	typename t_mutex = PSYQ_MUTEX_DEFAULT >
class psyq::simple_singleton:
	private boost::noncopyable
{
	typedef simple_singleton< t_value_type, t_tag, t_mutex > this_type;

//.............................................................................
public:
	typedef t_value_type value_type;
	typedef t_tag tag;

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance���Q�Ƃ���B
	        �܂�singleton-instance���Ȃ��Ȃ�Adefault-constructor�ō\�z����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	static t_value_type* get()
	{
		return this_type::get(boost::type< t_mutex >());
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instance��default-constructor�ō\�z����B
	        ���ł�singleton-instance������Ȃ�A�\�z�͍s�킸�����̂��̂�Ԃ��B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	static t_value_type* construct()
	{
		return this_type::construct(boost::in_place());
	}

	/** @brief sigleton-instance���\�z����B
	        ���ł�singleton-instance������Ȃ�A�\�z�͍s�킸�����̂��̂�Ԃ��B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
		@param[in] i_destruct_priority �j���̗D�揇�ʁB�j���͏����ɍs����B
	    @return singleton-instance�ւ̎Q�ƁB
	 */
	template< typename t_constructor >
	static t_value_type* construct(
		t_constructor const& i_constructor)
	{
		return this_type::construct_once(
			i_constructor, boost::type< t_mutex >());
	}

//.............................................................................
private:
	//-------------------------------------------------------------------------
	/** @brief singleton-instance��ێ�����B
	 */
	class instance_holder:
		private boost::noncopyable
	{
	public:
		//---------------------------------------------------------------------
		~instance_holder()
		{
			// �ێ����Ă���̈��instance��j������B
			t_value_type* const a_pointer(this->pointer);
			PSYQ_ASSERT(NULL != a_pointer);
			this->pointer = NULL;
			a_pointer->~t_value_type();
		}

		instance_holder():
		pointer(NULL)
		{
			// pass
		}

		t_value_type* get_pointer() const
		{
			return this->pointer;
		}

		template< typename t_constructor >
		void construct(
			t_constructor const& i_constructor)
		{
			// �ێ����Ă���̈��instance���\�z����B
			PSYQ_ASSERT(NULL == this->pointer);
			i_constructor.template apply< t_value_type >(&this->storage);
			this->pointer = reinterpret_cast< t_value_type* >(&this->storage);
		}

	private:
		typename boost::aligned_storage<
			sizeof(t_value_type),
			boost::alignment_of< t_value_type >::value >::type
				storage;
		t_value_type* pointer;
	};

	//-------------------------------------------------------------------------
	template< typename t_mutex_policy >
	static t_value_type* get(boost::type< t_mutex_policy > const&)
	{
		if (this_type::construct_flag().count <= 0)
		{
			return NULL;
		}
		else
		{
			return this_type::instance().get_pointer();
		}
	}

	static t_value_type* get(boost::type< psyq::_dummy_mutex > const&)
	{
		return this_type::instance().get_pointer();
	}

	//-------------------------------------------------------------------------
	template< typename t_constructor, typename t_mutex_policy >
	static t_value_type* construct_once(
		t_constructor const& i_constructor,
		boost::type< t_mutex_policy > const&)
	{
		// sigleton-instance�\�z�֐�����x�����Ăяo���B
		boost::call_once(
			this_type::construct_flag(),
			boost::bind(
				&construct_instance< t_constructor >, &i_constructor));

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != this_type::instance().pointer);
		return this_type::instance().get_pointer();
	}

	template< typename t_constructor >
	static t_value_type* construct_once(
		t_constructor const& i_constructor,
		boost::type< psyq::_dummy_mutex > const&)
	{
		typename this_type::storage& a_instance(this_type::instance());
		if (NULL == a_instance.get_pointer())
		{
			a_instance.construct(i_constructor);
		}

		// singleton-instance���擾�B
		PSYQ_ASSERT(NULL != a_instance.get_pointer());
		return a_instance.get_pointer();
	}

	/** @brief singleton-instance���\�z����B
	    @param[in] i_constructor boost::in_place����擾�����\�z�֐�object�B
	 */
	template< typename t_constructor >
	static void construct_instance(
		t_constructor const* const i_constructor)
	{
		this_type::instance().construct(*i_constructor);
	}

	/** @brief singleton-instance���\�z�������ǂ�����flag���Q�Ƃ���B
	 */
	static boost::once_flag& construct_flag()
	{
		static boost::once_flag s_constructed = BOOST_ONCE_INIT;
		return s_constructed;
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instance��ێ�����̈���Q�Ƃ���B
	        �ÓI�Ǐ��ϐ��Ȃ̂ŁA�\�z�͍ŏ��ɂ��̊֐����Ă΂ꂽ���_�ōs����B
	        �ŏ��͕K��construct_instance()����Ă΂��B
	        �j���́Amain()�̏I����ɁA�\�z���������̋t���Ŏ����I�ɍs����B
	 */
	static typename this_type::instance_holder& instance()
	{
		static typename this_type::instance_holder s_instance;
		return s_instance;
	}

	//-------------------------------------------------------------------------
	simple_singleton();
};

#endif // PSYQ_SIMPLE_SINGLETON_HPP_
