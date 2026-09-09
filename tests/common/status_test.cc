
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

    TEST(StatusTestGroup, IsOk_ReturnTrueIfStatusIsOk)
    {
        const auto status = Status::ok();
        CHECK_TRUE(status.is_ok());
    }

    TEST(StatusTestGroup, IsOk_ReturnFalseIfStatusIsNotOk)
    {
        const auto status = Status::other_error();
        CHECK_FALSE(status.is_ok());
    }

    TEST(StatusTestGroup, IsOtherError_ReturnTrueIfStatusIsOtherError)
    {
        const auto status = Status::other_error();
        CHECK_TRUE(status.is_other_error());
    }

    TEST(StatusTestGroup, IsOtherError_ReturnFalseIfStatusIsNotOtherError)
    {
        const auto status = Status::ok();
        CHECK_FALSE(status.is_other_error());
    }

}