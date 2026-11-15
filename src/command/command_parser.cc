#include "command/command_parser.h"

#include <charconv>
#include <cstdint>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "database/row.h"

namespace easy_db
{
    namespace
    {
        std::string uppercase(std::string value)
        {
            for (auto &character : value)
            {
                if (character >= 'a' && character <= 'z')
                {
                    character = static_cast<char>(character - 'a' + 'A');
                }
            }
            return value;
        }
    }

    Result<std::unique_ptr<CommandStatement>> CommandParser::parse(
        const ParseTarget &target) const
    {
        std::istringstream input(target);
        std::vector<std::string> tokens;
        for (std::string token; input >> token;)
        {
            tokens.push_back(std::move(token));
        }

        if (tokens.empty())
        {
            return Result<std::unique_ptr<CommandStatement>>::error(
                Status::invalid_argument());
        }

        const auto command = uppercase(tokens[0]);
        if (command == "SELECT")
        {
            if (tokens.size() == 1 ||
                (tokens.size() == 2 && tokens[1] == "*"))
            {
                return Result<std::unique_ptr<CommandStatement>>::ok(
                    std::make_unique<CommandStatement>(
                        CommandStatement::select_all()));
            }
            return Result<std::unique_ptr<CommandStatement>>::error(
                Status::invalid_argument());
        }

        if (command != "INSERT" || tokens.size() != 4)
        {
            return Result<std::unique_ptr<CommandStatement>>::error(
                Status::invalid_argument());
        }

        std::uint32_t id = 0;
        const auto &id_token = tokens[1];
        const auto id_parse = std::from_chars(
            id_token.data(), id_token.data() + id_token.size(), id);
        if (id_parse.ec != std::errc{} ||
            id_parse.ptr != id_token.data() + id_token.size())
        {
            return Result<std::unique_ptr<CommandStatement>>::error(
                Status::invalid_argument());
        }

        auto row = Row::create(id, tokens[2], tokens[3]);
        if (row.is_error())
        {
            return Result<std::unique_ptr<CommandStatement>>::error(
                row.status());
        }

        return Result<std::unique_ptr<CommandStatement>>::ok(
            std::make_unique<CommandStatement>(
                CommandStatement::insert(row.value())));
    }
}
