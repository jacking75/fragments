﻿/// @file
/// @brief @copybrief psyq::if_then_engine::_private::evaluator
/// @author Hillco Psychi (https://twitter.com/psychi)
#ifndef PSYQ_IF_THEN_ENGINE_EVALUATOR_HPP_
#define PSYQ_IF_THEN_ENGINE_EVALUATOR_HPP_

#include <unordered_map>
#include <vector>
#include "../hash/primitive_bits.hpp"
#include "./expression.hpp"

/// @cond
namespace psyq
{
    namespace if_then_engine
    {
        namespace _private
        {
            template<typename, typename> class evaluator;
        } // namespace _private
    } // namespace if_then_engine
} // namespace psyq
/// @endcond

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief 条件評価器。条件式を保持し、評価する。
/// @par 使い方の概略
///   - evaluator::register_expression で、条件式を登録する。
///   - evaluator::evaluate_expression で、条件式を評価する。
/// @tparam template_reservoir      @copydoc evaluator::reservoir
/// @tparam template_expression_key @copydoc evaluator::expression_key
template<typename template_reservoir, typename template_expression_key>
class psyq::if_then_engine::_private::evaluator
{
    /// @brief thisが指す値の型。
    private: typedef evaluator this_type;

    //-------------------------------------------------------------------------
    /// @brief 条件評価器で使う _private::reservoir 。
    public: typedef template_reservoir reservoir;
    /// @brief 条件評価器で使うメモリ割当子。
    public: typedef
        typename this_type::reservoir::allocator_type
        allocator_type;

    //-------------------------------------------------------------------------
    /// @brief 条件評価器で使う条件式の識別値。
    public: typedef template_expression_key expression_key;
    /// @brief 条件評価器で使う条件式。
    public: typedef
        psyq::if_then_engine::_private::expression<
            typename template_reservoir::status_value::evaluation,
            typename template_reservoir::chunk_key,
            std::uint32_t>
        expression;

    //-------------------------------------------------------------------------
    /// @brief 条件式が参照する要素条件チャンク。
    public: typedef
        psyq::if_then_engine::_private::expression_chunk<
            std::vector<
                psyq::if_then_engine::_private::sub_expression<
                    typename this_type::expression_key>,
                typename evaluator::allocator_type>,
            std::vector<
                psyq::if_then_engine::_private::status_transition<
                    typename this_type::reservoir::status_key>,
                typename this_type::allocator_type>,
            std::vector<
                typename this_type::reservoir::status_comparison,
                typename this_type::allocator_type>>
        chunk;
    /// @brief 要素条件チャンクの識別値。
    public: typedef typename this_type::reservoir::chunk_key chunk_key;

    //-------------------------------------------------------------------------
    /// @brief 条件式の辞書。
    private: typedef
        std::unordered_map<
            typename this_type::expression_key,
            typename this_type::expression,
            psyq::hash::primitive_bits<typename this_type::expression_key>,
            std::equal_to<typename this_type::expression_key>,
            typename this_type::allocator_type>
        expression_map;
    /// @brief 要素条件チャンクの辞書。
    private: typedef
        std::unordered_map<
            typename this_type::chunk_key,
            typename this_type::chunk,
            psyq::hash::primitive_bits<typename this_type::chunk_key>,
            std::equal_to<typename this_type::chunk_key>,
            typename this_type::allocator_type>
        chunk_map;

    //-------------------------------------------------------------------------
    /// @name 構築と代入
    /// @{

    /// @brief 空の条件評価器を構築する。
    public: evaluator(
        /// [in] チャンク辞書のバケット数。
        std::size_t const in_chunk_count,
        /// [in] 条件式辞書のバケット数。
        std::size_t const in_expression_count,
        /// [in] メモリ割当子の初期値。
        typename this_type::allocator_type const& in_allocator =
            typename this_type::allocator_type()):
    chunks_(
        in_chunk_count,
        typename this_type::chunk_map::hasher(),
        typename this_type::chunk_map::key_equal(),
        in_allocator),
    expressions_(
        in_expression_count,
        typename this_type::expression_map::hasher(),
        typename this_type::expression_map::key_equal(),
        in_allocator)
    {}

#ifdef PSYQ_NO_STD_DEFAULTED_FUNCTION
    /// @brief ムーブ構築子。
    public: evaluator(
        /// [in,out] ムーブ元となるインスタンス。
        this_type&& io_source):
    chunks_(std::move(io_source.chunks_)),
    expressions_(std::move(io_source.expressions_))
    {}

    /// @brief ムーブ代入演算子。
    /// @return *this
    public: this_type& operator=(
        /// [in,out] ムーブ元となるインスタンス。
        this_type&& io_source)
    {
        this->chunks_ = std::move(io_source.chunks_);
        this->expressions_ = std::move(io_source.expressions_);
        return *this;
    }
#endif // defined(PSYQ_NO_STD_DEFAULTED_FUNCTION)

    /// @brief 条件評価器で使われているメモリ割当子を取得する。
    /// @return *this で使われているメモリ割当子のコピー。
    public: typename this_type::allocator_type get_allocator()
    const PSYQ_NOEXCEPT
    {
        return this->expressions_.get_allocator();
    }

    /// @brief 条件評価器を再構築する。
    public: void rebuild(
        /// [in] 要素条件チャンク辞書のバケット数。
        std::size_t const in_chunk_count,
        /// [in] 条件式辞書のバケット数。
        std::size_t const in_expression_count)
    {
        this->expressions_.rehash(in_expression_count);
        this->chunks_.rehash(in_chunk_count);
        for (auto& local_chunk: this->chunks_)
        {
            local_chunk.second.sub_expressions_.shrink_to_fit();
            local_chunk.second.status_transitions_.shrink_to_fit();
            local_chunk.second.status_comparisons_.shrink_to_fit();
        }
    }
    /// @}
    //-------------------------------------------------------------------------
    /// @name 条件式
    /// @{

    /// @brief 条件式が登録されているか判定する。
    /// @retval true  in_expression_key に対応する条件式が *this に登録されている。
    /// @retval false in_expression_key に対応する条件式は *this に登録されてない。
    public: bool is_registered(
        /// [in] 判定する状態値に対応する識別値。
        typename this_type::expression_key const& in_expression_key)
    const
    {
        return this->expressions_.find(in_expression_key)
            != this->expressions_.end();
    }

    /// @brief 条件式を登録する。
    /// @sa this_type::evaluate_expression で、登録した条件式を評価できる。
    /// @sa this_type::erase_chunk で、登録した条件式をチャンク単位で削除できる。
    /// @retval true 成功。条件式を *this に登録した。
    /// @retval false
    ///   失敗。条件式は登録されなかった。
    ///   - in_expression_key に対応する条件式が既にあると失敗する。
    ///   - in_elements_begin と in_elements_end が等価だと失敗する。
    public: template<typename template_iterator>
    bool register_expression(
        /// [in] 条件式を登録する要素条件チャンクの識別値。
        typename this_type::chunk_key const& in_chunk_key,
        /// [in] 登録する条件式の識別値。
        typename this_type::expression_key const& in_expression_key,
        /// [in] 要素条件を結合する論理演算子。
        typename this_type::expression::logic const in_logic,
        /// [in] 登録する条件式の要素条件コンテナの先頭を指す反復子。
        template_iterator const& in_elements_begin,
        /// [in] 登録する条件式の要素条件コンテナの末尾を指す反復子。
        template_iterator const& in_elements_end)
    {
        PSYQ_ASSERT(
            this_type::is_valid_elements(
                in_elements_begin, in_elements_end, this->expressions_));
        if (in_elements_begin == in_elements_end
            || this->is_registered(in_expression_key))
        {
            return false;
        }

        // 要素条件の種類を判定する。
        auto const local_emplace_chunk(
            this->chunks_.emplace(
                in_chunk_key,
                typename this_type::chunk_map::mapped_type(
                    this->chunks_.get_allocator())));
        auto const local_element_kind(
            this_type::make_element_kind(
                local_emplace_chunk.first->second, *in_elements_begin));

        // 要素条件を挿入する。
        auto& local_elements(*local_element_kind.second);
        auto const local_begin_index(
            static_cast<typename this_type::expression::element_index>(
                local_elements.size()));
        PSYQ_ASSERT(local_begin_index == local_elements.size());
        local_elements.insert(
            local_elements.end(), in_elements_begin, in_elements_end);

        // 条件式を挿入する。
        auto const local_emplace_expression(
            this->expressions_.emplace(
                in_expression_key,
                typename this_type::expression_map::mapped_type(
                    in_chunk_key,
                    in_logic,
                    local_element_kind.first,
                    local_begin_index,
                    static_cast<typename this_type::expression::element_index>(
                        local_elements.size()))));
        PSYQ_ASSERT(
            local_emplace_expression.second
            && local_emplace_expression.first->second.get_end_element()
                == local_elements.size()
            && local_begin_index
                < local_emplace_expression.first->second.get_end_element());
        return local_emplace_expression.second;
    }

    /// @brief 条件式を登録する。
    /// @sa this_type::evaluate_expression で、登録した条件式を評価できる。
    /// @sa this_type::erase_chunk で、登録した条件式をチャンク単位で削除できる。
    /// @retval true 成功。条件式を *this に登録した。
    /// @retval false
    ///   失敗。条件式は登録されなかった。
    ///   - in_expression_key に対応する条件式が既にあると失敗する。
    ///   - in_elements が空だと失敗する。
    public: template<typename template_element_container>
    bool register_expression(
        /// [in] 条件式を登録する要素条件チャンクの識別値。
        typename this_type::chunk_key const& in_chunk_key,
        /// [in] 登録する条件式の識別値。
        typename this_type::expression_key const& in_expression_key,
        /// [in] 要素条件を結合する論理演算子。
        typename this_type::expression::logic const in_logic,
        /// [in] 登録する条件式の要素条件コンテナ。
        template_element_container const& in_elements)
    {
        return this->register_expression(
            in_chunk_key,
            in_expression_key,
            in_logic,
            std::begin(in_elements),
            std::end(in_elements));
    }

    /// @brief 状態値を比較する条件式を登録する。
    /// @sa this_type::evaluate_expression で、登録した条件式を評価できる。
    /// @sa this_type::erase_chunk で、登録した条件式をチャンク単位で削除できる。
    /// @retval true
    ///   成功。条件式を *this に登録した。
    ///   条件式を登録した要素条件チャンクの識別値は、 in_comparison.get_key()
    ///   に対応する状態値が登録されている状態値ビット列チャンクの識別値と同じ。
    /// @retval false
    ///   失敗。条件式は登録されなかった。
    ///   - in_expression_key に対応する条件式が既にあると失敗する。
    ///   - in_comparison.get_key() に対応する状態値が
    ///     in_reservoir にないと失敗する。
    public: bool register_expression(
        /// [in] 条件式が参照する状態貯蔵器。
        typename this_type::reservoir const& in_reservoir,
        /// [in] 登録する条件式の識別値。
        typename this_type::expression_key const& in_expression_key,
        /// [in] 登録する状態比較要素条件。
        typename this_type::reservoir::status_comparison const& in_comparison)
    {
        auto const local_status_property(
            in_reservoir.find_property(in_comparison.get_key()));
        return !local_status_property.is_empty()
            && this->register_expression(
                local_status_property.get_chunk_key(),
                in_expression_key,
                this_type::expression::logic_AND,
                &in_comparison,
                &in_comparison + 1);
    }

    /// @brief 論理型の状態値を比較する条件式を登録する。
    /// @sa this_type::evaluate_expression で、登録した条件式を評価できる。
    /// @sa this_type::erase_chunk で、登録した条件式をチャンク単位で削除できる。
    /// @retval true
    ///   成功。条件式を *this に登録した。
    ///   条件式を登録した要素条件チャンクの識別値は、 in_status_key
    ///   に対応する状態値が登録されている状態値ビット列チャンクの識別値と同じ。
    /// @retval false
    ///   失敗。条件式は登録されなかった。
    ///   - in_expression_key に対応する条件式が既にあると失敗する。
    ///   - in_status_key に対応する状態値が論理型以外だと失敗する。
    public: bool register_expression(
        /// [in] 条件式が参照する状態貯蔵器。
        typename this_type::reservoir const& in_reservoir,
        /// [in] 登録する条件式の識別値。
        typename this_type::expression_key const& in_expression_key,
        /// [in] 評価する論理型の状態値の識別値。
        typename this_type::reservoir::status_key const& in_status_key,
        /// [in] 条件となる論理値。
        bool const in_condition)
    {
        return
            in_reservoir.find_kind(in_status_key)
                == this_type::reservoir::status_value::kind_BOOL
            && this->register_expression(
                in_reservoir,
                in_expression_key,
                typename this_type::reservoir::status_comparison(
                    in_status_key,
                    in_condition?
                        this_type::reservoir::status_value::comparison_NOT_EQUAL:
                        this_type::reservoir::status_value::comparison_EQUAL,
                    typename this_type::reservoir::status_value(false)));
    }

    /// @brief 条件式を取得する。
    /// @return
    ///   in_expression_key に対応する this_type::expression のコピー。
    ///   該当する条件式がない場合は this_type::expression::is_empty
    ///   が真となる値を返す。
    public: typename this_type::expression find_expression(
        /// [in] 取得する条件式に対応する識別値。
        typename this_type::expression_key const& in_expression_key)
    const
    {
        auto const local_find(this->expressions_.find(in_expression_key));
        return local_find != this->expressions_.end()?
            local_find->second:
            typename this_type::expression(
                typename this_type::chunk_key(),
                this_type::expression::logic_OR,
                this_type::expression::kind_SUB_EXPRESSION,
                0,
                0);
    }

    /// @brief 登録されている条件式を評価する。
    /// @sa this_type::register_expression で、条件式を登録できる。
    /// @retval 正 条件式の評価は真となった。
    /// @retval 0  条件式の評価は偽となった。
    /// @retval 負 条件式の評価に失敗した。
    ///   - 条件式が登録されていないと、失敗する。
    ///   - 条件式が参照する状態値が登録されていないと、失敗する。
    public: typename this_type::expression::evaluation evaluate_expression(
        /// [in] 評価する条件式に対応する識別値。
        typename this_type::expression_key const in_expression_key,
        /// [in] 条件式が参照する状態貯蔵器。
        typename this_type::reservoir const& in_reservoir)
    const
    {
        // 条件式の辞書から、該当する条件式を検索する。
        auto const local_expression_iterator(
            this->expressions_.find(in_expression_key));
        if (local_expression_iterator == this->expressions_.end())
        {
            return -1;
        }
        auto const& local_expression(local_expression_iterator->second);
        auto const local_chunk(
            this->_find_chunk(local_expression.get_chunk_key()));
        if (local_chunk == nullptr)
        {
            // 条件式があれば、要素条件チャンクもあるはず。
            PSYQ_ASSERT(false);
            return -1;
        }

        // 条件式の種別によって評価方法を分岐する。
        switch (local_expression.get_kind())
        {
            // 複合条件式を評価する。
            case this_type::expression::kind_SUB_EXPRESSION:
            typedef
                typename this_type::chunk::sub_expression_container::value_type
                sub_expression;
            return local_expression.evaluate(
                local_chunk->sub_expressions_,
                [&in_reservoir, this](sub_expression const& in_expression)
                ->typename this_type::expression::evaluation
                {
                    auto const local_evaluate_expression(
                        this->evaluate_expression(
                            in_expression.get_key(), in_reservoir));
                    if (local_evaluate_expression < 0)
                    {
                        return -1;
                    }
                    return in_expression.compare_condition(
                        0 < local_evaluate_expression);
                });

            // 状態変化条件式を評価する。
            case this_type::expression::kind_STATUS_TRANSITION:
            typedef
                typename this_type::chunk::status_transition_container::value_type
                status_transition;
            return local_expression.evaluate(
                local_chunk->status_transitions_,
                [&in_reservoir](status_transition const& in_transition)
                ->typename this_type::expression::evaluation
                {
                    return in_reservoir.find_transition(in_transition.get_key());
                });

            // 状態比較条件式を評価する。
            case this_type::expression::kind_STATUS_COMPARISON:
            typedef
                typename this_type::chunk::status_comparison_container::value_type
                status_comparison;
            return local_expression.evaluate(
                local_chunk->status_comparisons_,
                [&in_reservoir](status_comparison const& in_comparison)
                ->typename this_type::expression::evaluation
                {
                    return in_reservoir.compare_status(in_comparison);
                });

            // 条件式の種別が未知だった。
            default:
            PSYQ_ASSERT(false);
            return -1;
        }
    }
    /// @}
    //-------------------------------------------------------------------------
    /// @name 要素条件チャンク
    /// @{

    /// @brief 要素条件チャンクを予約する。
    public: void reserve_chunk(
        /// [in] 予約する要素条件チャンクに対応する識別値。
        typename this_type::chunk_key const& in_chunk_key,
        /// [in] 複合条件式の要素条件の予約数。
        std::size_t const in_sub_expression_capacity,
        /// [in] 状態変更条件式の要素条件の予約数。
        std::size_t const in_status_transition_capacity,
        /// [in] 状態比較条件式の要素条件の予約数。
        std::size_t const in_status_comparison_capacity)
    {
        this_type::reserve_chunk(
            this->chunks_,
            in_chunk_key,
            in_sub_expression_capacity,
            in_status_transition_capacity,
            in_status_comparison_capacity);
    }

    /// @brief 要素条件チャンクと、それを使っている条件式を破棄する。
    /// @retval true  成功。チャンクを破棄した。
    /// @retval false 失敗。 in_chunk_key に対応するチャンクがない。
    public: bool erase_chunk(
        /// [in] 破棄する要素条件チャンクに対応する識別値。
        typename this_type::chunk_key const& in_chunk_key)
    {
        // 要素条件チャンクを削除する。
        if (this->chunks_.erase(in_chunk_key) == 0)
        {
            return false;
        }

        // 条件式を削除する。
        for (
            auto i(this->expressions_.begin());
            i != this->expressions_.end();)
        {
            if (in_chunk_key != i->second.get_chunk_key())
            {
                ++i;
            }
            else
            {
                i = this->expressions_.erase(i);
            }
        }
        return true;
    }

    /// @brief 要素条件チャンクを取得する。
    /// @warning psyq::if_then_engine 管理者以外は、この関数は使用禁止。
    /// @return
    /// in_chunk_key に対応する要素条件チャンクを指すポインタ。
    /// 該当する要素条件チャンクがない場合は nullptr を返す。
    public: typename this_type::chunk const* _find_chunk(
        /// [in] 取得する要素条件チャンクに対応する識別値。
        typename this_type::chunk_key const& in_chunk_key)
    const
    {
        auto const local_iterator(this->chunks_.find(in_chunk_key));
        return local_iterator != this->chunks_.end()?
            &local_iterator->second: nullptr;
    }
    /// @}
    //-------------------------------------------------------------------------
    private: static std::pair<
         typename this_type::expression::kind,
         typename this_type::chunk::sub_expression_container*>
    make_element_kind(
        typename this_type::chunk& in_chunk,
        typename this_type::chunk::sub_expression_container::value_type const&)
    {
        return std::make_pair(
            this_type::expression::kind_SUB_EXPRESSION,
            &in_chunk.sub_expressions_);
    }

    private: static std::pair<
         typename this_type::expression::kind,
         typename this_type::chunk::status_transition_container*>
    make_element_kind(
        typename this_type::chunk& in_chunk,
        typename this_type::chunk::status_transition_container::value_type const&)
    {
        return std::make_pair(
            this_type::expression::kind_STATUS_TRANSITION,
            &in_chunk.status_transitions_);
    }

    private: static std::pair<
         typename this_type::expression::kind,
         typename this_type::chunk::status_comparison_container*>
    make_element_kind(
        typename this_type::chunk& in_chunk,
        typename this_type::chunk::status_comparison_container::value_type const&)
    {
        return std::make_pair(
            this_type::expression::kind_STATUS_COMPARISON,
            &in_chunk.status_comparisons_);
    }

    //-------------------------------------------------------------------------
    private: template<typename template_element_iterator>
    static bool is_valid_elements(
        template_element_iterator const& in_elements_begin,
        template_element_iterator const& in_elements_end,
        typename this_type::expression_map const& in_expressions)
    {
        for (auto i(in_elements_begin); i != in_elements_end; ++i)
        {
            if (!this_type::is_valid_element(*i, in_expressions))
            {
                return false;
            }
        }
        return true;
    }

    private: static bool is_valid_element(
        typename this_type::chunk::sub_expression_container::value_type const&
            in_sub_expression,
        typename this_type::expression_map const& in_expressions)
    {
        // 要素条件にある条件式がすでにあることを確認する。
        auto const local_validation(
            in_expressions.find(in_sub_expression.get_key())
            != in_expressions.end());
        PSYQ_ASSERT(local_validation);
        return local_validation;
    }

    private: template<typename template_element>
    static bool is_valid_element(
        template_element const&,
        typename this_type::expression_map const&)
    {
        return true;
    }

    //-------------------------------------------------------------------------
    /// @brief 要素条件チャンクを予約する。
    private: static typename this_type::chunk& reserve_chunk(
        /// [in] 予約する要素条件チャンクの辞書。
        typename this_type::chunk_map& io_chunks,
        /// [in] 予約するチャンクに対応する識別値。
        typename this_type::chunk_key const& in_chunk_key,
        /// [in] 複合条件式の要素条件の予約数。
        std::size_t const in_sub_expression_capacity,
        /// [in] 状態変更条件式の要素条件の予約数。
        std::size_t const in_status_transition_capacity,
        /// [in] 状態比較条件式の要素条件の予約数。
        std::size_t const in_status_comparison_capacity)
    {
        auto const local_emplace(
            io_chunks.emplace(
                in_chunk_key,
                typename this_type::chunk_map::mapped_type(
                    io_chunks.get_allocator())));
        auto& local_chunk(local_emplace.first->second);
        local_chunk.sub_expressions_.reserve(in_sub_expression_capacity);
        local_chunk.status_transitions_.reserve(in_status_transition_capacity);
        local_chunk.status_comparisons_.reserve(in_status_comparison_capacity);
        return local_chunk;
    }

    //-------------------------------------------------------------------------
    /// @brief 要素条件チャンクの辞書。
    private: typename this_type::chunk_map chunks_;
    /// @brief 条件式の辞書。
    private: typename this_type::expression_map expressions_;

}; // class psyq::if_then_engine::_private::evaluator

#endif // !defined(PSYQ_IF_THEN_ENGINE_EVALUATOR_HPP_)
// vim: set expandtab:
