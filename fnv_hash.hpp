#ifndef PSYQ_FNV_HASH_HPP_
#define PSYQ_FNV_HASH_HPP_

namespace psyq
{
    /// ���̖��O��Ԃ�user������access����̂͋֎~�B
    namespace closed
    {
        /// @cond
        template< typename, typename > class fnv_hash;
        /// @endcond

        class fnv1_maker;
        class fnv1a_maker;
        class fnv_traits32;
        class fnv_traits64;
    }

    /// 32bit FNV-1 hash�֐�object
    typedef psyq::closed::fnv_hash<
        psyq::closed::fnv1_maker, psyq::closed::fnv_traits32>
            fnv1_hash32;

    /// 64bit FNV-1 hash�֐�object
    typedef psyq::closed::fnv_hash<
        psyq::closed::fnv1_maker, psyq::closed::fnv_traits64>
            fnv1_hash64;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief FNV-hash
    @tparam template_hash_policy @copydoc fnv_hash::policy
    @tparam template_hash_traits @copydoc fnv_hash::traits
 */
template< typename template_hash_policy, typename template_hash_traits >
class psyq::closed::fnv_hash
{
    /// this�̎w���l�̌^�B
    public: typedef psyq::closed::fnv_hash<
        template_hash_policy, template_hash_traits>
            self;

    //-------------------------------------------------------------------------
    /// FNV-hash�̐���policy�B
    public: typedef template_hash_policy policy;

    /// FNV-hash�̌^�����B
    public: typedef template_hash_traits traits;

    /// FNV-hash�l�̌^�B
    public: typedef typename template_hash_traits::value value;

    /// �g�p�����hash�l�B
    public: static typename template_hash_traits::value const EMPTY =
        self::traits::EMPTY;

    /// �g�p����FNV-hash�f���B
    public: static typename template_hash_traits::value const PRIME =
        self::traits::PRIME;

    //-------------------------------------------------------------------------
    /** @brief �������hash�l�𐶐��B
        @tparam template_char_type �����̌^�B
        @param[in] in_string NULL�����ŏI�����镶����̐擪�ʒu�B
        @param[in] in_offset FNV-hash�J�n�l�B
        @param[in] in_prime  FNV-hash�f���B
        @return �������hash�l�B
     */
    public: template<typename template_char_type>
    static typename self::value make(
        template_char_type const* const in_string,
        typename self::value const      in_offset = self::EMPTY,
        typename self::value const      in_prime = self::PRIME)
    {
        typename self::value local_hash(in_offset);
        if (in_string != NULL)
        {
            for (template_char_type const* i(in_string); *i != 0; ++i)
            {
                local_hash = template_hash_policy::make(
                    i, i + 1, local_hash, in_prime);
            }
        }
        return local_hash;
    }

    /** @brief �z���hash�l�𐶐��B
        @tparam template_value_type �z��̗v�f�̌^�B
        @param[in] in_begin  �z��̐擪�ʒu�B
        @param[in] in_end    �z��̖����ʒu�B
        @param[in] in_offset FNV-hash�J�n�l�B
        @param[in] in_prime  FNV-hash�f���B
     */
    public: template<typename template_value_type>
    static typename self::value make(
        template_value_type const* const in_begin,
        template_value_type const* const in_end,
        typename self::value const       in_offset = self::EMPTY,
        typename self::value const       in_prime = self::PRIME)
    {
        return template_hash_policy::make(
            in_begin, in_end, in_offset, in_prime);
    }

    /** @brief �z���hash�l�𐶐��B
        @tparam template_iterator_type �z��̗v�f���w�������q�̌^�B
        @param[in] in_begin  ������̐擪�ʒu�B
        @param[in] in_end    ������̖����ʒu�B
        @param[in] in_offset FNV-hash�J�n�l�B
        @param[in] in_prime  FNV-hash�f���B
     */
    public: template<typename template_iterator_type>
    static typename self::value make(
        template_iterator_type const& in_begin,
        template_iterator_type const& in_end,
        typename self::value const    in_offset = self::EMPTY,
        typename self::value const    in_prime = self::PRIME)
    {
        typename self::value local_hash(in_offset);
        for (template_iterator_type i(in_begin); in_end != i; ++i)
        {
            local_hash = template_hash_policy::make(
                &(*i), &(*i) + 1, local_hash, in_prime);
        }
        return local_hash;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief FNV-1�ŁAhash�l�𐶐��B

    http://www.radiumsoftware.com/0605.html#060526
    http://d.hatena.ne.jp/jonosuke/20100406/p1
 */
class psyq::closed::fnv1_maker
{
    //-------------------------------------------------------------------------
    /** @brief byte�z���hash�l�𐶐��B
        @param[in] in_begin  byte�z��̐擪�ʒu�B
        @param[in] in_end    byte�z��̖����ʒu�B
        @param[in] in_offset hash�J�n�l�B
        @param[in] in_prime  FNV-hash�f���B
     */
    public: template<typename template_value_type>
    static template_value_type make(
        void const* const          in_begin,
        void const* const          in_end,
        template_value_type const& in_offset,
        template_value_type const& in_prime)
    {
        template_value_type local_hash(in_offset);
        for (
            char const* i(static_cast<char const*>(in_begin));
            i < in_end;
            ++i)
        {
            local_hash = (local_hash * in_prime) ^ *i;
        }
        return local_hash;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief FNV-1a�ŁAhash�l�𐶐��B

    http://www.radiumsoftware.com/0605.html#060526
    http://d.hatena.ne.jp/jonosuke/20100406/p1
 */
class psyq::closed::fnv1a_maker
{
    //-------------------------------------------------------------------------
    /** @brief byte�z���hash�l�𐶐��B
        @param[in] in_begin  byte�z��̐擪�ʒu�B
        @param[in] in_end    byte�z��̖����ʒu�B
        @param[in] in_offset FNV-hash�J�n�l�B
        @param[in] in_prime  FNV-hash�f���B
     */
    public: template<typename template_value_type>
    static template_value_type make(
        void const* const          in_begin,
        void const* const          in_end,
        template_value_type const& in_offset,
        template_value_type const& in_prime)
    {
        template_value_type local_hash(in_offset);
        for (
            char const* i(static_cast<char const*>(in_begin));
            i < in_end;
            ++i)
        {
            local_hash = (local_hash ^ *i) * in_prime;
        }
        return local_hash;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// 32bit��FNV-hash�֐��̌^�����B
class psyq::closed::fnv_traits32
{
    /// hash�l�̌^�B
    public: typedef boost::uint32_t value;

    /// ��hash�l�B
    public: static value const EMPTY = 0x811c9dc5;

    /// FNV-hash�f���B
    public: static value const PRIME = 0x1000193;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// 64bit��FNV-hash�֐��̌^�����B
class psyq::closed::fnv_traits64
{
    /// hash�l�̌^�B
    public: typedef boost::uint64_t value;

    /// ��hash�l�B
    public: static value const EMPTY = 0xcbf29ce484222325ULL;

    /// FNV-hash�f���B
    public: static value const PRIME = 0x100000001b3ULL;
};

#endif // PSYQ_FNV_HASH_HPP_
