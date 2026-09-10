#ifndef EASY_DB_STATUS_H_
#define EASY_DB_STATUS_H_

namespace easy_db
{

    class Status
    {
    public:
        Status(const Status &status) = default;
        Status &operator=(const Status &status) = default;

        Status(Status &&status) noexcept = default;
        Status &operator=(Status &&status) noexcept = default;

        static Status ok();
        static Status out_of_range();
        static Status other_error();

        bool is_ok() const;
        bool is_out_of_range() const;
        bool is_other_error() const;

    private:
        enum class StatusCode
        {
            Ok,
            OutOfRange,
            OtherError,
        };
        explicit Status(const StatusCode code) : code_(code) {}
        StatusCode code_;
    };
};

#endif