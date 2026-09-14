#include <CppUTest/TestHarness.h>
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

}