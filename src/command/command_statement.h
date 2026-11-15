#ifndef EASY_DB_COMMAND_COMMAND_STATEMENT_H_
#define EASY_DB_COMMAND_COMMAND_STATEMENT_H_

#include <optional>

#include "database/row.h"

namespace easy_db
{
    enum class StatementType
    {
        Insert,
        SelectAll,
    };

    class CommandStatement
    {
    public:
        static CommandStatement insert(Row row);
        static CommandStatement select_all();

        StatementType statement_type() const;
        const Row *insert_row() const;

    private:
        explicit CommandStatement(StatementType statement_type);
        CommandStatement(StatementType statement_type, Row row);

        StatementType statement_type_;
        std::optional<Row> insert_row_;
    };
}

#endif
