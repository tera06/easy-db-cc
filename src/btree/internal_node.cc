#include "internal_node.h"

#include <algorithm>

namespace easy_db
{
    Result<InternalNode::num_keys_t> InternalNode::num_keys() const
    {
        return page_.read_uint32(kNumKeysOffset);
    }

    Result<void> InternalNode::set_num_keys(num_keys_t num_keys)
    {
        if (num_keys > kMaxCells)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        return page_.write_uint32(num_keys, kNumKeysOffset);
    }

    Result<PageManager::page_id_t> InternalNode::right_child_id() const
    {
        return page_.read_uint32(kRightChildOffset);
    }

    Result<void> InternalNode::set_right_child_id(child_id_t page_id)
    {
        return page_.write_uint32(page_id, kRightChildOffset);
    }

    Result<PageManager::page_id_t> InternalNode::child_id(child_num_t child_num) const
    {
        const auto result_num_keys = num_keys();
        if (result_num_keys.is_error())
        {
            return Result<child_id_t>::error(result_num_keys.status());
        }

        const auto num_keys = result_num_keys.value();
        if (child_num > num_keys)
        {
            return Result<child_id_t>::error(Status::invalid_argument());
        }

        if (child_num == num_keys)
        {
            return right_child_id();
        }

        const auto offset = kHeaderSize + child_num * kCellSize + kChildOffset;
        return page_.read_uint32(offset);
    }

    Result<void> InternalNode::set_child_id(child_num_t child_num, child_id_t child_id)
    {
        const auto result_num_keys = num_keys();
        if (result_num_keys.is_error())
        {
            return Result<void>::error(result_num_keys.status());
        }

        const auto num_keys = result_num_keys.value();
        if (child_num > num_keys)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        if (child_num == num_keys)
        {
            return set_right_child_id(child_id);
        }

        const auto offset = kHeaderSize + child_num * kCellSize + kChildOffset;
        return page_.write_uint32(child_id, offset);
    }

    Result<InternalNode::key_t> InternalNode::key(cell_num_t cell_num) const
    {
        const auto result_num_keys = num_keys();
        if (result_num_keys.is_error())
        {
            return Result<key_t>::error(result_num_keys.status());
        }

        const auto num_keys = result_num_keys.value();
        if (cell_num >= num_keys)
        {
            return Result<key_t>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kKeyOffset;
        return page_.read_uint32(offset);
    }

    Result<void> InternalNode::set_key(cell_num_t cell_num, key_t key)
    {
        const auto result_num_keys = num_keys();
        if (result_num_keys.is_error())
        {
            return Result<void>::error(result_num_keys.status());
        }

        const auto num_keys = result_num_keys.value();
        if (cell_num >= num_keys)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kKeyOffset;
        return page_.write_uint32(key, offset);
    }

    Result<InternalNode::child_num_t> InternalNode::find_child(key_t key) const
    {
        const auto result_num_keys = num_keys();
        if (result_num_keys.is_error())
        {
            return Result<child_num_t>::error(result_num_keys.status());
        }

        child_num_t min_child_num = 0;
        child_num_t max_child_num = result_num_keys.value();
        if (max_child_num == 0)
        {
            return Result<child_num_t>::error(Status::key_not_exist());
        }

        while (min_child_num != max_child_num)
        {
            const child_num_t mid_child_num = (min_child_num + max_child_num) / 2;
            const auto result_mid_key = this->key(mid_child_num);
            if (result_mid_key.is_error())
            {
                return Result<child_num_t>::error(result_mid_key.status());
            }

            if (key <= result_mid_key.value())
            {
                max_child_num = mid_child_num;
            }
            else
            {
                min_child_num = mid_child_num + 1;
            }
        }

        return Result<child_num_t>::ok(min_child_num);
    }

    Result<void> InternalNode::initialize()
    {
        auto result = set_node_type(NodeType::Internal);
        if (result.is_error())
        {
            return result;
        }

        result = set_is_root(false);
        if (result.is_error())
        {
            return result;
        }

        result = set_parent_id(PageManager::kInvalidPageId);
        if (result.is_error())
        {
            return result;
        }

        result = set_num_keys(0);
        if (result.is_error())
        {
            return result;
        }

        result = set_right_child_id(PageManager::kInvalidPageId);
        if (result.is_error())
        {
            return result;
        }

        const auto result_iter = page_.iterator_at(kHeaderSize);
        if (result_iter.is_error())
        {
            return Result<void>::error(result_iter.status());
        }

        std::fill_n(
            result_iter.value(),
            Page::kPageSize - kHeaderSize,
            std::byte{0});

        return Result<void>::ok();
    }
}
