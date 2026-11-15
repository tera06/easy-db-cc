#include "command/command_statement.h"

#include <utility>

namespace easy_db
{
    CommandStatement::CommandStatement(StatementType statement_type)
        : statement_type_(statement_type)
    {
    }

    CommandStatement::CommandStatement(StatementType statement_type, Row row)
        : statement_type_(statement_type), insert_row_(std::move(row))
    {
    }

    CommandStatement CommandStatement::insert(Row row)
    {
        return CommandStatement(StatementType::Insert, std::move(row));
    }

    CommandStatement CommandStatement::select_all()
    {
        return CommandStatement(StatementType::SelectAll);
    }

    StatementType CommandStatement::statement_type() const
    {
        return statement_type_;
    }

    const Row *CommandStatement::insert_row() const
    {
        return insert_row_ ? &*insert_row_ : nullptr;
    }
}
