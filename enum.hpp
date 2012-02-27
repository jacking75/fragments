#ifndef PSYQ_ENUM_HPP_
#define PSYQ_ENUM_HPP_

#include <boost/preprocessor.hpp>

namespace psyq
{
	template< typename, std::size_t > class _enum_container;
	template< typename, typename > class _enum_value;
};

//-----------------------------------------------------------------------------
/** @brief �񋓎q���`�B
    @param d_enum_name �񋓎q�̌^���B
    @param d_name_type �񋓎q�������O������̌^�B
    @param d_property_type
        �񋓎q���������l�̌^�B�񋓎q�������l�������Ȃ��Ȃ�Avoid���w�肷��B
    @param d_values
        �񋓎q�̖��O�Ƒ����l���`�����z��BBOOST_PP_SEQ�`���ŋL�q����B
        PSYQ_ENUM_VALUE��PSYQ_ENUM_NAME���g���ƁA�L�q���ȗ����ł���B
        boost�̎d�l�ɂ��A��`�ł���񋓎q�̐���BOOST_PP_LIMIT_SEQ���ő�B
        boost-1.47.0���ƁABOOST_PP_LIMIT_SEQ��256�ƂȂ��Ă���B
 */
#define PSYQ_ENUM(d_enum_name, d_name_type, d_property_type, d_values)\
	class d_enum_name: private boost::noncopyable\
	{\
		private:\
		d_enum_name() {}\
		typedef psyq::_enum_container<\
			psyq::_enum_value< d_name_type, d_property_type >,\
			BOOST_PP_SEQ_SIZE(d_values) >\
				PSYQ_ENUM_container;\
		class PSYQ_ENUM_values: public PSYQ_ENUM_container\
		{\
			public:\
			BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_values),\
				PSYQ_PRIVATE_ENUM_VALUE_GETTER,\
				d_values)\
			PSYQ_ENUM_values(): PSYQ_ENUM_container()\
			{\
				BOOST_PP_REPEAT(\
					BOOST_PP_SEQ_SIZE(d_values),\
					PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT,\
					d_values)\
			}\
		};\
		class PSYQ_ENUM_ordinal: private boost::noncopyable\
		{\
			public:\
			BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_values),\
				PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE,\
				d_values)\
			private: PSYQ_ENUM_ordinal();\
		};\
		public:\
		typedef PSYQ_ENUM_ordinal ordinal;\
		typedef PSYQ_ENUM_values values;\
		typedef PSYQ_ENUM_container::pointer pointer;\
		typedef PSYQ_ENUM_container::reference reference;\
		static PSYQ_ENUM_container::value_type::ordinal_type const\
			size = PSYQ_ENUM_container::size;\
	};

//-----------------------------------------------------------------------------
/** @brief PSYQ_ENUM��d_values�����ŁA�񋓎q�̒�`�Ɏg���B
    @param d_name     �񋓎q�̖��O�B
    @param d_property �񋓎q�̑����l�B
 */
#define PSYQ_ENUM_VALUE(d_name, d_property) ((d_name)(d_property))

/** @brief PSYQ_ENUM��d_values�����ŁA�����l�̂Ȃ��񋓎q�̒�`�Ɏg���B
    @param d_name �񋓎q�̖��O�B
 */
#define PSYQ_ENUM_NAME(d_name) ((d_name))

//-----------------------------------------------------------------------------
/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT(d_z, d_ordinal, d_values)\
	new(this->PSYQ_ENUM_container::get(d_ordinal)) value_type(\
		d_ordinal,\
		BOOST_PP_STRINGIZE(\
			BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_values)))\
		BOOST_PP_IF(\
			BOOST_PP_LESS(\
				1, BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(d_ordinal, d_values))),\
			PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT_PROPERTY,\
			BOOST_PP_TUPLE_EAT(2))(d_ordinal, d_values));

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT_PROPERTY(d_ordinal, d_values)\
	, BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(d_ordinal, d_values))

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_GETTER(d_z, d_ordinal, d_values)\
	PSYQ_ENUM_container::value_type* const\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_values))() const\
	{\
		return this->PSYQ_ENUM_container::get(d_ordinal);\
	}

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE(d_z, d_ordinal, d_values)\
	static PSYQ_ENUM_container::value_type::ordinal_type const\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_values)) = d_ordinal;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
namespace psyq {
template< typename t_value_type, std::size_t t_size >
class _enum_container:
	private boost::noncopyable
{
	typedef _enum_container< t_value_type, t_size > this_type;

	//.........................................................................
	public:
	typedef t_value_type value_type;
	typedef value_type const* pointer;
	typedef value_type const& reference;

	//-------------------------------------------------------------------------
	~_enum_container()
	{
		for (typename this_type::value_type::ordinal_type i = t_size; 0 < i;)
		{
			--i;
			this->get(i)->~value_type();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q���擾�B
	    @param[in] i_ordinal �擾����񋓎q�̏����B
	    @retrun �񋓎q�ւ�pointer�B�������A�Ή�����񋓎q���Ȃ��ꍇ��NULL�B
	 */
	typename this_type::pointer operator()(
		typename this_type::value_type::ordinal_type const i_ordinal)
		const
	{
		return i_ordinal < t_size? this->get(i_ordinal): NULL;
	}

	/** @brief �񋓎q���擾�B
	    @param[in] i_name �擾����񋓎q�̖��O�B
	    @retrun �񋓎q�ւ�pointer�B�������A�Ή�����񋓎q���Ȃ��ꍇ��NULL�B
	 */
	typename this_type::pointer operator()(
		typename this_type::value_type::name_type const& i_name)
		const
	{
		pointer const a_values(this->get(0));
		for (typename this_type::value_type::ordinal_type i = 0; i < t_size; ++i)
		{
			if (i_name == a_values[i].name)
			{
				return &a_values[i];
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q���Q�ƁB
	    @param[in] i_ordinal �Q�Ƃ���񋓎q�̏����B
	    @retrun �񋓎q�ւ̎Q�ƁB
	 */
	typename this_type::reference operator[](
		typename this_type::value_type::ordinal_type const i_ordinal)
		const
	{
		return *(this->get(i_ordinal));
	}

	//.........................................................................
	protected:
	//-------------------------------------------------------------------------
	_enum_container()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	value_type* get(
		typename this_type::value_type::ordinal_type const i_index)
		const
	{
		PSYQ_ASSERT(i_index < t_size);
		return const_cast< typename this_type::value_type* >(
			reinterpret_cast< value_type const* >(&this->_storage)) + i_index;
	}

	//.........................................................................
	public:
	/// �ێ����Ă���񋓎q�̐��B
	static typename this_type::value_type::ordinal_type const size = t_size;

	private:
	typename boost::aligned_storage<
		sizeof(value_type[t_size]),
		boost::alignment_of< value_type[t_size] >::value >::type
			_storage;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_name_type, typename t_property_type >
class _enum_value
{
	//.........................................................................
	public:
	typedef std::size_t ordinal_type;
	typedef t_name_type name_type;
	typedef t_property_type property_type;

	//-------------------------------------------------------------------------
	_enum_value(
		ordinal_type const   i_ordinal,
		name_type const&     i_name,
		property_type const& i_property = property_type()):
		ordinal(i_ordinal),
		name(i_name),
		property(i_property)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	ordinal_type  ordinal;  ///< �񋓎q�̏����B
	name_type     name;     ///< �񋓎q�̖��O�B
	property_type property; ///< �񋓎q�̑����B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_name_type >
class _enum_value< t_name_type, void >
{
	//.........................................................................
	public:
	typedef std::size_t ordinal_type;
	typedef t_name_type name_type;

	//-------------------------------------------------------------------------
	_enum_value(
		ordinal_type const i_ordinal,
		name_type const&   i_name):
		ordinal(i_ordinal),
		name(i_name)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	ordinal_type ordinal; ///< �񋓎q�̏����B
	name_type    name;    ///< �񋓎q�̖��O�B
};
} // namespace psyq

#endif // PSYQ_ENUM_HPP_
