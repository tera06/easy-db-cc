#ifndef EASY_DB_COMMAND_COMMAND_EXECUTOR_H_
#define EASY_DB_COMMAND_COMMAND_EXECUTOR_H_

#include <string>

#include "command/command_statement.h"
#include "common/result.h"
#include "database/table.h"

namespace easy_db
{
    class CommandExecutor
    {
    public:
        using CommandResult = std::string;

        explicit CommandExecutor(Table &table);

        Result<CommandResult> execute(
            const CommandStatement &statement) const;

    private:
        Table &table_;
    };
}

#endif
