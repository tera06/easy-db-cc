#include "btree.h"

#include <algorithm>

#include "btree/internal_node.h"

namespace easy_db
{
    Btree::Btree(const std::string &filename)
        : page_manager_(std::make_unique<PageManager>(filename))
    {
    }

    Result<void> Btree::initialize()
    {
        if (initialized_)
        {
            return Result<void>::ok();
        }

        auto result_num_pages = page_manager_->num_pages();
        if (result_num_pages.is_error())
        {
            return Result<void>::error(result_num_pages.status());
        }

        if (result_num_pages.value() == 0)
        {
            auto result = create_new_tree();
            if (result.is_ok())
            {
                initialized_ = true;
            }

            return result;
        }

        if (result_num_pages.value() < 2)
        {
            return Result<void>::error(Status::invalid_argument());
        }
        auto result_metadata_page = page_manager_->get_page(kMetadataPageId);
        if (result_metadata_page.is_error())
        {
            return Result<void>::error(result_metadata_page.status());
        }

        auto result_magic = result_metadata_page.value().read_uint32(kMetadataMagicOffset);
        if (result_magic.is_error())
        {
            return Result<void>::error(result_magic.status());
        }
        if (result_magic.value() != kMetadataMagic)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        auto result_root_page_id =
            result_metadata_page.value().read_uint32(kMetadataRootPageIdOffset);
        if (result_root_page_id.is_error())
        {
            return Result<void>::error(result_root_page_id.status());
        }
        if (result_root_page_id.value() == PageManager::kInvalidPageId ||
            result_root_page_id.value() == 0 ||
            result_root_page_id.value() >= result_num_pages.value())
        {
            return Result<void>::error(Status::invalid_argument());
        }

        root_page_id_ = result_root_page_id.value();
        auto result_root_page = page_manager_->get_page(root_page_id_);
        if (result_root_page.is_error())
        {
            return Result<void>::error(result_root_page.status());
        }
        Node root_node(result_root_page.value());
        auto result_is_root = root_node.is_root();
        if (result_is_root.is_error())
        {
            return Result<void>::error(result_is_root.status());
        }
        if (!result_is_root.value())
        {
            return Result<void>::error(Status::invalid_argument());
        }

        initialized_ = true;
        return Result<void>::ok();
    }

    Result<void> Btree::create_new_tree()
    {
        auto result = create_metadata_page();
        if (result.is_error())
        {
            return result;
        }

        result = create_root_leaf_node();
        if (result.is_error())
        {
            return result;
        }

        return Result<void>::ok();
    }

    Result<void> Btree::create_metadata_page()
    {
        auto result_metadata_page_id = page_manager_->create_new_page();
        if (result_metadata_page_id.is_error())
        {
            return Result<void>::error(result_metadata_page_id.status());
        }

        return initialize_metadata_page(result_metadata_page_id.value());
    }

    Result<void> Btree::initialize_metadata_page(PageManager::page_id_t metadata_page_id)
    {
        auto result_metadata_page = page_manager_->get_page(metadata_page_id);
        if (result_metadata_page.is_error())
        {
            return Result<void>::error(result_metadata_page.status());
        }
        auto &metadata_page = result_metadata_page.value();

        auto result = metadata_page.write_uint32(kMetadataMagic, kMetadataMagicOffset);
        if (result.is_error())
        {
            return result;
        }

        result = metadata_page.write_uint32(
            PageManager::kInvalidPageId,
            kMetadataRootPageIdOffset);
        if (result.is_error())
        {
            return result;
        }
        mark_dirty(metadata_page_id);

        return Result<void>::ok();
    }

    Result<void> Btree::create_root_leaf_node()
    {
        auto result_root_page_id = create_leaf_node();
        if (result_root_page_id.is_error())
        {
            return Result<void>::error(result_root_page_id.status());
        }
        root_page_id_ = result_root_page_id.value();

        auto result = set_is_root(root_page_id_, true);
        if (result.is_error())
        {
            return result;
        }

        result = set_parent_id(
            root_page_id_,
            PageManager::kInvalidPageId);
        if (result.is_error())
        {
            return result;
        }
        result = set_root_node_id_in_metadata(root_page_id_);
        if (result.is_error())
        {
            return result;
        }
        return Result<void>::ok();
    }

    Result<void> Btree::insert(key_t key, const value_t &value)
    {
        auto result_initialized = require_initialized();
        if (result_initialized.is_error())
        {
            return result_initialized;
        }

        auto result_leaf_node_page_id = find_leaf_node_page_id(key);
        if (result_leaf_node_page_id.is_error())
        {
            return Result<void>::error(result_leaf_node_page_id.status());
        }

        auto result_page = page_manager_->get_page(result_leaf_node_page_id.value());
        if (result_page.is_error())
        {
            return Result<void>::error(result_page.status());
        }
        LeafNode leaf_node(result_page.value());

        auto result_num_cells = leaf_node.num_cells();
        if (result_num_cells.is_error())
        {
            return Result<void>::error(result_num_cells.status());
        }

        std::vector<LeafNodeCell> leaf_node_cells;
        leaf_node_cells.reserve(result_num_cells.value() + 1);
        bool key_exists = false;
        for (LeafNode::cell_num_t cell_num = 0;
             cell_num < result_num_cells.value();
             ++cell_num)
        {
            auto result_existing_key = leaf_node.key(cell_num);
            if (result_existing_key.is_error())
            {
                return Result<void>::error(result_existing_key.status());
            }
            auto result_existing_value = leaf_node.value(cell_num);
            if (result_existing_value.is_error())
            {
                return Result<void>::error(result_existing_value.status());
            }

            if (result_existing_key.value() == key)
            {
                leaf_node_cells.push_back({key, value});
                key_exists = true;
            }
            else
            {
                leaf_node_cells.push_back(
                    {result_existing_key.value(), result_existing_value.value()});
            }
        }

        if (!key_exists)
        {
            auto insert_position = std::lower_bound(
                leaf_node_cells.begin(),
                leaf_node_cells.end(),
                key,
                [](const LeafNodeCell &cell, key_t search_key)
                {
                    return cell.key < search_key;
                });
            leaf_node_cells.insert(insert_position, {key, value});
        }

        if (leaf_node_cells.size() > LeafNode::kMaxCells)
        {
            return split_leaf_node(
                result_leaf_node_page_id.value(),
                leaf_node_cells);
        }

        auto result_set_num_cells =
            leaf_node.set_num_cells(static_cast<LeafNode::num_cells_t>(leaf_node_cells.size()));
        if (result_set_num_cells.is_error())
        {
            return result_set_num_cells;
        }

        for (LeafNode::cell_num_t cell_num = 0;
             cell_num < leaf_node_cells.size();
             ++cell_num)
        {
            auto result = leaf_node.set_key(cell_num, leaf_node_cells[cell_num].key);
            if (result.is_error())
            {
                return result;
            }
            result = leaf_node.set_value(cell_num, leaf_node_cells[cell_num].value);
            if (result.is_error())
            {
                return result;
            }
        }

        mark_dirty(result_leaf_node_page_id.value());
        return Result<void>::ok();
    }

    Result<Btree::value_t> Btree::get_value(key_t key) const
    {
        auto result_initialized = require_initialized();
        if (result_initialized.is_error())
        {
            return Result<value_t>::error(result_initialized.status());
        }

        auto result_leaf_node_page_id = find_leaf_node_page_id(key);
        if (result_leaf_node_page_id.is_error())
        {
            return Result<value_t>::error(result_leaf_node_page_id.status());
        }

        auto result_page = page_manager_->get_page(result_leaf_node_page_id.value());
        if (result_page.is_error())
        {
            return Result<value_t>::error(result_page.status());
        }
        LeafNode leaf_node(result_page.value());

        auto result_num_cells = leaf_node.num_cells();
        if (result_num_cells.is_error())
        {
            return Result<value_t>::error(result_num_cells.status());
        }
        for (LeafNode::cell_num_t cell_num = 0;
             cell_num < result_num_cells.value();
             ++cell_num)
        {
            auto result_key = leaf_node.key(cell_num);
            if (result_key.is_error())
            {
                return Result<value_t>::error(result_key.status());
            }
            if (result_key.value() == key)
            {
                return leaf_node.value(cell_num);
            }
        }

        return Result<value_t>::error(Status::key_not_exist());
    }

    Result<std::vector<Btree::value_t>> Btree::get_all() const
    {
        auto result_initialized = require_initialized();
        if (result_initialized.is_error())
        {
            return Result<std::vector<value_t>>::error(result_initialized.status());
        }

        auto current_page_id = root_page_id_;
        while (true)
        {
            auto result_page = page_manager_->get_page(current_page_id);
            if (result_page.is_error())
            {
                return Result<std::vector<value_t>>::error(result_page.status());
            }
            Node node(result_page.value());
            auto result_node_type = node.node_type();
            if (result_node_type.is_error())
            {
                return Result<std::vector<value_t>>::error(result_node_type.status());
            }
            if (result_node_type.value() == Node::NodeType::Leaf)
            {
                break;
            }

            InternalNode internal_node(result_page.value());
            auto result_child_page_id = internal_node.child_id(0);
            if (result_child_page_id.is_error())
            {
                return Result<std::vector<value_t>>::error(result_child_page_id.status());
            }
            current_page_id = result_child_page_id.value();
        }

        std::vector<value_t> values;
        while (current_page_id != PageManager::kInvalidPageId)
        {
            auto result_page = page_manager_->get_page(current_page_id);
            if (result_page.is_error())
            {
                return Result<std::vector<value_t>>::error(result_page.status());
            }
            LeafNode leaf_node(result_page.value());

            auto result_num_cells = leaf_node.num_cells();
            if (result_num_cells.is_error())
            {
                return Result<std::vector<value_t>>::error(result_num_cells.status());
            }
            for (LeafNode::cell_num_t cell_num = 0;
                 cell_num < result_num_cells.value();
                 ++cell_num)
            {
                auto result_value = leaf_node.value(cell_num);
                if (result_value.is_error())
                {
                    return Result<std::vector<value_t>>::error(result_value.status());
                }
                values.push_back(result_value.value());
            }

            auto result_next_leaf_page_id = leaf_node.next_leaf_id();
            if (result_next_leaf_page_id.is_error())
            {
                return Result<std::vector<value_t>>::error(result_next_leaf_page_id.status());
            }
            current_page_id = result_next_leaf_page_id.value();
        }

        return Result<std::vector<value_t>>::ok(values);
    }

    Result<void> Btree::flush()
    {
        auto result_initialized = require_initialized();
        if (result_initialized.is_error())
        {
            return result_initialized;
        }

        std::vector<PageManager::page_id_t> page_ids(
            dirty_page_ids_.begin(),
            dirty_page_ids_.end());
        std::sort(page_ids.begin(), page_ids.end());

        for (const auto page_id : page_ids)
        {
            if (page_id == 0)
            {
                continue;
            }
            auto result = page_manager_->flush(page_id);
            if (result.is_error())
            {
                return result;
            }
        }

        if (dirty_page_ids_.find(0) != dirty_page_ids_.end())
        {
            auto result = page_manager_->flush(0);
            if (result.is_error())
            {
                return result;
            }
        }

        dirty_page_ids_.clear();
        return Result<void>::ok();
    }

    Result<PageManager::page_id_t> Btree::find_leaf_node_page_id(key_t key) const
    {
        auto current_page_id = root_page_id_;
        while (true)
        {
            auto result_page = page_manager_->get_page(current_page_id);
            if (result_page.is_error())
            {
                return Result<PageManager::page_id_t>::error(result_page.status());
            }

            Node node(result_page.value());
            auto result_node_type = node.node_type();
            if (result_node_type.is_error())
            {
                return Result<PageManager::page_id_t>::error(result_node_type.status());
            }
            if (result_node_type.value() == Node::NodeType::Leaf)
            {
                return Result<PageManager::page_id_t>::ok(current_page_id);
            }

            InternalNode internal_node(result_page.value());
            auto result_child_num = internal_node.find_child(key);
            if (result_child_num.is_error())
            {

                return Result<PageManager::page_id_t>::error(result_child_num.status());
            }
            auto result_child_page_id = internal_node.child_id(result_child_num.value());
            if (result_child_page_id.is_error())
            {
                return Result<PageManager::page_id_t>::error(result_child_page_id.status());
            }
            current_page_id = result_child_page_id.value();
        }
    }

    Result<PageManager::page_id_t> Btree::create_leaf_node()
    {
        auto result_page_id = page_manager_->create_new_page();
        if (result_page_id.is_error())
        {
            return Result<PageManager::page_id_t>::error(result_page_id.status());
        }
        auto result_page = page_manager_->get_page(result_page_id.value());
        if (result_page.is_error())
        {
            return Result<PageManager::page_id_t>::error(result_page.status());
        }
        LeafNode leaf_node(result_page.value());
        auto result = leaf_node.initialize();
        if (result.is_error())
        {
            return Result<PageManager::page_id_t>::error(result.status());
        }
        mark_dirty(result_page_id.value());
        return Result<PageManager::page_id_t>::ok(result_page_id.value());
    }

    Result<PageManager::page_id_t> Btree::create_internal_node()
    {
        auto result_page_id = page_manager_->create_new_page();
        if (result_page_id.is_error())
        {
            return Result<PageManager::page_id_t>::error(result_page_id.status());
        }
        auto result_page = page_manager_->get_page(result_page_id.value());
        if (result_page.is_error())
        {
            return Result<PageManager::page_id_t>::error(result_page.status());
        }
        InternalNode internal_node(result_page.value());
        auto result = internal_node.initialize();
        if (result.is_error())
        {
            return Result<PageManager::page_id_t>::error(result.status());
        }
        mark_dirty(result_page_id.value());
        return Result<PageManager::page_id_t>::ok(result_page_id.value());
    }

    Result<void> Btree::split_leaf_node(
        PageManager::page_id_t leaf_node_page_id,
        const std::vector<LeafNodeCell> &leaf_node_cells)
    {
        const auto left_cell_count = leaf_node_cells.size() / 2;
        auto result_left_page = page_manager_->get_page(leaf_node_page_id);
        if (result_left_page.is_error())
        {
            return Result<void>::error(result_left_page.status());
        }
        LeafNode left_leaf_node(result_left_page.value());

        auto result_right_leaf_node_page_id = create_leaf_node();
        if (result_right_leaf_node_page_id.is_error())
        {
            return Result<void>::error(result_right_leaf_node_page_id.status());
        }
        auto result_right_page = page_manager_->get_page(result_right_leaf_node_page_id.value());
        if (result_right_page.is_error())
        {
            return Result<void>::error(result_right_page.status());
        }
        LeafNode right_leaf_node(result_right_page.value());

        auto result_old_next_leaf_page_id = left_leaf_node.next_leaf_id();
        if (result_old_next_leaf_page_id.is_error())
        {
            return Result<void>::error(result_old_next_leaf_page_id.status());
        }

        auto result = left_leaf_node.set_num_cells(
            static_cast<LeafNode::num_cells_t>(left_cell_count));
        if (result.is_error())
        {
            return result;
        }
        result = right_leaf_node.set_num_cells(
            static_cast<LeafNode::num_cells_t>(leaf_node_cells.size() - left_cell_count));
        if (result.is_error())
        {
            return result;
        }

        for (std::size_t cell_num = 0; cell_num < leaf_node_cells.size(); ++cell_num)
        {
            auto &target_leaf_node =
                cell_num < left_cell_count ? left_leaf_node : right_leaf_node;
            const auto target_cell_num = static_cast<LeafNode::cell_num_t>(
                cell_num < left_cell_count ? cell_num : cell_num - left_cell_count);
            result = target_leaf_node.set_key(target_cell_num, leaf_node_cells[cell_num].key);
            if (result.is_error())
            {
                return result;
            }
            result = target_leaf_node.set_value(
                target_cell_num,
                leaf_node_cells[cell_num].value);
            if (result.is_error())
            {
                return result;
            }
        }

        result = right_leaf_node.set_next_leaf_id(result_old_next_leaf_page_id.value());
        if (result.is_error())
        {
            return result;
        }
        result = left_leaf_node.set_next_leaf_id(result_right_leaf_node_page_id.value());
        if (result.is_error())
        {
            return result;
        }

        auto result_parent_page_id = left_leaf_node.parent_id();
        if (result_parent_page_id.is_error())
        {
            return Result<void>::error(result_parent_page_id.status());
        }
        result = right_leaf_node.set_parent_id(result_parent_page_id.value());
        if (result.is_error())
        {
            return result;
        }
        mark_dirty(leaf_node_page_id);
        mark_dirty(result_right_leaf_node_page_id.value());

        return insert_into_parent(
            leaf_node_page_id,
            leaf_node_cells[left_cell_count - 1].key,
            result_right_leaf_node_page_id.value());
    }

    Result<void> Btree::insert_into_parent(
        PageManager::page_id_t left_node_page_id,
        key_t separator,
        PageManager::page_id_t right_node_page_id)
    {
        auto result_left_page = page_manager_->get_page(left_node_page_id);
        if (result_left_page.is_error())
        {
            return Result<void>::error(result_left_page.status());
        }
        Node left_node(result_left_page.value());
        auto result_is_root = left_node.is_root();
        if (result_is_root.is_error())
        {
            return Result<void>::error(result_is_root.status());
        }

        if (result_is_root.value())
        {
            auto result_new_root_page_id = create_internal_node();
            if (result_new_root_page_id.is_error())
            {
                return Result<void>::error(result_new_root_page_id.status());
            }
            auto result_new_root_page = page_manager_->get_page(result_new_root_page_id.value());
            if (result_new_root_page.is_error())
            {
                return Result<void>::error(result_new_root_page.status());
            }
            InternalNode new_root(result_new_root_page.value());
            const std::vector<key_t> keys{separator};
            const std::vector<PageManager::page_id_t> children{
                left_node_page_id,
                right_node_page_id};
            auto result = write_internal_node(new_root, keys, children);
            if (result.is_error())
            {
                return result;
            }
            result = set_parent_id(
                left_node_page_id,
                result_new_root_page_id.value());
            if (result.is_error())
            {
                return result;
            }
            result = set_is_root(left_node_page_id, false);
            if (result.is_error())
            {
                return result;
            }
            result = set_parent_id(
                right_node_page_id,
                result_new_root_page_id.value());
            if (result.is_error())
            {
                return result;
            }
            result = set_is_root(right_node_page_id, false);
            if (result.is_error())
            {
                return result;
            }
            result = set_parent_id(
                result_new_root_page_id.value(),
                PageManager::kInvalidPageId);
            if (result.is_error())
            {
                return result;
            }
            result = set_is_root(result_new_root_page_id.value(), true);
            if (result.is_error())
            {
                return result;
            }
            return set_root_node_id_in_metadata(result_new_root_page_id.value());
        }

        auto result_parent_page_id = left_node.parent_id();
        if (result_parent_page_id.is_error())
        {
            return Result<void>::error(result_parent_page_id.status());
        }
        auto result_parent_page = page_manager_->get_page(result_parent_page_id.value());
        if (result_parent_page.is_error())
        {
            return Result<void>::error(result_parent_page.status());
        }
        InternalNode parent_internal_node(result_parent_page.value());
        auto result_num_keys = parent_internal_node.num_keys();
        if (result_num_keys.is_error())
        {
            return Result<void>::error(result_num_keys.status());
        }

        std::vector<key_t> keys;
        std::vector<PageManager::page_id_t> children;
        keys.reserve(result_num_keys.value() + 1);
        children.reserve(result_num_keys.value() + 2);
        for (InternalNode::cell_num_t cell_num = 0;
             cell_num < result_num_keys.value();
             ++cell_num)
        {
            auto result_key = parent_internal_node.key(cell_num);
            if (result_key.is_error())
            {
                return Result<void>::error(result_key.status());
            }
            keys.push_back(result_key.value());
        }
        for (InternalNode::child_num_t child_num = 0;
             child_num <= result_num_keys.value();
             ++child_num)
        {
            auto result_child_page_id = parent_internal_node.child_id(child_num);
            if (result_child_page_id.is_error())
            {
                return Result<void>::error(result_child_page_id.status());
            }
            children.push_back(result_child_page_id.value());
        }

        auto left_child_position = std::find(
            children.begin(),
            children.end(),
            left_node_page_id);
        if (left_child_position == children.end())
        {
            return Result<void>::error(Status::invalid_argument());
        }
        const auto insertion_index = static_cast<std::size_t>(
            std::distance(children.begin(), left_child_position));
        if (children[insertion_index] != left_node_page_id)
        {
            return Result<void>::error(Status::invalid_argument());
        }
        keys.insert(keys.begin() + insertion_index, separator);
        children.insert(children.begin() + insertion_index + 1, right_node_page_id);

        if (keys.size() <= InternalNode::kMaxCells)
        {
            auto result = write_internal_node(parent_internal_node, keys, children);
            if (result.is_error())
            {
                return result;
            }
            result = set_parent_id(
                right_node_page_id,
                result_parent_page_id.value());
            if (result.is_error())
            {
                return result;
            }
            mark_dirty(result_parent_page_id.value());
            return Result<void>::ok();
        }

        return split_internal_node(
            result_parent_page_id.value(),
            keys,
            children);
    }
    // TODO
    Result<void> Btree::split_internal_node(
        PageManager::page_id_t internal_node_page_id,
        const std::vector<key_t> &internal_node_keys,
        const std::vector<PageManager::page_id_t> &child_page_ids)
    {
        const auto middle_key_index = internal_node_keys.size() / 2;
        const auto separator = internal_node_keys[middle_key_index];
        std::vector<key_t> left_keys(
            internal_node_keys.begin(),
            internal_node_keys.begin() + middle_key_index);
        std::vector<key_t> right_keys(
            internal_node_keys.begin() + middle_key_index + 1,
            internal_node_keys.end());
        std::vector<PageManager::page_id_t> left_children(
            child_page_ids.begin(),
            child_page_ids.begin() + middle_key_index + 1);
        std::vector<PageManager::page_id_t> right_children(
            child_page_ids.begin() + middle_key_index + 1,
            child_page_ids.end());

        auto result_left_page = page_manager_->get_page(internal_node_page_id);
        if (result_left_page.is_error())
        {
            return Result<void>::error(result_left_page.status());
        }
        InternalNode left_internal_node(result_left_page.value());
        auto result_parent_page_id = left_internal_node.parent_id();
        if (result_parent_page_id.is_error())
        {
            return Result<void>::error(result_parent_page_id.status());
        }

        auto result_right_internal_node_page_id = create_internal_node();
        if (result_right_internal_node_page_id.is_error())
        {
            return Result<void>::error(result_right_internal_node_page_id.status());
        }
        auto result_right_page =
            page_manager_->get_page(result_right_internal_node_page_id.value());
        if (result_right_page.is_error())
        {
            return Result<void>::error(result_right_page.status());
        }
        InternalNode right_internal_node(result_right_page.value());

        auto result = write_internal_node(left_internal_node, left_keys, left_children);
        if (result.is_error())
        {
            return result;
        }
        result = write_internal_node(right_internal_node, right_keys, right_children);
        if (result.is_error())
        {
            return result;
        }

        for (const auto child_page_id : right_children)
        {
            result = set_parent_id(
                child_page_id,
                result_right_internal_node_page_id.value());
            if (result.is_error())
            {
                return result;
            }
        }
        result = right_internal_node.set_parent_id(result_parent_page_id.value());
        if (result.is_error())
        {
            return result;
        }
        mark_dirty(internal_node_page_id);
        mark_dirty(result_right_internal_node_page_id.value());

        return insert_into_parent(
            internal_node_page_id,
            separator,
            result_right_internal_node_page_id.value());
    }

    Result<void> Btree::write_internal_node(
        InternalNode &internal_node,
        const std::vector<key_t> &internal_node_keys,
        const std::vector<PageManager::page_id_t> &child_page_ids)
    {
        if (internal_node_keys.size() > InternalNode::kMaxCells ||
            child_page_ids.size() != internal_node_keys.size() + 1)
        {
            return Result<void>::error(Status::invalid_argument());
        }

        auto result = internal_node.set_num_keys(
            static_cast<InternalNode::num_keys_t>(internal_node_keys.size()));
        if (result.is_error())
        {
            return result;
        }
        for (std::size_t cell_num = 0; cell_num < internal_node_keys.size(); ++cell_num)
        {
            result = internal_node.set_key(
                static_cast<InternalNode::cell_num_t>(cell_num),
                internal_node_keys[cell_num]);
            if (result.is_error())
            {
                return result;
            }
        }
        for (std::size_t child_num = 0; child_num < child_page_ids.size(); ++child_num)
        {
            result = internal_node.set_child_id(
                static_cast<InternalNode::child_num_t>(child_num),
                child_page_ids[child_num]);
            if (result.is_error())
            {
                return result;
            }
        }
        return Result<void>::ok();
    }

    Result<void> Btree::set_parent_id(
        PageManager::page_id_t node_page_id,
        PageManager::page_id_t parent_page_id)
    {
        auto result_page = page_manager_->get_page(node_page_id);
        if (result_page.is_error())
        {
            return Result<void>::error(result_page.status());
        }
        Node node(result_page.value());

        auto result = node.set_parent_id(parent_page_id);
        if (result.is_error())
        {
            return result;
        }
        mark_dirty(node_page_id);
        return Result<void>::ok();
    }

    Result<void> Btree::set_is_root(PageManager::page_id_t node_page_id, bool is_root)
    {
        auto result_page = page_manager_->get_page(node_page_id);
        if (result_page.is_error())
        {
            return Result<void>::error(result_page.status());
        }

        Node node(result_page.value());

        auto result = node.set_is_root(is_root);
        if (result.is_error())
        {
            return result;
        }
        mark_dirty(node_page_id);
        return Result<void>::ok();
    }

    Result<void> Btree::set_root_node_id_in_metadata(PageManager::page_id_t root_page_id)
    {
        auto result_metadata_page = page_manager_->get_page(kMetadataPageId);
        if (result_metadata_page.is_error())
        {
            return Result<void>::error(result_metadata_page.status());
        }
        auto result = result_metadata_page.value().write_uint32(
            root_page_id,
            kMetadataRootPageIdOffset);
        if (result.is_error())
        {
            return result;
        }
        root_page_id_ = root_page_id;
        mark_dirty(0);
        return Result<void>::ok();
    }

    Result<void> Btree::require_initialized() const
    {
        if (!initialized_)
        {
            return Result<void>::error(Status::invalid_argument());
        }
        return Result<void>::ok();
    }

    void Btree::mark_dirty(PageManager::page_id_t page_id)
    {
        dirty_page_ids_.insert(page_id);
    }
}
