#ifndef PSYQ_ENUM_HPP_
#define PSYQ_ENUM_HPP_

#include <boost/noncopyable.hpp>
#include <boost/preprocessor.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>

namespace psyq
{
	template< typename, std::size_t > class _enumeration;
	template< typename, typename > class _enum_item;
};

//-----------------------------------------------------------------------------
/** @brief �񋓎q���`�B
    @param d_enum  �񋓎q�̌^���B
    @param d_name  �񋓎q�������O������̌^�B
    @param d_value �񋓎q�����l�̌^�B�l�������Ȃ��ꍇ�́Avoid���w�肷��B
    @param d_items
        �񋓎q�̖��O�ƒl���`�����z��BBOOST_PP_SEQ�`���ŋL�q����B
        PSYQ_ENUM_VALUE��PSYQ_ENUM_NAME���g���ƁA�L�q���ȗ����ł���B
        boost�̎d�l�ɂ��A��`�ł���񋓎q�̐���BOOST_PP_LIMIT_SEQ���ő�B
        boost-1.47.0���ƁABOOST_PP_LIMIT_SEQ��256�ƂȂ��Ă���B
 */
#define PSYQ_ENUM(d_enum, d_name, d_value, d_items)\
	class d_enum: private boost::noncopyable\
	{\
		private:\
		d_enum();\
		typedef psyq::_enumeration<\
			psyq::_enum_item< d_name, d_value >,\
			BOOST_PP_SEQ_SIZE(d_items) >\
				PSYQ_ENUM_basic_enumeration;\
		class PSYQ_ENUM_enumeration: public PSYQ_ENUM_basic_enumeration\
		{\
			public:\
			BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_items),\
				PSYQ_PRIVATE_ENUM_VALUE_GETTER,\
				d_items)\
			PSYQ_ENUM_enumeration(): PSYQ_ENUM_basic_enumeration()\
			{\
				BOOST_PP_REPEAT(\
					BOOST_PP_SEQ_SIZE(d_items),\
					PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT,\
					d_items)\
			}\
		};\
		class PSYQ_ENUM_ordinal: private boost::noncopyable\
		{\
			public:\
			BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_items),\
				PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE,\
				d_items)\
			private: PSYQ_ENUM_ordinal();\
		};\
		public:\
		typedef PSYQ_ENUM_ordinal ordinal;\
		typedef PSYQ_ENUM_enumeration enumeration;\
		typedef PSYQ_ENUM_basic_enumeration::item item;\
		static PSYQ_ENUM_basic_enumeration::item::ordinal const SIZE =\
			PSYQ_ENUM_basic_enumeration::SIZE;\
	};

//-----------------------------------------------------------------------------
/** @brief PSYQ_ENUM��d_items�����ŁA�񋓎q�̒�`�Ɏg���B
    @param d_name  �񋓎q�̖��O�B
    @param d_value �񋓎q�̒l�B
 */
#define PSYQ_ENUM_VALUE(d_name, d_value) ((d_name)(d_value))

/** @brief PSYQ_ENUM��d_items�����ŁA�l�̂Ȃ��񋓎q�̒�`�Ɏg���B
    @param d_name �񋓎q�̖��O�B
 */
#define PSYQ_ENUM_NAME(d_name) ((d_name))

/** @brief PSYQ_ENUM��d_items�����ŁAFNV-1 hash�l�����񋓎q�̒�`�Ɏg���B
    @param d_name �񋓎q�̖��O�B
 */
#define PSYQ_ENUM_NAME_FNV1_HASH32(d_name)\
	((d_name)(psyq::fnv1_hash32::generate(BOOST_PP_STRINGIZE(d_name))))

//-----------------------------------------------------------------------------
/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT(d_z, d_ordinal, d_items)\
	new(this->PSYQ_ENUM_basic_enumeration::get(d_ordinal)) item(\
		d_ordinal,\
		BOOST_PP_STRINGIZE(\
			BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_items)))\
		BOOST_PP_IF(\
			BOOST_PP_LESS(\
				1, BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(d_ordinal, d_items))),\
			PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT_PROPERTY,\
			BOOST_PP_TUPLE_EAT(2))(d_ordinal, d_items));

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT_PROPERTY(d_ordinal, d_items)\
	, BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(d_ordinal, d_items))

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_VALUE_GETTER(d_z, d_ordinal, d_items)\
	PSYQ_ENUM_basic_enumeration::item* const\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_items))() const\
	{\
		return this->PSYQ_ENUM_basic_enumeration::get(d_ordinal);\
	}

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE(d_z, d_ordinal, d_items)\
	static PSYQ_ENUM_basic_enumeration::item::ordinal const\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_items)) = d_ordinal;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
namespace psyq {
template< typename t_item, std::size_t t_size >
class _enumeration:
	private boost::noncopyable
{
	typedef _enumeration< t_item, t_size > this_type;

//.............................................................................
public:
	typedef t_item item;

	//-------------------------------------------------------------------------
	~_enumeration()
	{
		for (typename t_item::ordinal i = t_size; 0 < i;)
		{
			--i;
			this->get(i)->~t_item();
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q���擾�B
	    @param[in] i_ordinal �擾����񋓎q�̏����B
	    @retrun �񋓎q�ւ�pointer�B�������A�Ή�����񋓎q���Ȃ��ꍇ��NULL�B
	 */
	t_item const* operator()(typename t_item::ordinal const i_ordinal)
	const
	{
		return i_ordinal < t_size? this->get(i_ordinal): NULL;
	}

	/** @brief �񋓎q���擾�B
	    @param[in] i_name �擾����񋓎q�̖��O�B
	    @retrun �񋓎q�ւ�pointer�B�������A�Ή�����񋓎q���Ȃ��ꍇ��NULL�B
	 */
	t_item const* operator()(typename t_item::name const& i_name)
	const
	{
		t_item const* const a_items(this->get(0));
		for (typename t_item::ordinal i = 0; i < t_size; ++i)
		{
			if (i_name == a_items[i].name)
			{
				return &a_items[i];
			}
		}
		return NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q���Q�ƁB
	    @param[in] i_ordinal �Q�Ƃ���񋓎q�̏����B
	    @retrun �񋓎q�ւ̎Q�ƁB
	 */
	t_item const& operator[](typename t_item::ordinal const i_ordinal)
	const
	{
		return *(this->get(i_ordinal));
	}

//.............................................................................
protected:
	//-------------------------------------------------------------------------
	_enumeration()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	t_item* get(typename t_item::ordinal const i_index) const
	{
		PSYQ_ASSERT(i_index < t_size);
		return const_cast< t_item* >(
			reinterpret_cast< t_item const* >(&this->storage_)) + i_index;
	}

//.............................................................................
public:
	/// �ێ����Ă���񋓎q�̐��B
	static typename t_item::ordinal const SIZE = t_size;

private:
	typename boost::aligned_storage<
		sizeof(t_item[t_size]),
		boost::alignment_of< t_item[t_size] >::value >::type
			storage_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_name, typename t_value >
class _enum_item
{
	typedef psyq::_enum_item< t_name, t_value > this_type;

//.............................................................................
public:
	typedef std::size_t ordinal;
	typedef t_name name;
	typedef t_value value_type;

	//-------------------------------------------------------------------------
	_enum_item(
		typename this_type::ordinal const i_ordinal,
		t_name const&                     i_name,
		t_value const&                    i_value = t_value()):
	name_(i_name),
	value_(i_value),
	ordinal_(i_ordinal)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	typename this_type::ordinal get_ordinal() const
	{
		return this->ordinal_;
	}

	t_name const& get_name() const
	{
		return this->name_;
	}

	t_value const& get_value() const
	{
		return this->value_;
	}

//.............................................................................
private:
	t_name                      name_;    ///< �񋓎q�̖��O�B
	t_value                     value_;   ///< �񋓎q�̒l�B
	typename this_type::ordinal ordinal_; ///< �񋓎q�̏����B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_name >
class _enum_item< t_name, void >
{
	typedef psyq::_enum_item< t_name, void > this_type;

//.............................................................................
public:
	typedef std::size_t ordinal;
	typedef t_name name;

	//-------------------------------------------------------------------------
	_enum_item(
		typename this_type::ordinal const i_ordinal,
		typename this_type::name const&   i_name):
	name_(i_name),
	ordinal_(i_ordinal)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	typename this_type::ordinal get_ordinal() const
	{
		return this->ordinal_;
	}

	t_name const& get_name() const
	{
		return this->name_;
	}

//.............................................................................
private:
	t_name                      name_;    ///< �񋓎q�̖��O�B
	typename this_type::ordinal ordinal_; ///< �񋓎q�̏����B
};
} // namespace psyq

#endif // PSYQ_ENUM_HPP_
