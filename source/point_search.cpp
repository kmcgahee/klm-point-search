
#include <algorithm> // for sort

#include "point_search.h"
#include "point_search_export.h"
#include "search_context.h"

SearchContext* create(const Point* points_begin, const Point* points_end)
{
    if (!points_begin || !points_end) { return nullptr; }

    return new SearchContext(points_begin, points_end);
}

int32_t search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
{
    if (!out_points || !sc) { return 0; } // because no points were copied.

    int32_t num_points_copied = sc->copy_best_points(rect, count, out_points);

    if (num_points_copied <= 0) { return 0; }

    std::sort(out_points, out_points + num_points_copied,
              [] (Point const& a, Point const& b) { return a.rank < b.rank; });
            
    return num_points_copied;
}

SearchContext* destroy(SearchContext* sc)
{
    // Only exception that could occur is stack overflow (from recursion), which is at system level and 
    // not-recoverable.  If "sc" is already deleted or an invalid address then not much we can
    // do about that either.  So just assume memory de-alloc is always successful and return nullptr.
    delete sc;
    return nullptr; 
}
