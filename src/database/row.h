#ifndef EASY_DB_DATABASE_ROW_H_
#define EASY_DB_DATABASE_ROW_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "common/result.h"

namespace easy_db
{
    class Row
    {
    private:
        static constexpr std::size_t kUsernameSize = 32;
        static constexpr std::size_t kEmailSize = 255;
        static constexpr std::size_t kIdOffset = 0;
        static constexpr std::size_t kSerializedSize =
            sizeof(std::uint32_t) + kUsernameSize + kEmailSize;

    public:
        using row_serialized_t = std::array<std::byte, kSerializedSize>;
        using id_t = uint32_t;

        static Result<Row> create(
            std::uint32_t id,
            std::string username,
            std::string email);

        id_t get_id() const;
        const std::string &get_username() const;
        const std::string &get_email() const;
        Result<row_serialized_t> serialize() const;
        static Result<Row> deserialize(const row_serialized_t &row_serialized);

    private:
        static constexpr std::size_t kUsernameOffset =
            kIdOffset + sizeof(std::uint32_t);
        static constexpr std::size_t kEmailOffset =
            kUsernameOffset + kUsernameSize;

        const id_t id;
        const std::string username;
        const std::string email;

        Row(id_t id, std::string username, std::string email);

        static std::string read_string_from_row_serialized(
            const row_serialized_t &row_serialized,
            std::size_t offset,
            std::size_t size);
    };
}

#endif
