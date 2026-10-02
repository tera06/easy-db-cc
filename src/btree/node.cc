#include "node.h"
#include <cstring>

namespace easy_db
{
    Result<Node::NodeType> Node::node_type() const
    {
        auto result_page_iter = page_.const_iterator_at(kNodeTypeOffset);
        if (result_page_iter.is_error())
        {
            return Result<NodeType>::error(result_page_iter.status());
        }
        auto page_iter = result_page_iter.value();

        auto node_type_byte = *page_iter;

        if (node_type_byte == std::byte{0x00})
        {
            return Result<NodeType>::ok(NodeType::Leaf);
        }

        if (node_type_byte == std::byte{0x01})
        {
            return Result<NodeType>::ok(NodeType::Internal);
        }

        return Result<NodeType>::error(Status::invalid_node_type());
    }

    Result<void> Node::set_node_type(NodeType node_type)
    {
        auto result_page_iter = page_.iterator_at(kNodeTypeOffset);
        if (result_page_iter.is_error())
        {
            return Result<void>::error(result_page_iter.status());
        }
        auto page_iter = result_page_iter.value();

        switch (node_type)
        {
        case NodeType::Leaf:
            *page_iter = std::byte{0x00};
            break;

        case NodeType::Internal:
            *page_iter = std::byte{0x01};
            break;
        default:
            return Result<void>::error(Status::invalid_node_type());
        }

        return Result<void>::ok();
    }

    Result<bool> Node::is_root() const
    {
        auto result_page_iter = page_.const_iterator_at(kIsRootOffset);
        if (result_page_iter.is_error())
        {
            return Result<bool>::error(result_page_iter.status());
        }
        auto page_iter = result_page_iter.value();

        if (*page_iter == std::byte{0x00})
        {
            return Result<bool>::ok(false);
        }

        if (*page_iter == std::byte{0x01})
        {
            return Result<bool>::ok(true);
        }
        return Result<bool>::error(Status::invalid_is_root());
    }

    Result<void> Node::set_is_root(bool is_root)
    {
        auto result_page_iter = page_.iterator_at(kIsRootOffset);
        if (result_page_iter.is_error())
        {
            return Result<void>::error(result_page_iter.status());
        }

        auto page_iter = result_page_iter.value();
        if (is_root)
        {
            *page_iter = std::byte{0x01};
        }
        else
        {
            *page_iter = std::byte{0x00};
        }

        return Result<void>::ok();
    }

    Result<PageManager::page_id_t> Node::parent_id() const
    {
        return page_.read_uint32(kParentNodeIdOffset);
    }
    Result<void> Node::set_parent_id(PageManager::page_id_t parent_id)
    {
        return page_.write_uint32(parent_id, kParentNodeIdOffset);
    }

}