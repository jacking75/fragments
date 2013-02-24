#ifndef PSYQ_STRING_HASH_HPP_
#define PSYQ_STRING_HASH_HPP_

namespace psyq
{
    /// @cond
    template<typename, typename> class string_hash;
    /// @endcond
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ������ƕ������hash�l���ꏏ�ɕێ�����B

    ��r���Z�q�� compare() �Ŕ�r���s���Ƃ��́A
    �ŏ���hash�l�̔�r��������ŁA��������r����B

    @tparam template_string_type @copydoc string_hash::string
    @tparam template_hash_type   @copydoc string_hash::hasher
 */
template<
    typename template_string_type,
    typename template_hash_type>
class psyq::string_hash
{
    /// this���w���l�̌^�B
    typedef psyq::string_hash<template_string_type, template_hash_type> self;

    /** @brief ������̌^�B

        ������̒����𓾂邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        self::string::size_type self::string::size() const
        @endcode

        ��������r���邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        int self::string::compare(self::string const&) const
        @endcode

        ��������������邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        std::swap(self::string&, self::string&)
        @endcode
     */
    public: typedef template_string_type string;

    /** hash�����֐�object�̌^�B

        hash�l�𐶐����邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        self::hasher::result_type self::hasher::operator()(self::string const&);
        @endcode
     */
    public: typedef template_hash_type hasher;

    //-------------------------------------------------------------------------
    /** @brief default-constructor
        @param[in] in_string ������̏����l�B
        @param[in] in_hasher hash�����֐�object�̏����l�B
     */
    public: string_hash(
        typename self::string const& in_string = self::string(),
        typename self::hasher const& in_hasher = self::hasher()):
    string_(in_string),
    hasher_(in_hasher)
    {
        this->rehash();
    }

    //-------------------------------------------------------------------------
    /** @brief �l�������B
        @param[in,out] io_target ��������l�B
     */
    public: void swap(
        self& io_target)
    {
        std::swap(this->string_, io_target.string_);
        std::swap(this->hash_, io_target.hash_);
        std::swap(this->hasher_, io_target.hasher_);
    }

    //-------------------------------------------------------------------------
    /** @brief ��������擾�B
        @return �ێ����Ă��镶����B
     */
    public: typename self::string const& get_string() const
    {
        return this->string_;
    }

    /** @brief �������copy�B

        �ێ����Ă��镶�����copy����B
        @param[in] in_source copy���̕�����B
     */
    public: void copy_string(
        typename self::string const& in_source)
    {
        this->string_ = in_source;
        this->rehash();
    }

    /** @brief ������������B

        �ێ����Ă��镶����ƌ�������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap_string(
        typename self::string& io_target)
    {
        this->string_.swap(io_target);
        this->rehash();
    }

    /** @brief �������hash�l���擾�B
        @return �ێ����Ă��镶�����hash�l�B
     */
    public: typename typename self::hasher::result_type const& get_hash() const
    {
        return this->hash_;
    }

    /** @brief hash�����֐�object���擾�B
        @return �ێ����Ă���hash�����֐�object�B
     */
    public: typename self::hasher const& get_hasher() const
    {
        return this->hasher_;
    }

    //-------------------------------------------------------------------------
    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this == �E��
     */
    public: bool operator==(
        self const& in_right)
    const
    {
        return
            this->get_hash() == in_right.get_hash() &&
            this->get_string() == in_right.get_string();
    }

    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this != �E��
     */
    public: bool operator!=(
        self const& in_right)
    const
    {
        return !this->operator==(in_right);
    }

    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this < �E��
     */
    public: bool operator<(
        self const& in_right)
    const
    {
        return this->compare(in_right) < 0;
    }

    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this <= �E��
     */
    public: bool operator<=(
        self const& in_right)
    const
    {
        return this->compare(in_right) <= 0;
    }

    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this > �E��
     */
    public: bool operator>(
        self const& in_right)
    const
    {
        return 0 < this->compare(in_right);
    }

    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @return *this >= �E��
     */
    public: bool operator>=(
        self const& in_right)
    const
    {
        return 0 <= this->compare(in_right);
    }

    //-------------------------------------------------------------------------
    /** @brief �l���r�B

        *this�����Ӓl�Ƃ��āA�l���r����B

        @param[in] in_right �E�Ӓl�B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        self const& in_right)
    const
    {
        // hash�l���r�B
        if (this->get_hash() < in_right.get_hash())
        {
            return -1;
        }
        if (in_right.get_hash() < this->get_hash())
        {
            return 1;
        }

        // ��������r�B
        return this->get_string().compare(in_right.get_string());
    }

    //-------------------------------------------------------------------------
    private: void rehash()
    {
        this->hash_ = this->hasher_.make(
            this->get_string().data(),
            this->get_string().data() + this->string_.size());
    }

    //-------------------------------------------------------------------------
    private: typename self::string              string_; ///< ������B
    private: typename self::hasher::result_type hash_;   ///< �������hash�l�B
    private: typename self::hasher              hasher_; ///< hash�����֐�object�B
};

namespace std
{
    /** @brief �l�������B
        @tparam template_string_type @copydoc psyq::strin_hash::string
        @tparam template_hash_type   @copydoc psyq::string_hash::hasher
        @param[in,out] io_left  ��������l�B
        @param[in,out] io_right ��������l�B
     */
    template<
        typename template_string_type,
        typename template_hash_type>
    void swap(
        psyq::string_hash<
            template_string_type, template_hash_type> const&
                i_left,
        psyq::string_hash<
            template_string_type, template_hash_type> const&
                i_right)
    {
        io_left.swap(io_right);
    }
} // namespace std

#endif // PSYQ_STRING_HASH_HPP_
