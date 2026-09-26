#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "common/result.h"
#include "page/page.h"

namespace easy_db
{
    class PageManager
    {
    public:
        using page_id_t = std::uint64_t;
        using num_pages_t = std::uint64_t;

        PageManager(const std::string &filename);

        PageManager(const PageManager &page_manager) = delete;
        PageManager &operator=(const PageManager &page_manager) = delete;

        Result<page_id_t> create_new_page();
        Result<Page &> get_page(const page_id_t page_id);
        Result<void> flush(const page_id_t page_id);

    private:
        const std::string filename_;
        page_id_t next_page_id_;
        std::unordered_map<page_id_t, std::unique_ptr<Page>> pages_;

        Result<num_pages_t> get_num_pages();
    };
}