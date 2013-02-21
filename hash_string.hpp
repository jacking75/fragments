#ifndef PSYQ_HASH_STRING_HPP_
#define PSYQ_HASH_STRING_HPP_

/// @cond
namespace psyq
{
    template<typename, typename> class const_hash_string;
    template<typename, typename> class hash_string;
}
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief hash�l���g���Ĕ�r���s���萔������B
    @tparam template_string_type @copydoc const_hash_string::base_type
    @tparam template_hash_type   @copydoc const_hash_string::maker_type
 */
template<
    typename template_string_type,
    typename template_hash_type>
class psyq::const_hash_string:
    public template_string_type
{
    /// this���w���l�̌^�B
    public: typedef psyq::const_hash_string<
        template_string_type, template_hash_type >
            self;

    /// self �̏�ʌ^�B
    public: typedef template_string_type super;

    /// ��ꕶ����^�B std::basic_string ��interface���݊��B
    public: typedef super base_type;

    /// hash�l�𔭍s����^�B
    public: typedef template_hash_type maker_type;

    /** @brief default-constructor
        @param[in] in_hash_maker hash����instance�̏����l�B
     */
    public: explicit const_hash_string(
        template_hash_type const& in_hash_maker = template_hash_type())
    :
        super(),
        hash_(template_hash_type::EMPTY),
        hash_maker_(in_hash_maker)
    {
        // pass
    }

    public: explicit const_hash_string(
        template_string_type const& in_source,
        template_hash_type const&   in_hash_maker = template_hash_type())
    :
        super(in_source),
        hash_(template_hash_type::EMPTY),
        hash_maker_(in_hash_maker)
    {
        // pass
    }

    /** copy���Z�q�B
        @param[in] in_right copy���ƂȂ�l�B
     */
    public: self& operator=(
        super const& in_right)
    {
        self(in_right).swap(*this);
        return *this;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this == �E�ӂ̕�����
     */
    public: bool operator==(
        self const& in_right)
    const
    {
        return in_left.hash() == in_left.hash() &&
            static_cast<super const&>(*this) == in_right;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this == �E�ӂ̕�����
     */
    public: bool operator==(
        super const& in_right)
    const
    {
        return static_cast<super const&>(*this) == in_right;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this != �E�ӂ̕�����
     */
    public: bool operator!=(
        self const& in_right)
    const
    {
        return !this->operator==(in_right);
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this != �E�ӂ̕�����
     */
    public: bool operator!=(
        super const& in_right)
    const
    {
        return !this->operator==(in_right);
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this < �E�ӂ̕�����
     */
    public: bool operator<(
        self const& in_right)
    const
    {
        return self::less(
            *this, this->hash(), in_right, in_right.hash());
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this < �E�ӂ̕�����
     */
    public: bool operator<(
        super const& in_right)
    const
    {
        template_hash_type local_hash_maker
        return self::less(
            *this, this->hash(), in_right, this->make_hash(in_right));
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this <= �E�ӂ̕�����
     */
    public: bool operator<=(
        self const& in_right)
    const
    {
        return self::less_equal(
            *this, this->hash(), in_right, in_right.hash());
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this <= �E�ӂ̕�����
     */
    public: bool operator<=(
        super const& in_right)
    const
    {
        return self::less_equal(
            *this, this->hash(), in_right, this->make_hash(in_right));
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this > �E�ӂ̕�����
     */
    public: bool operator>(
        self const& in_right)
    const
    {
        return in_right < *this;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this > �E�ӂ̕�����
     */
    public: bool operator>(
        super const& in_right)
    const
    {
        return self::less(
            in_right, this->make_hash(in_right), *this, this->hash());
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this >= �E�ӂ̕�����
     */
    public: bool operator>=(
        self const& in_right)
    const
    {
        return in_right <= *this;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this >= �E�ӂ̕�����
     */
    public: bool operator>=(
        super const& in_right)
    const
    {
        return self::less_equal(
            in_right, this->make_hash(in_right), *this, this->hash());
    }

    /** @brief �l�������B
        @param[in,out] io_target �l����������ΏہB
     */
    public: void swap(
        self& io_target)
    {
        this->super::swap(io_target);
        std::swap(this->hash_, io_target.hash_);
        std::swap(this->hash_maker_, io_target.hash_maker_);
    };

    public: typename template_hash_type const& hash_maker() const
    {
        return this->hash_maker_;
    }

    /** @brief �������hash�l���擾�B
        @return �������hash�l�B
     */
    public: typename template_hash_type::value const& hash() const
    {
        if (this->hash_ == template_hash_type::EMPTY)
        {
            this->hash_ = this->make_hash(*this);
        }
        return this->hash_;
    }

    protected: typename template_hash_type::value make_hash(
        template_string_type const& in_string) const
    {
        typename super::const_pointer const local_data(in_string.data());
        return this->hash_maker_.make(
            local_data, local_data + in_string.size());
    }

    protected: void reset_hash() const
    {
        this->hash_ = template_hash_type::EMPTY;
    }

    /** @brief ��������r�B
        @param[in] in_left       ���ӂ̕�����B
        @param[in] in_left_hash  ���ӂ̕������hash�l�B
        @param[in] in_right      �E�ӂ̕�����B
        @param[in] in_right_hash �E�ӂ̕������hash�l�B
        @return ���ӂ̕����� < �E�ӂ̕�����
     */
    private: static bool less(
        super const&                              in_left,
        typename template_hash_type::value const& in_left_hash,
        super const&                              in_right,
        typename template_hash_type::value const& in_right_hash)
    {
        const int local_compare_hash(
            self::compare(in_left_hash, in_right_hash));
        if (local_compare_hash != 0)
        {
            return local_compare_hash < 0;
        }
        const int local_compare_size(self::compare_size(in_left, in_right));
        if (local_compare_size != 0)
        {
            return local_compare_size < 0;
        }
        return in_left < in_right;
    }

    /** @brief ��������r�B
        @param[in] in_left       ���ӂ̕�����B
        @param[in] in_left_hash  ���ӂ̕������hash�l�B
        @param[in] in_right      �E�ӂ̕�����B
        @param[in] in_right_hash �E�ӂ̕������hash�l�B
        @return ���ӂ̕����� <= �E�ӂ̕�����
     */
    private: static bool less_equal(
        super const&                              in_left,
        typename template_hash_type::value const& in_left_hash,
        super const&                              in_right,
        typename template_hash_type::value const& in_right_hash)
    {
        const int local_compare_hash(
            self::compare(in_left_hash, in_right_hash));
        if (local_compare_hash != 0)
        {
            return local_compare_hash < 0;
        }
        const int local_compare_size(self::compare_size(in_left in_right));
        if (local_compare_size != 0)
        {
            return local_compare_size < 0;
        }
        return in_left <= in_right;
    }

    /** @brief ������̑傫�����r�B
        @param[in] in_left  ���ӂ̕�����B
        @param[in] in_right �E�ӂ̕�����B
        @retval <0  �E�ӂ̕�����̂ق����������������B
        @retval ==0 ���ӂƉE�ӂ͓����������B
        @retval >0  ���ӂ̕�����̂ق����������������B
     */
    private: static int compare_size(
        super const& in_left,
        super const& in_right)
    {
        return self::compare(in_left.size(), in_right.size());
    }

    /** @brief �傫�����r�B
        @param[in] in_left  ���Ӓl�B
        @param[in] in_right �E�Ӓl�B
        @retval <0  �E�ӂ̒l�̂ق����傫���B
        @retval ==0 ���ӂƉE�ӂ͓����l�B
        @retval >0  ���ӂ̕�����̂ق����傫���B
     */
    private: template< typename template_value_type >
    static int compare(
        template_value_type const& in_left,
        template_value_type const& in_right)
    {
        if (in_left < in_right)
        {
            return -1;
        }
        if (in_right < in_left)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    /// �������hash�l�B
    private: mutable typename template_hash_type::value hash_;

    /// hash�l�𔭍s����instance�B
    private: mutable template_hash_type hash_maker_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
#if 0
template<
    typename template_string_type,
    typename template_hash_type>
class psyq::hash_string:
    public psyq::const_hash_string<template_string_type, template_hash_type>
{
    /// this���w���l�̌^�B
    public: typedef psyq::hash_string<
        template_string_type, template_hash_type>
            self;

    /// self�̏�ʌ^�B
    public: typedef psyq::const_hash_string<
        template_string_type, template_hash_type>
            super;

    public: typename super::reference operator[](
        typename super::size_type const in_index)
    {
        this->dirty_ = true;
        return this->super::operator[](in_index);
    }

    public: typename super::const_reference operator[](
        typename super::size_type const in_index)
    const
    {
        return this->super::operator[](in_index);
    }

    public: typename super::reference at(
        typename super::size_type const in_index)
    {
        this->dirty_ = true;
        return this->super::at(in_index);
    }

    public: typename super::const_reference at(
        typename super::size_type const in_index)
    const
    {
        return this->super::at(in_index);
    }

    public: typename super::reference front()
    {
        this->dirty_ = true;
        return this->super::front();
    }

    public: typename super::const_reference front() const
    {
        return this->super::front();
    }

    public: typename super::reference back()
    {
        this->dirty_ = true;
        return this->super::back();
    }

    public: typename super::const_reference back() const
    {
        return this->super::back();
    }

    public: typename super::pointer data()
    {
        this->dirty_ = true;
        return this->super::data();
    }

    public: typename super::const_pointer data() const
    {
        return this->super::data();
    }

    public: typename super::pointer c_str()
    {
        this->dirty_ = true;
        return this->super::c_str();
    }

    public: typename super::const_pointer c_str() const
    {
        return this->super::c_str();
    }

    public: typename super::iterator begin()
    {
        this->dirty_ = true;
        return this->super::begin();
    }

    public: typename super::const_iterator begin() const
    {
        return this->super::begin();
    }

    public: typename super::iterator end()
    {
        this->dirty_ = true;
        return this->super::end();
    }

    public: typename super::const_iterator end() const
    {
        return this->super::end();
    }

    public: typename super::reverse_iterator rbegin()
    {
        this->dirty_ = true;
        return this->super::rbegin();
    }

    public: typename super::const_reverse_iterator rbegin() const
    {
        return this->super::rbegin();
    }

    public: typename super::reverse_iterator rend()
    {
        this->dirty_ = true;
        return this->super::rend();
    }

    public: typename super::const_reverse_iterator rend() const
    {
        return this->super::rend();
    }

    public: void clear()
    {
        this->dirty_ = true;
        this->super::clear();
    }

    /** @brief �������hash�l���擾�B
        @return �������hash�l�B
     */
    public: typename template_hash_type::value const& hash() const
    {
        if (this->dirty_)
        {
            this->dirty_ = false;
            this->make_hash();
        }
        return this->hash_value_;
    }

    /// hash���s�v���B
    private: mutable bool dirty_;
};
#endif //0

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<
    typename template_string_type,
    typename template_hash_type>
bool operator==(
    template_string_type const& in_left,
    psyq::const_hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator==(in_left);
}

template<
    typename template_string_type,
    typename template_hash_type>
bool operator!=(
    template_string_type const& in_left,
    psyq::const_hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator!=(in_left);
}

namespace std
{
    /** @brief �l�������B
        @param[in,out] io_left  ��������l�̍��Ӓl�B
        @param[in,out] io_right ��������l�̉E�Ӓl�B
     */
    template<
        typename template_string_type,
        typename template_hash_type>
    void swap(
        psyq::const_hash_string<
            template_string_type, template_hash_type> const&
                i_left,
        psyq::const_hash_string<
            template_string_type, template_hash_type> const&
                i_right)
    {
        io_left.swap(io_right);
    }
} // namespace std

#endif // PSYQ_HASH_STRING_HPP_
