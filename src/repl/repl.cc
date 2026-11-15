#include "repl/repl.h"

#include <string>

namespace easy_db
{
    namespace
    {
        std::string format_error_message(const Status &status)
        {
            if (status.is_invalid_argument())
            {
                return "Error: invalid command";
            }
            if (status.is_key_not_exist())
            {
                return "Error: key not found";
            }
            if (status.is_out_of_range())
            {
                return "Error: out of range";
            }
            if (status.is_fail_open_file())
            {
                return "Error: failed to open file";
            }
            return "Error: command failed";
        }
    }

    Repl::Repl(Table &table, std::istream &input, std::ostream &output)
        : owned_table_(nullptr),
          command_(std::make_unique<Command>(table)),
          initialization_status_(Status::ok()),
          input_(input),
          output_(output)
    {
    }

    Repl::Repl(const std::string &filename,
               std::istream &input,
               std::ostream &output)
        : owned_table_(nullptr),
          command_(nullptr),
          initialization_status_(Status::ok()),
          input_(input),
          output_(output)
    {
        auto table = std::make_unique<Table>(filename);
        const auto initialize_result = table->initialize();
        if (initialize_result.is_error())
        {
            initialization_status_ = initialize_result.status();
            return;
        }

        owned_table_ = std::move(table);
        command_ = std::make_unique<Command>(*owned_table_);
    }

    Status Repl::start()
    {
        if (!command_)
        {
            return initialization_status_.is_ok()
                       ? Status::other_error()
                       : initialization_status_;
        }

        while (true)
        {
            const auto prompt_status = print_prompt();
            if (!prompt_status.is_ok())
            {
                return prompt_status;
            }

            const auto input_result = read_user_input();
            if (input_result.is_error())
            {
                return input_result.status();
            }

            const auto user_input = input_result.value();
            if (user_input.empty())
            {
                continue;
            }

            if (user_input == "exit" || user_input == "quit")
            {
                return Status::ok();
            }

            const auto execute_result = command_->execute(user_input);
            if (execute_result.is_error())
            {
                UserOutput error_output = format_error_message(execute_result.status());
                const auto print_status = print_user_output(error_output);
                if (!print_status.is_ok())
                {
                    return print_status;
                }
                continue;
            }

            UserOutput output = execute_result.value();
            const auto print_status = print_user_output(output);
            if (!print_status.is_ok())
            {
                return print_status;
            }
        }
    }

    Status Repl::print_prompt() const
    {
        output_ << "> ";
        output_.flush();
        return Status::ok();
    }

    Result<Repl::UserInput> Repl::read_user_input() const
    {
        UserInput input;
        if (!std::getline(input_, input))
        {
            return Result<UserInput>::error(Status::other_error());
        }
        return Result<UserInput>::ok(input);
    }

    Status Repl::print_user_output(UserOutput &output)
    {
        output_ << output << '\n';
        return Status::ok();
    }
}
