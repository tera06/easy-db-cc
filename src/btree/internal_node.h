#ifndef EASY_DB_INTERNAL_NODE_H_
#define EASY_DB_INTERNAL_NODE_H_

#include "page/page.h"
#include "node.h"

namespace easy_db
{
    class InternalNode : public Node
    {
    public:
        using num_keys_t = uint32_t;
        using key_t = uint32_t;
        using child_num_t = uint32_t;
        using child_id_t = PageManager::page_id_t;
        using cell_num_t = uint32_t;
        using max_cells_t = uint32_t;
        static constexpr max_cells_t kMaxCells = 3;

        explicit InternalNode(Page &page) : Node(page) {}

        Result<num_keys_t> num_keys() const;
        Result<void> set_num_keys(num_keys_t num_keys);
        Result<child_id_t> right_child_id() const;
        Result<void> set_right_child_id(child_id_t page_id);
        Result<child_id_t> child_id(child_num_t child_num) const;
        Result<void> set_child_id(child_num_t child_num, child_id_t child_id);
        Result<key_t> key(cell_num_t cell_num) const;
        Result<void> set_key(cell_num_t cell_num, key_t key);
        Result<child_num_t> find_child(key_t key) const;
        Result<void> initialize();

    private:
        static constexpr std::size_t kNumKeysSize = sizeof(num_keys_t);
        static constexpr std::size_t kNumKeysOffset = kCommonHeaderSize;
        static constexpr std::size_t kRightChildSize = sizeof(child_id_t);
        static constexpr std::size_t kRightChildOffset = kNumKeysOffset + kNumKeysSize;
        static constexpr std::size_t kHeaderSize = kCommonHeaderSize + kNumKeysSize + kRightChildSize;

        static constexpr std::size_t kChildSize = sizeof(child_id_t);
        static constexpr std::size_t kChildOffset = 0;
        static constexpr std::size_t kKeySize = sizeof(key_t);
        static constexpr std::size_t kKeyOffset = kChildOffset + kChildSize;
        static constexpr std::size_t kCellSize = kChildSize + kKeySize;
    };
}

#endif
