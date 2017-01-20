
#include <algorithm> // for sort
#include <cstdint>
#include <numeric> // for iota

#include "point_tree.h"

using namespace PointTree;

// This uses an algorithm where it pre-sorts points in each of (x,y) dimensions,
// then maintains the order of these sorted arrays when building the tree. 
// This gives a worst-case complexity of O(nlogn) which is pretty good. 
Node* KdTree::build_tree(const Point* begin, const Point* end)
{
    if (!begin || !end) { return nullptr; }

    size_t num_points = std::distance(begin, end);

    if (num_points <= 0) { return nullptr; }

    // Save time and space by sticking with raw array, rather than storing references in a vector.
    const Point* points = begin;

    // Use a vector of indices that is sorted by ascending x values, and another sorted by y value.
    // For example [512, 4, 35, ...] means that element 512 in "points" has the lowest x value.
    // This allows an easy way to determine the median, and the point it corresponds to.
    indices_t x_indices(num_points);
    std::iota(x_indices.begin(), x_indices.end(), 0);
    indices_t y_indices = x_indices;

    std::sort(x_indices.begin(), x_indices.end(), [points]
        (index_t i1, index_t i2){ return points[i1].x < points[i2].x; });
    std::sort(y_indices.begin(), y_indices.end(), [points]
        (index_t i1, index_t i2){ return points[i1].y < points[i2].y; });

    // Need a place to store indices after each "split" in the tree when recursively inserting the median value.
    indices_t temp_indices(num_points);

    root_ = insert_median(x_indices, y_indices, temp_indices, points, 0, num_points-1, true);

    return root_;
}

// This algorithm avoids allocating extra memory to store the points inside the rect, and instead
// uses the already allocated "out_points" array.
int32_t KdTree::find_best_points(const Rect rect, int32_t max_count, Point* out_points)
{
    if (!out_points) { return -1; }

    // Track how many points are inside rect so we know when we've hit the maximum we want to save.
    int32_t total_inside = 0;
    
    // Keep track of the worst element so we know which one to replace when we find a better one.
    // The value of "worst_rank" doesn't mean anything until out_points fills up (which it might not).
    index_t worst_idx = 0;
    int32_t worst_rank = 0;

    find_best_points(rect, root_, out_points, max_count, total_inside, worst_idx, worst_rank, true); 

    return std::min(total_inside, max_count);
}
   
void KdTree::find_best_points(const Rect rect, Node* node, Point* out_points, int32_t max_count,
                              int32_t& total_inside, index_t& worst_idx, int32_t& worst_rank, bool split_on_x)
{
    if (!node) { return; }

    process_node(node, rect, max_count, out_points, worst_idx, worst_rank, total_inside);

    // Also need to visit children that could potentially be in rectangle. 
    if (split_on_x)
    {
        if (rect.lx < node->point.x) 
        {
            find_best_points(rect, node->left, out_points, max_count, total_inside, worst_idx, worst_rank, !split_on_x);
        }
        if (rect.hx >= node->point.x)
        {
            find_best_points(rect, node->right, out_points, max_count, total_inside, worst_idx, worst_rank, !split_on_x);
        }
    }
    else // splitting on y dimension
    {
        if (rect.ly < node->point.y)
        { 
            find_best_points(rect, node->left, out_points, max_count, total_inside, worst_idx, worst_rank, !split_on_x);
        }
        if (rect.hy >= node->point.y)
        {
            find_best_points(rect, node->right, out_points, max_count, total_inside, worst_idx, worst_rank, !split_on_x); 
        }
    }

}

Node* KdTree::insert_median(indices_t& indices, indices_t& other_indices, indices_t& temp_indices, 
                            const Point* points, index_t start, index_t end, bool split_x) 
{
    Node* new_node = nullptr;

    if (end > start)
    {
        // Avoid overflow when calculating median.
        index_t median_idx = start + (end-start)/2;
        index_t median_point_idx = indices[median_idx];
        const Point* median = &points[median_point_idx]; 

        while (median_idx > start)
        {
            // Make sure that if the calculated median is not unique then we select the first occurance as the 
            // effective median in order to guarantee that the left child is strictly less than the parent value. 
            index_t before_median_point_idx = indices[median_idx-1];
            const Point* before_median = &points[before_median_point_idx];
            if (is_same(median, before_median, split_x))
            {
                --median_idx;
                median_point_idx = before_median_point_idx;
                median = before_median;
            }
            else
            {
                break;
            }
        }

        // Since the points are being split at the median, we need to go through and re-group
        // the other indices so they fall before/after the median since that's how they will
        // be split between left/right children.  This operation will still keep the indices sorted.
        index_t lower = start;
        index_t upper = median_idx + 1;
        for (index_t i = start; i <= end; ++i)
        {
            index_t compare_index = other_indices[i];

            // The median node will be created during this call, so we don't want to store it again.
            if (compare_index == median_point_idx) { continue; }

            const Point* compare_point = &points[compare_index];

            if (is_greater(median, compare_point, split_x))
            {
                temp_indices[lower] = other_indices[i];
                ++lower;
            }
            else // compare_point is equal to, or comes after median
            {
                temp_indices[upper] = other_indices[i];
                ++upper;
            }
        }

        // Copy just the temporary contents determined by this call because still need to preverve other
        // indices values for other stack frames.
        for (index_t i = start; i <= end; ++i)
        {
            other_indices[i] = temp_indices[i];
        }

        new_node = new Node(*median);
        new_node->left  = insert_median(other_indices, indices, temp_indices, points, start, lower-1, !split_x);
        new_node->right = insert_median(other_indices, indices, temp_indices, points, median_idx+1, upper-1, !split_x);
    }
    else if (start == end) 
    {
        index_t point_idx = indices[end];
        new_node = new Node(points[point_idx]);
    }
    else // start > end
    {
        new_node = nullptr; // because reached end of a leaf, shouldn't be a node here.
    }

    return new_node;
}

size_t KdTree::size(const Node* node)
{
    if (!node) { return 0; }
    size_t num_left_children = size(node->left);
    size_t num_right_children = size(node->right);
    // Add one to account for the current node.
    return 1 + num_left_children + num_right_children;
}

void KdTree::process_node(const Node* node, const Rect rect, int32_t max_count, Point* out_points, 
                          index_t& worst_idx, int32_t& worst_rank, int32_t& total_found_inside)
{
    const Point& p = node->point;

    bool inside_rect = (p.x >= rect.lx) && (p.x <= rect.hx) &&
                       (p.y >= rect.ly) && (p.y <= rect.hy);
    if (inside_rect)
    {
        ++total_found_inside;
        if (total_found_inside < max_count)
        {
            out_points[worst_idx] = p;
            ++worst_idx;
        }
        else if ((total_found_inside == max_count) ||
                 (p.rank < worst_rank))
        {
            out_points[worst_idx] = p;
            find_worst_rank(out_points, max_count, worst_rank, worst_idx);
        }
    }
}

void KdTree::delete_all(Node* node)
{
    if (!node) { return; }
    delete_all(node->left);
    delete_all(node->right);
    delete node;
}

void KdTree::find_worst_rank(const Point* points, int32_t num_points, int32_t& worst_rank, index_t& worst_idx)
{
    // Don't use std::distance because we're using raw arrays so lookup time will be linear.
    int32_t current_worst_rank = INT32_MIN;
    index_t current_worst_idx = 0;
    for (index_t i = 0; i < num_points; ++i)
    {
        if (points[i].rank > current_worst_rank)
        {
            current_worst_rank = points[i].rank;
            current_worst_idx = i;
        }
    }
    worst_rank = current_worst_rank;
    worst_idx = current_worst_idx;
}

bool KdTree::is_greater(const Point* p1, const Point* p2, bool compare_x)
{
    return compare_x ? (p1->x > p2->x) : (p1->y > p2->y);
}

bool KdTree::is_same(const Point* p1, const Point* p2, bool compare_x)
{
    return compare_x ? (p1->x == p2->x) : (p1->y == p2->y);
}
