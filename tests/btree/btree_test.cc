#include <filesystem>

#include "btree/btree.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(BtreeTestGroup)
    {
        std::string filename =
            (std::filesystem::temp_directory_path() / "easy_db_btree_test.db").string();

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(BtreeTestGroup, Initialize_FindAndReplaceValues)
    {
        Btree btree(filename);
        CHECK_TRUE(btree.initialize().is_ok());

        Btree::value_t value{};
        value.fill(std::byte{0x2A});
        CHECK_TRUE(btree.insert(42, value).is_ok());

        auto result = btree.get_value(42);
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == value);

        value.fill(std::byte{0x7B});
        CHECK_TRUE(btree.insert(42, value).is_ok());
        result = btree.get_value(42);
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == value);

        const auto missing_result = btree.get_value(43);
        CHECK_TRUE(missing_result.is_error());
        CHECK_TRUE(missing_result.status().is_key_not_exist());
    }

    TEST(BtreeTestGroup, InsertSplitAndReloadMultipleLevels)
    {
        {
            Btree btree(filename);
            CHECK_TRUE(btree.initialize().is_ok());

            for (std::uint32_t key = 0; key < 100; ++key)
            {
                Btree::value_t value{};
                value.fill(static_cast<std::byte>(key));
                CHECK_TRUE(btree.insert(key, value).is_ok());
            }

            const auto result_values = btree.get_all();
            CHECK_TRUE(result_values.is_ok());
            CHECK_EQUAL(100, result_values.value().size());
            for (std::size_t index = 0; index < result_values.value().size(); ++index)
            {
                CHECK_TRUE(
                    result_values.value()[index][0] ==
                    static_cast<std::byte>(index));
            }
            CHECK_TRUE(btree.flush().is_ok());
        }

        {
            Btree btree(filename);
            CHECK_TRUE(btree.initialize().is_ok());

            for (std::uint32_t key = 0; key < 100; ++key)
            {
                const auto result = btree.get_value(key);
                CHECK_TRUE(result.is_ok());
                CHECK_TRUE(result.value()[0] == static_cast<std::byte>(key));
            }
        }
    }

    TEST(BtreeTestGroup, InsertDescendingKeysAcrossLeafAndInternalSplits)
    {
        Btree btree(filename);
        CHECK_TRUE(btree.initialize().is_ok());

        for (std::uint32_t key = 80; key > 0; --key)
        {
            Btree::value_t value{};
            value.fill(static_cast<std::byte>(key));
            CHECK_TRUE(btree.insert(key, value).is_ok());
        }

        for (std::uint32_t key = 1; key <= 80; ++key)
        {
            const auto result = btree.get_value(key);
            CHECK_TRUE(result.is_ok());
            CHECK_TRUE(result.value()[0] == static_cast<std::byte>(key));
        }
    }
}
