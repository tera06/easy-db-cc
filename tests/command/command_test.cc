#include <filesystem>

#include "command/command.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(CommandTestGroup)
    {
        const std::string filename =
            (std::filesystem::temp_directory_path() / "easy_db_command_test.db").string();

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(CommandTestGroup, Execute_InsertAndSelectAll)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());
        Command command(table);

        const auto insert_result =
            command.execute("INSERT 12 alice alice@example.com");
        CHECK_TRUE(insert_result.is_ok());
        STRCMP_EQUAL("Inserted row 12", insert_result.value().c_str());

        const auto select_result = command.execute("SELECT *");
        CHECK_TRUE(select_result.is_ok());
        STRCMP_EQUAL(
            "12, alice, alice@example.com",
            select_result.value().c_str());
    }

    TEST(CommandTestGroup, Execute_RejectsMalformedCommands)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());
        Command command(table);

        const auto result = command.execute("INSERT nope alice alice@example.com");

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(CommandTestGroup, Execute_SelectFromEmptyTable)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());
        Command command(table);

        const auto result = command.execute("SELECT");

        CHECK_TRUE(result.is_ok());
        STRCMP_EQUAL("(no rows)", result.value().c_str());
    }
}
