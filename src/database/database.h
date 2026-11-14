#ifndef EASY_DB_DATABASE_DATABASE_H_
#define EASY_DB_DATABASE_DATABASE_H_

#include <memory>
#include <string>

#include "common/result.h"
#include "database/table.h"

namespace easy_db
{
    class Database
    {
    public:
        static Result<std::unique_ptr<Table>> open(const std::string &filename);
    };
}

#endif
