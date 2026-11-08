#ifndef EASY_DB_BTREE_H_
#define EASY_DB_BTREE_H_

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "btree/leaf_node.h"

namespace easy_db
{
    class InternalNode;

    class Btree
    {
    public:
        using key_t = LeafNode::key_t;
        using value_t = LeafNode::value_t;

        explicit Btree(const std::string &filename);

        Result<void> initialize();
        Result<void> insert(key_t key, const value_t &value);
        Result<value_t> get_value(key_t key) const;
        Result<std::vector<value_t>> get_all() const;
        Result<void> flush();

    private:
        struct LeafNodeCell
        {
            key_t key;
            value_t value;
        };

        static constexpr std::uint32_t kMetadataMagic = 0x45444254;
        static constexpr std::size_t kMetadataMagicOffset = 0;
        static constexpr std::size_t kMetadataRootPageIdOffset = sizeof(std::uint32_t);
        static constexpr PageManager::page_id_t kMetadataPageId = 0;

        std::unique_ptr<PageManager> page_manager_;
        PageManager::page_id_t root_page_id_ = PageManager::kInvalidPageId;
        std::unordered_set<PageManager::page_id_t> dirty_page_ids_;
        bool initialized_ = false;

        Result<void> create_new_tree();
        Result<void> create_metadata_page();
        Result<void> create_root_leaf_node();
        Result<PageManager::page_id_t> create_leaf_node();
        Result<PageManager::page_id_t> create_internal_node();

        Result<void> initialize_metadata_page(PageManager::page_id_t metadata_page_id);
        Result<void>
        set_root_node_id_in_metadata(PageManager::page_id_t root_node_id);

        Result<void> set_parent_id(
            PageManager::page_id_t node_page_id,
            PageManager::page_id_t parent_page_id);
        Result<void> set_is_root(PageManager::page_id_t node_page_id, bool is_root);

        Result<PageManager::page_id_t> find_leaf_node_page_id(key_t key) const;
        Result<void> split_leaf_node(
            PageManager::page_id_t leaf_node_page_id,
            const std::vector<LeafNodeCell> &leaf_node_cells);
        Result<void> insert_into_parent(
            PageManager::page_id_t left_node_page_id,
            key_t separator,
            PageManager::page_id_t right_node_page_id);
        Result<void> split_internal_node(
            PageManager::page_id_t internal_node_page_id,
            const std::vector<key_t> &internal_node_keys,
            const std::vector<PageManager::page_id_t> &child_page_ids);
        Result<void> write_internal_node(
            InternalNode &internal_node,
            const std::vector<key_t> &internal_node_keys,
            const std::vector<PageManager::page_id_t> &child_page_ids);

        Result<void> require_initialized() const;
        void mark_dirty(PageManager::page_id_t page_id);
    };
}

#endif
