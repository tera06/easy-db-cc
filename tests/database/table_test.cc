#include <filesystem>

#include "database/table.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(TableTestGroup)
    {
        std::string filename =
            (std::filesystem::temp_directory_path() / "easy_db_table_test.db").string();

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(TableTestGroup, InsertAndSelectAll_ReturnsRowsInKeyOrder)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());

        const auto second_row = Row::create(2, "second", "second@example.com");
        const auto first_row = Row::create(1, "first", "first@example.com");
        CHECK_TRUE(second_row.is_ok());
        CHECK_TRUE(first_row.is_ok());
        CHECK_TRUE(table.insert(second_row.value()).is_ok());
        CHECK_TRUE(table.insert(first_row.value()).is_ok());

        const auto result = table.select_all();
        CHECK_TRUE(result.is_ok());
        CHECK_EQUAL(2, result.value().size());

        const auto first_serialized = result.value()[0].serialize();
        const auto second_serialized = result.value()[1].serialize();
        CHECK_TRUE(first_serialized.is_ok());
        CHECK_TRUE(second_serialized.is_ok());
        CHECK_TRUE(first_serialized.value() == first_row.value().serialize().value());
        CHECK_TRUE(second_serialized.value() == second_row.value().serialize().value());
    }

    TEST(TableTestGroup, Insert_ReplacesRowWithMatchingId)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());

        const auto original = Row::create(1, "original", "old@example.com");
        const auto replacement = Row::create(1, "replacement", "new@example.com");
        CHECK_TRUE(original.is_ok());
        CHECK_TRUE(replacement.is_ok());
        CHECK_TRUE(table.insert(original.value()).is_ok());
        CHECK_TRUE(table.insert(replacement.value()).is_ok());

        const auto result = table.select_all();
        CHECK_TRUE(result.is_ok());
        CHECK_EQUAL(1, result.value().size());
        const auto serialized = result.value()[0].serialize();
        CHECK_TRUE(serialized.is_ok());
        CHECK_TRUE(serialized.value() == replacement.value().serialize().value());
    }

    TEST(TableTestGroup, SelectAll_ReturnsErrorBeforeInitialization)
    {
        Table table(filename);

        const auto result = table.select_all();

        CHECK_TRUE(result.is_error());
    }

    TEST(TableTestGroup, FlushAndInitialize_PreserveRows)
    {
        const auto row = Row::create(7, "persisted", "persisted@example.com");
        CHECK_TRUE(row.is_ok());
        {
            Table table(filename);
            CHECK_TRUE(table.initialize().is_ok());
            CHECK_TRUE(table.insert(row.value()).is_ok());
            CHECK_TRUE(table.flush().is_ok());
        }

        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());
        const auto result = table.select_all();
        CHECK_TRUE(result.is_ok());
        CHECK_EQUAL(1, result.value().size());
        const auto serialized = result.value()[0].serialize();
        CHECK_TRUE(serialized.is_ok());
        CHECK_TRUE(serialized.value() == row.value().serialize().value());
    }
}
