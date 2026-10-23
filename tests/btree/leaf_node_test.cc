#include <array>
#include <limits>

#include "btree/leaf_node.h"

#include <CppUTest/TestHarness.h>

namespace easy_db
{
    TEST_GROUP(LeafNodeTestGroup)
    {
        LeafNode::cell_num_t kMaxCells = 13;
        std::size_t kHeaderSize = 14;
    };

    TEST(LeafNodeTestGroup, SetNumCells_ReturnWrittenValue)
    {
        Page page;
        LeafNode leaf_node(page);

        const LeafNode::num_cells_t num_cells = kMaxCells;

        const auto set_result = leaf_node.set_num_cells(num_cells);
        CHECK_TRUE(set_result.is_ok());

        const auto result = leaf_node.num_cells();
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == num_cells);
    }
    TEST(LeafNodeTestGroup, SetNumCells_ReturnInvalidArgumentIfValueExceedsMaxCells)
    {
        Page page;
        LeafNode leaf_node(page);

        const auto result = leaf_node.set_num_cells(
            kMaxCells + 1);

        CHECK_TRUE(result.is_error());
        CHECK_TRUE(result.status().is_invalid_argument());
    }

    TEST(LeafNodeTestGroup, SetNextLeafId_ReturnWrittenValue)
    {
        Page page;
        LeafNode leaf_node(page);

        const PageManager::page_id_t next_leaf_id = 456;

        const auto set_result = leaf_node.set_next_leaf_id(next_leaf_id);
        CHECK_TRUE(set_result.is_ok());

        const auto result = leaf_node.next_leaf_id();
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == next_leaf_id);
    }

    TEST(LeafNodeTestGroup, SetKey_ReturnWrittenValue)
    {
        Page page;
        LeafNode leaf_node(page);

        const LeafNode::key_t key = 42;

        const auto set_result = leaf_node.set_key(kMaxCells - 1, key);
        CHECK_TRUE(set_result.is_ok());

        const auto result = leaf_node.key(kMaxCells - 1);
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == key);
    }

    TEST(LeafNodeTestGroup, SetKey_ReturnInvalidArgumentIfCellNumberIsOutOfRange)
    {
        Page page;
        LeafNode leaf_node(page);

        const auto invalid_result = leaf_node.set_key(
            kMaxCells,
            static_cast<LeafNode::key_t>(1000));
        CHECK_TRUE(invalid_result.is_error());
        CHECK_TRUE(invalid_result.status().is_invalid_argument());
    }

    TEST(LeafNodeTestGroup, SetValue_ReturnWrittenValue)
    {
        Page page;
        LeafNode leaf_node(page);

        LeafNode::value_t value{};
        value.fill(std::byte{0xAB});

        const auto set_result = leaf_node.set_value(kMaxCells - 1, value);
        CHECK_TRUE(set_result.is_ok());

        const auto result = leaf_node.value(kMaxCells - 1);
        CHECK_TRUE(result.is_ok());
        CHECK_TRUE(result.value() == value);
    }

    TEST(LeafNodeTestGroup, SetValue_ReturnInvalidArgumentIfCellNumberIsOutOfRange)
    {
        Page page;
        LeafNode leaf_node(page);

        LeafNode::value_t value{};
        value.fill(std::byte{0x22});
        const auto invalid_result = leaf_node.set_value(
            kMaxCells,
            value);
        CHECK_TRUE(invalid_result.is_error());
        CHECK_TRUE(invalid_result.status().is_invalid_argument());
    }

    TEST(LeafNodeTestGroup, Initialize_SetsDefaultHeaderValuesAndZeroesCellBytes)
    {
        Page page;
        LeafNode leaf_node(page);

        const auto set_num_cells_result = leaf_node.set_num_cells(1);
        CHECK_TRUE(set_num_cells_result.is_ok());

        const auto set_key_result = leaf_node.set_key(0, 7);
        CHECK_TRUE(set_key_result.is_ok());

        LeafNode::value_t value{};
        value.fill(std::byte{0xAB});

        const auto set_value_result = leaf_node.set_value(0, value);
        CHECK_TRUE(set_value_result.is_ok());

        const auto initialize_result = leaf_node.initialize();
        CHECK_TRUE(initialize_result.is_ok());

        const auto node_type_result = leaf_node.node_type();
        CHECK_TRUE(node_type_result.is_ok());
        CHECK_TRUE(node_type_result.value() == Node::NodeType::Leaf);

        const auto is_root_result = leaf_node.is_root();
        CHECK_TRUE(is_root_result.is_ok());
        CHECK_FALSE(is_root_result.value());

        const auto parent_id_result = leaf_node.parent_id();
        CHECK_TRUE(parent_id_result.is_ok());
        CHECK_TRUE(parent_id_result.value() == PageManager::kInvalidPageId);

        const auto num_cells_result = leaf_node.num_cells();
        CHECK_TRUE(num_cells_result.is_ok());
        CHECK_TRUE(num_cells_result.value() == 0);

        const auto next_leaf_result = leaf_node.next_leaf_id();
        CHECK_TRUE(next_leaf_result.is_ok());
        CHECK_TRUE(next_leaf_result.value() == PageManager::kInvalidPageId);

        for (std::size_t offset = kHeaderSize; offset < Page::kPageSize; ++offset)
        {
            const auto iterator_result = page.const_iterator_at(offset);
            CHECK_TRUE(iterator_result.is_ok());
            CHECK_TRUE(*iterator_result.value() == std::byte{0x00});
        }
    }
}