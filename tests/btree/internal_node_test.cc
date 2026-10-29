#include "btree/internal_node.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(InternalNodeTestGroup)
    {
        std::size_t kHeaderSize = 14;
        InternalNode::num_keys_t kMaxCells = 3;
    };

    TEST(InternalNodeTestGroup, SetNumKeys_ReturnWrittenValue)
    {
        Page page;
        InternalNode internal_node(page);

        const InternalNode::num_keys_t num_keys = 3;

        const auto set_result = internal_node.set_num_keys(num_keys);
        CHECK_TRUE(set_result.is_ok());

        const auto result = internal_node.num_keys();
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == num_keys);
    }

    TEST(InternalNodeTestGroup, SetNumKeys_ReturnInvalidArgumentIfValueExceedsMaxCells)
    {
        Page page;
        InternalNode internal_node(page);

        const auto result = internal_node.set_num_keys(
            kMaxCells + 1);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, SetRightChildId_ReturnWrittenValue)
    {
        Page page;
        InternalNode internal_node(page);

        const PageManager::page_id_t right_child_id = 456;

        const auto set_result = internal_node.set_right_child_id(right_child_id);
        CHECK_TRUE(set_result.is_ok());

        const auto result = internal_node.right_child_id();
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == right_child_id);
    }

    TEST(InternalNodeTestGroup, ChildId_ReturnsChildForCellIndex)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());
        CHECK_TRUE(internal_node.set_child_id(0, 101).is_ok());
        CHECK_TRUE(internal_node.set_child_id(1, 202).is_ok());

        const auto result = internal_node.child_id(1);

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == 202);
    }

    TEST(InternalNodeTestGroup, ChildId_ReturnsRightChildAtNumKeysIndex)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());
        CHECK_TRUE(internal_node.set_right_child_id(303).is_ok());

        const auto result = internal_node.child_id(2);

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == 303);
    }

    TEST(InternalNodeTestGroup, ChildId_ReturnsInvalidArgumentIfIndexExceedsNumKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());

        const auto result = internal_node.child_id(3);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, SetChildId_ReturnsInvalidArgumentIfIndexExceedsNumKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());

        const auto result = internal_node.set_child_id(3, 404);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, SetChildId_SetsRightChildAtNumKeysIndex)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());

        const auto set_result = internal_node.set_child_id(2, 505);
        CHECK_TRUE(set_result.is_ok());

        const auto result = internal_node.right_child_id();
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == 505);
    }

    TEST(InternalNodeTestGroup, Key_ReturnsKeyForValidCell)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());
        CHECK_TRUE(internal_node.set_key(1, 42).is_ok());

        const auto result = internal_node.key(1);

        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == 42);
    }

    TEST(InternalNodeTestGroup, Key_ReturnsInvalidArgumentIfIndexIsOutOfRange)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(2).is_ok());

        const auto result = internal_node.key(2);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, Key_ReturnsInvalidArgumentIfNodeHasNoKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(0).is_ok());

        const auto result = internal_node.key(0);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, SetKey_ReturnsInvalidArgumentIfNodeHasNoKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(0).is_ok());

        const auto result = internal_node.set_key(0, 42);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(InternalNodeTestGroup, FindChild_ReturnsKeyNotExistIfNodeHasNoKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(0).is_ok());

        const auto result = internal_node.find_child(42);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_key_not_exist());
    }

    TEST(InternalNodeTestGroup, FindChild_ReturnsChildIndexForBoundaryAndBetweenKeys)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_num_keys(3).is_ok());
        CHECK_TRUE(internal_node.set_key(0, 10).is_ok());
        CHECK_TRUE(internal_node.set_key(1, 20).is_ok());
        CHECK_TRUE(internal_node.set_key(2, 30).is_ok());

        const auto less_than_first = internal_node.find_child(5);
        CHECK_TRUE(less_than_first.is_ok());
        CHECK_TRUE(less_than_first.value() == 0);

        const auto equal_to_key = internal_node.find_child(20);
        CHECK_TRUE(equal_to_key.is_ok());
        CHECK_TRUE(equal_to_key.value() == 1);

        const auto between_keys = internal_node.find_child(25);
        CHECK_TRUE(between_keys.is_ok());
        CHECK_TRUE(between_keys.value() == 2);

        const auto greater_than_last = internal_node.find_child(35);
        CHECK_TRUE(greater_than_last.is_ok());
        CHECK_TRUE(greater_than_last.value() == 3);
    }

    TEST(InternalNodeTestGroup, Initialize_SetsNodeHeadersAndClearsCells)
    {
        Page page;
        InternalNode internal_node(page);

        CHECK_TRUE(internal_node.set_node_type(Node::NodeType::Leaf).is_ok());
        CHECK_TRUE(internal_node.set_is_root(true).is_ok());
        CHECK_TRUE(internal_node.set_parent_id(123).is_ok());
        CHECK_TRUE(internal_node.set_num_keys(1).is_ok());
        CHECK_TRUE(internal_node.set_key(0, 456).is_ok());
        CHECK_TRUE(internal_node.set_child_id(0, 789).is_ok());

        const auto result = internal_node.initialize();

        CHECK_TRUE(result.is_ok());

        const auto node_type = internal_node.node_type();
        CHECK_TRUE(node_type.is_ok());
        CHECK_TRUE(node_type.value() == Node::NodeType::Internal);

        const auto is_root = internal_node.is_root();
        CHECK_TRUE(is_root.is_ok());
        CHECK_FALSE(is_root.value());

        const auto parent_id = internal_node.parent_id();
        CHECK_TRUE(parent_id.is_ok());
        CHECK_TRUE(parent_id.value() == PageManager::kInvalidPageId);

        const auto num_keys = internal_node.num_keys();
        CHECK_TRUE(num_keys.is_ok());
        CHECK_TRUE(num_keys.value() == 0);

        const auto right_child_id = internal_node.right_child_id();
        CHECK_TRUE(right_child_id.is_ok());
        CHECK_TRUE(right_child_id.value() == PageManager::kInvalidPageId);

        for (std::size_t offset = kHeaderSize; offset < Page::kPageSize; ++offset)
        {
            const auto byte = page.const_iterator_at(offset);
            CHECK_TRUE(byte.is_ok());
            CHECK_TRUE(*byte.value() == std::byte{0});
        }
    }
}
