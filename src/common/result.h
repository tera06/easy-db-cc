#ifndef EASY_DB_RESULT_H_
#define EASY_DB_RESULT_H_

#include <memory>
#include "status.h"
namespace easy_db
{
    template <typename T>
    class Result
    {
    public:
        Result(const Result &result) : status_(result.status_)
        {
            if (!result.p_value_)
            {
                p_value_ = nullptr;
                return;
            }

            p_value_ = std::make_unique<T>(*(result.p_value_));
        }

        Result &operator=(const Result &result)
        {
            if (this == &result)
            {
                return *this;
            }
            status_ = result.status_;

            if (!result.p_value_)
            {
                p_value_ = nullptr;
            }
            else
            {
                p_value_ = std::make_unique<T>(*(result.p_value_));
            }

            return *this;
        }

        Result(Result &&result) noexcept = default;
        Result &operator=(Result &&result) noexcept = default;

        static Result ok(const T &value)
        {
            return Result(Status::ok(), std::make_unique<T>(value));
        }

        static Result
        error(const Status &status)
        {
            return Result(status, nullptr);
        }

        bool is_ok() const
        {
            return status_.is_ok();
        }

        bool is_error() const
        {
            return !status_.is_ok();
        }

        Status status() const
        {
            return status_;
        }

        const T &value() const
        {
            return *p_value_;
        }

        T &value()
        {
            return *p_value_;
        }

    private:
        Result(const Status &status, std::unique_ptr<T> p_value) : status_(status), p_value_(std::move(p_value))
        {
        }
        Status status_;
        std::unique_ptr<T> p_value_;
    };
}
#endif
