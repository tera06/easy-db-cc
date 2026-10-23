
#include <CppUTest/TestHarness.h>
#include "common/status.h"

namespace easy_db
{
    TEST_GROUP(StatusTestGroup){

    };

    TEST(StatusTestGroup, Ok)
    {
        auto status = Status::ok();
        CHECK_TRUE(status.is_ok());
    }

    TEST(StatusTestGroup, OtherError)
    {
        auto status = Status::other_error();
        CHECK_TRUE(status.is_other_error());
    }

    TEST(StatusTestGroup, OutOfRange)
    {
        auto status = Status::out_of_range();
        CHECK_TRUE(status.is_out_of_range());
    }

    TEST(StatusTestGroup, InvalidPageId)
    {
        auto status = Status::invalid_page_id();
        CHECK_TRUE(status.is_invalid_page_id());
    }

    TEST(StatusTestGroup, FailOpenFile)
    {
        auto status = Status::fail_open_file();
        CHECK_TRUE(status.is_fail_open_file());
    }

    TEST(StatusTestGroup, FileNotExist)
    {
        auto status = Status::file_not_exist();
        CHECK_TRUE(status.is_file_not_exist());
    }

    TEST(StatusTestGroup, NotFindPage)
    {
        auto status = Status::not_find_page();
        CHECK_TRUE(status.is_not_find_page());
    }

    TEST(StatusTestGroup, FailFileSize)
    {
        auto status = Status::fail_file_size();
        CHECK_TRUE(status.is_fail_file_size());
    }

    TEST(StatusTestGroup, FailSeek)
    {
        auto status = Status::fail_seek();
        CHECK_TRUE(status.is_fail_seek());
    }

    TEST(StatusTestGroup, FailReadFile)
    {
        auto status = Status::fail_read_file();
        CHECK_TRUE(status.is_fail_read_file());
    }

    TEST(StatusTestGroup, FailWriteFile)
    {
        auto status = Status::fail_write_file();
        CHECK_TRUE(status.is_fail_write_file());
    }

    TEST(StatusTestGroup, InvalidNodeType)
    {
        auto status = Status::invalid_node_type();
        CHECK_TRUE(status.is_invalid_node_type());
    }

    TEST(StatusTestGroup, InvalidIsRoot)
    {
        auto status = Status::invalid_is_root();
        CHECK_TRUE(status.is_invalid_is_root());
    }

    TEST(StatusTestGroup, InvalidArgument)
    {
        auto status = Status::invalid_argument();
        CHECK_TRUE(status.is_invalid_argument());
    }

    TEST(StatusTestGroup, IsOk_ReturnFalseIfStatusIsNotOk)
    {
        const auto status = Status::other_error();
        CHECK_FALSE(status.is_ok());
    }

    TEST(StatusTestGroup, IsOtherError_ReturnFalseIfStatusIsNotOtherError)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_other_error());
    }

    TEST(StatusTestGroup, IsOutOfRange_ReturnFalseIfStatusIsNotOutOfRange)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_out_of_range());
    }

    TEST(StatusTestGroup, IsInvalidPageId_ReturnFalseIfStatusIsNotInvalidPageId)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_invalid_page_id());
    }

    TEST(StatusTestGroup, IsFailOpenFile_ReturnFalseIfStatusIsNotFailOpenFile)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_fail_open_file());
    }

    TEST(StatusTestGroup, IsFileNotExist_ReturnFalseIfStatusIsNotFileNotExist)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_file_not_exist());
    }

    TEST(StatusTestGroup, IsNotFindPage_ReturnFalseIfStatusIsNotNotFindPage)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_not_find_page());
    }

    TEST(StatusTestGroup, IsFailFileSize_ReturnFalseIfStatusIsNotFailFileSize)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_fail_file_size());
    }

    TEST(StatusTestGroup, IsFailSeek_ReturnFalseIfStatusIsNotFailSeek)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_fail_seek());
    }

    TEST(StatusTestGroup, IsFailReadFile_ReturnFalseIfStatusIsNotFailReadFile)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_fail_read_file());
    }

    TEST(StatusTestGroup, IsFailWriteFile_ReturnFalseIfStatusIsNotFailWriteFile)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_fail_write_file());
    }

    TEST(StatusTestGroup, IsInvalidNodeType_ReturnFalseIfStatusIsNotInvalidNodeType)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_invalid_node_type());
    }

    TEST(StatusTestGroup, IsInvalidIsRoot_ReturnFalseIfStatusIsNotInvalidIsRoot)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_invalid_is_root());
    }

    TEST(StatusTestGroup, IsInvalidArgument_ReturnFalseIfStatusIsNotInvalidArgument)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_invalid_argument());
    }

}