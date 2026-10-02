#ifndef EASY_DB_PAGE_H_
#define EASY_DB_PAGE_H_

#include <array>
#include "common/result.h"
#include <cstdint>
namespace easy_db
{
    class Page
    {
    public:
        static constexpr std::size_t kPageSize = 4096;

        using iterator = std::array<std::byte, kPageSize>::iterator;
        using const_iterator = std::array<std::byte, kPageSize>::const_iterator;

        Result<iterator>
        iterator_at(std::size_t offset);

        Result<const_iterator> const_iterator_at(std::size_t offset) const;
        Result<uint32_t> read_uint32(std::size_t offset) const;
        Result<void> write_uint32(uint32_t value, std::size_t offset);

    private:
        std::array<std::byte, kPageSize> data_ = {};
    };
}

#endif