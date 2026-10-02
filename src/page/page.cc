
#include "page.h"
#include "common/status.h"
namespace easy_db
{
    Result<Page::iterator>
    Page::iterator_at(std::size_t offset)
    {
        if (offset >= kPageSize)
        {
            return Result<iterator>::error(Status::out_of_range());
        }
        return Result<iterator>::ok(data_.begin() + offset);
    }

    Result<Page::const_iterator> Page::const_iterator_at(std::size_t offset) const
    {
        if (offset >= kPageSize)
        {
            return Result<const_iterator>::error(Status::out_of_range());
        }
        return Result<const_iterator>::ok(data_.cbegin() + offset);
    }

    Result<uint32_t> Page::read_uint32(std::size_t offset) const
    {
        if (offset + sizeof(std::uint32_t) > kPageSize)
        {
            return Result<std::uint32_t>::error(Status::out_of_range());
        }
        auto byte0 = std::to_integer<uint32_t>(data_[offset]);
        auto byte1 = std::to_integer<uint32_t>(data_[offset + 1]);
        auto byte2 = std::to_integer<uint32_t>(data_[offset + 2]);
        auto byte3 = std::to_integer<uint32_t>(data_[offset + 3]);
        const auto value =
            byte0 |
            (byte1 << 8) |
            (byte2 << 16) |
            (byte3 << 24);

        return Result<std::uint32_t>::ok(value);
    }

    Result<void> Page::write_uint32(uint32_t value, std::size_t offset)
    {
        if (offset + sizeof(std::uint32_t) > kPageSize)
        {
            return Result<void>::error(Status::out_of_range());
        }

        data_[offset] =
            static_cast<std::byte>(value & 0xFF);
        data_[offset + 1] = static_cast<std::byte>((value >> 8) & 0xFF);
        data_[offset + 2] = static_cast<std::byte>((value >> 16) & 0xFF);

        data_[offset + 3] = static_cast<std::byte>((value >> 24) & 0xFF);

        return Result<void>::ok();
    }

}