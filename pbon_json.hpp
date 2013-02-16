/* Copyright (c) 2013, Hillco Psychi, All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** @brief PBON/JSON is a library for translating between PBON and JSON.

    PBON/JSON�́APBON��JSON�̑��ݕϊ����s�����߂�library�B
    PBON�́APacked-Binary-Object-Notation�̗��B

    - Call pbon::json::value constructor, import the JSON.
      pbon::json::value ��constructor���Ăяo���AJSON����l�����o���B

    @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PBON_JSON_HPP_
#define PBON_JSON_HPP_

namespace pbon
{
typedef std::int8_t  int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;
typedef int8  char8;
typedef int16 char16;
typedef int32 char32;
typedef float  float32;
typedef double float64;

enum type
{
    type_NULL,
    type_BOOL,
    type_ARRAY,
    type_OBJECT,
    type_CHAR8 = 1 << 4,
    type_CHAR16,
    type_CHAR32,
    type_INT8 = 2 << 4,
    type_INT16,
    type_INT32,
    type_INT64,
    type_FLOAT32 = (3 << 4) + 2,
    type_FLOAT64,
};

template< typename template_type > pbon::type get_type()
{
    return pbon::type_NULL;
}

template<> pbon::type get_type< pbon::char8 >()
{
    return pbon::type_CHAR8;
}

template<> pbon::type get_type< pbon::char16 >()
{
    return pbon::type_CHAR16;
}

template<> pbon::type get_type< pbon::int32 >()
{
    return pbon::type_INT32;
}

template<> pbon::type get_type< pbon::int64 >()
{
    return pbon::type_INT64;
}

template<> pbon::type get_type< pbon::float32 >()
{
    return pbon::type_FLOAT32;
}

template<> pbon::type get_type< pbon::float64 >()
{
    return pbon::type_FLOAT64;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief PBON�̗v�f�B
    @tparam template_attribute_type @copydoc pbon::value::attribute
 */
template< typename template_attribute_type >
class value
{
    /// this���w���^�B
    public: typedef value self;

    /// packed-binary�̑����̌^�B
    public: typedef template_attribute_type attribute;

    /// packed-binary��header���B
    private: struct header
    {
        pbon::int32 endian_;
        pbon::int16 type_;
        pbon::int16 root_;
    };

    /** @brief packed-binary�̍ŏ�ʗv�f���擾�B
        @param[in] in_packed_binary packed-binary�̐擪�ʒu�B
        @retval !=NULL packed-binary�̍ŏ�ʗv�f�ւ�pointer�B
        @retval ==NULL �����Ȃ�packed-binary�������B
     */
    public: static const self* get_root(
        const void* const in_packed_binary)
    {
        if (in_packed_binary == NULL)
        {
            return NULL;
        }
        const self::header& local_header(
            *static_cast< const self::header* >(in_packed_binary));
        if (local_header.endian_ != 'pbon')
        {
            // endian���قȂ�̂ň����Ȃ��B
            return NULL;
        }
        if (local_header.type_ != pbon::get_type< self::attribute >())
        {
            // �����̌^���قȂ�̂ň����Ȃ��B
            return NULL;
        }
        return self::get_address< self >(in_packed_binary, local_header.root_);
    }

    /** @brief ��ʗv�f���擾�B
        @retval !=NULL ��ʗv�f�ւ�pointer�B
        @retval ==NULL ��ʗv�f���Ȃ��B
     */
    public: const self* get_super() const
    {
        if (this->super_ == 0)
        {
            return NULL;
        }
        return self::get_address< self >(this, this->super_);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t get_size() const
    {
        return this->size_;
    }

    /** @brief �����Ă���l�̌^���擾�B
     */
    public: pbon::type get_type() const
    {
        return static_cast< pbon::type >(this->type_);
    }

    public: bool is_array() const
    {
        return this->get_type() == pbon::type_ARRAY;
    }

    public: bool is_object() const
    {
        return this->get_type() == pbon::type_OBJECT;
    }

    /** @brief �����Ă���l�ւ�pointer���擾�B
        @tparam template_valuetype �����Ă���l�̌^�B
     */
    protected: template< typename template_valuetype >
    const template_valuetype* get_value() const
    {
        return self::get_address< template_valuetype >(this, this->value_);
    }

    /** @brief ���Έʒu����address���擾�B
        @tparam template_value_type    pointer���w���l�̌^�B
        @tparam template_position_type ���Έʒu�̌^�B
        @param[in] in_base_address     ��ʒu�ƂȂ�pointer�B
        @param[in] in_byte_position    ��ʒu�����byte�P�ʂł̑��Έʒu�B
     */
    private: template<
        typename template_value_type,
        typename template_position_type >
    static const template_value_type* get_address(
        const void* const            in_base_address,
        const template_position_type in_byte_position)
    {
        return reinterpret_cast< const template_value_type* >(
            static_cast< const char* >(in_base_address) + in_byte_position);
    }

    private: typename self::attribute value_; ///< �l�B�������͒l�ւ̑��Έʒu�B
    private: typename self::attribute size_;  ///< �l�̐��B
    private: typename self::attribute type_;  ///< �l�̌^�B
    private: typename self::attribute super_; ///< ��ʗv�f�ւ̑��Έʒu�B
};
typedef pbon::value< pbon::int32 > value32;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// pbon::value �̔z��B
template< typename template_attribute_type >
class array:
    private pbon::value< template_attribute_type >
{
    public: typedef pbon::array< template_attribute_type > self;
    private: typedef pbon::value< template_attribute_type > super;
    public: typedef super value;

    using super::attribute;
    using super::get_super;

    public: static const self* cast(
        const super* const in_value)
    {
        if (in_value == NULL || !in_value->is_array())
        {
            return NULL;
        }
        return static_cast< const self* >(in_value);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t get_size() const
    {
        if (!this->is_array())
        {
            return 0;
        }
        return this->super::get_size();
    }

    public: const typename self::value* get_begin() const
    {
        if (!this->is_array())
        {
            return NULL;
        }
        return this->get_value< typename self::value >();
    }

    public: const typename self::value* get_end() const
    {
        if (!this->is_array())
        {
            return NULL;
        }
        return this->get_value< typename self::value >() + this->get_size();
    }

    public: const typename self::value* at(
        const std::size_t in_index)
    {
        if (!this->is_array() || this->get_size() <= in_index)
        {
            return NULL;
        }
        return this->get_value< self::value >() + in_index;
    }
};
typedef pbon::array< pbon::int32 > array32;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// pbon::value �̎����B
template< typename template_attribute_type >
class object:
    private value< template_attribute_type >
{
    public: typedef pbon::object< template_attribute_type > self;
    private: typedef pbon::value< template_attribute_type > super;
    public: typedef std::pair< super, super > value;

    using super::attribute;
    using super::get_super;

    public: static const self* cast(
        const super* const in_value)
    {
        if (in_value == NULL || !in_value->is_object())
        {
            return NULL;
        }
        return static_cast< const self* >(in_value);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t get_size() const
    {
        if (!this->is_object())
        {
            return 0;
        }
        return this->super::get_size() / 2;
    }

    public: const typename self::value* get_begin() const
    {
        if (!this->is_object())
        {
            return NULL;
        }
        return this->get_value< typename self::value >();
    }

    public: const typename self::value* get_end() const
    {
        if (!this->is_object())
        {
            return NULL;
        }
        return this->get_value< typename self::value >() + this->get_size();
    }

    public: template< typename template_key_type >
    const typename self::value* lower_bound(
        const template_key_type& in_key) const;

    public: template< typename template_key_type >
    const typename self::value* upper_bound(
        const template_key_type& in_key) const;

    public: template< typename template_key_type >
    const typename self::value* find(
        const template_key_type& in_key) const;
};
typedef pbon::object< pbon::int32 > object32;

namespace json
{

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief JSON�̒l�B
 */
class value
{
    /// this���w���l�̌^�B
    public: typedef pbon::json::value self;

    //-------------------------------------------------------------------------
    public: template< typename template_value_type > struct type {};
    public: typedef std::pair< unsigned, unsigned > parse_result;

    /// �l�̎������B
    private: enum hold
    {
        hold_EMPTY,   ///< �l����B
        hold_VALUE,   ///< ���ڒl�������Ă�B
        hold_POINTER, ///< pointer�Ƃ��Ēl�������Ă�B
    };

    //-------------------------------------------------------------------------
    private: pbon::int64 storage_;
    private: self::hold  hold_;

    //-------------------------------------------------------------------------
    /// @brief ��̒l���\�z�B
    public: value():
    storage_(0),
    hold_(self::hold_EMPTY)
    {
        // pass
    }

    /** @brief copy constructor
        @param[in] in_source ������ƂȂ�l�B
        @todo �������B
     */
    public: value(
        const self& in_source):
    hold_(in_source.hold_)
    {
    }

    /** @brief JSON�`���̕��������͂��A�l�����o���B
        @tparam template_number_type @copydoc pbon::json::parser::number
        @tparam template_string_type @copydoc pbon::json::parser::string
        @param[in] in_string      �l����荞��JSON�`���̕�����B
        @param[in] in_number_type �ێ����鐔�l�̌^�B
     */
    template<
        typename template_number_type,
        typename template_string_type >
    value(
        const template_string_type&               in_string,
        const self::type< template_number_type >& in_number_type)
    {
        self::parse_result local_result;
        new(this) self(in_string, in_number_type, local_result);
    }

    /** @brief JSON�`���̕��������͂��A�l�����o���B
        @tparam template_number_type   @copydoc pbon::json::parser::number
        @tparam template_string_type   @copydoc pbon::json::parser::string
        @param[in] in_string      �l����荞��JSON�`���̕�����B
        @param[in] in_number_type �ێ����鐔�l�̌^�B
        @param[out] out_result    JSON�̉�͌��ʁB
        - ���������ꍇ��(0, 0)�B
        - ���s�����ꍇ�́A��荞�݂Ɏ��s���������ʒu��(�s�ԍ�, ���ʒu)�B
     */
    template<
        typename template_number_type,
        typename template_string_type >
    value(
        const template_string_type&               in_string,
        const self::type< template_number_type >& in_number_type,
        self::parse_result&                       out_result)
    {
        typename template_string_type::allocator_type local_allocator(
            in_string.get_allocator());
        new(this) self(in_string, in_number_type, local_allocator, out_result);
    }

    /** @brief JSON�`���̕��������͂��A�l�����o���B
        @tparam template_number_type   @copydoc pbon::json::parser::number
        @tparam template_string_type   @copydoc pbon::json::parser::string
        @param[in] in_string        �l����荞��JSON�`���̕�����B
        @param[in] in_number_type   �ێ����鐔�l�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
     */
    template<
        typename template_number_type,
        typename template_string_type,
        typename template_allocator_type >
    value(
        const template_string_type&               in_string,
        const self::type< template_number_type >& in_number_type,
        template_allocator_type&                  io_allocator)
    {
        self::parse_result local_result;
        new(this) self(
            in_string, in_number_type, io_allocator, local_result);
    }

    /** @brief JSON�`���̕��������͂��A�l�����o���B
        @tparam template_number_type   @copydoc pbon::json::parser::number
        @tparam template_string_type   @copydoc pbon::json::parser::string
        @param[in] in_string        �l����荞��JSON�`���̕�����B
        @param[in] in_number_type   �ێ����鐔�l�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_result      JSON�̉�͌��ʁB
        - ���������ꍇ��(0, 0)�B
        - ���s�����ꍇ�́A��荞�݂Ɏ��s���������ʒu��(�s�ԍ�, ���ʒu)�B
     */
    template<
        typename template_number_type,
        typename template_string_type,
        typename template_allocator_type >
    value(
        const template_string_type&               in_string,
        const self::type< template_number_type >& in_number_type,
        template_allocator_type&                  io_allocator,
        self::parse_result&                       out_result)
    {
        typedef std::list< pbon::json::value, template_allocator_type > array;
        typedef std::map<
            template_string_type,
            pbon::json::value,
            std::less< template_string_type >,
            template_allocator_type >
                object;
        new(this) self(
            in_string.begin(),
            in_string.end(),
            in_number_type,
            self::type< template_string_type >(),
            self::type< array >(),
            self::type< object >(),
            io_allocator,
            out_result);
    }

    /** @brief JSON�`���̕��������͂��A�l�����o���B
        @tparam template_iterator_type @copydoc pbon::json::parser::iterator
        @tparam template_number_type   @copydoc pbon::json::parser::number
        @tparam template_string_type   @copydoc pbon::json::parser::string
        @tparam template_array_type    @copydoc pbon::json::parser::array
        @tparam template_object_type   @copydoc pbon::json::parser::object
        @param[in] in_string_begin  �l����荞��JSON�`���̕�����̐擪�ʒu�B
        @param[in] in_string_end    �l����荞��JSON�`���̕�����̖����ʒu�B
        @param[in] in_number_type   �ێ����鐔�l�̌^�B
        @param[in] in_string_type   �ێ����镶����̌^�B
        @param[in] in_array_type    �ێ�����z��̌^�B
        @param[in] in_object_type   �ێ�����object�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
     */
    template<
        typename template_number_type,
        typename template_string_type,
        typename template_array_type,
        typename template_object_type,
        typename template_iterator_type,
        typename template_allocator_type >
    value(
        const template_iterator_type&             in_string_begin,
        const template_iterator_type&             in_string_end,
        const self::type< template_number_type >& in_number_type,
        const self::type< template_string_type >& in_string_type,
        const self::type< template_array_type >&  in_array_type,
        const self::type< template_object_type >& in_object_type,
        template_allocator_type&                  io_allocator)
    {
        self::parse_result local_result;
        new(this) self(
            in_string_begin,
            in_string_end,
            in_number_type,
            in_string_type,
            in_array_type,
            in_object_type,
            io_allocator,
            local_result);
    }

    /** @brief JSON�`���̕����񂩂�l�����o���B
        @tparam template_iterator_type @copydoc pbon::json::parser::iterator
        @tparam template_number_type   @copydoc pbon::json::parser::number
        @tparam template_string_type   @copydoc pbon::json::parser::string
        @tparam template_array_type    @copydoc pbon::json::parser::array
        @tparam template_object_type   @copydoc pbon::json::parser::object
        @param[in] in_string_begin  �l����荞��JSON�`���̕�����̐擪�ʒu�B
        @param[in] in_string_end    �l����荞��JSON�`���̕�����̖����ʒu�B
        @param[in] in_number_type   �ێ����鐔�l�̌^�B
        @param[in] in_string_type   �ێ����镶����̌^�B
        @param[in] in_array_type    �ێ�����z��̌^�B
        @param[in] in_object_type   �ێ�����object�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_result      JSON�̉�͌��ʁB
        - ���������ꍇ��(0, 0)�B
        - ���s�����ꍇ�́A��荞�݂Ɏ��s���������ʒu��(�s�ԍ�, ���ʒu)�B
     */
    template<
        typename template_number_type,
        typename template_string_type,
        typename template_array_type,
        typename template_object_type,
        typename template_iterator_type,
        typename template_allocator_type >
    value(
        const template_iterator_type&             in_string_begin,
        const template_iterator_type&             in_string_end,
        const self::type< template_number_type >& in_number_type,
        const self::type< template_string_type >& in_string_type,
        const self::type< template_array_type >&  in_array_type,
        const self::type< template_object_type >& in_object_type,
        template_allocator_type&                  io_allocator,
        self::parse_result&                       out_result):
    storage_(0),
    hold_(self::hold_EMPTY)
    {
        // �g��Ȃ������B
        in_number_type;
        in_string_type;
        in_array_type;
        in_object_type;

        // JSON���������͂��Ēl�����o���B
        pbon::json::parser<
            template_iterator_type,
            template_number_type,
            template_string_type,
            template_array_type,
            template_object_type >
                local_parser(
                    in_string_begin, in_string_end, io_allocator, *this);
        out_result = self::parse_result(
            local_parser.get_line(), local_parser.get_column());
    }

    /** @brief �C�ӌ^�̒l���\�z�B

        memory�����Ă��s�킸�ɍ\�z�B�\�z�ł��Ȃ������ꍇ�́A��ƂȂ�B

        @tparam template_value_type �����l�̌^�B
        @param[in] in_value �����l�B
        @param[in] in_dummy
             constructor����ʂ��邽�߂�dummy�B�l�͉��ł������B
     */
    public: template< typename template_value_type >
    value(
        const template_value_type& in_value,
        const bool                 in_dummy):
    storage_(0)
    {
        in_dummy; // �g��Ȃ������B
        if (!this->set_value(in_value))
        {
            this->hold_ = self::hold_EMPTY;
        }
    }

    /** @brief �C�ӌ^�̒l���\�z�B

        @tparam template_value_type     �����l�̌^�B
        @tparam template_allocator_type memory�����q�̌^�B
        @param[in] in_value �����l�B
        @param[in] in_dummy
             constructor����ʂ��邽�߂�dummy�B�l�͉��ł������B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @todo �������B
     */
    public: template<
        typename template_value_type,
        typename template_allocator_type >
    value(
        const template_value_type& in_value,
        const bool                 in_dummy,
        template_allocator_type&   io_allocator):
    storage_(0)
    {
        in_dummy;
        if (!this->set_value(in_value))
        {
            io_allocator;
            this->hold_ = self::hold_POINTER;
        }
    }

    /// @brief destructor
    /// @todo �������B
    ~value()
    {
    }

    /** @brief ������Z�q�B
        @param[in] in_source ������ƂȂ�l�B
     */
    public: self& operator=(const self& in_source)
    {
        this->~value();
        return *new(this) self(in_source);
    }

    /** @brief �l�������B
        @param[in,out] io_target �l����������ΏہB
     */
    public: void swap(self& io_target)
    {
        std::swap(this->storage_, io_target.storage_);
        std::swap(this->hold_, io_target.hold_);
    }

    /** @brief �l���󂩔���B
        @retval true  �l���󂾂����B
        @retval false �l����ł͂Ȃ������B
     */
    public: bool is_empty() const
    {
        return this->hold_ == self::hold_EMPTY;
    }

    private: template< typename template_value_type >
    bool set_value(
        const template_value_type& in_value)
    {
        std::size_t local_value_size(sizeof(template_value_type));
        if (sizeof(this->storage_) < local_value_size)
        {
            return false;
        }
        this->~value();
        new(&this->storage_) template_value_type(in_value);
        this->hold_ = self::hold_VALUE;
        return true;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief JSON��͊�B
    @tparam template_iterator_type @copydoc parser::iterator
    @tparam template_number_type   @copydoc parser::number
    @tparam template_string_type   @copydoc parser::string
    @tparam template_array_type    @copydoc parser::array
    @tparam template_object_type   @copydoc parser::object
 */
template<
    typename template_iterator_type,
    typename template_number_type,
    typename template_string_type,
    typename template_array_type,
    typename template_object_type >
class parser
{
    /// this���w���l�̌^�B
    public: typedef parser<
        template_iterator_type,
        template_number_type,
        template_string_type,
        template_array_type,
        template_object_type >
            self;

    //-------------------------------------------------------------------------
    /// JSON�̉�͂Ɏg�������q�B
    public: typedef template_iterator_type iterator;

    /// pbon::json::value �Ɏ������鐔�l�̌^�B
    public: typedef template_number_type number;

    /** @brief pbon::json::value �Ɏ������镶����̌^�B

        �����ɕ�����ǉ����邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        string::push_back(const string::value_type)
        @endcode
     */
    public: typedef template_string_type string;

    /** @brief pbon::json::value �Ɏ�������z��̌^�B

        �����ɒl��ǉ����邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        array::push_back(const pbon::json::value&)
        @endcode
     */
    public: typedef template_array_type array;

    /** @brief pbon::json::value �Ɏ�������object�̌^�B

        �l��}�����邽�߁A�ȉ��ɑ�������֐����g���邱�ƁB
        @code
        array::insert(std::pair< string, const pbon::json::value >&)
        @endcode
     */
    public: typedef template_object_type object;

    //-------------------------------------------------------------------------
    /** @tparam template_allocator_type
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in] in_begin         ��͂��镶����̐擪�ʒu�B
        @param[in] in_end           ��͂��镶����̖����ʒu�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o�����l�̏o�͐�B
     */
    public: template< typename template_allocator_type >
    parser(
        const template_iterator_type& in_begin,
        const template_iterator_type& in_end,
        template_allocator_type&      io_allocator,
        pbon::json::value&            out_value):
    current_(in_begin),
    end_(in_end),
    last_char_(self::END_CHAR),
    line_(1),
    column_(1),
    undo_(false)
    {
        if (this->parse(io_allocator, out_value))
        {
            this->line_ = 0;
            this->column_ = 0;
        }
    }

    //-------------------------------------------------------------------------
    /** @brief ���O�ɉ�͂��������̍s�ԍ����擾�B
        @retval >0  ���O�ɉ�͂��������̍s�ԍ��B
        @retval <=0 ��͂͐������ďI�������B
     */
    public: unsigned get_line() const
    {
        return this->line_;
    }

    /** @brief ���O�ɉ�͂��������̌��ʒu���擾�B
        @retval >0  ���O�ɉ�͂��������̌��ʒu�B
        @retval <=0 ��͂͐������ďI�������B
     */
    public: unsigned get_column() const
    {
        return this->column_;
    }

    //-------------------------------------------------------------------------
    /** @brief JSON�����l����͂��Ď��o���B
        @tparam template_allocator_type
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o�����l�̏o�͐�B
        @retval true  �����B
        @retval false ���s�B�l�͏o�͂���Ȃ��B
     */
    private: template< typename template_allocator_type >
    bool parse(
        template_allocator_type& io_allocator,
        pbon::json::value&       out_value)
    {
        this->skip_white_space();
        const int local_char(this->read_char());
        switch (local_char)
        {
            case '"':
            return this->parse_string(io_allocator, out_value);

            case '[':
            return this->parse_array(io_allocator, out_value);

            case '{':
            return this->parse_object(io_allocator, out_value);

            case 'n':
            if (this->match("ull"))
            {
                pbon::json::value().swap(out_value);
                return true;
            }
            return false;

            case 't':
            if (this->match("rue"))
            {
                pbon::json::value(true, true, io_allocator).swap(out_value);
                return true;
            }
            return false;

            case 'f':
            if (this->match("alse"))
            {
                pbon::json::value(false, true, io_allocator).swap(out_value);
                return true;
            }
            return false;

            case self::END_CHAR:
            return false;

            default:
            this->undo_char();
            if (('0' <= local_char && local_char <= '9') ||
                local_char == '-')
            {
                return this->parse_number(io_allocator, out_value);
            }
            return false;
        }
    }

    /** @brief JSON�������Ă���z�����͂��Ď��o���B
        @tparam template_allocator_type @
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o�����z��̏o�͐�B
        @retval true  �����B
        @retval false ���s�B�z��͏o�͂���Ȃ��B
     */
    private: template< typename template_allocator_type >
    bool parse_array(
        template_allocator_type& io_allocator,
        pbon::json::value&       out_value)
    {
        template_array_type local_array;
        if (!this->expect(']'))
        {
            for (;;)
            {
                local_array.push_back(pbon::json::value());
                if (!this->parse(io_allocator, local_array.back()))
                {
                    return false;
                }
                if (!this->expect(','))
                {
                    break;
                }
            }
        }
        pbon::json::value(local_array, true, io_allocator).swap(out_value);
        return this->expect(']');
    }

    /** @brief JSON�������Ă���object����͂��Ď��o���B
        @tparam template_allocator_type
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o����object�̏o�͐�B
        @retval true  �����B
        @retval false ���s�Bobject�͏o�͂���Ȃ��B
        @todo �������B
     */
    private: template< typename template_allocator_type >
    bool parse_object(
        template_allocator_type& io_allocator,
        pbon::json::value&       out_value)
    {
        out_value;io_allocator;
        return false;
    }

    /** @brief JSON�������Ă��鐔�l����͂��Ď��o���B
        @tparam template_allocator_type
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o�������l�̏o�͐�B
        @retval true  �����B
        @retval false ���s�B���l�͏o�͂���Ȃ��B
     */
    private: template< typename template_allocator_type >
    bool parse_number(
        template_allocator_type& io_allocator,
        pbon::json::value&       out_value)
    {
        template_string_type local_string;
        for (;;)
        {
            const int local_char(this->read_char());
            if (('0' <= local_char && local_char <= '9') ||
                local_char == '+' ||
                local_char == '-' ||
                local_char == '.' ||
                local_char == 'e' ||
                local_char == 'E')
            {
                local_string.push_back(
                    static_cast< typename template_string_type::value_type >(
                        local_char));
            }
            else
            {
                this->undo_char();
                break;
            }
        }

        std::istringstream local_stream(local_string);
        template_number_type local_number;
        local_stream >> local_number;
        if (!local_stream.eof())
        {
            return false;
        }
        pbon::json::value(local_number, true, io_allocator).swap(out_value);
        return true;
    }

    /** @brief JSON�������Ă��镶�������͂��Ď��o���B
        @tparam template_allocator_type
            std::allocator �݊���interface������memory�����q�̌^�B
        @param[in,out] io_allocator �g�p����memory�����q�B
        @param[out] out_value       JSON������o����������̏o�͐�B
        @retval true  �����B
        @retval false ���s�B������͏o�͂���Ȃ��B
     */
    private: template< typename template_allocator_type >
    bool parse_string(
        template_allocator_type& io_allocator,
        pbon::json::value&       out_value)
    {
        template_string_type local_string;
        for (;;)
        {
            int local_char(this->read_char());
            if (local_char < ' ')
            {
                this->undo_char();
                return false;
            }
            if (local_char == '"')
            {
                pbon::json::value(local_string, true, io_allocator).swap(
                    out_value);
                return true;
            }
            if (local_char == '\\')
            {
                local_char = this->read_char();
                switch (local_char)
                {
                    case '"':
                    break;

                    case '\\':
                    break;

                    case '/':
                    break;

                    case 'b':
                    local_char = '\b';
                    break;

                    case 'f':
                    local_char = '\f';
                    break;

                    case 'n':
                    local_char = '\n';
                    break;

                    case 'r':
                    local_char = '\r';
                    break;

                    case 't':
                    local_char = '\t';
                    break;

                    case 'u':
                    if (this->parse_code_point(local_string))
                    {
                        continue;
                    }
                    return false;

                    default:
                    return false;
                }
            }
            local_string.push_back(
                static_cast< typename template_string_type::value_type >(
                    local_char));
        }
    }

    /** @brief �������code-point�\�L����́B
     */
    private: bool parse_code_point(
        template_string_type& out_string)
    {
        int local_unicode_char(this->parse_quad_hex());
        if (local_unicode_char == self::END_CHAR)
        {
            return false;
        }
        if (0xd800 <= local_unicode_char && local_unicode_char <= 0xdfff)
        {
            // 16bit surrogate pair�̌㔼������B
            if (0xdc00 <= local_unicode_char)
            {
                return false;
            }

            // 16bit surrogate pair�̑O���������B
            if (this->read_char() != '\\' || this->read_char() != 'u')
            {
                this->undo_char();
                return false;
            }
            const int local_second(this->parse_quad_hex());
            if (local_second < 0xdc00 || 0xdfff < local_second)
            {
                return false;
            }
            local_unicode_char = 0x10000 + (
                ((local_unicode_char - 0xd800) << 10) |
                ((local_second - 0xdc00) & 0x3ff));
        }
        if (local_unicode_char < 0x80)
        {
            out_string.push_back(
                static_cast< template_string_type::value_type >(
                    local_unicode_char));
        }
        else
        {
            if (local_unicode_char < 0x800)
            {
                out_string.push_back(
                    static_cast< template_string_type::value_type >(
                        0xc0 | (local_unicode_char >> 6)));
            } else
            {
                if (local_unicode_char < 0x10000)
                {
                    out_string.push_back(
                        static_cast< template_string_type::value_type >(
                            0xe0 | (local_unicode_char >> 12)));
                }
                else
                {
                    out_string.push_back(
                        static_cast< template_string_type::value_type >(
                            0xf0 | (local_unicode_char >> 18)));
                    out_string.push_back(
                        static_cast< template_string_type::value_type >(
                            0x80 | ((local_unicode_char >> 12) & 0x3f)));
                }
                out_string.push_back(
                    static_cast< template_string_type::value_type >(
                        0x80 | ((local_unicode_char >> 6) & 0x3f)));
            }
            out_string.push_back(
                static_cast< template_string_type::value_type >(
                    0x80 | (local_unicode_char & 0x3f)));
        }
        return true;
    }

    /** @brief �������16�i���\�L4������́B
     */
    private: int parse_quad_hex()
    {
        int local_unicode_char(0);
        for (int i = 0; i < 4; i++)
        {
            int local_hex(this->read_char());
            if ('0' <= local_hex && local_hex <= '9')
            {
                local_hex -= '0';
            }
            else if ('A' <= local_hex && local_hex <= 'F')
            {
                local_hex -= 'A' - 0xa;
            }
            else if ('a' <= local_hex && local_hex <= 'f')
            {
                local_hex -= 'a' - 0xa;
            }
            else
            {
                if (local_hex != self::END_CHAR)
                {
                    this->undo_char();
                }
                return self::END_CHAR;
            }
            local_unicode_char = local_unicode_char * 16 + local_hex;
        }
        return local_unicode_char;
    }

    //-------------------------------------------------------------------------
    /** @brief 1�����ǂݍ��ށB
        @return �ǂݍ��񂾕����B
     */
    private: int read_char()
    {
        if (this->undo_)
        {
            this->undo_ = false;
            return this->last_char_;
        }
        if (this->current_ == this->end_)
        {
            this->last_char_ = self::END_CHAR;
            return self::END_CHAR;
        }
        if (this->last_char_ == '\n')
        {
            this->column_ = 1;
            ++this->line_;
        }
        else
        {
            ++this->column_;
        }
        this->last_char_ = *this->current_;
        ++this->current_;
        return this->last_char_;
    }

    private: void undo_char()
    {
        if (this->last_char_ != self::END_CHAR)
        {
            //assert(!this->undo_);
            this->undo_ = true;
        }
    }

    /** @brief �󔒕�����skip����B
     */
    private: void skip_white_space()
    {
        for (;;)
        {
            switch (this->read_char())
            {
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                break;

                default:
                this->undo_char();
                return;
            }
        }
    }

    private: bool expect(
        const int in_expect_char)
    {
        this->skip_white_space();
        if (this->read_char() != in_expect_char)
        {
            this->undo_char();
            return false;
        }
        return true;
    }

    private: bool match(
        const char* const in_begin)
    {
        for (const char* i = in_begin; *i != 0; ++i)
        {
            if (this->read_char() != *i)
            {
                this->undo_char();
                return false;
            }
        }
        this->skip_white_space();
        switch (this->last_char_)
        {
            case ',':
            case ']':
            case '}':
            case self::END_CHAR:
            return true;

            default:
            return false;
        }
    }

    private: enum { END_CHAR = -1 };

    //-------------------------------------------------------------------------
    private: template_iterator_type current_;
    private: template_iterator_type end_;
    private: int      last_char_;
    private: unsigned line_;
    private: unsigned column_;
    private: bool     undo_;
};

} // namespace json
} // namespace pbon

#endif // PBON_JSON_HPP_
