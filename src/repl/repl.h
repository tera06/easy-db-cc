#ifndef EASY_DB_REPL_REPL_H_
#define EASY_DB_REPL_REPL_H_

#include <iostream>
#include <memory>
#include <string>

#include "command/command.h"
#include "common/result.h"
#include "database/database.h"
#include "database/table.h"

namespace easy_db
{
    class Repl
    {
    public:
        using UserInput = std::string;
        using UserOutput = std::string;

        explicit Repl(Table &table,
                      std::istream &input = std::cin,
                      std::ostream &output = std::cout);
        explicit Repl(const std::string &filename,
                      std::istream &input = std::cin,
                      std::ostream &output = std::cout);

        Status start();

    private:
        Status print_prompt() const;
        Result<UserInput> read_user_input() const;
        Status print_user_output(UserOutput &output);

        std::unique_ptr<Table> owned_table_;
        std::unique_ptr<Command> command_;
        Status initialization_status_;
        std::istream &input_;
        std::ostream &output_;
    };
}

#endif
