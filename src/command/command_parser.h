#ifndef EASY_DB_COMMAND_COMMAND_PARSER_H_
#define EASY_DB_COMMAND_COMMAND_PARSER_H_

#include <memory>
#include <string>

#include "command/command_statement.h"
#include "common/result.h"

namespace easy_db
{
    class CommandParser
    {
    public:
        using ParseTarget = std::string;

        Result<std::unique_ptr<CommandStatement>> parse(
            const ParseTarget &target) const;
    };
}

#endif
