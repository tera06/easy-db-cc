#include <array>
#include <filesystem>
#include <fstream>

#include "database/database.h"
#include "page/page.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(DatabaseTestGroup)
    {
        const std::string filename =
            (std::filesystem::temp_directory_path() / "easy_db_database_test.db").string();

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(DatabaseTestGroup, Open_CreatesAndInitializesTable)
    {
        const auto result = Database::open(filename);

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(std::filesystem::exists(filename));

        const auto row = Row::create(1, "user", "user@example.com");
        CHECK_TRUE(row.is_ok());
        CHECK_TRUE(result.value()->insert(row.value()).is_ok());

        const auto rows = result.value()->select_all();
        CHECK_TRUE(rows.is_ok());
        CHECK_EQUAL(1, rows.value().size());
        CHECK_EQUAL(1, rows.value()[0].get_id());
    }

    TEST(DatabaseTestGroup, Open_PropagatesInitializationError)
    {
        {
            std::ofstream file(filename, std::ios::binary);
            std::array<char, Page::kPageSize * 2> invalid_database{};
            file.write(invalid_database.data(), invalid_database.size());
        }

        const auto result = Database::open(filename);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }
}
