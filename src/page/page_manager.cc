#include <filesystem>
#include <fstream>
#include "page_manager.h"

namespace easy_db
{
    PageManager::PageManager(const std::string &filename) : filename_(filename)
    {
        auto result_num_pages = num_pages();
        if (result_num_pages.is_ok())
        {
            next_page_id_ = result_num_pages.value();
            return;
        }

        if (result_num_pages.status().is_file_not_exist())
        {
            std::ofstream file(filename_, std::ios::binary);

            if (!file)
            {
                throw std::logic_error("Failed to create the database file.");
            }

            next_page_id_ = 0;
            return;
        }

        throw std::logic_error("Failed to get the number of pages.");
    }

    Result<PageManager::page_id_t> PageManager::create_new_page()
    {
        auto page_ptr = std::make_unique<Page>();
        pages_.emplace(next_page_id_, std::move(page_ptr));
        const auto page_id = next_page_id_;
        next_page_id_++;
        return Result<PageManager::page_id_t>::ok(page_id);
    }

    Result<Page &> PageManager::get_page(const PageManager::page_id_t page_id)
    {
        if (page_id >= next_page_id_)
        {
            return Result<Page &>::error(Status::invalid_page_id());
        }

        auto it = pages_.find(page_id);

        if (it != pages_.end())
        {
            auto &page = it->second;
            return Result<Page &>::ok(*page);
        }

        auto page_ptr = std::make_unique<Page>();

        std::ifstream file(filename_, std::ios::binary);
        if (!file)
        {
            return Result<Page &>::error(Status::fail_open_file());
        }

        std::streamoff offset = Page::kPageSize * page_id;
        file.seekg(offset, std::ios::beg);
        if (!file)
        {
            return Result<Page &>::error(Status::fail_seek());
        }

        auto page_iter_result = page_ptr->iterator_at(0);
        if (page_iter_result.is_error())
        {
            return Result<Page &>::error(page_iter_result.status());
        }

        auto page_iter = page_iter_result.value();
        file.read(reinterpret_cast<char *>(&*page_iter), Page::kPageSize);
        if (!file)
        {
            return Result<Page &>::error(Status::fail_read_file());
        }

        pages_.emplace(page_id, std::move(page_ptr));

        return Result<Page &>::ok(*pages_.at(page_id));
    }

    Result<void> PageManager::flush(const PageManager::page_id_t page_id)
    {
        auto pages_iter = pages_.find(page_id);
        if (pages_iter == pages_.end())
        {
            return Result<void>::error(Status::not_find_page());
        }
        auto &page = pages_iter->second;

        std::fstream file(filename_, std::ios::in | std::ios::out | std::ios::binary);
        if (!file)
        {
            return Result<void>::error(Status::fail_open_file());
        }

        std::streamoff offset = Page::kPageSize * page_id;
        file.seekp(offset, std::ios::beg);
        if (!file)
        {
            return Result<void>::error(Status::fail_seek());
        }

        auto result_page_iter = page->iterator_at(0);
        if (result_page_iter.is_error())
        {
            return Result<void>::error(result_page_iter.status());
        }

        auto page_iter = result_page_iter.value();

        file.write(reinterpret_cast<char *>(&*page_iter), Page::kPageSize);
        if (!file)
        {
            return Result<void>::error(Status::fail_write_file());
        }
        return Result<void>::ok();
    }

    Result<PageManager::num_pages_t> PageManager::num_pages()
    {
        std::filesystem::path file_path = filename_;
        if (!std::filesystem::exists(file_path))
        {
            return Result<PageManager::num_pages_t>::error(Status::file_not_exist());
        }

        std::error_code ec;
        auto filesize = std::filesystem::file_size(file_path, ec);
        if (ec)
        {
            return Result<PageManager::num_pages_t>::error(Status::fail_file_size());
        }

        auto num_pages = filesize / Page::kPageSize;
        return Result<PageManager::num_pages_t>::ok(num_pages);
    }

}