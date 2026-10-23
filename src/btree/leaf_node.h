#include "page/page.h"
#include "node.h"
namespace easy_db
{
    class LeafNode : public Node
    {
    private:
        static constexpr std::size_t kValueSize = 293;

    public:
        using num_cells_t = uint32_t;
        using key_t = uint32_t;
        using cell_num_t = uint32_t;
        using value_t = std::array<std::byte, kValueSize>;

        explicit LeafNode(Page &page) : Node(page) {}

        Result<num_cells_t> num_cells() const;
        Result<void> set_num_cells(num_cells_t num_cells);
        Result<PageManager::page_id_t> next_leaf_id() const;
        Result<void> set_next_leaf_id(PageManager::page_id_t page_id);
        Result<key_t> key(cell_num_t cell_num) const;
        Result<void> set_key(cell_num_t cell_num, key_t key);
        Result<value_t> value(cell_num_t cell_num) const;
        Result<void> set_value(cell_num_t cell_num, const value_t &value);

        Result<void> initialize();

    private:
        static constexpr std::size_t kNumCellsSize = sizeof(num_cells_t);
        static constexpr std::size_t kNumCellsOffset = kCommonHeaderSize;
        static constexpr std::size_t kNextLeafSize = sizeof(PageManager::page_id_t);
        static constexpr std::size_t kNextLeafOffset = kNumCellsOffset + kNumCellsSize;
        static constexpr std::size_t kHeaderSize = kCommonHeaderSize + kNumCellsSize + kNextLeafSize;

        static constexpr std::size_t kKeySize = sizeof(key_t);
        static constexpr std::size_t kKeyOffset = 0;
        static constexpr std::size_t kValueOffset = kKeyOffset + kKeySize;
        static constexpr std::size_t kCellSize = kKeySize + kValueSize;
        static constexpr std::size_t kSpaceForCells = Page::kPageSize - kHeaderSize;
        static constexpr std::size_t kMaxCells = kSpaceForCells / kCellSize;
        static constexpr std::size_t kRightSplitCount = (kMaxCells + 1) / 2;
        static constexpr std::size_t kLeftSplitCount = (kMaxCells + 1) - kRightSplitCount;
    };
}