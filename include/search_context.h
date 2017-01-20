
#ifndef SEARCH_CONTEXT_H_INCLUDED
#define SEARCH_CONTEXT_H_INCLUDED

#include "point_tree.h"

struct SearchContext
{
    PointTree::KdTree points;

    // Store (copy) all Points in array starting at "begin".
    // "end" should reference next element after the last Point of the array.
    SearchContext(const Point* begin, const Point* end)
    {
        points.build_tree(begin, end);
    }

    // Return number of Points found within "rect" that have been copied to "out_points" array.
    // If this exceeds "max_count" then only the best points are copied (those with the lowest rank).
    // Note: the copied points are not sorted by rank.
    int32_t copy_best_points(const Rect rect, int32_t max_count, Point* out_points)
    {
        return points.find_best_points(rect, max_count, out_points);
    }
};

#endif