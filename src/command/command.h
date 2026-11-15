#ifndef EASY_DB_COMMAND_COMMAND_H_
#define EASY_DB_COMMAND_COMMAND_H_

#include <string>

#include "command/command_executor.h"
#include "command/command_parser.h"

namespace easy_db
{
    class Command
    {
    public:
        using UserCommand = std::string;
        using CommandResult = std::string;

        explicit Command(Table &table);

        Result<CommandResult> execute(const UserCommand &user_command) const;

    private:
        CommandParser parser_;
        CommandExecutor executor_;
    };
}

#endif
