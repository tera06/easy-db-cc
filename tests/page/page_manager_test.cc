#include <filesystem>

#include "page/page_manager.h"

#include <CppUTest/TestHarness.h>
namespace easy_db
{
    TEST_GROUP(PageManagerTestGroup)
    {
        const std::string filename =
            std::filesystem::temp_directory_path() /
            "easy_db_page_manager_test.db";

        void setup()
        {
            std::filesystem::remove(filename);
        }

        void teardown()
        {
            std::filesystem::remove(filename);
        }
    };

    TEST(PageManagerTestGroup, Constructor_CreateEmptyDatabaseFileIfFileDoesNotExist)
    {
        CHECK_FALSE(std::filesystem::exists(filename));

        PageManager page_manager(filename);

        auto filesize = std::filesystem::file_size(filename);
        CHECK_TRUE(std::filesystem::exists(filename));
        CHECK_TRUE(filesize == 0);
    }

    TEST(PageManagerTestGroup, Constructor_SetNextPageIdFromExistingFile)
    {
        {
            PageManager page_manager(filename);

            page_manager.create_new_page();
            page_manager.create_new_page();

            const auto page0_result = page_manager.get_page(0);
            CHECK_TRUE(page0_result.is_ok());

            const auto page1_result = page_manager.get_page(1);
            CHECK_TRUE(page1_result.is_ok());

            CHECK_TRUE(page_manager.flush(0).is_ok());
            CHECK_TRUE(page_manager.flush(1).is_ok());
        }

        {
            PageManager page_manager(filename);

            const auto result = page_manager.create_new_page();

            CHECK_TRUE(result.is_ok());
            CHECK_TRUE(result.value() == 2);
        }
    }

    TEST(PageManagerTestGroup, CreateNewPage_ReturnPageIdStartingFromZero)
    {
        PageManager page_manager(filename);

        const auto result = page_manager.create_new_page();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == 0);
    }

    TEST(PageManagerTestGroup, CreateNewPage_ReturnSequentialPageIds)
    {
        PageManager page_manager(filename);

        const auto result1 = page_manager.create_new_page();
        const auto result2 = page_manager.create_new_page();
        const auto result3 = page_manager.create_new_page();

        CHECK_TRUE(result1.is_ok());
        CHECK_TRUE(result2.is_ok());
        CHECK_TRUE(result3.is_ok());

        CHECK_TRUE(result1.value() == 0);
        CHECK_TRUE(result2.value() == 1);
        CHECK_TRUE(result3.value() == 2);
    }

    TEST(PageManagerTestGroup, GetPage_ReturnInvalidPageIdIfPageDoesNotExist)
    {
        PageManager page_manager(filename);

        const auto result = page_manager.get_page(0);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_page_id());
    }

    TEST(PageManagerTestGroup, GetPage_ReturnCreatedPage)
    {
        PageManager page_manager(filename);

        const auto create_result = page_manager.create_new_page();
        CHECK_TRUE(create_result.is_ok());

        const auto page_id = create_result.value();

        const auto result = page_manager.get_page(page_id);

        CHECK_TRUE(result.is_ok());
    }

    TEST(PageManagerTestGroup, GetPage_ReturnSamePageIfPageIsAlreadyLoaded)
    {
        PageManager page_manager(filename);

        const auto create_result = page_manager.create_new_page();
        CHECK_TRUE(create_result.is_ok());

        const auto page_id = create_result.value();

        const auto result1 = page_manager.get_page(page_id);
        CHECK_TRUE(result1.is_ok());

        auto &page1 = result1.value();

        const auto iterator_result = page1.iterator_at(0);
        CHECK_TRUE(iterator_result.is_ok());

        *iterator_result.value() = std::byte{0x2A};

        const auto result2 = page_manager.get_page(page_id);
        CHECK_TRUE(result2.is_ok());

        auto &page2 = result2.value();

        const auto iterator_result2 = page2.iterator_at(0);
        CHECK_TRUE(iterator_result2.is_ok());

        CHECK_TRUE(*iterator_result2.value() == std::byte{0x2A});
        CHECK_TRUE(&page1 == &page2);
    }

    TEST(PageManagerTestGroup, GetPage_LoadPageFromFileIfPageIsNotLoaded)
    {
        {
            PageManager page_manager(filename);

            const auto create_result = page_manager.create_new_page();
            CHECK_TRUE(create_result.is_ok());

            const auto page_id = create_result.value();

            const auto result = page_manager.get_page(page_id);
            CHECK_TRUE(result.is_ok());

            auto &page = result.value();

            const auto iterator_result = page.iterator_at(0);
            CHECK_TRUE(iterator_result.is_ok());

            *iterator_result.value() = std::byte{0x2A};

            const auto flush_result = page_manager.flush(page_id);
            CHECK_TRUE(flush_result.is_ok());
        }

        {
            PageManager page_manager(filename);

            const auto result = page_manager.get_page(0);
            CHECK_TRUE(result.is_ok());

            auto &page = result.value();

            const auto iterator_result = page.iterator_at(0);
            CHECK_TRUE(iterator_result.is_ok());

            CHECK_TRUE(*iterator_result.value() == std::byte{0x2A});
        }
    }

    TEST(PageManagerTestGroup, Flush_WritePageDataToFile)
    {
        {
            PageManager page_manager(filename);

            const auto create_result = page_manager.create_new_page();
            CHECK_TRUE(create_result.is_ok());

            const auto page_id = create_result.value();

            const auto page_result = page_manager.get_page(page_id);
            CHECK_TRUE(page_result.is_ok());

            auto &page = page_result.value();

            const auto iterator_result = page.iterator_at(0);
            CHECK_TRUE(iterator_result.is_ok());

            *iterator_result.value() = std::byte{0x2A};

            const auto flush_result = page_manager.flush(page_id);
            CHECK_TRUE(flush_result.is_ok());
        }

        {
            PageManager page_manager(filename);

            const auto result = page_manager.get_page(0);

            CHECK_TRUE(result.is_ok());

            const auto iterator_result = result.value().const_iterator_at(0);
            CHECK_TRUE(iterator_result.is_ok());

            CHECK_TRUE(*iterator_result.value() == std::byte{0x2A});
        }
    }

    TEST(PageManagerTestGroup, Flush_ReturnNotFindPageIfPageIsNotLoaded)
    {
        PageManager page_manager(filename);

        const auto result = page_manager.flush(0);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_not_find_page());
    }

}