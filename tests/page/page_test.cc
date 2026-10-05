#include <CppUTest/TestHarness.h>
#include <limits>
#include "page/page.h"

namespace easy_db
{
    TEST_GROUP(PageTestGroup){};

    TEST(PageTestGroup, IteratorAt_WithinRange)
    {
        auto page = Page();

        {
            auto result = page.iterator_at(4095);
            CHECK_TRUE(result.is_ok());
            auto iter = result.value();
            *iter = std::byte{0x2A};
        }

        {
            auto result = page.iterator_at(4095);
            auto iter = result.value();
            CHECK_TRUE(*iter == std::byte{0x2A});
        }
    }

    TEST(PageTestGroup, IteratorAt_OutOfRange)
    {
        auto page = Page();

        auto result = page.iterator_at(4096);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_out_of_range());
    }

    TEST(PageTestGroup, ConstIteratorAt_WithinRange)
    {
        auto page = Page();
        {
            auto result = page.iterator_at(4095);
            auto iter = result.value();
            *iter = std::byte{0x2A};
        }

        {
            const auto result = page.const_iterator_at(4095);
            CHECK_TRUE(result.is_ok());
            auto iter = result.value();
            CHECK_TRUE(*iter == std::byte{0x2A});
        }
    }

    TEST(PageTestGroup, ConstIteratorAt_OutOfRange)
    {
        auto page = Page();
        auto result = page.const_iterator_at(4096);
        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_out_of_range());
    }

    TEST(PageTestGroup, ReadUint32_ReadWrittenValue)
    {
        auto page = Page();

        const auto write_result = page.write_uint32(0x12345678, 0);
        CHECK_TRUE(write_result.is_ok());

        const auto read_result = page.read_uint32(0);

        CHECK_TRUE(read_result.is_ok());
        CHECK_TRUE(read_result.value() == 0x12345678);
    }

    TEST(PageTestGroup, ReadUint32_AtLastValidOffset)
    {
        auto page = Page();

        const std::size_t offset = 4096 - sizeof(std::uint32_t);

        const auto write_result = page.write_uint32(0x12345678, offset);
        CHECK_TRUE(write_result.is_ok());

        const auto read_result = page.read_uint32(offset);

        CHECK_TRUE(read_result.is_ok());
        CHECK_TRUE(read_result.value() == 0x12345678);
    }

    TEST(PageTestGroup, ReadUint32_OutOfRange)
    {
        auto page = Page();

        const std::size_t offset = 4096 - sizeof(std::uint32_t) + 1;

        const auto result = page.read_uint32(offset);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_out_of_range());
    }

    TEST(PageTestGroup, WriteUint32_OutOfRange)
    {
        auto page = Page();

        const std::size_t offset = 4096 - sizeof(std::uint32_t) + 1;

        const auto result = page.write_uint32(0x12345678, offset);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_out_of_range());
    }

    TEST(PageTestGroup, WriteAndReadUint32_HandleZero)
    {
        auto page = Page();

        const auto write_result = page.write_uint32(0, 0);
        CHECK_TRUE(write_result.is_ok());

        const auto read_result = page.read_uint32(0);

        CHECK_TRUE(read_result.is_ok());
        CHECK_TRUE(read_result.value() == 0);
    }

    TEST(PageTestGroup, WriteAndReadUint32_HandleMaxValue)
    {
        auto page = Page();

        const auto write_result =
            page.write_uint32(std::numeric_limits<std::uint32_t>::max(), 0);
        CHECK_TRUE(write_result.is_ok());

        const auto read_result = page.read_uint32(0);

        CHECK_TRUE(read_result.is_ok());
        CHECK_TRUE(
            read_result.value() ==
            std::numeric_limits<std::uint32_t>::max());
    }

}