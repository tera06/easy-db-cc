#include "leaf_node.h"
#include <algorithm>

namespace easy_db
{
    Result<LeafNode::num_cells_t> LeafNode::num_cells() const
    {
        return page_.read_uint32(kNumCellsOffset);
    }

    Result<void> LeafNode::set_num_cells(num_cells_t num_cells)
    {
        if (num_cells > kMaxCells)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        return page_.write_uint32(num_cells, kNumCellsOffset);
    }

    Result<PageManager::page_id_t> LeafNode::next_leaf_id() const
    {
        return page_.read_uint32(kNextLeafOffset);
    }
    Result<void> LeafNode::set_next_leaf_id(PageManager::page_id_t page_id)
    {
        return page_.write_uint32(page_id, kNextLeafOffset);
    }
    Result<LeafNode::key_t> LeafNode::key(cell_num_t cell_num) const
    {
        if (cell_num > kMaxCells - 1)
        {
            return Result<key_t>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kKeyOffset;

        return page_.read_uint32(offset);
    }
    Result<void> LeafNode::set_key(cell_num_t cell_num, key_t key)
    {
        if (cell_num > kMaxCells - 1)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kKeyOffset;

        return page_.write_uint32(key, offset);
    }
    Result<LeafNode::value_t> LeafNode::value(cell_num_t cell_num) const
    {
        if (cell_num > kMaxCells - 1)
        {
            return Result<value_t>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kValueOffset;

        auto result_iter = page_.const_iterator_at(offset);
        if (result_iter.is_error())
        {
            return Result<value_t>::error(result_iter.status());
        }

        value_t value{};
        std::copy_n(
            result_iter.value(),
            kValueSize,
            value.begin());

        return Result<value_t>::ok(value);
    }

    Result<void> LeafNode::set_value(cell_num_t cell_num, const value_t &value)
    {
        if (cell_num > kMaxCells - 1)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        const auto offset = kHeaderSize + cell_num * kCellSize + kValueOffset;

        auto result_iter = page_.iterator_at(offset);
        if (result_iter.is_error())
        {
            return Result<void>::error(result_iter.status());
        }

        std::copy_n(
            value.begin(),
            kValueSize,
            result_iter.value());

        return Result<void>::ok();
    }

    Result<void> LeafNode::initialize()
    {
        auto result = set_node_type(NodeType::Leaf);
        if (result.is_error())
            return result;

        result = set_is_root(false);
        if (result.is_error())
            return result;

        result = set_parent_id(PageManager::kInvalidPageId);
        if (result.is_error())
            return result;

        result = set_num_cells(0);
        if (result.is_error())
            return result;

        result = set_next_leaf_id(PageManager::kInvalidPageId);
        if (result.is_error())
            return result;

        auto result_iter = page_.iterator_at(kHeaderSize);
        if (result_iter.is_error())
            return Result<void>::error(result_iter.status());

        std::fill_n(
            result_iter.value(),
            Page::kPageSize - kHeaderSize,
            std::byte{0});

        return Result<void>::ok();
    }
}