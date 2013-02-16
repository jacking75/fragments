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

    - Call pbon::json::Value::ImportJson(), import the JSON.
      pbon::json::Value::ImportJson() �ŁAJSON����荞�ށB

    - Call pbon::json::Value::ImportPbon(), import the PBON.
      pbon::json::Value::ImportPbon() �ŁAPBON����荞�ށB

    - Call pbon::json::Value::ExportJson(), export the JSON.
      pbon::json::Value::ExportJson() �ŁAJSON�������o���B

    - Call pbon::json::Value::ExportPbon(), export the PBON.
      pbon::json::Value::ExportPbon() �ŁAPBON�������o���B

    - Call pbon::Value::GetRoot(), get root value of PBON from packed-binary.
      pbon::Value::GetRoot() �ŁApacked-binary����PBON�̍ŏ�ʗv�f���擾����B

    @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PBON_JSON_HPP_
#define PBON_JSON_HPP_

namespace pbon
{

typedef std::int8_t  Int8;
typedef std::int16_t Int16;
typedef std::int32_t Int32;
typedef std::int64_t Int64;
typedef Int8  Char8;
typedef Int16 Char16;
typedef Int32 Char32;
typedef float  Float32;
typedef double Float64;

enum Type
{
    Type_NULL,
    Type_BOOL,
    Type_ARRAY,
    Type_OBJECT,
    Type_CHAR8 = 1 << 4,
    Type_CHAR16,
    Type_CHAR32,
    Type_INT8 = 2 << 4,
    Type_INT16,
    Type_INT32,
    Type_INT64,
    Type_FLOAT32 = (3 << 4) + 2,
    Type_FLOAT64,
};

template< typename template_Type > pbon::Type GetType()
{
    return pbon::Type_NULL;
}

template<> pbon::Type GetType< pbon::Char8 >()
{
    return pbon::Type_CHAR8;
}

template<> pbon::Type GetType< pbon::Char16 >()
{
    return pbon::Type_CHAR16;
}

template<> pbon::Type GetType< pbon::Int32 >()
{
    return pbon::Type_INT32;
}

template<> pbon::Type GetType< pbon::Int64 >()
{
    return pbon::Type_INT64;
}

template<> pbon::Type GetType< pbon::Float32 >()
{
    return pbon::Type_FLOAT32;
}

template<> pbon::Type GetType< pbon::Float64 >()
{
    return pbon::Type_FLOAT64;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief packed-binary�̗v�f�B
    @tparam template_AttributeType @copydoc pbon::Value::Attribute
 */
template< typename template_AttributeType >
class Value
{
    /// this���w���^�B
    public: typedef Value This;

    /// packed-binary�̑����̌^�B
    public: typedef template_AttributeType Attribute;

    /// packed-binary��header���B
    private: struct Header
    {
        pbon::Int32 Endian_;
        pbon::Int16 Type_;
        pbon::Int16 Root_;
    };

    /** @brief packed-binary�̍ŏ�ʗv�f���擾�B
        @param[in] in_PackedBinary packed-binary�̐擪�ʒu�B
        @retval !=NULL packed-binary�̍ŏ�ʗv�f�ւ�pointer�B
        @retval ==NULL �����Ȃ�packed-binary�������B
     */
    public: static const This* GetRoot(
        const void* const in_PackedBinary)
    {
        if (in_PackedBinary == NULL)
        {
            return NULL;
        }
        const This::Header& local_Header(
            *static_cast< const This::Header* >(in_PackedBinary));
        if (local_Header.Endian_ != 'pbon')
        {
            // endian���قȂ�̂ň����Ȃ��B
            return NULL;
        }
        if (local_Header.Type_ != pbon::GetType< This::Attribute >())
        {
            // �����̌^���قȂ�̂ň����Ȃ��B
            return NULL;
        }
        return This::GetAddress< This >(in_PackedBinary, local_Header.Root_);
    }

    /** @brief ��ʗv�f���擾�B
        @retval !=NULL ��ʗv�f�ւ�pointer�B
        @retval ==NULL ��ʗv�f���Ȃ��B
     */
    public: const This* GetSuper() const
    {
        if (this->Super_ == 0)
        {
            return NULL;
        }
        return This::GetAddress< This >(this, this->Super_);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t GetSize() const
    {
        return this->Size_;
    }

    /** @brief �����Ă���l�̌^���擾�B
     */
    public: pbon::Type GetType() const
    {
        return static_cast< pbon::Type >(this->Type_);
    }

    public: bool IsArray() const
    {
        return this->GetType() == pbon::Type_ARRAY;
    }

    public: bool IsObject() const
    {
        return this->GetType() == pbon::Type_OBJECT;
    }

    /** @brief �����Ă���l�ւ�pointer���擾�B
        @tparam template_ValueType �����Ă���l�̌^�B
     */
    protected: template< typename template_ValueType >
    const template_ValueType* GetValue() const
    {
        return This::GetAddress< template_ValueType >(this, this->Value_);
    }

    /** @brief ���Έʒu����address���擾�B
        @tparam template_ValueType    pointer���w���l�̌^�B
        @tparam template_PositionType ���Έʒu�̌^�B
        @param[in] in_BaseAddress     ��ʒu�ƂȂ�pointer�B
        @param[in] in_BytePosition    ��ʒu�����byte�P�ʂł̑��Έʒu�B
     */
    private: template<
        typename template_ValueType,
        typename template_PositionType >
    static const template_ValueType* GetAddress(
        const void* const           in_BaseAddress,
        const template_PositionType in_BytePosition)
    {
        return reinterpret_cast< const template_ValueType* >(
            static_cast< const char* >(in_BaseAdress) + in_BytePosition);
    }

    private: typename This::Attribute Value_; ///< �l�B�������͒l�ւ̑��Έʒu�B
    private: typename This::Attribute Size_;  ///< �l�̐��B
    private: typename This::Attribute Type_;  ///< �l�̌^�B
    private: typename This::Attribute Super_; ///< ��ʗv�f�ւ̑��Έʒu�B
};
typedef pbon::Value< pbon::Int32 > Value32;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// pbon::Value �̔z��B
template< typename template_AttributeType >
class Array:
    private pbon::Value< template_AttributeType >
{
    public: typedef pbon::Array< template_AttributeType > This;
    private: typedef pbon::Value< template_AttributeType > Super;
    public: typedef Super Value;

    using Super::Attribute;
    using Super::GetSuper;

    public: static const This* Cast(
        const Super* const in_Value)
    {
        if (in_Value == NULL || !in_Value->IsArray())
        {
            return NULL;
        }
        return static_cast< const This* >(in_Value);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t GetSize() const
    {
        if (!this->IsArray())
        {
            return 0;
        }
        return this->Super::GetSize();
    }

    public: const typename This::Value* GetBegin() const
    {
        if (!this->IsArray())
        {
            return NULL;
        }
        return this->GetValue< typename This::Value >();
    }

    public: const typename This::Value* GetEnd() const
    {
        if (!this->IsArray())
        {
            return NULL;
        }
        return this->GetValue< typename This::Value >() + this->GetSize();
    }

    public: const typename This::Value* At(
        const std::size_t in_Index)
    {
        if (!this->IsArray() || this->GetSize() <= in_Index)
        {
            return NULL;
        }
        return this->GetValue< This::Value >() + in_Index;
    }
};
typedef pbon::Array< pbon::Int32 > Array32;

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// pbon::Value �̎����B
template< typename template_AttributeType >
class Object:
    private Value< template_AttributeType >
{
    public: typedef pbon::Object< template_AttributeType > This;
    private: typedef pbon::Value< template_AttributeType > Super;
    public: typedef std::pair< Super, Super > Value;

    using Super::Attribute;
    using Super::GetSuper;

    public: static const This* Cast(
        const Super* const in_Value)
    {
        if (in_Value == NULL || !in_Value->IsObject())
        {
            return NULL;
        }
        return static_cast< const This* >(in_Value);
    }

    /** @brief �����Ă���l�̐����擾�B
     */
    public: std::size_t GetSize() const
    {
        if (!this->IsObject())
        {
            return 0;
        }
        return this->Super::GetSize() / 2;
    }

    public: const typename This::Value* GetBegin() const
    {
        if (!this->IsObject())
        {
            return NULL;
        }
        return this->GetValue< typename This::Value >();
    }

    public: const typename This::Value* GetEnd() const
    {
        if (!this->IsObject())
        {
            return NULL;
        }
        return this->GetValue< typename This::Value >() + this->GetSize();
    }

    public: template< typename template_KeyType >
    const typename This::Value* LowerBound(
        const template_KeyType& in_Key) const;

    public: template< typename template_KeyType >
    const typename This::Value* UpperBound(
        const template_KeyType& in_Key) const;

    public: template< typename template_KeyType >
    const typename This::Value* Find(
        const template_KeyType& in_Key) const;
};
typedef pbon::Object< pbon::Int32 > Object32;

namespace json
{

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief JSON�̗v�f�B
 */
class Value
{
    /// this���w���l�̌^�B
    public: typedef pbon::json::Value This;

    private: enum Type
    {
        Type_NULL,
        Type_BOOL,
        Type_INT,
        Type_FLOAT,
        Type_HOLDER,
    };

    private: class Placeholder
    {
        public: virtual void DeleteSelf() = 0;
        public: virtual Placeholder* Clone() = 0;
    };

    //-------------------------------------------------------------------------
    private: union
    {
        bool          Bool_;
        pbon::Int64   Int_;
        pbon::Float64 Float_;
        Placeholder*  Holder_;
    };
    private: pbon::Int8 Type_;

    //-------------------------------------------------------------------------
    public: Value():
    Type_(pbon::Type_NULL)
    {
        this->Int_ = 0;
    }

    public: explicit Value(
        const bool in_Bool):
    Type_(pbon::Type_BOOL)
    {
        this->Bool_ = in_Bool;
    }

    ~Value()
    {
        if (this->Type_ == Type_HOLDER)
        {
            this->Holder_->DeleteSelf();
        }
    }

    void Swap(This& io_Target)
    {
        std::swap(this->Int_, io_Target.Int_);
        std::swap(this->Type_, io_Target.Type_);
    }

    //-------------------------------------------------------------------------
    /** @brief JSON�`���̕����񂩂�l�����o���B
        @tparam template_NumberType JSON�̒l�Ɏg�����l�̌^�B
        @tparam template_StringType std::basic_string �݊��̕�����^�B
        @param[in] in_JsonString �l����荞��JSON�`���̕�����B
        @return
        - ���������ꍇ��(0, 0)�B
        - ���s�����ꍇ�́A��荞�݂Ɏ��s���������ʒu��(�s�ԍ�, ���ʒu)�B
     */
    public: template<
        typename template_NumberType,
        typename template_StringType >
    std::pair< unsigned, unsigned > ImportJson(
        const template_StringType in_JsonString)
    {
        return this->ImportJson< template_NumberType, template_StringType >(
            in_JsonString.begin(), in_JsonString.end());
    }

    /** @brief JSON�`���̕����񂩂�l�����o���B
        @tparam template_NumberType JSON�̒l�Ɏg�����l�̌^�B
        @tparam template_StringType
            JSON�̒l�Ɏg��������̌^�Bstd::basic_string �݊����K�{�B
        @tparam template_IteratorType JSON�̉�͂Ŏg�������q�̌^�B
        @param[in] in_JsonBegin �l����荞��JSON�`���̕�����̐擪�ʒu�B
        @param[in] in_JsonEnd   �l����荞��JSON�`���̕�����̖����ʒu�B
        @return
        - ���������ꍇ��(0, 0)�B
        - ���s�����ꍇ�́A��荞�݂Ɏ��s���������ʒu��(�s�ԍ�, ���ʒu)�B
     */
    public: template<
        typename template_NumberType,
        typename template_StringType,
        typename template_IteratorType >
    std::pair< unsigned, unsigned > ImportJson(
        const template_IteratorType& in_JsonBegin,
        const template_IteratorType& in_JsonEnd)
    {
        typedef template_IteratorType Iterator;
        typedef template_NumberType Number;
        typedef template_StringType String;
        typedef String::allocator_type::rebind< This >::other VectorAllocator;
        typedef std::vector< This, VectorAllocator > Vector;
        typedef String Key;
        typedef VectorAllocator::rebind< std::pair< Key, This > >::other
            MapAllocator;
        typedef std::map< Key, This, std::less< Key >, MapAllocator >
            Map;
        This::JsonParser< Iterator, Number, String, Vector, Map >
            local_Parser(in_JsonBegin, in_JsonEnd);
        if (!local_Parser.Parse(*this))
        {
            std::make_pair(local_Parser.GetLine(), local_Parser.GetColumn());
        }
        return std::make_pair(unsigned(0), unsigned(0));
    }

    /** @brief PBON�`����binary�񂩂�l�����o���B
        @param[in] in_PbonBinary �l����荞��PBON�`����binary��B
     */
    public: template< typename template_ArrayType >
    void ImportPbon(
        const template_ArrayType& in_PbonBinary);

    /** @brief JSON�`���ŕ�����ɒl�������o���B
        @param[out] out_JsonString �l�������o����B
     */
    public: template< typename template_StringType >
    void ExportJson(
        template_StringType& out_JsonString) const;

    /** @brief PBON�`����binary�ɒl�������o���B
        @param[out] out_PbonBinary �l�������o����B
     */
    public: template< typename template_ArrayType >
    void ExportPbon(template_ArrayType& out_PbonBinary) const;

    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
    /** @brief JSON��͊�B
        @tparam template_IteratorType ��͂Ɏg�������q�̌^�B
        @tparam template_NumberType   JSON�̒l�Ŏg�����l�̌^�B
        @tparam template_StringType   JSON�̒l�Ŏg��������̌^�B
        @tparam template_ArrayType    JSON�̒l�Ŏg���z��̌^�B
        @tparam template_ObjectType   JSON�̒l�Ŏg��object�̌^�B
     */
    private: template<
        typename template_IteratorType,
        typename template_NumberType,
        typename template_StringType,
        typename template_ArrayType,
        typename template_ObjectType >
    class JsonParser
    {
        /// this���w���l�̌^�B
        public: typedef JsonParser<
            template_IteratorType,
            template_NumberType,
            template_StringType,
            template_ArrayType,
            template_ObjectType >
                This;

        private: enum { END_CHAR = -1 };

        //---------------------------------------------------------------------
        /** @param[in] in_Begin ��͂��镶����̐擪�ʒu�B
            @param[in] in_End   ��͂��镶����̖����ʒu�B
         */
        public: JsonParser(
            const template_IteratorType& in_Begin,
            const template_IteratorType& in_End):
        Current_(in_Begin),
        End_(in_End),
        LastChar_(This::END_CHAR),
        Line_(1),
        Column_(1),
        Undo_(false)
        {
            // pass
        }

        //---------------------------------------------------------------------
        /** @brief ��͒��̍s�ԍ����擾�B
            @return ��͒��̍s�ԍ��B�K��1�ȏ�B
         */
        public: unsigned GetLine() const
        {
            return this->Line_;
        }

        /** @brief ��͒��̍s�̌��ʒu���擾�B
            @return ��͒��̍s�̌��ʒu�B�K��1�ȏ�B
         */
        public: unsigned GetColumn() const
        {
            return this->Column_;
        }

        //---------------------------------------------------------------------
        /** @brief JSON�������Ă���l����͂��Ď��o���B
            @param[out] out_Value JSON������o�����l�̏o�͐�B
            @retval !=false �����B
            @retval ==false ���s�B�l�͏o�͂���Ȃ��B
         */
        public: bool Parse(
            pbon::json::Value& out_Value)
        {
            this->SkipWhiteSpace();
            const int local_Char(this->ReadChar());
            switch (local_Char)
            {
                case '"':
                return this->ParseString(out_Value);

                case '[':
                return this->ParseArray(out_Value);

                case '{':
                return this->ParseObject(out_Value);

                case 'n':
                if (this->Match("ull"))
                {
                    pbon::json::Value().Swap(out_Value);
                    return true;
                }
                return false;

                case 't':
                if (this->Match("rue"))
                {
                    pbon::json::Value(true).Swap(out_Value);
                    return true;
                }
                return false;

                case 'f':
                if (this->Match("alse"))
                {
                    pbon::json::Value(false).Swap(out_Value);
                    return true;
                }
                return false;

                default:
                this->UndoChar();
                if (('0' <= local_Char && local_Char <= '9') ||
                    local_Char == '-')
                {
                    return this->ParseNumber(out_Value);
                }
                return false;
            }
        }

        /** @brief JSON�������Ă���z�����͂��Ď��o���B
            @param[out] out_Value JSON������o�����l�̏o�͐�B
            @retval !=false �����B
            @retval ==false ���s�B�l�͏o�͂���Ȃ��B
         */
        private: bool ParseArray(pbon::json::Value&)
        {
            template_ArrayType auto_Array;
            if (!this->Expect(']'))
            {
                for (;;)
                {
                    auto_Array.push_back(pbon::json::Value());
                    if (!this->Parse(auto_Array.back()))
                    {
                        return false;
                    }
                    if (!this->Expect(','))
                    {
                        break;
                    }
                }
            }
            //pbon::json::Value(auto_Array).swap(out_Value);
            return this->Expect(']');
        }

        /** @brief JSON�������Ă���object����͂��Ď��o���B
            @param[out] out_Value JSON������o�����l�̏o�͐�B
            @retval !=false �����B
            @retval ==false ���s�B�l�͏o�͂���Ȃ��B
         */
        private: bool ParseObject(pbon::json::Value&)
        {
            return false;
        }

        /** @brief JSON�������Ă��鐔�l����͂��Ď��o���B
            @param[out] out_Value JSON������o�����l�̏o�͐�B
            @retval !=false �����B
            @retval ==false ���s�B�l�͏o�͂���Ȃ��B
         */
        private: bool ParseNumber(pbon::json::Value&)
        {
            return false;
        }

        /** @brief JSON�������Ă��镶�������͂��Ď��o���B
            @param[out] out_Value JSON������o�����l�̏o�͐�B
            @retval !=false �����B
            @retval ==false ���s�B�l�͏o�͂���Ȃ��B
         */
        private: bool ParseString(
            pbon::json::Value&)
        {
            template_StringType local_String;
            for (;;)
            {
                int auto_Char(this->ReadChar());
                if (auto_Char < ' ')
                {
                    this->UndoChar();
                    return false;
                }
                if (auto_Char == '"')
                {
                    //pbon::json::Value(local_String).Swap(out_Value);
                    return true;
                }
                if (auto_Char == '\\')
                {
                    auto_Char = this->ReadChar();
                    switch (auto_Char)
                    {
                        case '"':
                        break;

                        case '\\':
                        break;

                        case '/':
                        break;

                        case 'b':
                        auto_Char = '\b';
                        break;

                        case 'f':
                        auto_Char = '\f';
                        break;

                        case 'n':
                        auto_Char = '\n';
                        break;

                        case 'r':
                        auto_Char = '\r';
                        break;

                        case 't':
                        auto_Char = '\t';
                        break;

                        case 'u':
                        if (this->ParseCodePoint(local_String))
                        {
                            continue;
                        }
                        return false;

                        default:
                        return false;
                    }
                }
                local_String.push_back(
                    static_cast< typename template_StringType::value_type >(
                        auto_Char));
            }
        }

        /** @brief �������code-point�\�L����́B
         */
        private: bool ParseCodePoint(
            template_StringType& out_String)
        {
            int local_UnicodeChar(this->ParseQuadHex());
            if (local_UnicodeChar == This::END_CHAR)
            {
                return false;
            }
            if (0xd800 <= local_UnicodeChar && local_UnicodeChar <= 0xdfff)
            {
                // 16bit surrogate pair�̌㔼������B
                if (0xdc00 <= local_UnicodeChar)
                {
                    return false;
                }

                // 16bit surrogate pair�̑O���������B
                if (this->ReadChar() != '\\' || this->ReadChar() != 'u')
                {
                    this->UndoChar();
                    return false;
                }
                const int auto_Second(this->ParseQuadHex());
                if (auto_Second < 0xdc00 || 0xdfff < auto_Second)
                {
                    return false;
                }
                local_UnicodeChar = 0x10000 + (
                    ((local_UnicodeChar - 0xd800) << 10) |
                    ((auto_Second - 0xdc00) & 0x3ff));
            }
            if (local_UnicodeChar < 0x80)
            {
                out_String.push_back(
                    static_cast< template_StringType::value_type >(
                        local_UnicodeChar));
            }
            else
            {
                if (local_UnicodeChar < 0x800)
                {
                    out_String.push_back(
                        static_cast< template_StringType::value_type >(
                            0xc0 | (local_UnicodeChar >> 6)));
                } else
                {
                    if (local_UnicodeChar < 0x10000)
                    {
                        out_String.push_back(
                            static_cast< template_StringType::value_type >(
                                0xe0 | (local_UnicodeChar >> 12)));
                    }
                    else
                    {
                        out_String.push_back(
                            static_cast< template_StringType::value_type >(
                                0xf0 | (local_UnicodeChar >> 18)));
                        out_String.push_back(
                            static_cast< template_StringType::value_type >(
                                0x80 | ((local_UnicodeChar >> 12) & 0x3f)));
                    }
                    out_String.push_back(
                        static_cast< template_StringType::value_type >(
                            0x80 | ((local_UnicodeChar >> 6) & 0x3f)));
                }
                out_String.push_back(
                    static_cast< template_StringType::value_type >(
                        0x80 | (local_UnicodeChar & 0x3f)));
            }
            return true;
        }

        /** @brief �������16�i���\�L4������́B
         */
        private: int ParseQuadHex()
        {
            int local_UnicodeChar(0);
            for (int i = 0; i < 4; i++)
            {
                int local_Hex(this->ReadChar());
                if ('0' <= local_Hex && local_Hex <= '9')
                {
                    local_Hex -= '0';
                }
                else if ('A' <= local_Hex && local_Hex <= 'F')
                {
                    local_Hex -= 'A' - 0xa;
                }
                else if ('a' <= local_Hex && local_Hex <= 'f')
                {
                    local_Hex -= 'a' - 0xa;
                }
                else
                {
                    if (local_Hex != This::END_CHAR)
                    {
                        this->UndoChar();
                    }
                    return This::END_CHAR;
                }
                local_UnicodeChar = local_UnicodeChar * 16 + local_Hex;
            }
            return local_UnicodeChar;
        }

        //---------------------------------------------------------------------
        private: int ReadChar()
        {
            if (this->Undo_)
            {
                this->Undo_ = false;
                return this->LastChar_;
            }
            if (this->Current_ == this->End_)
            {
                this->LastChar_ = This::END_CHAR;
                return This::END_CHAR;
            }
            if (this->LastChar_ == '\n')
            {
                this->Column_ = 1;
                ++this->Line_;
            }
            else
            {
                ++this->Column_;
            }
            this->LastChar_ = *this->Current_;
            ++this->Current_;
            return this->LastChar_;
        }

        private: void UndoChar()
        {
            if (this->LastChar_ != This::END_CHAR)
            {
                //assert(!this->Undo_);
                this->Undo_ = true;
            }
        }

        private: void SkipWhiteSpace()
        {
            for (;;)
            {
                switch (this->ReadChar())
                {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                    break;

                    default:
                    this->UndoChar();
                    return;
                }
            }
        }

        private: bool Expect(
            const int in_ExpectChar)
        {
            this->SkipWhiteSpace();
            if (this->ReadChar() != in_ExpectChar)
            {
                this->UndoChar();
                return false;
            }
            return true;
        }

        private: bool Match(
            const char* const in_Begin)
        {
            for (const char* i = in_Begin; *i != 0; ++i)
            {
                if (this->ReadChar() != *i)
                {
                    this->UndoChar();
                    return false;
                }
            }
            this->SkipWhiteSpace();
            switch (this->LastChar_)
            {
                case ',':
                case ']':
                case '}':
                case This::END_CHAR:
                return true;

                default:
                return false;
            }
        }

        //---------------------------------------------------------------------
        private: template_IteratorType Current_;
        private: template_IteratorType End_;
        private: int      LastChar_;
        private: unsigned Line_;
        private: unsigned Column_;
        private: bool     Undo_;
    };
};
} // namespace json
} // namespace pbon

#endif // PBON_JSON_HPP_
