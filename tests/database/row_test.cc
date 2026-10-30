#include "database/row.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(RowTestGroup){};

    TEST(RowTestGroup, Serialize_UsesFixedLayoutAndZeroPadding)
    {
        const auto row_result = Row::create(0x12345678, "abc", "x@y");
        CHECK_TRUE(row_result.is_ok());

        const auto result = row_result.value().serialize();

        CHECK_TRUE(result.is_ok());
        const auto &serialized = result.value();
        CHECK_EQUAL(0x78, std::to_integer<unsigned char>(serialized[0]));
        CHECK_EQUAL(0x56, std::to_integer<unsigned char>(serialized[1]));
        CHECK_EQUAL(0x34, std::to_integer<unsigned char>(serialized[2]));
        CHECK_EQUAL(0x12, std::to_integer<unsigned char>(serialized[3]));
        CHECK_EQUAL(
            static_cast<unsigned char>('a'),
            std::to_integer<unsigned char>(serialized[4]));
        CHECK_EQUAL(
            static_cast<unsigned char>('x'),
            std::to_integer<unsigned char>(serialized[36]));
        CHECK_EQUAL(0, std::to_integer<unsigned char>(serialized[7]));
        CHECK_EQUAL(0, std::to_integer<unsigned char>(serialized[39]));
    }

    TEST(RowTestGroup, Create_ReturnsInvalidArgumentForOversizedUsername)
    {
        const auto result = Row::create(1, std::string(33, 'u'), "email");

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(RowTestGroup, Create_ReturnsInvalidArgumentForOversizedEmail)
    {
        const auto result = Row::create(1, "user", std::string(256, 'e'));

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(RowTestGroup, Create_ReturnsInvalidArgumentForEmbeddedNullInUsername)
    {
        const auto result = Row::create(1, std::string("user\0name", 9), "email");

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(RowTestGroup, Create_ReturnsInvalidArgumentForEmbeddedNullInEmail)
    {
        const auto result = Row::create(1, "username", std::string("em\0ail", 7));

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(RowTestGroup, Deserialize_RoundTripsFields)
    {
        const auto row_result = Row::create(0xfedcba98, "username", "email@example.com");
        CHECK_TRUE(row_result.is_ok());
        const auto serialized_result = row_result.value().serialize();
        CHECK_TRUE(serialized_result.is_ok());

        const auto deserialized_result = Row::deserialize(serialized_result.value());

        CHECK_TRUE(deserialized_result.is_ok());
        const auto round_trip_serialized_result = deserialized_result.value().serialize();
        CHECK_TRUE(round_trip_serialized_result.is_ok());
        CHECK_TRUE(round_trip_serialized_result.value() == serialized_result.value());
    }

    TEST(RowTestGroup, SerializeAndDeserialize_SupportMaximumFieldLengths)
    {
        const auto row_result = Row::create(
            1,
            std::string(32, 'u'),
            std::string(255, 'e'));
        CHECK_TRUE(row_result.is_ok());
        const auto serialized_result = row_result.value().serialize();
        CHECK_TRUE(serialized_result.is_ok());

        const auto result = Row::deserialize(serialized_result.value());

        CHECK_TRUE(result.is_ok());
        const auto round_trip_result = result.value().serialize();
        CHECK_TRUE(round_trip_result.is_ok());
        CHECK_TRUE(round_trip_result.value() == serialized_result.value());
    }
}
