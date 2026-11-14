#include "database/table.h"

namespace easy_db
{
    Table::Table(const std::string &filename)
        : btree_(std::make_unique<Btree>(filename))
    {
    }

    Result<void> Table::initialize()
    {
        return btree_->initialize();
    }

    Result<void> Table::insert(const Row &row)
    {
        const auto serialized_row = row.serialize();
        if (serialized_row.is_error())
        {
            return Result<void>::error(serialized_row.status());
        }

        return btree_->insert(
            row.get_id(),
            serialized_row.value());
    }

    Result<std::vector<Row>> Table::select_all() const
    {
        const auto serialized_rows = btree_->get_all();
        if (serialized_rows.is_error())
        {
            return Result<std::vector<Row>>::error(serialized_rows.status());
        }

        std::vector<Row> rows;
        rows.reserve(serialized_rows.value().size());
        for (const auto &serialized_row : serialized_rows.value())
        {
            const auto row = Row::deserialize(serialized_row);
            if (row.is_error())
            {
                return Result<std::vector<Row>>::error(row.status());
            }
            rows.push_back(row.value());
        }

        return Result<std::vector<Row>>::ok(rows);
    }

    Result<void> Table::flush()
    {
        return btree_->flush();
    }
}
