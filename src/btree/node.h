#ifndef EASY_DB_BTREE_NODE_H_
#define EASY_DB_BTREE_NODE_H_

#include "page/page.h"
#include "page/page_manager.h"

namespace easy_db
{
    class Node
    {
    public:
        enum class NodeType
        {
            Leaf,
            Internal
        };
        explicit Node(Page &page) : page_(page) {}
        Result<NodeType> node_type() const;
        Result<void> set_node_type(NodeType node_type);
        Result<bool> is_root() const;
        Result<void> set_is_root(bool is_root);
        Result<PageManager::page_id_t> parent_id() const;
        Result<void> set_parent_id(PageManager::page_id_t parent_id);

    protected:
        Page &page_;

        static constexpr std::size_t kNodeTypeSize = sizeof(std::byte);
        static constexpr std::size_t kNodeTypeOffset = 0;
        static constexpr std::size_t kIsRootSize = sizeof(std::byte);
        static constexpr std::size_t kIsRootOffset = kNodeTypeOffset + kNodeTypeSize;
        static constexpr std::size_t kParentNodeIdSize = sizeof(PageManager::page_id_t);
        static constexpr std::size_t kParentNodeIdOffset = kIsRootOffset + kIsRootSize;
        static constexpr std::size_t kCommonHeaderSize = kNodeTypeSize + kIsRootSize + kParentNodeIdSize;
    };
}

#endif