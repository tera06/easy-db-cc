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

    Status Status::out_of_range()
    {
        return Status(StatusCode::OutOfRange);
    }

    Status Status::invalid_page_id()
    {
        return Status(StatusCode::InvalidPageId);
    }

    Status Status::fail_open_file()
    {
        return Status(StatusCode::FailOpenFile);
    }

    Status Status::file_not_exist()
    {
        return Status(StatusCode::FileNotExist);
    }

    Status Status::not_find_page()
    {
        return Status(StatusCode::NotFindPage);
    }

    Status Status::fail_file_size()
    {
        return Status(StatusCode::FailFileSize);
    }

    Status Status::fail_seek()
    {
        return Status(StatusCode::FailSeek);
    }

    Status Status::fail_read_file()
    {
        return Status(StatusCode::FailReadFile);
    }

    Status Status::fail_write_file()
    {
        return Status(StatusCode::FailWriteFile);
    }
    Status Status::invalid_node_type()
    {
        return Status(StatusCode::InvalidNodeType);
    }

    Status Status::invalid_is_root()
    {
        return Status(StatusCode::InvalidIsRoot);
    }

    Status Status::invalid_argument()
    {
        return Status(StatusCode::InvalidArgument);
    }

    bool Status::is_ok() const
    {
        return code_ == StatusCode::Ok;
    }

    bool Status::is_other_error() const
    {
        return code_ == StatusCode::OtherError;
    }

    bool Status::is_out_of_range() const
    {
        return code_ == StatusCode::OutOfRange;
    }

    bool Status::is_invalid_page_id() const
    {
        return code_ == StatusCode::InvalidPageId;
    }

    bool Status::is_fail_open_file() const
    {
        return code_ == StatusCode::FailOpenFile;
    }

    bool Status::is_file_not_exist() const
    {
        return code_ == StatusCode::FileNotExist;
    }

    bool Status::is_not_find_page() const
    {
        return code_ == StatusCode::NotFindPage;
    }

    bool Status::is_fail_file_size() const
    {
        return code_ == StatusCode::FailFileSize;
    }

    bool Status::is_fail_seek() const
    {
        return code_ == StatusCode::FailSeek;
    }

    bool Status::is_fail_read_file() const
    {
        return code_ == StatusCode::FailReadFile;
    }

    bool Status::is_fail_write_file() const
    {
        return code_ == StatusCode::FailWriteFile;
    }

    bool Status::is_invalid_node_type() const
    {
        return code_ == StatusCode::InvalidNodeType;
    }

    bool Status::is_invalid_is_root() const
    {
        return code_ == StatusCode::InvalidIsRoot;
    }

    bool Status::is_invalid_argument() const
    {
        return code_ == StatusCode::InvalidArgument;
    }

}
