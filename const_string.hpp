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
/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_CONST_STRING_HPP_
#define PSYQ_CONST_STRING_HPP_

//#include "reference_string.hpp"

#ifndef PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT
#define PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT\
    std::allocator<typename template_char_traits::char_type>
#endif // !PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT

namespace psyq
{
    /// @cond
    template<
        typename template_char_type,
        typename template_char_traits = PSYQ_BASIC_REFERENCE_STRING_DEFAULT,
        typename template_allocator_type =
            PSYQ_BASIC_CONST_STRING_ALLOCATOR_DEFAULT>
                class basic_const_string;
    /// @endcond

    /// char�^�̕��������� basic_const_string
    typedef psyq::basic_const_string<char> const_string;

    /// wchar_t�^�̕��������� basic_const_string
    typedef psyq::basic_const_string<wchar_t> const_wstring;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief std::basic_string ��interface��͂����A������萔�B

    - constructor�ŕ���pointer��n����memory���蓖�Ă��s���A�������copy����B
    - ��L�ȊO�ł́Amemory���蓖�Ă���؍s��Ȃ��B
    - ������萔�Ȃ̂ŁA�����������������interface�͎����Ȃ��B

    @tparam template_char_type
        @copydoc psyq::basic_reference_string::value_type
    @tparam template_char_traits
        @copydoc psyq::basic_reference_string::traits_type
    @tparam template_allocator_type memory�����q�̌^�B

    @note �X���b�h�Z�[�t�ł͂Ȃ��̂ŁA����Ή��������B
 */
template<
    typename template_char_type,
    typename template_char_traits,
    typename template_allocator_type>
class psyq::basic_const_string:
    public psyq::basic_reference_string<
        template_char_type, template_char_traits>
{
    /// this���w���l�̌^�B
    public: typedef psyq::basic_const_string<
        template_char_type, template_char_traits, template_allocator_type>
            self;

    /// self �̏�ʌ^�B
    public: typedef psyq::basic_reference_string<
        template_char_type, template_char_traits>
            super;

    /// memory�����q�̌^�B
    public: typedef template_allocator_type allocator_type;

    //-------------------------------------------------------------------------
    /** @brief �󕶎�����\�z����Bmemory���蓖�Ă͍s��Ȃ��B
     */
    public: explicit basic_const_string(
        typename self::allocator_type const& in_allocator =
            self::allocator_type())
    :
        super(),
        holder_(NULL),
        allocator_(in_allocator)
    {
        // pass
    }

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_source copy���̕�����B
     */
    public: basic_const_string(
        self const& in_source)
    :
        super(in_source),
        holder_(in_source.holder_),
        allocator_(in_source.allocator_)
    {
        self::hold_string(in_source.holder_);
    }

    /** @brief ��������ړ�����Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in,out] io_source move���̕�����B
     */
    public: basic_const_string(
        self&& io_source)
    {
        this->swap(io_source);
    }

    /** @brief ������literal���Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @tparam template_size �Q�Ƃ��镶����literal�̗v�f���B
        @param[in] in_string    �Q�Ƃ��镶����literal�B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: template <std::size_t template_size>
    basic_const_string(
        typename self::value_type const (&in_string)[template_size],
        typename self::allocator_type const& in_allocator =
            self::allocator_type())
    :
        super(in_string),
        holder_(NULL),
        allocator_(in_allocator)
    {
        // pass
    }

    /** @brief ��������Q�Ƃ���Bmemory���蓖�Ă͍s��Ȃ��B
        @param[in] in_string �Q�Ƃ��镶����B
        @param[in] in_offset �Q�Ƃ��镶����̊J�noffset�ʒu�B
        @param[in] in_count  �Q�Ƃ��镶�����B
     */
    public: basic_const_string(
        self const&                    in_string,
        typename self::size_type const in_offset,
        typename self::size_type const in_count = super::npos)
    :
        super(
            in_string.data() + in_offset,
            super::trim_count(in_string, in_offset, in_count)),
        holder_(in_string.holder_),
        allocator_(in_string.allocator_)
    {
        self::hold_string(in_string.holder_);
    }

    /** @brief memory�����蓖�Ă��s���A�������copy����B
        @param[in] in_string    copy���̕�����̐擪�ʒu�B
        @param[in] in_size      copy���̕�����̒����B
        @param[in] in_allocator memory�����q�̏����l�B
     */
    public: basic_const_string(
        typename self::const_pointer const   in_string,
        typename self::size_type const       in_size,
        typename self::allocator_type const& in_allocator =
            self::allocator_type())
    :
        super(),
        holder_(NULL),
        allocator_(in_allocator)
    {
        if (in_size <= 0)
        {
            return;
        }

        // ������buffer�̊m�ہB
        typename self::holder_allocator local_allocator(this->allocator_);
        typename self::holder* const local_holder(
            local_allocator.allocate(self::count_allocate_size(in_size)));
        if (local_holder == NULL)
        {
            PSYQ_ASSERT(false);
            return;
        }

        // ������buffer���������B
        typename self::value_type* const local_string(
            reinterpret_cast<self::value_type*>(local_holder + 1));
        new(this) super(local_string, in_size);
        this->holder_ = local_holder;
        local_holder->refrence_count_ = 1;
        local_holder->string_length_ = in_size;
        PSYQ_ASSERT(in_string != NULL);
        super::traits_type::copy(local_string, in_string, in_size);
        local_string[in_size] = 0;
    }

    /// destructor
    public: ~basic_const_string()
    {
        typename self::holder* const local_holder(this->holder_);
        if (local_holder != NULL)
        {
            //bool const local_destroy(
            //    1 == std::atomic_fetch_sub_explicit(
            //        &local_holder->refrence_count_, 1, std::memory_order_release));
            //if (local_destroy)
            if ((--local_holder->refrence_count_) == 0)
            {
                //std::atomic_thread_fence(std::memory_order_acquire);
                local_holder->~holder();
                typename self::holder_allocator(this->allocator_).deallocate(
                    local_holder, self::count_allocate_size(this->size()));
            }
        }
    }

    //-------------------------------------------------------------------------
    public: self& operator=(self const& in_right)
    {
        if (this != &in_right)
        {
            self(in_right).swap(*this);
        }
        return *this;
    }

    public: self& operator=(self&& io_right)
    {
        this->swap(io_right);
        return *this;
    }

    public: typename self::allocator_type const& get_allocator() const
    {
        return this->allocator_;
    }

    /** @brief ���������������B
        @param[in,out] io_target �������镶����B
     */
    public: void swap(self& io_right)
    {
        this->super::swap(io_right);
        std::swap(this->holder_, io_right.holder_);
        std::swap(this->allocator_, io_right.allocator_);
    }

    //-------------------------------------------------------------------------
    /// ������ێ��q�B
    private: struct holder
    {
        std::size_t refrence_count_; ///< ������̔�Q�Ɛ��B
        std::size_t string_length_;  ///< �Q�Ƃ��Ă镶����̕������B
    };

    /// ������ێ��q��memory�����q�B
    private: typedef typename self::allocator_type::template
        rebind<typename self::holder>::other
            holder_allocator;

    private: static void hold_string(
        typename self::holder* const io_holder)
    {
        if (io_holder != NULL)
        {
            //std::atomic_fetch_add_explicit(
            //    &io_holder->refrence_count_, 1, std::memory_order_relaxed);
            ++io_holder->refrence_count_;
        }
    }

    private:
    static typename self::holder_allocator::size_type count_allocate_size(
        typename self::size_type const in_string_length)
    {
        std::size_t const local_holder_size(sizeof(typename self::holder));
        std::size_t const local_string_size(
            sizeof(typename self::value_type) * (in_string_length + 1) +
            local_holder_size - 1);
        return 1 + local_string_size / local_holder_size;
    }

    //-------------------------------------------------------------------------
    private: typename self::holder*        holder_;    ///< ������ێ��q�B
    private: typename self::allocator_type allocator_; ///< �g�p����memory�����q�B
};

//-----------------------------------------------------------------------------
namespace std
{
    /** @brief ���������������B
        @tparam template_char_type
            @copydoc psyq::basic_reference_string::value_type
        @tparam template_char_traits
            @copydoc psyq::basic_reference_string::traits_type
        @tparam template_allocator_type memory�����q�̌^�B
        @param[in] in_left  �������镶����B
        @param[in] in_right �������镶����B
     */
    template<
        typename template_char_type,
        typename template_char_traits,
        typename template_allocator_type>
    void swap(
        psyq::basic_const_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_left,
        psyq::basic_const_string<
            template_char_type, template_char_traits, template_allocator_type>&
                io_right)
    {
        io_left.swap(io_right);
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief CSV�`���̕��������͂��ASTL�݊��R���e�i�Ɋi�[����B
    @param[in,out] io_csv_rows         ��͂������ʂ��i�[����STL�݊��R���e�i�B
    @param[in]     in_csv_string       ��͂���CSV�`����STL�݊�������B
    @param[in]     in_field_separator  field��؂蕶���B
    @param[in]     in_record_separator record��؂蕶���B
    @param[in]     in_quote_begin      ���p���̊J�n�����B
    @param[in]     in_quote_end        ���p���̏I�������B
    @param[in]     in_quote_escape     ���p����escape�����B
 */
template<typename template_csv>
bool parse_csv(
    template_csv& io_csv_rows,
    typename template_csv::value_type::value_type const& in_csv_string,
    typename template_csv::value_type::value_type::value_type const
        in_field_separator = ',',
    typename template_csv::value_type::value_type::value_type const
        in_record_separator = '\n',
    typename template_csv::value_type::value_type::value_type const
        in_quote_begin = '"',
    typename template_csv::value_type::value_type::value_type const
        in_quote_end = '"',
    typename template_csv::value_type::value_type::value_type const
        in_quote_escape = '"')
{
    PSYQ_ASSERT(in_quote_escape != 0);

    io_csv_rows.push_back(typename template_csv::value_type());

    bool local_quote(false);
    template_csv::value_type::value_type local_field;
    template_csv::value_type::value_type::value_type local_last_char(0);
    for (auto i(in_csv_string.begin()); i != in_csv_string.end(); ++i)
    {
        if (local_quote)
        {
            if (local_last_char != in_quote_escape)
            {
                if (*i != in_quote_end)
                {
                    if (*i != in_quote_escape)
                    {
                        local_field.push_back(*i);
                    }
                    local_last_char = *i;
                }
                else
                {
                    // ���p�����I���B
                    local_quote = false;
                    local_last_char = 0;
                }
            }
            else if (*i == in_quote_end)
            {
                // ���p���̏I��������escape����B
                local_field.push_back(*i);
                local_last_char = 0;
            }
            else if (local_last_char == in_quote_end)
            {
                // ���p�����I�����A�����������߂��B
                local_quote = false;
                --i;
                local_last_char = 0;
            }
            else
            {
                local_field.push_back(local_last_char);
                local_field.push_back(*i);
                local_last_char = *i;
            }
        }
        else if (*i == in_quote_begin)
        {
            // ���p���̊J�n�B
            local_quote = true;
        }
        else if (*i == in_field_separator)
        {
            // field�̋�؂�B
            io_csv_rows.last().push_back(std::move(local_field));
            local_field.clear();
        }
        else if (*i == in_record_separator)
        {
            // record�̋�؂�B
            if (!local_field.empty() || !io_csv_rows.last().empty())
            {
                io_csv_rows.last().push_back(std::move(local_field));
            }
            local_field.clear();
            io_csv_rows.push_back(typename template_csv::value_type());
        }
        else
        {
            local_field.push_back(*i);
        }
    }

    // �ŏIfield�̏����B
    if (!local_field.empty())
    {
        if (local_quote)
        {
            //local_field.insert(local_field.begin(), in_quote_begin);
        }
        io_csv_rows.last().push_back(std::move(local_field));
    }
    else if (io_csv_rows.last().empty())
    {
        io_csv_rows.pop_back();
    }
}

#endif // PSYQ_CONST_STRING_HPP_
