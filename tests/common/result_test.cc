#include <CppUTest/TestHarness.h>
#include "common/result.h"

namespace easy_db
{
    TEST_GROUP(ResultTestGroup){};

    TEST(ResultTestGroup, OkAndValue_ReturnOkResultAndGetValue)
    {
        {
            auto result = Result<int>::ok(1);

            CHECK_TRUE(result.status().is_ok());
            CHECK_TRUE(result.value() == 1);
        }

        {
            auto result = Result<std::string>::ok("result");

            CHECK_TRUE(result.status().is_ok());
            CHECK_TRUE(result.value() == "result");
        }
    }

    TEST(ResultTestGroup, ErrorAndValue_ReturnErrorResultAndNotGetValue)
    {
        auto result = Result<std::string>::error(Status::other_error());

        CHECK_TRUE(result.status().is_other_error());
        CHECK_THROWS(std::logic_error, result.value());
    }

    TEST(ResultTestGroup, CopyConstructor_CopySourceResultHaveValue)
    {
        {
            auto other = Result<int>::ok(1);

            auto result = other;

            CHECK_TRUE(other.status().is_ok());
            CHECK_TRUE(result.status().is_ok());
            CHECK_TRUE(other.value() == 1);
            CHECK_TRUE(result.value() == other.value());
            CHECK_TRUE(&(result.value()) != &(other.value()));
        }

        {
            auto other = Result<std::string>::ok("result");

            auto result = other;

            CHECK_TRUE(other.status().is_ok());
            CHECK_TRUE(result.status().is_ok());
            CHECK_TRUE(other.value() == "result");
            CHECK_TRUE(result.value() == other.value());
            CHECK_TRUE(&(result.value()) != &(other.value()));
        }
    }

    TEST(ResultTestGroup, CopyConstructor_CopySourceResultDoNotHaveValue)
    {
        auto other = Result<std::string>::error(Status::other_error());

        auto result = other;

        CHECK_TRUE(other.status().is_other_error());
        CHECK_TRUE(result.status().is_other_error());
        CHECK_THROWS(std::logic_error, other.value());
        CHECK_THROWS(std::logic_error, result.value());
    }

    TEST(ResultTestGroup, CopyAssgnmentOperator_CopySourceResultHaveValue)
    {
        auto other = Result<std::string>::ok("result");
        auto mid = Result<std::string>::error(Status::other_error());
        auto result = Result<std::string>::error(Status::other_error());

        result = mid = other;

        CHECK_TRUE(other.status().is_ok());
        CHECK_TRUE(mid.status().is_ok());
        CHECK_TRUE(result.status().is_ok());

        CHECK_TRUE(other.value() == "result");
        CHECK_TRUE(mid.value() == other.value());
        CHECK_TRUE(result.value() == other.value());

        CHECK_TRUE(&(mid.value()) != &(other.value()));
        CHECK_TRUE(&(result.value()) != &(other.value()));
    }

    TEST(ResultTestGroup, CopyAssgnmentOperator_CopySourceResultDoNotHaveValue)
    {
        auto other = Result<std::string>::error(Status::other_error());
        auto mid = Result<std::string>::ok("result1");
        auto result = Result<std::string>::ok("result2");

        result = mid = other;

        CHECK_TRUE(other.status().is_other_error());
        CHECK_TRUE(mid.status().is_other_error());
        CHECK_TRUE(result.status().is_other_error());

        CHECK_THROWS(std::logic_error, other.value());
        CHECK_THROWS(std::logic_error, mid.value());
        CHECK_THROWS(std::logic_error, result.value());
    }

    TEST(ResultTestGroup, CopyAssgnmentOperator_AssignSelf)
    {
        auto result = Result<std::string>::ok("result");

        result = result;

        CHECK_TRUE(result.status().is_ok());
        CHECK_TRUE(result.value() == "result");
    }

    TEST(ResultTestGroup, MoveConstructor_MoveSourceResultHaveValue)
    {
        auto other = Result<std::string>::ok("result");

        auto result = std::move(other);

        CHECK_TRUE(result.status().is_ok());
        CHECK_TRUE(result.value() == "result");
    }

    TEST(ResultTestGroup, MoveAssignmentOperator_MoveSourceResultHaveValue)
    {
        auto other = Result<std::string>::ok("result");
        auto result = Result<std::string>::error(Status::other_error());

        result = std::move(other);

        CHECK_TRUE(result.status().is_ok());
        CHECK_TRUE(result.value() == "result");
    }

    TEST(ResultTestGroup, IsOk_IfStatusIsOkReturnTrue)
    {
        auto result = Result<std::string>::ok("result");
        CHECK_TRUE(result.is_ok());
    }

    TEST(ResultTestGroup, IsOk_IfStatusIsErrorReturnFalse)
    {
        auto result = Result<std::string>::error(Status::other_error());
        CHECK_FALSE(result.is_ok());
    }

    TEST(ResultTestGroup, IsError_IfStatusIsNotOkReturnTrue)
    {
        auto result = Result<std::string>::error(Status::other_error());
        CHECK_TRUE(result.is_error());
    }

    TEST(ResultTestGroup, IsError_IfStatusIsOkReturnFalse)
    {
        auto result = Result<std::string>::ok("result");
        CHECK_FALSE(result.is_error());
    }
}