#include <cstdint>
#include <limits>

#include "btree/node.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(NodeTestGroup){};

    TEST(NodeTestGroup, SetNodeType_Leaf)
    {
        Page page;
        Node node(page);

        const auto set_result = node.set_node_type(Node::NodeType::Leaf);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.node_type();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == Node::NodeType::Leaf);
    }

    TEST(NodeTestGroup, SetNodeType_Internal)
    {
        Page page;
        Node node(page);

        const auto set_result = node.set_node_type(Node::NodeType::Internal);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.node_type();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == Node::NodeType::Internal);
    }

    TEST(NodeTestGroup, NodeType_ReturnInvalidNodeTypeIfValueIsInvalid)
    {
        Page page;
        Node node(page);

        const auto iterator_result = page.iterator_at(0);
        CHECK_TRUE(iterator_result.is_ok());

        *iterator_result.value() = std::byte{0x02};

        const auto result = node.node_type();

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_node_type());
    }

    TEST(NodeTestGroup, SetIsRoot_True)
    {
        Page page;
        Node node(page);

        const auto set_result = node.set_is_root(true);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.is_root();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value());
    }

    TEST(NodeTestGroup, SetIsRoot_False)
    {
        Page page;
        Node node(page);

        const auto set_result = node.set_is_root(false);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.is_root();

        CHECK_TRUE(result.is_ok());
        CHECK_FALSE(result.value());
    }

    TEST(NodeTestGroup, IsRoot_ReturnInvalidIsRootIfValueIsInvalid)
    {
        Page page;
        Node node(page);

        const auto iterator_result = page.iterator_at(1);
        CHECK_TRUE(iterator_result.is_ok());

        *iterator_result.value() = std::byte{0x02};

        const auto result = node.is_root();

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_is_root());
    }

    TEST(NodeTestGroup, SetParentId_ReturnWrittenValue)
    {
        Page page;
        Node node(page);

        const PageManager::page_id_t parent_id = 123;

        const auto set_result = node.set_parent_id(parent_id);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.parent_id();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == parent_id);
    }

    TEST(NodeTestGroup, SetParentId_Zero)
    {
        Page page;
        Node node(page);

        const PageManager::page_id_t parent_id = 0;

        const auto set_result = node.set_parent_id(parent_id);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.parent_id();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == parent_id);
    }

    TEST(NodeTestGroup, SetParentId_MaxValue)
    {
        Page page;
        Node node(page);

        const PageManager::page_id_t parent_id =
            std::numeric_limits<PageManager::page_id_t>::max();

        const auto set_result = node.set_parent_id(parent_id);

        CHECK_TRUE(set_result.is_ok());

        const auto result = node.parent_id();

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == parent_id);
    }
}