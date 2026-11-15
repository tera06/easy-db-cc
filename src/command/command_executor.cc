#include "command/command_executor.h"

#include <sstream>

namespace easy_db
{
    CommandExecutor::CommandExecutor(Table &table) : table_(table)
    {
    }

    Result<CommandExecutor::CommandResult> CommandExecutor::execute(
        const CommandStatement &statement) const
    {
        if (statement.statement_type() == StatementType::Insert)
        {
            const auto *row = statement.insert_row();
            if (row == nullptr)
            {
                return Result<CommandResult>::error(
                    Status::invalid_argument());
            }

            const auto insert_result = table_.insert(*row);
            if (insert_result.is_error())
            {
                return Result<CommandResult>::error(insert_result.status());
            }

            const auto flush_result = table_.flush();
            if (flush_result.is_error())
            {
                return Result<CommandResult>::error(flush_result.status());
            }

            return Result<CommandResult>::ok(
                "Inserted row " + std::to_string(row->get_id()));
        }

        if (statement.statement_type() == StatementType::SelectAll)
        {
            const auto rows = table_.select_all();
            if (rows.is_error())
            {
                return Result<CommandResult>::error(rows.status());
            }

            std::ostringstream output;
            bool first = true;
            for (const auto &row : rows.value())
            {
                if (!first)
                {
                    output << '\n';
                }
                output << row.get_id() << ", "
                       << row.get_username() << ", "
                       << row.get_email();
                first = false;
            }
            if (first)
            {
                output << "(no rows)";
            }
            return Result<CommandResult>::ok(output.str());
        }

        return Result<CommandResult>::error(Status::invalid_argument());
    }
}
