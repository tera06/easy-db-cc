#include "command/command.h"

namespace easy_db
{
    Command::Command(Table &table) : executor_(table)
    {
    }

    Result<Command::CommandResult> Command::execute(
        const UserCommand &user_command) const
    {
        const auto statement = parser_.parse(user_command);
        if (statement.is_error())
        {
            return Result<CommandResult>::error(statement.status());
        }

        return executor_.execute(*statement.value());
    }
}
