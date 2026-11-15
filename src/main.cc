#include <iostream>

#include "repl/repl.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: easy_db <database-file>\n";
        return 2;
    }

    easy_db::Repl repl(argv[1]);
    const auto status = repl.start();
    if (!status.is_ok())
    {
        std::cerr << "Error: database REPL exited unexpectedly\n";
        return 1;
    }

    return 0;
}