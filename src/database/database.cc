#include "database/database.h"

#include <utility>

namespace easy_db
{
    Result<std::unique_ptr<Table>> Database::open(const std::string &filename)
    {
        auto table = std::make_unique<Table>(filename);
        const auto initialize_result = table->initialize();
        if (initialize_result.is_error())
        {
            return Result<std::unique_ptr<Table>>::error(
                initialize_result.status());
        }

        return Result<std::unique_ptr<Table>>::ok(std::move(table));
    }
}
