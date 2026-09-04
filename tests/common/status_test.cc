
#include <CppUTest/TestHarness.h>
#include "common/status.h"

using easy_db::Status;

TEST_GROUP(StatusTestGroup){

};

TEST(StatusTestGroup, Ok_Test)
{
    auto status = Status::ok();
    CHECK_TRUE(status.is_ok());
}

TEST(StatusTestGroup, OtherError_Test)
{
    auto status = Status::other_error();
    CHECK_TRUE(status.is_other_error());
}

TEST(StatusTestGroup, IsOk_ReturnTrueIfStatusIsOk_Test)
{
    const auto status = Status::ok();
    CHECK_TRUE(status.is_ok());
}

TEST(StatusTestGroup, IsOk_ReturnFalseIfStatusIsNotOk_Test)
{
    const auto status = Status::other_error();
    CHECK_FALSE(status.is_ok());
}

TEST(StatusTestGroup, IsOtherError_ReturnTrueIfStatusIsOtherError_Test)
{
    const auto status = Status::other_error();
    CHECK_TRUE(status.is_other_error());
}

TEST(StatusTestGroup, IsOtherError_ReturnFalseIfStatusIsNotOtherError_Test)
{
    const auto status = Status::ok();
    CHECK_FALSE(status.is_other_error());
}
