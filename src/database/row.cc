#include "database/row.h"

#include <algorithm>
#include <utility>

namespace easy_db
{
    Row::Row(id_t id, std::string username, std::string email)
        : id(id), username(std::move(username)), email(std::move(email))
    {
    }

    Row::id_t Row::get_id() const
    {
        return id;
    }

    Result<Row> Row::create(
        id_t id,
        std::string username,
        std::string email)
    {
        if (username.size() > kUsernameSize ||
            email.size() > kEmailSize ||
            username.find('\0') != std::string::npos ||
            email.find('\0') != std::string::npos)
        {
            return Result<Row>::error(Status::invalid_argument());
        }

        return Result<Row>::ok(
            Row(id, std::move(username), std::move(email)));
    }

    std::string Row::read_string_from_row_serialized(
        const row_serialized_t &row_serialized,
        std::size_t offset,
        std::size_t size)
    {
        const auto begin = row_serialized.begin() + offset;
        const auto end = begin + size;
        const auto terminator = std::find(begin, end, std::byte{0});
        std::string value;
        value.reserve(static_cast<std::size_t>(terminator - begin));
        for (auto it = begin; it != terminator; ++it)
        {
            value.push_back(static_cast<char>(
                std::to_integer<unsigned char>(*it)));
        }
        return value;
    }

    Result<Row::row_serialized_t> Row::serialize() const
    {
        row_serialized_t row_serialized{};
        for (std::size_t i = 0; i < sizeof(id); ++i)
        {
            row_serialized[kIdOffset + i] =
                static_cast<std::byte>((id >> (i * 8)) & 0xff);
        }

        std::transform(
            username.begin(),
            username.end(),
            row_serialized.begin() + kUsernameOffset,
            [](char value)
            {
                return static_cast<std::byte>(static_cast<unsigned char>(value));
            });
        std::transform(
            email.begin(),
            email.end(),
            row_serialized.begin() + kEmailOffset,
            [](char value)
            {
                return static_cast<std::byte>(static_cast<unsigned char>(value));
            });

        return Result<row_serialized_t>::ok(row_serialized);
    }

    Result<Row> Row::deserialize(const row_serialized_t &row_serialized)
    {
        std::uint32_t id = 0;
        for (std::size_t i = 0; i < sizeof(id); ++i)
        {
            id |= static_cast<std::uint32_t>(
                      std::to_integer<unsigned char>(row_serialized[kIdOffset + i]))
                  << (i * 8);
        }

        return create(
            id,
            read_string_from_row_serialized(row_serialized, kUsernameOffset, kUsernameSize),
            read_string_from_row_serialized(row_serialized, kEmailOffset, kEmailSize));
    }
}
