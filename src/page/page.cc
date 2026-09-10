
#include "page.h"

namespace easy_db
{
    Result<Page::iterator>
    Page::iterator_at(std::size_t offset)
    {
        if (offset > kPageSize)
        {
            return Result<iterator>::error(Status::out_of_range());
        }
        return Result<iterator>::ok(data_.begin() + offset);
    }

    Result<Page::const_iterator> Page::const_iterator_at(std::size_t offset) const
    {
        if (offset > kPageSize)
        {
            return Result<const_iterator>::error(Status::out_of_range());
        }
        return Result<const_iterator>::ok(data_.cbegin() + offset);
    }

}