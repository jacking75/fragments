#ifndef PSYQ_SIMPLE_SINGLETON_HPP_
#define PSYQ_SIMPLE_SINGLETON_HPP_

#include <boost/noncopyable.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/bind.hpp>
//#include <psyq/singleton.hpp>

/// @cond
namespace psyq
{
	template< typename, typename > class simple_singleton;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief singleton管理object。構築した逆順で破棄される。
    @tparam t_value singleton-objectの型。
    @tparam t_tag   同じ型のsingleton-objectで、区別に使うtag。
 */
template< typename t_value, typename t_tag = t_value >
class psyq::simple_singleton:
	private boost::noncopyable
{
	/// このobjectの型。
	public: typedef simple_singleton< t_value, t_tag > this_type;

	//-------------------------------------------------------------------------
	/// singleton-objectの型。
	public: typedef t_value value_type;

	/// 同じ型のsingleton-objectの、区別に使うtag。
	public: typedef t_tag tag;

	//-------------------------------------------------------------------------
	/** @brief singleton-instanceを保持する。
	 */
	private: class instance_holder:
		private boost::noncopyable
	{
		//---------------------------------------------------------------------
		public: ~instance_holder()
		{
			// 保持している領域のinstanceを破棄する。
			t_value* const a_pointer(*this->pointer_);
			PSYQ_ASSERT(NULL != a_pointer);
			*this->pointer_ = NULL;
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
			// 保持している領域にinstanceを構築する。
			PSYQ_ASSERT(NULL == this->pointer_);
			i_constructor.template apply< t_value >(&this->storage_);
			io_pointer = reinterpret_cast< t_value* >(&this->storage_);
			this->pointer_ = &io_pointer;
		}

		//---------------------------------------------------------------------
		/// singleton-instanceを格納する領域。
		private: typename boost::aligned_storage<
			sizeof(t_value), boost::alignment_of< t_value >::value >::type
				storage_;

		/// singleton-instanceへのpointerの格納場所。
		private: t_value** pointer_;
	};

	//-------------------------------------------------------------------------
	private: simple_singleton();

	//-------------------------------------------------------------------------
	/** @brief sigleton-instanceを参照する。
	        まだsingleton-instanceがないなら、default-constructorで構築する。
	    @return singleton-instanceへの参照。
	 */
	public: static t_value* get()
	{
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief sigleton-instanceをdefault-constructorで構築する。
	        すでにsingleton-instanceがあるなら、構築は行わず既存のものを返す。
	    @return singleton-instanceへの参照。
	 */
	public: static t_value* construct()
	{
		return this_type::construct(boost::in_place());
	}

	/** @brief sigleton-instanceを構築する。
	        すでにsingleton-instanceがあるなら、構築は行わず既存のものを返す。
	    @param[in] i_constructor boost::in_place から取得した構築関数object。
	    @return singleton-instanceへのpointer。
	 */
	public: template< typename t_constructor >
	static t_value* construct(t_constructor const& i_constructor)
	{
		// sigleton-instance構築関数を一度だけ呼び出す。
		PSYQ_CALL_ONCE(
			this_type::construct_flag(),
			boost::bind(
				&construct_instance< t_constructor >, &i_constructor));

		// singleton-instanceを取得。
		PSYQ_ASSERT(NULL != this_type::pointer());
		return this_type::pointer();
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instanceを構築する。
	    @param[in] i_constructor boost::in_placeから取得した構築関数object。
	 */
	private: template< typename t_constructor >
	static void construct_instance(
		t_constructor const* const i_constructor)
	{
		this_type::instance().construct(this_type::pointer(), *i_constructor);
	}

	/** @brief singleton-instanceを構築したかどうかのflagを参照する。
	 */
	private: static PSYQ_ONCE_FLAG& construct_flag()
	{
		PSYQ_ONCE_FLAG_INIT(s_construct_flag);
		return s_construct_flag;
	}

	//-------------------------------------------------------------------------
	/** @brief singleton-instanceへのpointerを参照する。
	 */
	private: static t_value*& pointer()
	{
		static t_value* s_pointer(NULL);
		return s_pointer;
	}

	/** @brief singleton-instanceを保持する領域を参照する。
	        静的局所変数なので、構築は最初にこの関数が呼ばれた時点で行われる。
	        最初は必ずconstruct_instance()から呼ばれる。
	        破棄は、main()の終了後に、構築した順序の逆順で自動的に行われる。
	 */
	private: static typename this_type::instance_holder& instance()
	{
		static typename this_type::instance_holder s_instance;
		return s_instance;
	}
};

#endif // PSYQ_SIMPLE_SINGLETON_HPP_
