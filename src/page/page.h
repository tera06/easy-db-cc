#ifndef EASY_DB_PAGE_H_
#define EASY_DB_PAGE_H_

#include <array>
#include "common/result.h"
namespace easy_db
{
    class Page
    {
    private:
        static constexpr std::size_t kPageSize = 4096;

    public:
        using iterator = std::array<std::byte, kPageSize>::iterator;
        using const_iterator = std::array<std::byte, kPageSize>::const_iterator;
        Result<iterator>
        iterator_at(std::size_t offset)
        {
            if (offset > kPageSize)
            {
                return Result<iterator>::error(Status::out_of_range());
            }
            return Result<iterator>::ok(data_.begin() + offset);
        }

        Result<const_iterator> const_iterator_at(std::size_t offset) const
        {
            if (offset > kPageSize)
            {
                return Result<const_iterator>::error(Status::out_of_range());
            }
            return Result<const_iterator>::ok(data_.cbegin() + offset);
        }

    private:
        std::array<std::byte, kPageSize> data_ = {};
    };
}

#endif