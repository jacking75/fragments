﻿/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
    @brief
 */
#ifndef PSYQ_MESSAGE_PACK_TEST_HPP_
#define PSYQ_MESSAGE_PACK_TEST_HPP_

#include <sstream>
//#include "psyq/message_pack/container.hpp"
//#include "psyq/message_pack/value.hpp"
//#include "psyq/message_pack/object.hpp"
//#include "psyq/message_pack/endianness.hpp"
//#include "psyq/message_pack/serializer.hpp"
//#include "psyq/message_pack/root_object.hpp"
//#include "psyq/message_pack/deserialize.hpp"

namespace psyq
{
    namespace test
    {
        inline void message_pack()
        {
            std::set<int> local_integer_set;
            while (local_integer_set.size() < 0x10000)
            {
                local_integer_set.insert(local_integer_set.size());
            }
            std::string local_string_0x10("0123456789ABCDEF");
            std::string local_string_0x10000;
            local_string_0x10000.reserve(0x10000);
            local_string_0x10000 = local_string_0x10;
            while (local_string_0x10000.size() < 0x10000)
            {
                local_string_0x10000 += local_string_0x10000;
            }
            typedef psyq::string::view<char> string_view;
            string_view local_string_0x1f(local_string_0x10000.data(), 0x1f);
            string_view local_string_0xff(local_string_0x10000.data(), 0xff);
#ifdef PSYQ_STRING_VIEW_BASE_HPP_
            string_view local_string_0xffff(local_string_0x10000.data(), 0xffff);
#else
            std::string local_string_0xffff(local_string_0x10000, 0, 0xffff);
#endif

            psyq::message_pack::serializer<std::stringstream, 16>
                local_serializer(
                    std::stringstream(std::ios_base::in | std::ios_base::out));
            local_serializer.make_serial_array(31);
            local_serializer.write_container_binary(
                local_integer_set.begin(), local_integer_set.size());
            local_serializer.write_extended(
                0x7f, 0x123456789abcdefLL, psyq::message_pack::little_endian);
            local_serializer.write_array(local_integer_set);
            local_serializer << local_integer_set;
            local_serializer << std::make_tuple(0, 0.0f, 0.0, false);
            local_serializer << (std::numeric_limits<std::int64_t>::min)();
            local_serializer << (std::numeric_limits<std::int32_t>::min)();
            local_serializer << (std::numeric_limits<std::int16_t>::min)();
            local_serializer << (std::numeric_limits<std::int8_t >::min)();
            local_serializer << -0x20;
            local_serializer << false;
            local_serializer << 0.25;
            local_serializer << 0.5f;
            local_serializer << true;
            local_serializer << int(0x7f);
            local_serializer << (std::numeric_limits<std::uint8_t >::max)();
            local_serializer << (std::numeric_limits<std::uint16_t>::max)();
            local_serializer << (std::numeric_limits<std::uint32_t>::max)();
            local_serializer << (std::numeric_limits<std::uint64_t>::max)();
            local_serializer << "0123456789ABCDEF0123456789ABCDE";
            local_serializer << local_string_0xff;
            local_serializer << local_string_0xffff;
            local_serializer << local_string_0x10000;
            local_serializer.write_container_binary(local_string_0xff);
            local_serializer.write_container_binary(local_string_0xffff);
            local_serializer.write_container_binary(local_string_0x10000);
            local_serializer.make_serial_extended(5, local_string_0x10.size());
            local_serializer.fill_container_raw(local_string_0x10);
            local_serializer.make_serial_extended(8, local_string_0xff.size());
            local_serializer.fill_container_raw(local_string_0xff);
            local_serializer.make_serial_extended(16, local_string_0xffff.size());
            local_serializer.fill_container_raw(local_string_0xffff);
            local_serializer.make_serial_extended(17, local_string_0x10000.size());
            local_serializer.fill_container_raw(local_string_0x10000);
            local_serializer.write_nil();

            auto local_serialize_string(local_serializer.get_stream().str());
            auto local_stream(
                std::move(local_serializer.reset(std::stringstream())));
            local_stream.seekg(0);
            typedef psyq::message_pack::deserializer
                <decltype(local_stream), psyq::message_pack::pool<>, 8>
                    message_pack_deserializer;
            message_pack_deserializer local_deserializer(
                std::move(local_stream), message_pack_deserializer::pool());
            message_pack_deserializer::root_object local_root_object;
            local_deserializer >> local_root_object;
            auto local_message_pack_object(local_root_object.get_array()->data());
            PSYQ_ASSERT(local_message_pack_object != nullptr);
            ++local_message_pack_object;
            ++local_message_pack_object;
            ++local_message_pack_object;

            auto const local_integer_map(local_message_pack_object->get_map());
            PSYQ_ASSERT(local_integer_map != nullptr);
            local_integer_map->find(true);
            local_integer_map->find(2);
            local_integer_map->find(3.0f);
            local_integer_map->find(4.0);
            local_integer_map->find(
                psyq::message_pack::object::make_string("100"));
            local_integer_map->find(
                psyq::message_pack::object::make_string(std::string("101")));
            ++local_message_pack_object;

            auto const local_tuple4(local_message_pack_object->get_array());
            PSYQ_ASSERT(local_tuple4 != nullptr);
            ++local_message_pack_object;

            std::uint8_t  local_unsigned_integer_8;
            std::uint16_t local_unsigned_integer_16;
            std::uint32_t local_unsigned_integer_32;
            std::uint64_t local_unsigned_integer_64;
            std::int8_t  local_integer_8;
            std::int16_t local_integer_16;
            std::int32_t local_integer_32;
            std::int64_t local_integer_64;
            float  local_float_32;
            double local_float_64;
            local_integer_64 = (std::numeric_limits<std::int64_t>::min)();
            PSYQ_ASSERT(*local_message_pack_object == local_integer_64);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_integer_32 = (std::numeric_limits<std::int32_t>::min)();
            PSYQ_ASSERT(*local_message_pack_object == local_integer_32);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_integer_16 = (std::numeric_limits<std::int16_t>::min)();
            PSYQ_ASSERT(*local_message_pack_object == local_integer_16);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_integer_8 = (std::numeric_limits<std::int8_t>::min)();
            PSYQ_ASSERT(*local_message_pack_object == local_integer_8);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            PSYQ_ASSERT(*local_message_pack_object == -0x20);
            ++local_message_pack_object;

            PSYQ_ASSERT(*local_message_pack_object == false);
            ++local_message_pack_object;

            local_float_64 = 0.25;
            PSYQ_ASSERT(*local_message_pack_object == local_float_64);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_float_32 = 0.5f;
            PSYQ_ASSERT(*local_message_pack_object == local_float_32);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            PSYQ_ASSERT(*local_message_pack_object == true);
            ++local_message_pack_object;

            PSYQ_ASSERT(*local_message_pack_object == 0x7f);
            ++local_message_pack_object;

            local_unsigned_integer_8 = (std::numeric_limits<std::uint8_t>::max)();
            PSYQ_ASSERT(*local_message_pack_object == local_unsigned_integer_8);
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_unsigned_integer_16 = (std::numeric_limits<std::uint16_t>::max)();
            PSYQ_ASSERT(*local_message_pack_object == local_unsigned_integer_16);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_unsigned_integer_32 = (std::numeric_limits<std::uint32_t>::max)();
            PSYQ_ASSERT(*local_message_pack_object == local_unsigned_integer_32);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_float_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            local_unsigned_integer_64 = (std::numeric_limits<std::uint64_t>::max)();
            PSYQ_ASSERT(*local_message_pack_object == local_unsigned_integer_64);
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_unsigned_integer_32));
            PSYQ_ASSERT(local_message_pack_object->get_numeric_value(local_unsigned_integer_64));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_8));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_16));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_32));
            PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_integer_64));
            //PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_float_32));
            //PSYQ_ASSERT(!local_message_pack_object->get_numeric_value(local_float_64));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0x1f == std::string(
                    local_message_pack_object->get_string()->begin(),
                    local_message_pack_object->get_string()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xff == std::string(
                    local_message_pack_object->get_string()->begin(),
                    local_message_pack_object->get_string()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xffff == std::string(
                        local_message_pack_object->get_string()->begin(),
                        local_message_pack_object->get_string()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0x10000 == std::string(
                    local_message_pack_object->get_string()->begin(),
                    local_message_pack_object->get_string()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xff == std::string(
                    (char const*)local_message_pack_object->get_binary()->begin(),
                    (char const*)local_message_pack_object->get_binary()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xffff == std::string(
                    (char const*)local_message_pack_object->get_binary()->begin(),
                    (char const*)local_message_pack_object->get_binary()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0x10000 == std::string(
                    (char const*)local_message_pack_object->get_binary()->begin(),
                    (char const*)local_message_pack_object->get_binary()->end()));
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0x10 == std::string(
                    (char const*)local_message_pack_object->get_extended()->begin(),
                    (char const*)local_message_pack_object->get_extended()->end()));
            PSYQ_ASSERT(local_message_pack_object->get_extended()->type() == 5);
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xff == std::string(
                    (char const*)local_message_pack_object->get_extended()->begin(),
                    (char const*)local_message_pack_object->get_extended()->end()));
            PSYQ_ASSERT(local_message_pack_object->get_extended()->type() == 8);
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0xffff == std::string(
                    (char const*)local_message_pack_object->get_extended()->begin(),
                    (char const*)local_message_pack_object->get_extended()->end()));
            PSYQ_ASSERT(local_message_pack_object->get_extended()->type() == 16);
            ++local_message_pack_object;

            PSYQ_ASSERT(
                local_string_0x10000 == std::string(
                    (char const*)local_message_pack_object->get_extended()->begin(),
                    (char const*)local_message_pack_object->get_extended()->end()));
            PSYQ_ASSERT(local_message_pack_object->get_extended()->type() == 17);
            ++local_message_pack_object;

            PSYQ_ASSERT(
                *local_message_pack_object == psyq::message_pack::object());
            ++local_message_pack_object;

            local_serializer << local_root_object;
            PSYQ_ASSERT(local_serializer.get_stream().str() == local_serialize_string);
        }
    } // namespace test
} // namespace psyq

#endif // PSYQ_MESSAGE_PACK_TEST_HPP_
