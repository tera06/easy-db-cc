#ifndef EASY_DB_DATABASE_TABLE_H_
#define EASY_DB_DATABASE_TABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "btree/btree.h"
#include "common/result.h"
#include "database/row.h"

namespace easy_db
{
    class Table
    {
    public:
        explicit Table(const std::string &filename);

        Result<void> initialize();
        Result<void> insert(const Row &row);
        Result<std::vector<Row>> select_all() const;
        Result<void> flush();

    private:
        std::unique_ptr<Btree> btree_;
    };
}

#endif
