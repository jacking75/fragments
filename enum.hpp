#ifndef PSYQ_ENUM_HPP_
#define PSYQ_ENUM_HPP_

#include <boost/noncopyable.hpp>
#include <boost/preprocessor.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>

namespace psyq
{
	template< typename, std::size_t > class _enum_set;
	template< typename, typename > class _enum_item;
	template< typename t_name > class _enum_item< t_name, void >;
};

//-----------------------------------------------------------------------------
/** @brief �񋓎q���`�B
    @param d_enum  �񋓌^�̖��O�B
    @param d_name  �񋓎q�̖��O�̌^�B
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
		private: typedef psyq::_enum_set<\
			psyq::_enum_item< d_name, d_value >,\
			BOOST_PP_SEQ_SIZE(d_items) >\
				PSYQ_ENUM_basic_set;\
		private: class PSYQ_ENUM_set: public PSYQ_ENUM_basic_set\
		{\
			public: PSYQ_ENUM_set(): PSYQ_ENUM_basic_set()\
			{\
				BOOST_PP_REPEAT(\
					BOOST_PP_SEQ_SIZE(d_items),\
					PSYQ_PRIVATE_ENUM_VALUE_CONSTRUCT,\
					d_items)\
			}\
			public: BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_items),\
				PSYQ_PRIVATE_ENUM_VALUE_GETTER,\
				d_items)\
		};\
		private: class PSYQ_ENUM_ordinal: private boost::noncopyable\
		{\
			public: BOOST_PP_REPEAT(\
				BOOST_PP_SEQ_SIZE(d_items),\
				PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE,\
				d_items)\
			private: PSYQ_ENUM_ordinal();\
		};\
		public: typedef PSYQ_ENUM_ordinal ordinal;\
		public: typedef PSYQ_ENUM_set enumeration;\
		public: typedef PSYQ_ENUM_basic_set::item item;\
		public: static PSYQ_ENUM_basic_set::item::ordinal const SIZE =\
			PSYQ_ENUM_basic_set::SIZE;\
		private: d_enum();\
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
	new(this->PSYQ_ENUM_basic_set::get(d_ordinal)) item(\
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
	PSYQ_ENUM_basic_set::item const&\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_items))() const\
	{\
		return *this->PSYQ_ENUM_basic_set::get(d_ordinal);\
	}

/// @brief PSYQ_ENUM�Ŏg����macro�Buser�͎g�p�֎~�B
#define PSYQ_PRIVATE_ENUM_ORDINAL_DEFINE(d_z, d_ordinal, d_items)\
	static PSYQ_ENUM_basic_set::item::ordinal const\
	BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(d_ordinal, d_items)) = d_ordinal;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񋓌^�̊��class�Buser�͎g�p�֎~�B
    @tparam t_item �񋓎q�̌^�B
    @tparam t_size �񋓎q�̐��B
 */
template< typename t_item, std::size_t t_size >
class psyq::_enum_set:
	private boost::noncopyable
{
	typedef psyq::_enum_set< t_item, t_size > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_item item;

	//-------------------------------------------------------------------------
	/// �ێ����Ă���񋓎q�̐��B
	public: static typename t_item::ordinal const SIZE = t_size;

	//-------------------------------------------------------------------------
	protected: _enum_set()
	{
		// pass
	}

	//-------------------------------------------------------------------------
	public: ~_enum_set()
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
	public: t_item const* operator()(
		typename t_item::ordinal const i_ordinal)
	const
	{
		return i_ordinal < t_size? this->get(i_ordinal): NULL;
	}

	/** @brief �񋓎q���擾�B
	    @param[in] i_name �擾����񋓎q�̖��O�B
	    @retrun �񋓎q�ւ�pointer�B�������A�Ή�����񋓎q���Ȃ��ꍇ��NULL�B
	 */
	public: t_item const* operator()(typename t_item::name const& i_name) const
	{
		t_item const* const a_items(this->get(0));
		for (typename t_item::ordinal i = 0; i < t_size; ++i)
		{
			if (a_items[i].get_name() == i_name)
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
	public: t_item const& operator[](
		typename t_item::ordinal const i_ordinal)
	const
	{
		return *(this->get(i_ordinal));
	}

	//-------------------------------------------------------------------------
	protected: t_item* get(typename t_item::ordinal const i_index) const
	{
		PSYQ_ASSERT(i_index < t_size);
		return const_cast< t_item* >(
			reinterpret_cast< t_item const* >(&this->storage_)) + i_index;
	}

	//-------------------------------------------------------------------------
	/// memory�̈�B
	private: typename boost::aligned_storage<
		sizeof(t_item[t_size]),
		boost::alignment_of< t_item[t_size] >::value >::type
			storage_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �񋓎q�Buser�͎g�p�֎~�B
    @tparam t_name     �񋓎q�̖��O�̌^�B
    @tparam t_property �񋓎q�����l�̌^�B
 */
template< typename t_name, typename t_property >
class psyq::_enum_item
{
	typedef psyq::_enum_item< t_name, t_property > this_type;

	//-------------------------------------------------------------------------
	public: typedef std::size_t ordinal;
	public: typedef t_name name;
	public: typedef t_property property;

	//-------------------------------------------------------------------------
	public: _enum_item(
		typename this_type::ordinal const i_ordinal,
		t_name const&                     i_name,
		t_property const&                 i_property = t_property()):
	name_(i_name),
	ordinal_(i_ordinal),
	property_(i_property)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q�̏������擾�B
	    @return �񋓎q�̏����B
	 */
	public: typename this_type::ordinal get_ordinal() const
	{
		return this->ordinal_;
	}

	/** @brief �񋓎q�̖��O���擾�B
	    @return �񋓎q�̖��O�B
	 */
	public: t_name const& get_name() const
	{
		return this->name_;
	}

	/** @brief �񋓎q�����l���擾�B
	    @return �񋓎q�����l�B
	 */
	public: t_property const& get_property() const
	{
		return this->property_;
	}

	//-------------------------------------------------------------------------
	private: t_name                      name_;     ///< �񋓎q�̖��O�B
	private: typename this_type::ordinal ordinal_;  ///< �񋓎q�̏����B
	private: t_property                  property_; ///< �񋓎q�̒l�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �l�������Ȃ��񋓎q�B
    @tparam t_name �񋓎q�̖��O�̌^�B
 */
template< typename t_name >
class psyq::_enum_item< t_name, void >
{
	typedef psyq::_enum_item< t_name, void > this_type;

	//-------------------------------------------------------------------------
	public: typedef std::size_t ordinal;
	public: typedef t_name name;

	//-------------------------------------------------------------------------
	public: _enum_item(
		typename this_type::ordinal const i_ordinal,
		typename this_type::name const&   i_name):
	name_(i_name),
	ordinal_(i_ordinal)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �񋓎q�̏������擾�B
	    @return �񋓎q�̏����B
	 */
	public: typename this_type::ordinal get_ordinal() const
	{
		return this->ordinal_;
	}

	/** @brief �񋓎q�̖��O���擾�B
	    @return �񋓎q�̖��O�B
	 */
	public: t_name const& get_name() const
	{
		return this->name_;
	}

	//-------------------------------------------------------------------------
	private: t_name                      name_;    ///< �񋓎q�̖��O�B
	private: typename this_type::ordinal ordinal_; ///< �񋓎q�̏����B
};

#endif // PSYQ_ENUM_HPP_
