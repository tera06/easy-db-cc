#ifndef EASY_DB_STATUS_H_
#define EASY_DB_STATUS_H_

namespace easy_db
{

    class Status
    {
    public:
        static Status ok();
        static Status other_error();
        static Status out_of_range();
        static Status invalid_page_id();
        static Status fail_open_file();
        static Status file_not_exist();
        static Status not_find_page();
        static Status fail_file_size();
        static Status fail_seek();
        static Status fail_read_file();
        static Status fail_write_file();
        static Status invalid_node_type();
        static Status invalid_is_root();
        static Status invalid_argument();
        static Status key_not_exist();

        bool is_ok() const;
        bool is_other_error() const;
        bool is_out_of_range() const;
        bool is_invalid_page_id() const;
        bool is_fail_open_file() const;
        bool is_file_not_exist() const;
        bool is_not_find_page() const;
        bool is_fail_file_size() const;
        bool is_fail_seek() const;
        bool is_fail_read_file() const;
        bool is_fail_write_file() const;
        bool is_invalid_node_type() const;
        bool is_invalid_is_root() const;
        bool is_invalid_argument() const;
        bool is_key_not_exist() const;

    private:
        enum class StatusCode
        {
            Ok,
            OtherError,
            OutOfRange,
            InvalidPageId,
            FailOpenFile,
            FileNotExist,
            NotFindPage,
            FailFileSize,
            FailSeek,
            FailReadFile,
            FailWriteFile,
            InvalidNodeType,
            InvalidIsRoot,
            InvalidArgument,
            KeyNotExist,
        };
        explicit Status(const StatusCode code) : code_(code) {}
        StatusCode code_;
    };
};

#endif