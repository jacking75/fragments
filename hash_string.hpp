#ifndef PSYQ_HASH_STRING_HPP_
#define PSYQ_HASH_STRING_HPP_

namespace psyq
{
    /// @cond
    template<typename, typename> class hash_string;
    /// @endcond
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief hash�l���g���Ĕ�r���s��������B
    @tparam template_string_type @copydoc hash_string::super
    @tparam template_hash_type   @copydoc hash_string::hasher
 */
template<
    typename template_string_type,
    typename template_hash_type>
class psyq::hash_string:
    public template_string_type
{
    /// this���w���l�̌^�B
    public: typedef psyq::hash_string<
        template_string_type, template_hash_type >
            self;

    /// self �̏�ʌ^�B std::basic_string ��interface���݊��B
    public: typedef template_string_type super;

    /// hash�l�𐶐�����hash�����q�̌^�B
    public: typedef template_hash_type hasher;

    //-------------------------------------------------------------------------
    /** @brief default-constructor
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: explicit hash_string(
        typename super::allocator_type const& in_allocator =
            super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        // pass
    }

    /** @brief �ϊ�constructor
        @param[in] in_string �����l�Ɏg��������B
        @param[in] in_hasher hash�����q�̏����l�B
     */
    public: hash_string(
        super const&              in_string,
        template_hash_type const& in_hasher = template_hash_type())
    :
        super(in_string),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        // pass
    }

    /** @brief �ϊ�constructor
        @param[in] in_string    �����l�Ɏg��������B
        @param[in] in_offset    ������̊J�n�ʒu�B
        @param[in] in_count     �������B
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: template<typename template_other_string_type>
    hash_string(
        template_other_string_type const&     in_string,
        typename super::size_type const       in_offset = 0,
        typename super::size_type const       in_count = super::npos,
        typename super::allocator_type const& in_allocator =
            typename super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(in_string, in_offset, in_count, in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        // pass
    }

    /** @param[in] in_string    �����l�Ɏg��������̐擪�ʒu�BNULL�����ŏI���B
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: explicit hash_string(
        typename super::const_pointer const   in_string,
        typename super::allocator_type const& in_allocator =
            super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(in_string, in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        // pass
    }

    /** @param[in] in_string    �����l�Ɏg��������̐擪�ʒu�B
        @param[in] in_size      �����l�Ɏg��������̒����B
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: hash_string(
        typename super::const_pointer const   in_string,
        typename super::size_type const       in_size,
        typename super::allocator_type const& in_allocator =
            typename super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(in_string, in_size, in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        // pass
    }

    /** @brief ������literal����\�z�B
        @param[in] in_offset    ������literal�̊J�noffset�ʒu�B
        @param[in] in_string    �����l�Ɏg��������literal�B
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: template <std::size_t template_size>
    hash_string(
        typename self::size_type              in_offset,
        typename super::value_type const      (&in_string)[template_size],
        typename super::allocator_type const& in_allocator =
            typename super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, template_size),
            in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        PSYQ_ASSERT(0 < template_size);
    }

    /** @brief ������literal����\�z�B
        @param[in] in_offset    ������literal�̊J�noffset�ʒu�B
        @param[in] in_count     �������B
        @param[in] in_string    �����l�Ɏg��������literal�B
        @param[in] in_allocator memory�����q�̏����l�B
        @param[in] in_hasher    hash�����q�̏����l�B
     */
    public: template <std::size_t template_size>
    hash_string(
        typename self::size_type              in_offset,
        typename self::size_type              in_count,
        typename super::value_type const      (&in_string)[template_size],
        typename super::allocator_type const& in_allocator =
            typename super::allocator_type(),
        template_hash_type const&             in_hasher =
            template_hash_type())
    :
        super(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, in_count),
            in_allocator),
        hash_(template_hash_type::EMPTY),
        hasher_(in_hasher)
    {
        PSYQ_ASSERT(0 < template_size);
    }

    //-------------------------------------------------------------------------
    /** copy���Z�q�B
        @param[in] in_right copy���ƂȂ�l�B
     */
    public: self& operator=(
        super const& in_right)
    {
        if (this != &in_right)
        {
            self(in_right).swap(*this);
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    public: self& assign(
        typename super::size_type const  in_count,
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::assign(in_count, in_char);
        return *this;
    }

    public: self& assign(
        super const& in_string)
    {
        this->reset_hash();
        this->super::assign(in_string);
        return *this;
    }

    public: self& assign(
        super const&                    in_string,
        typename super::size_type const in_offset,
        typename super::size_type const in_count)
    {
        this->reset_hash();
        this->super::assign(in_string, in_offset, in_count);
        return *this;
    }

    public: self& assign(
        typename super::const_pointer const in_string)
    {
        this->reset_hash();
        this->super::assign(in_string);
        return *this;
    }

    public: self& assign(
        typename super::const_pointer const in_string,
        typename super::size_type const     in_size)
    {
        this->reset_hash();
        this->super::assign(in_string, in_size);
        return *this;
    }

    public: template<typename template_iterator_type>
    self& assign(
        template_iterator_type const in_begin,
        template_iterator_type const in_end)
    {
        this->reset_hash();
        this->super::assign(in_begin, in_end);
        return *this;
    }

    public: template <std::size_t template_size>
    self& assign(
        typename self::size_type         in_offset,
        typename super::value_type const (&in_string)[template_size])
    {
        PSYQ_ASSERT(0 < template_size);
        this->reset_hash();
        return this->assign(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, template_size));
    }

    public: template <std::size_t template_size>
    self& assign(
        typename self::size_type         in_offset,
        typename self::size_type         in_count,
        typename super::value_type const (&in_string)[template_size])
    {
        PSYQ_ASSERT(0 < template_size);
        this->reset_hash();
        return this->assign(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, in_count));
    }

    //-------------------------------------------------------------------------
    public: typename super::reference at(
        typename super::size_type const in_index)
    {
        this->reset_hash();
        return this->super::at(in_index);
    }

    public: typename super::const_reference at(
        typename super::size_type const in_index)
    const
    {
        return this->super::at(in_index);
    }

    public: typename super::reference operator[](
        typename super::size_type const in_index)
    {
        this->reset_hash();
        return this->super::operator[](in_index);
    }

    public: typename super::const_reference operator[](
        typename super::size_type const in_index)
    const
    {
        return this->super::operator[](in_index);
    }

    //-------------------------------------------------------------------------
    public: typename super::reference front()
    {
        this->reset_hash();
        return this->super::front();
    }

    public: typename super::const_reference front() const
    {
        return this->super::front();
    }

    public: typename super::reference back()
    {
        this->reset_hash();
        return this->super::back();
    }

    public: typename super::const_reference back() const
    {
        return this->super::back();
    }

    //-------------------------------------------------------------------------
    public: typename super::iterator begin()
    {
        this->reset_hash();
        return this->super::begin();
    }

    public: typename super::const_iterator begin() const
    {
        return this->super::begin();
    }

    public: typename super::iterator end()
    {
        this->reset_hash();
        return this->super::end();
    }

    public: typename super::const_iterator end() const
    {
        return this->super::end();
    }

    //-------------------------------------------------------------------------
    public: typename super::reverse_iterator rbegin()
    {
        this->reset_hash();
        return this->super::rbegin();
    }

    public: typename super::const_reverse_iterator rbegin() const
    {
        return this->super::rbegin();
    }

    public: typename super::reverse_iterator rend()
    {
        this->reset_hash();
        return this->super::rend();
    }

    public: typename super::const_reverse_iterator rend() const
    {
        return this->super::rend();
    }

    //-------------------------------------------------------------------------
    public: void clear()
    {
        this->reset_hash();
        return this->super::clear();
    }

    //-------------------------------------------------------------------------
    public: self& insert(
        typename super::size_type const  in_index,
        typename super::size_type const  in_count,
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::insert(in_index, in_count, in_char);
        return *this;
    }

    public: self& insert(
        typename super::size_type const     in_index,
        typename super::const_pointer const in_string)
    {
        this->reset_hash();
        this->super::insert(in_index, in_string);
        return *this;
    }

    public: self& insert(
        typename super::size_type const     in_index,
        typename super::const_pointer const in_string,
        typename super::size_type const     in_count)
    {
        this->reset_hash();
        this->super::insert(in_index, in_string, in_count);
        return *this;
    }

    public: self& insert(
        typename super::size_type const in_index,
        typename super const&           in_string)
    {
        this->reset_hash();
        this->super::insert(in_index, in_string);
        return *this;
    }

    public: self& insert(
        typename super::size_type const in_index,
        typename super const&           in_string,
        typename super::size_type const in_string_index,
        typename super::size_type const in_string_offset)
    {
        this->reset_hash();
        this->super::insert(
            in_index, in_string, in_string_index, in_string_offset);
        return *this;
    }

    public: typename super::iterator insert(
        typename super::const_iterator const in_position,
        typename super::value_type const     in_char)
    {
        this->reset_hash();
        return this->super::insert(in_position, in_char);
    }

    public: typename super::iterator insert(
        typename super::const_iterator const in_position,
        typename super::size_type const      in_count,
        typename super::value_type const     in_char)
    {
        this->reset_hash();
        return this->super::insert(in_position, in_count, in_char);
    }

    public: template<typename template_iterator_type>
    typename super::iterator insert(
        typename super::const_iterator const in_position,
        template_iterator_type const         in_begin,
        template_iterator_type const         in_end)
    {
        this->reset_hash();
        return this->super::insert(in_position, in_begin, in_end);
    }

    //-------------------------------------------------------------------------
    public: self& erase(
        typename super::size_type const in_index = 0,
        typename super::size_type const in_count = super::npos)
    {
        this->reset_hash();
        this->super::erase(in_index, in_count);
        return *this;
    }

    public: typename super::iterator erase(
        typename super::const_iterator const in_position)
    {
        this->reset_hash();
        return this->super::erase(in_position);
    }

    public: typename super::iterator erase(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end)
    {
        this->reset_hash();
        return this->super::erase(in_begin, in_end);
    }

    //-------------------------------------------------------------------------
    public: void push_back(
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::push_back(in_char);
    }

    public: void pop_back()
    {
        this->reset_hash();
        this->super::pop_back();
    }

    //-------------------------------------------------------------------------
    public: self& append(
        typename super::size_type const  in_count,
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::append(in_count, in_char);
        return *this;
    }

    public: self& append(
        typename super const& in_string)
    {
        this->reset_hash();
        this->super::append(in_string);
        return *this;
    }

    public: self& append(
        typename super const&           in_string,
        typename super::size_type const in_offset,
        typename super::size_type const in_count)
    {
        this->reset_hash();
        this->super::append(in_string, in_offset, in_count);
        return *this;
    }

    public: self& append(
        typename super::const_pointer const in_string)
    {
        this->reset_hash();
        this->super::append(in_string);
        return *this;
    }

    public: self& append(
        typename super::const_pointer const in_string,
        typename super::size_type const     in_size)
    {
        this->reset_hash();
        this->super::append(in_string, in_size);
        return *this;
    }

    public: template<typename template_iterator_type>
    self& append(
        template_iterator_type const in_begin,
        template_iterator_type const in_end)
    {
        this->reset_hash();
        this->super::append(in_begin, in_end);
        return *this;
    }

    public: template <std::size_t template_size>
    self& append(
        typename self::size_type         in_offset,
        typename super::value_type const (&in_string)[template_size])
    {
        PSYQ_ASSERT(0 < template_size);
        this->reset_hash();
        return this->append(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, template_size));
    }

    public: template <std::size_t template_size>
    self& append(
        typename self::size_type         in_offset,
        typename self::size_type         in_count,
        typename super::value_type const (&in_string)[template_size])
    {
        PSYQ_ASSERT(0 < template_size);
        this->reset_hash();
        return this->append(
            &in_string[in_offset],
            self::trim_count(template_size - 1, in_offset, in_count));
    }

    //-------------------------------------------------------------------------
    public: self& operator+=(
        super const& in_string)
    {
        this->reset_hash();
        this->super::operator+=(in_string);
        return *this;
    }

    public: self& operator+=(
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::operator+=(in_char);
        return *this;
    }

    public: self& operator+=(
        typename super::const_pointer const in_string)
    {
        this->reset_hash();
        this->super::operator+=(in_string);
        return *this;
    }

    //-------------------------------------------------------------------------
    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        self const& in_right)
    const
    {
        return self::compare_string(
            this->data(),
            this->size(),
            this->hash(),
            in_right.data(),
            in_right.size(),
            in_right.hash(),
            this->hasher_);
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_other_string_type>
    int compare(
        template_other_string_type const& in_right)
    const
    {
        return self::compare_string(
            this->data(),
            this->size(),
            this->hash(),
            in_right.data(),
            in_right.size(),
            template_hash_type::EMPTY,
            this->hasher_);
    }

    /** @brief ��������r�B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right       �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        typename self::size_type const in_left_offset,
        typename self::size_type const in_left_count,
        self const&                    in_right)
    const
    {
        return self::compare_sub_string(
            *this,
            in_left_offset,
            in_left_count,
            in_right.data(),
            in_right.size(),
            in_right.hash());
    }

    /** @brief ��������r�B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right       �E�ӂ̕�����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_other_string_type>
    int compare(
        typename self::size_type const    in_left_offset,
        typename self::size_type const    in_left_count,
        template_other_string_type const& in_right)
    const
    {
        return self::compare_sub_string(
            *this,
            in_left_offset,
            in_left_count,
            in_right.data(),
            in_right.size(),
            template_hash_type::EMPTY);
    }

    /** @brief ��������r�B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right_begin �E�ӂ̕�����̐擪�ʒu�B
        @param[in] in_right_size  �E�ӂ̕�����̒����B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: int compare(
        typename self::size_type const     in_left_offset,
        typename self::size_type const     in_left_count,
        typename self::const_pointer const in_right_begin,
        typename self::size_type const     in_right_size)
    const
    {
        return self::compare_sub_string(
            *this,
            in_left_offset,
            in_left_count,
            in_right_begin,
            in_right_size,
            template_hash_type::EMPTY);
    }

    /** @brief ��������r�B
        @tparam template_other_string_type ������^�B
        @param[in] in_left_offset  ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count   ���ӂ̕�����̕������B
        @param[in] in_right        �E�ӂ̕�����B
        @param[in] in_right_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_right_count  �E�ӂ̕�����̕������B
        @retval �� �E�ӂ̂ق����傫���B
        @retval �� ���ӂ̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    public: template<typename template_other_string_type>
    int compare(
        typename self::size_type const    in_left_offset,
        typename self::size_type const    in_left_count,
        template_other_string_type const& in_right,
        typename self::size_type const    in_right_offset,
        typename self::size_type const    in_right_count)
    const
    {
        return this->compare(
            in_left_offset,
            in_left_count,
            in_right.data() + in_right_offset,
            self::trim_count(
                in_right, in_right_offset, in_right_count));
    }

    //-------------------------------------------------------------------------
    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this == �E�ӂ̕�����
     */
    public: bool operator==(
        self const& in_right)
    const
    {
        return
            this->size() == in_right.size() &&
            this->hash() == in_right.hash() &&
            this->operator==(static_cast<super const&>(in_right));
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this == �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator==(
        template_other_string_type const& in_right)
    const
    {
        return static_cast<super const&>(*this) == in_right;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this != �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator!=(
        template_other_string_type const& in_right)
    const
    {
        return !this->operator==(in_right);
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this < �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator<(
        template_other_string_type const& in_right)
    const
    {
        return this->compare(in_right) < 0;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this <= �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator<=(
        template_other_string_type const& in_right)
    const
    {
        return this->compare(in_right) <= 0;
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this > �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator>(
        template_other_string_type const& in_right)
    const
    {
        return 0 < this->compare(in_right);
    }

    /** @brief ��������r�B
        @param[in] in_right �E�ӂ̕�����B
        @return *this >= �E�ӂ̕�����
     */
    public: template<typename template_other_string_type>
    bool operator>=(
        self const& in_right)
    const
    {
        return 0 <= this->compare(in_right);
    }

    //-------------------------------------------------------------------------
    public: self& replace(
        typename super::size_type const in_index,
        typename super::size_type const in_count,
        super const&                    in_string)
    {
        this->reset_hash();
        this->super::replace(in_index, in_count, in_string);
        return *this;
    }

    public: self& replace(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end,
        super const&                         in_string)
    {
        this->reset_hash();
        this->super::replace(in_begin, in_end, in_string);
        return *this;
    }

    public: self& replace(
        typename super::size_type const in_index,
        typename super::size_type const in_count,
        super const&                    in_string,
        typename super::size_type const in_string_index,
        typename super::size_type const in_string_count)
    {
        this->reset_hash();
        this->super::replace(
            in_index, in_count, in_string, in_string_index, in_string_count);
        return *this;
    }

    public: template<typename template_iterator_type>
    self& replace(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end,
        template_iterator_type const         in_string_begin,
        template_iterator_type const         in_string_end)
    {
        this->reset_hash();
        this->super::replace(in_begin, in_end, in_string_begin, in_string_end);
        return *this;
    }

    public: self& replace(
        typename super::size_type const     in_index,
        typename super::size_type const     in_count,
        typename super::const_pointer const in_string,
        typename super::size_type const     in_string_size)
    {
        this->reset_hash();
        this->super::replace(in_index, in_count, in_string, in_string_size);
        return *this;
    }

    public: self& replace(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end,
        typename super::const_pointer const  in_string,
        typename super::size_type const      in_string_size)
    {
        this->reset_hash();
        this->super::replace(in_begin, in_end, in_string, in_string_size);
        return *this;
    }

    public: self& replace(
        typename super::size_type const     in_index,
        typename super::size_type const     in_count,
        typename super::const_pointer const in_string)
    {
        this->reset_hash();
        this->super::replace(in_index, in_count, in_string);
        return *this;
    }

    public: self& replace(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end,
        typename super::const_pointer const  in_string)
    {
        this->reset_hash();
        this->super::replace(in_begin, in_end, in_string);
        return *this;
    }

    public: self& replace(
        typename super::size_type const  in_index,
        typename super::size_type const  in_count,
        typename super::size_type const  in_char_count,
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::replace(in_index, in_count, in_char_count, in_char);
        return *this;
    }

    public: self& replace(
        typename super::const_iterator const in_begin,
        typename super::const_iterator const in_end,
        typename super::size_type const      in_char_count,
        typename super::value_type const     in_char)
    {
        this->reset_hash();
        this->super::replace(in_begin, in_end, in_char_count, in_char);
        return *this;
    }

    //-------------------------------------------------------------------------
    public: void resize(
        typename super::size_type const in_count)
    {
        this->reset_hash();
        this->super::resize(in_count);
    }

    public: void resize(
        typename super::size_type const  in_count,
        typename super::value_type const in_char)
    {
        this->reset_hash();
        this->super::resize(in_count, in_char);
    }

    //-------------------------------------------------------------------------
    /** @brief �l�������B
        @param[in,out] io_target �l����������ΏہB
     */
    public: void swap(
        self& io_target)
    {
        this->super::swap(io_target);
        std::swap(this->hash_, io_target.hash_);
        std::swap(this->hasher_, io_target.hasher_);
    };

    //-------------------------------------------------------------------------
    /** @brief ��������r�B
        @param[in] in_left        ���ӂ̕�����B
        @param[in] in_left_offset ���ӂ̕�����̊J�n�ʒu�B
        @param[in] in_left_count  ���ӂ̕�����̕������B
        @param[in] in_right_begin �E�ӂ̕�����̐擪�ʒu�B
        @param[in] in_right_size  �E�ӂ̕�����̑傫���B
        @param[in] in_right_hash  �E�ӂ̕������hash�l�B
        @param[in] in_hasher      hash�����q�B
        @retval �� �E�ӂ̕�����̂ق����傫���B
        @retval �� ���ӂ̕�����̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    private: template<typename template_left_type>
    static int compare_sub_string(
        template_left_type const&                    in_left,
        typename template_left_type::size_type const in_left_offset,
        typename template_left_type::size_type const in_left_count,
        typename super::const_pointer const          in_right_begin,
        typename super::size_type const              in_right_size,
        typename template_hash_type::value const&    in_right_hash,
        typename template_hash_type const&           in_hasher)
    {
        return self::compare_string(
            in_left.data() + in_left_offset,
            self::trim_count(in_left, in_left_offset, in_left_count),
            template_hash_type::EMPTY,
            in_right_begin,
            in_right_size,
            in_right_hash,
            in_hasher);
    }

    /** @brief ��������r�B
        @param[in] in_left_begin  ���ӂ̕�����̐擪�ʒu�B
        @param[in] in_left_size   ���ӕ�����̑傫���B
        @param[in] in_left_hash   ���ӂ̕������hash�l�B
        @param[in] in_right_begin �E�ӂ̕�����̐擪�ʒu�B
        @param[in] in_right_size  �E�ӂ̕�����̑傫���B
        @param[in] in_right_hash  �E�ӂ̕������hash�l�B
        @param[in] in_hasher      hash�����q�B
        @retval �� �E�ӂ̕�����̂ق����傫���B
        @retval �� ���ӂ̕�����̂ق����傫���B
        @retval 0  ���ӂƉE�ӂ͓����B
     */
    private: static int compare_string(
        typename super::const_pointer const       in_left_begin,
        typename super::size_type const           in_left_size,
        typename template_hash_type::value const& in_left_hash,
        typename super::const_pointer const       in_right_begin,
        typename super::size_type const           in_right_size,
        typename template_hash_type::value const& in_right_hash,
        typename template_hash_type const&        in_hasher)
    {
        // �������r�B
        if (in_left_size < in_right_size)
        {
            return -1;
        }
        if (in_right_size < in_left_size)
        {
            return 1;
        }
        if (in_left_size <= 0 || in_left_begin == in_right_begin)
        {
            return 0;
        }

        // hash�l���r�B
        template_hash_type::value const local_left_hash(
            in_left_hash != template_hash_type::EMPTY?
                in_left_hash:
                self::make_hash(in_left_begin, in_left_size, in_hasher));
        template_hash_type::value const local_right_hash(
            in_right_hash != template_hash_type::EMPTY?
                in_right_hash:
                self::make_hash(in_right_begin, in_right_size, in_hasher));
        if (local_left_hash < local_right_hash)
        {
            return -1;
        }
        if (local_right_hash < local_left_hash)
        {
            return 1;
        }

        // ��������r�B
        return super::traits_type::compare(
            in_left_begin, in_right_begin, in_right_size);
    }

    //-------------------------------------------------------------------------
    /** @brief hash�����q���擾�B
        @return hash�����q�B
     */
    public: typename template_hash_type const& hash_function() const
    {
        return this->hasher_;
    }

    /** @brief �������hash�l���擾�B
        @return �������hash�l�B
     */
    public: typename template_hash_type::value const& hash() const
    {
        if (!this->empty() && this->hash_ == template_hash_type::EMPTY)
        {
            this->hash_ = self::make_hash(*this, this->hasher_);
        }
        return this->hash_;
    }

    /** @brief �������hash�l���擾�B
        @tparam template_other_string_type
            ������̌^�B

            ������̐擪�ʒu���擾���邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
            @code
            template_other_string_type::const_pointer
                template_other_string_type::data() const
            @endcode

            ������̑傫�����擾���邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
            @code
            template_other_string_type::size_type
                template_other_string_type::size() const
            @endcode
        @param[in] in_string hash�l�����肷�镶����B
        @param[in] in_hasher �g�p����hash�����q�B
        @return �������hash�l�B
     */
    public: template<typename template_other_string_type>
    static typename template_hash_type::value make_hash(
        template_other_string_type const& in_string,
        template_hash_type const&         in_hasher)
    {
        return self::make_hash(
            in_string.data(), in_string.size(), in_hasher);
    }

    /** @brief �z���hash�l���擾�B
        @tparam template_value_type �z��̗v�f�̌^�B
        @param[in] in_data   hash�l�����肷��z��̐擪�ʒu�B
        @param[in] in_size   hash�l�����肷��z��̑傫���B
        @param[in] in_hasher �g�p����hash�����q�B
        @return �z���hash�l�B
     */
    public: template<typename template_value_type>
    static typename template_hash_type::value make_hash(
        template_value_type const* const in_data,
        typename self::size_type const   in_size,
        template_hash_type const&        in_hasher)
    {
        return self::make_hash(in_data, in_data + in_size, in_hasher);
    }

    /** @brief �z���hash�l���擾�B
        @param[in] in_begin  hash�l�����肷��z��̐擪�ʒu�B
        @param[in] in_end    hash�l�����肷��z��̖����ʒu�B
        @param[in] in_hasher �g�p����hash�����q�B
        @return �z���hash�l�B
     */
    public: static typename template_hash_type::value make_hash(
        void const* const         in_begin,
        void const* const         in_end,
        template_hash_type const& in_hasher)
    {
        in_hasher;
        typename template_hash_type::value const local_hash(
            in_hasher.make(in_begin, in_end));
        if (local_hash == template_hash_type::EMPTY)
        {
            // ��z��Ɠ���hash�l�������̂ŕύX�B
            return local_hash + std::distance(
                static_cast<char const*>(in_begin),
                static_cast<char const*>(in_end));
        }
        return local_hash;
    }

    private: void reset_hash()
    {
        this->hash_ = template_hash_type::EMPTY;
    }

    private: template<typename template_string_type>
    static typename template_string_type::size_type trim_count(
        template_string_type const&                    in_string,
        typename template_string_type::size_type const in_offset,
        typename template_string_type::size_type const in_count)
    {
        return self::trim_count(
            in_string.size(),
            self::convert_count<template_string_type>(in_offset),
            self::convert_count<template_string_type>(in_count));
    }

    private: static typename self::size_type trim_count(
        typename self::size_type const in_size,
        typename self::size_type const in_offset,
        typename self::size_type const in_count)
    {
        if (in_size < in_offset)
        {
            return 0;
        }
        typename self::size_type const local_limit(in_size - in_offset);
        return in_count < local_limit? in_count: local_limit;
    }

    private: template<typename template_string_type>
    static typename template_string_type::size_type convert_count(
        typename template_string_type::size_type const in_count)
    {
        if (in_count != template_string_type::npos)
        {
            return in_count;
        }
        return (std::numeric_limits<typename template_string_type::size_type>::max)();
    }

    //-------------------------------------------------------------------------
    /// �������hash�l�B
    private: mutable typename template_hash_type::value hash_;

    /// hash�l�𔭍s����instance�B
    private: mutable template_hash_type hasher_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� == �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator==(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator==(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� != �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator!=(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator!=(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� < �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator<(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator>(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� <= �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator<=(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator>=(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� > �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator>(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator<(in_left);
}

/** @brief ������̔�r�B
    @tparam template_string_type @copydoc psyq::hash_string::super
    @tparam template_hash_type   @copydoc psyq::hash_string::hasher
    @param[in] in_left  ���ӂ̕�����B
    @param[in] in_right �E�ӂ̕�����B
    @return ���� >= �E��
 */
template<
    typename template_string_type,
    typename template_hash_type>
bool operator>=(
    template_string_type const& in_left,
    psyq::hash_string<template_string_type, template_hash_type> const&
        in_right)
{
    return in_right.operator<=(in_left);
}

namespace std
{
    /** @brief �l�������B
        @tparam template_string_type @copydoc psyq::hash_string::super
        @tparam template_hash_type   @copydoc psyq::hash_string::hasher
        @param[in,out] io_left  ��������l�B
        @param[in,out] io_right ��������l�B
     */
    template<
        typename template_string_type,
        typename template_hash_type>
    void swap(
        psyq::hash_string<
            template_string_type, template_hash_type> const&
                i_left,
        psyq::hash_string<
            template_string_type, template_hash_type> const&
                i_right)
    {
        io_left.swap(io_right);
    }
} // namespace std

#endif // PSYQ_HASH_STRING_HPP_
