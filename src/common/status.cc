#include "status.h"

namespace easy_db
{
    Status Status::ok()
    {
        return Status(StatusCode::Ok);
    }

    Status Status::other_error()
    {
        return Status(StatusCode::OtherError);
    }

    bool Status::is_ok() const
    {
        return code_ == StatusCode::Ok;
    }

    bool Status::is_other_error() const
    {
        return code_ == StatusCode::OtherError;
    }
}
