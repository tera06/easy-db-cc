#include <filesystem>
#include <sstream>

#include "repl/repl.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(ReplTestGroup)
    {
        const std::string filename =
            (std::filesystem::temp_directory_path() / "easy_db_repl_test.db").string();

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(ReplTestGroup, Start_ProcessesCommandsAndExitsOnQuit)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());

        std::istringstream input("INSERT 12 alice alice@example.com\nSELECT *\nquit\n");
        std::ostringstream output;
        Repl repl(table, input, output);

        const auto status = repl.start();

        CHECK_TRUE(status.is_ok());
        STRCMP_CONTAINS("Inserted row 12", output.str().c_str());
        STRCMP_CONTAINS("12, alice, alice@example.com", output.str().c_str());
    }

    TEST(ReplTestGroup, Start_ReportsInvalidCommands)
    {
        Table table(filename);
        CHECK_TRUE(table.initialize().is_ok());

        std::istringstream input("INSERT nope alice alice@example.com\nquit\n");
        std::ostringstream output;
        Repl repl(table, input, output);

        const auto status = repl.start();

        CHECK_TRUE(status.is_ok());
        STRCMP_CONTAINS("Error: invalid command", output.str().c_str());
    }

    TEST(ReplTestGroup, ConstructorWithFilename_InitializesDatabase)
    {
        std::istringstream input("exit\n");
        std::ostringstream output;
        Repl repl(filename, input, output);

        const auto status = repl.start();

        CHECK_TRUE(status.is_ok());
    }
}
