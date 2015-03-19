﻿/** @file
    @copydoc psyq::scenario_engine::behavior
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_HPP_
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_HPP_

//#include "scenario_engine/dispatcher.hpp"

/// @cond
namespace psyq
{
    namespace scenario_engine
    {
        template<typename> class behavior;
        template<typename> class behavior_builder;
    } // namespace scenario_engine
} // namespace psyq
/// @endcond

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KEY
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KEY "KEY"
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KEY)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_CONDITION
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_CONDITION "CONDITION"
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_CONDITION)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KIND
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KIND "KIND"
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KIND)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_KIND_STATE
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_KIND_STATE "STATE"
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_KIND_STATE)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT "ARGUMENT"
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_COPY
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_COPY ":="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_COPY)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_ADD
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_ADD "+="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_ADD)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_SUB
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_SUB "-="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_SUB)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MULT
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MULT "*="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MULT)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_DIV
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_DIV "/="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_DIV)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MOD
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MOD "%="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MOD)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_OR
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_OR "|="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_OR)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_XOR
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_XOR "^="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_XOR)

#ifndef PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_AND
#define PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_AND "&="
#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_AND)

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 条件挙動。条件式の評価が変化した際の挙動を規格化したオブジェクト。
 */
template<typename template_dispatcher>
class psyq::scenario_engine::behavior
{
    /// @brief thisが指す値の型。
    private: typedef behavior this_type;

    public: typedef template_dispatcher dispatcher;

    public: enum state_operator_enum: std::uint8_t
    {
        state_operator_COPY,
        state_operator_ADD,
        state_operator_SUB,
        state_operator_MULT,
        state_operator_DIV,
        state_operator_MOD,
        state_operator_OR,
        state_operator_XOR,
        state_operator_AND,
    };

    /// @brief 条件挙動関数オブジェクトのコンテナ。
    public: typedef std::vector<
        typename this_type::dispatcher::function_shared_ptr,
        typename this_type::dispatcher::allocator_type>
            function_shared_ptr_vector;

    /// @brief 条件挙動チャンク。
    public: struct chunk_struct
    {
        typename behavior::function_shared_ptr_vector functions;

        /// @note ここは条件式キーでなくて、チャンクキーにしないと。
        typename behavior::dispatcher::expression_key key;
    };

    /// @brief 条件挙動チャンクのコンテナ。
    public: typedef std::vector<
        typename this_type::chunk_struct,
        typename this_type::dispatcher::allocator_type>
            chunk_vector;

    //-------------------------------------------------------------------------
    public: template<typename template_state_archive, typename template_value>
    static typename this_type::dispatcher::function_shared_ptr
    make_state_operation_function(
        template_state_archive& io_states,
        bool const in_condition,
        typename template_state_archive::key_type const& in_state_key,
        typename this_type::state_operator_enum const in_operator,
        template_value const& in_value)
    {
        // 状態値を書き換える関数オブジェクトを生成する。
        return this_type::dispatcher::function_shared_ptr(
            new (io_allocator.allocate(1))
                typename this_type::dispatcher::function(
                    /// @note io_states を参照渡しするのは危険。もうないかも。
                    [&io_states, in_condition, in_state_key, in_operator, in_value](
                        typename this_type::dispatcher::expression_key const&,
                        bool const in_evaluation)
                    {
                        // 条件と評価が合致すれば、状態値を書き換える。
                        if (in_evaluation == in_condition)
                        {
                            auto const local_operate_state(
                                this_type::operate_state(
                                    io_states,
                                    in_state_key,
                                    in_operator,
                                    in_value));
                            PSYQ_ASSERT(local_operate_state);
                        }
                    }));
    }

    private: template<typename template_state_archive, typename template_value>
    static bool operate_state(
        template_state_archive& io_states,
        typename template_state_archive::key_type const& in_state_key,
        typename this_type::state_operator_enum const in_operator,
        template_value const& in_value)
    {
        switch (in_operator)
        {
            case this_type::state_operator_COPY:
            return io_states.set_value(in_state_key, in_value);

            default: break;
        }
        template_value local_value;
        if (!io_states.get_value(in_state_key, local_value))
        {
            return false;
        }
        switch (in_operator)
        {
            case this_type::state_operator_ADD:  local_value += in_value; break;
            case this_type::state_operator_SUB:  local_value -= in_value; break;
            case this_type::state_operator_MULT: local_value *= in_value; break;
            case this_type::state_operator_OR:   local_value |= in_value; break;
            case this_type::state_operator_XOR:  local_value ^= in_value; break;
            case this_type::state_operator_AND:  local_value &= in_value; break;

            case this_type::state_operator_DIV:
            PSYQ_ASSERT(in_value != 0);
            local_value /= in_value;
            break;

            case this_type::state_operator_MOD:
            PSYQ_ASSERT(in_value != 0);
            local_value %= in_value;
            break;

            default: return false;
        }
        return io_states.set_value(in_state_key, local_value);
    }

}; // class psyq::scenario_engine::behavior

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template<typename template_behavior>
class psyq::scenario_engine::behavior_builder
{
    private: typedef behavior_builder this_type;

    public: typedef template_behavior behavior;

    //-------------------------------------------------------------------------
    public: template<
        typename template_hasher,
        typename template_evaluator,
        typename template_string>
    static std::size_t build(
        typename this_type::behavior::function_shared_ptr_vector& io_functions,
        typename this_type::behavior::dispatcher& io_dispatcher,
        template_hasher& io_hasher,
        template_evaluator const& in_evaluator,
        typename template_evaluator::state_archive const& in_states,
        psyq::string::csv_table<template_string> const& in_string_table)
    {
        auto const local_row_count(in_string_table.get_row_count());
        auto const local_last_size(io_functions.size());
        io_functions.reserve(local_last_size + local_row_count);

        // 文字列表を解析し、条件挙動関数オブジェクトの一覧を構築する。
        for (decltype(local_row_count) i(0); i < local_row_count; ++i)
        {
            // 条件式キーを取得する。
            auto const local_key_cell(
                in_string_table.find_body_cell(
                    in_row_index,
                    PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KEY));
            auto local_key(io_hasher(local_key_cell));
            if (local_key
                == io_hasher(typename template_hasher::argument_type()))
            {
                // 条件式キーが正しくなかった。
                PSYQ_ASSERT(false);
                continue;
            }
            // 条件評価器に条件式があることを確認する。
            PSYQ_ASSERT(in_evaluator.find_expression(local_key) != nullptr);

            // 条件挙動関数オブジェクトを生成し、条件監視器に登録する。
            auto local_function(
                this_type::make_function(
                    in_evaluator, in_states, in_string_table, i));
            auto const local_register(
                io_dispatcher.register_function(
                    local_function,
                    local_expression_key,
                    in_evaluator, 
                    in_states));
            if (local_register)
            {
                io_functions.push_back(std::move(local_function));
            }
            else
            {
                // 条件挙動関数オブジェクトの登録に失敗した。
                PSYQ_ASSERT(false);
            }
        }
        io_functions.shrink_to_fit();
        return io_functions.size() - local_last_size;
    }

    //-------------------------------------------------------------------------
    private: template<typename template_evaluator, typename template_string>
    static typename this_type::dispatcher::function_shared_ptr make_function(
        template_evaluator const& in_evaluator,
        typename template_evaluator::state_archive const& in_states,
        psyq::string::csv_table<template_string> const& in_string_table,
        typename psyq::string::csv_table<template_string>::index_type const
            in_row_index)
    {
        // 挙動が起こる条件を取得する。
        auto const local_condition_cell(
            psyq::scenario_engine::_private::get_bool(
                in_string_table.find_body_cell(
                    in_row_index,
                    PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_CONDITION)));
        if (local_condition_cell < 0)
        {
            // 未知の条件だった。
            PSYQ_ASSERT(false);
            return typename this_type::dispatcher::function_shared_ptr();
        }
        bool const local_condition(local_condition_cell != 0);

        // 条件挙動関数の種類を取得する。
        auto const local_kind_cell(
            in_string_table.find_body_cell(
                in_row_index,
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_KIND));
        if (local_kind_cell
            == PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_KIND_STATE)
        {
            return this_type::make_state_operation_function(
                in_states, local_condition, in_string_table, in_row_index);
        }
        else
        {
            // 未知の種類だった。
            PSYQ_ASSERT(false);
            return typename this_type::dispatcher::function_shared_ptr();
        }
    }

    private: template<
        typename template_state_archive,
        typename template_string>
    static typename this_type::dispatcher::function_shared_ptr
    make_state_operation_function(
        template_state_archive const& in_states,
        bool const in_condition,
        psyq::string::csv_table<template_string> const& in_string_table,
        typename psyq::string::csv_table<template_string>::index_type const
            in_row_index)
    {
        // 状態キーを取得する。
        auto const local_key_cell(
            in_string_table.find_body_cell(
                in_row_index,
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT));
        auto local_key(io_hasher(local_key_cell));
        auto const local_key(io_hasher(local_state_key_cell));
        if (!in_states.is_registered(local_key))
        {
            // 状態値書庫にキーが登録されていなかった。
            PSYQ_ASSERT(false);
            return typename this_type::dispatcher::function_shared_ptr();
        }

        // 演算子を取得する。
        typename this_type::behavior::state_operator_enum local_operator;
        auto const local_get_operator(
            this_type::get_operator(
                local_operator,
                in_string_table.find_body_cell(
                    in_row_index,
                    PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT,
                    1)));
        if (!local_get_operator)
        {
            PSYQ_ASSERT(false);
            return typename this_type::dispatcher::function_shared_ptr();
        }

        // 演算値を取得する。
        auto const local_value_cell(
            in_string_table.find_body_cell(
                in_row_index,
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_COLUMN_ARGUMENT,
                2));
        /// @note とりあえず整数値のみ対応しておく。
        std::size_t local_rest_size;
        auto const local_value(
            local_value_cell.template to_integer<std::int32_t>(
                &local_rest_size));
        if (local_rest_size != 0)
        {
            PSYQ_ASSERT(false);
            return typename this_type::dispatcher::function_shared_ptr();
        }
        PSYQ_ASSERT(
            local_value != 0 || (
                local_operator != this_type::state_operator_DIV
                && local_operator != this_type::state_operator_MOD));

        // 状態値を書き換える関数オブジェクトを生成する。
        return this_type::behavior::make_state_operation_function(
            in_condition, local_state_key, local_operator, local_value);
    }

    private: template<typename template_string>
    static bool get_operator(
        typename this_type::state_operator_enum& out_operator,
        template_string const& in_string)
    {
        if (in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_COPY)
        {
            out_operator = this_type::state_operator_COPY;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_ADD)
        {
            out_operator = this_type::state_operator_ADD;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_SUB)
        {
            out_operator = this_type::state_operator_SUB;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MULT)
        {
            out_operator = this_type::state_operator_MULT;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_DIV)
        {
            out_operator = this_type::state_operator_DIV;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_MOD)
        {
            out_operator = this_type::state_operator_MOD;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_OR)
        {
            out_operator = this_type::state_operator_OR;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_XOR)
        {
            out_operator = this_type::state_operator_XOR;
        }
        else if (
            in_string ==
                PSYQ_SCENARIO_ENGINE_BEHAVIOR_BUILDER_CSV_OPERATOR_AND)
        {
            out_operator = this_type::state_operator_AND;
        }
        else
        {
            // 演算子が見つからなかった。
            PSYQ_ASSERT(false);
            return false;
        }
        return true;
    }

}; // psyq::scenario_engine::behavior_builder

#endif // !defined(PSYQ_SCENARIO_ENGINE_BEHAVIOR_HPP_)