/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_CSV_TABLE_HPP_
#define PSYQ_CSV_TABLE_HPP_

namespace psyq
{
    /// @cond
    struct csv_table_attribute;
    struct csv_table_key;
    template<typename, typename> class csv_table;
    /// @endcond
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief CSV表の属性の辞書の値の型。
struct psyq::csv_table_attribute
{
    std::size_t column; ///< 属性の列番号。
    std::size_t size;   ///< 属性の要素数。
};

/// @brief CSV表のcellの辞書のキーの型。
struct psyq::csv_table_key
{
    /** @param[in] in_row    @copydoc key::row
        @param[in] in_column @copydoc key::column
     */
    csv_table_key(
        std::size_t const in_row,
        std::size_t const in_column)
    :
        row(in_row),
        column(in_column)
    {}

    bool operator<(
        csv_table_key const& in_right)
    const
    {
        if (this->row != in_right.row)
        {
            return this->row < in_right.row;
        }
        return this->column < in_right.column;
    }

    bool operator <=(
        csv_table_key const& in_right)
    const
    {
        if (this->row != in_right.row)
        {
            return this->row < in_right.row;
        }
        return this->column <= in_right.column;
    }

    bool operator>(
        csv_table_key const& in_right)
    const
    {
        return in_right.operator<(*this);
    }

    bool operator>=(
        csv_table_key const& in_right)
    const
    {
        return in_right.operator<=(*this);
    }

    std::size_t row;    ///< cellの行番号。
    std::size_t column; ///< cellの列番号。
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<typename template_attribute_map, typename template_cell_map>
class psyq::csv_table
{
    private: typedef csv_table<template_cell_map, template_attribute_map> self;

    /** @brief CSV表の属性の辞書。

        以下の条件を満たしている必要がある。
        - std::map 互換の型。
        - attribute_map::key_type は、 std::basic_string 互換の文字列型。
        - attribute_map::mapped_type は、 csv_table_attribute 型。
     */
    public: typedef template_attribute_map attribute_map;

    /** @brief CSV表のcellの辞書。

        以下の条件を満たしている必要がある。
        - std::map 互換の型。
        - cell_map::key_type は、 csv_table_key 型。
        - cell_map::mapped_type は、 std::basic_string
          互換の文字列型の引数を1つだけ受け取るconstructorを持つ型。
     */
    public: typedef template_cell_map cell_map;

    //-------------------------------------------------------------------------
    /** @brief CSV形式の文字列を解析し、辞書に変換する。
        @tparam template_string std::basic_string 互換の文字列型。
        @param[in] in_csv_string       解析するCSV形式の文字列。
        @param[in] in_column_ceparator 列の区切り文字。
        @param[in] in_row_separator    行の区切り文字。
        @param[in] in_quote_begin      引用符の開始文字。
        @param[in] in_quote_end        引用符の終了文字。
        @param[in] in_quote_escape     引用符のescape文字。
     */
    public: template<typename template_string>
    explicit csv_table(
        template_string const&                     in_csv_string,
        typename template_string::value_type const in_column_ceparator = ',',
        typename template_string::value_type const in_row_separator = '\n',
        typename template_string::value_type const in_quote_begin = '"',
        typename template_string::value_type const in_quote_end = '"',
        typename template_string::value_type const in_quote_escape = '"');
    {
        PSYQ_ASSERT(in_quote_escape != 0);
        bool local_quote(false);
        std::size_t local_row(0);
        std::size_t local_column(0);
        std::size_t local_column_max(0);
        template_string local_field;
        typename template_string::value_type local_last_char(0);
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
                        // 引用符を終了。
                        local_quote = false;
                        local_last_char = 0;
                    }
                }
                else if (*i == in_quote_end)
                {
                    // 引用符の終了文字をescapeする。
                    local_field.push_back(*i);
                    local_last_char = 0;
                }
                else if (local_last_char == in_quote_end)
                {
                    // 引用符を終了し、文字を巻き戻す。
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
                // 引用符の開始。
                local_quote = true;
            }
            else if (*i == in_column_ceparator)
            {
                // 列の区切り。
                if (!local_field.empty())
                {
                    self::emplace_cell(
                        this->m_cell_map,
                        local_row,
                        local_column,
                        std::move(local_field));
                    local_field.clear();
                }
                ++local_column;
            }
            else if (*i == in_row_separator)
            {
                // 行の区切り。
                if (!local_field.empty())
                {
                    self::emplace_cell(
                        this->m_cell_map,
                        local_row,
                        local_column,
                        std::move(local_field));
                    local_field.clear();
                }
                if (local_column_max < local_column)
                {
                    local_column_max = local_column;
                }
                local_column = 0;
                ++local_row;
            }
            else
            {
                local_field.push_back(*i);
            }
        }

        // 引用符の開始はあったが終了がなかった場合。
        if (local_quote)
        {
            //local_field.insert(local_field.begin(), in_quote_begin);
        }

        // 最終fieldの処理。
        if (!local_field.empty())
        {
            self::emplace_cell(
                this->m_cell_map,
                local_row,
                local_column,
                std::move(local_field));
        }

        // CSV表の属性辞書を構築する。
        std::size_t const local_attributes_row(0);
        this->m_attribute_map = self::make_attribute_map(
            this->m_cell_map, local_attributes_row, local_column_max + 1);
    }

    private: template<typename template_string>
    static void emplace_cell(
        typename self::cell_map&      io_cells,
        std::size_t const             in_row,
        std::size_t const             in_column,
        template_string               in_field)
    {
        io_cells.emplace(
            typename self::cell_map::value_type(
                typename self::cell_map::key_type(
                    local_row, local_column),
                typename self::cell_map::mapped_type(std::move(in_field))));
    }

    private: static typename self::attribute_map make_attribute_map(
        typename self::cell_map const& in_cells,
        std::size_t const              in_row,
        std::size_t const              in_column_end)
    {
        typename self::attribute_map local_attributes;
        auto local_last_attribute(local_attributes.end());
        for (
            auto i(
                in_cells.lower_bound(
                    typename self::cell_map::key_type(in_row, 0)));
            i != in_cells.end() && i->first.row == in_row;
            ++i)
        {
            if (!local_attributes.empty())
            {
                local_last_attribute->second.size =
                    i->first.column - local_last_attribute->second.column;
            }
            local_last_attribute = local_attributes.emplace(
                typename self::attribute_map::value_type(
                    typename self::attribute_map::key_type(i->second),
                    typename self::attribute_map::mapped_type(
                        i->first.column, 0)));
        }
        if (!local_attributes.empty())
        {
            local_last_attribute->second.size =
                in_column_end - local_last_attribute->second.column;
        }
        return local_attributes;
    }

    //-------------------------------------------------------------------------
    public: typename self::attribute_map const& get_attribute_map() const
    {
        return this->m_attribute_map;
    }

    public: typename self::cell_map const& get_cell_map() const
    {
        return this->m_cell_map;
    }

    public: std::size_t get_row_size() const
    {
        return this->m_cell_map.empty()?
            0: (--this->m_cell_map.end())->first.row + 1;
    }

    //-------------------------------------------------------------------------
    /** @brief cellを検索する。
        @param[in] in_row             検索するcellの行番号。
        @param[in] in_attribute_key   検索するcellの属性名。
        @param[in] in_attribute_index 検索するcellの属性index番号。
     */
    public: typename self::cell_map::const_iterator find_cell(
        std::size_t const                       in_row,
        typename attribute_map::key_type const& in_attribute_key,
        std::size_t const                       in_attribute_index)
    const
    {
        const auto local_attribute(
            this->m_attribute_map.find(in_attribute_key));
        if (local_attribute == this->m_attribute_map.end() ||
            local_attribute->second.size <= in_attribute_index)
        {
            return this->m_cell_map.end();
        }
        return this->m_cell_map.find(
            typename self::cell_map::key_type(
                in_row, local_attribute->second.column + in_attribute_index));
    }

    //-------------------------------------------------------------------------
    private: typename self::attribute_map m_attribute_map;
    private: typename self::cell_map      m_cell_map;
};

#endif // PSYQ_CSV_TABLE_HPP_
