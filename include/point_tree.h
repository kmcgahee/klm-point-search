
#ifndef POINT_TREE_H_INCLUDED
#define POINT_TREE_H_INCLUDED

#include <vector>

#include "point_search.h"

namespace PointTree
{
// Use signed type to avoid underflow since some algorithms subtract from indices.
typedef intmax_t index_t; 
typedef std::vector<index_t> indices_t;

struct Node
{
    Point point;
    Node* left;
    Node* right;

    Node(const Point point) :
        point(point),
        left(nullptr),
        right(nullptr)
    {}

    bool is_leaf() { return !(left || right); }
};

class KdTree
{
// A balanced tree that is designed for finding the best Points inside a given rectangle.
// This tree assumes that the root (i.e. first level) always splits using the x-dimension.
// Rather than indexing (x,y) by the effective dimension (e.g. point.coords[dim]),
// this implementation uses conditions (e.g. if (split_x) then points.x).
// Mainly because that's how the Point struct was defined, and because the conditionals 
// alternate at each depth (TFTFTF..) so branch prediction should be really successful.

public: // methods

    KdTree() :
        root_(nullptr)
    {}

    ~KdTree() { delete_all(root_); };

    // Copy the points in array starting at "begin" and ending one element before "end"
    // into a balanced tree structure.
    // Return the root node of the tree, or nullptr if unsuccessful. 
    // Time complexity: O(nlogn) [worst case]
    // Memory complexity: O(n)
    Node* build_tree(const Point* begin, const Point* end);

    // Return the number of Points in the tree.
    size_t size() { return size(root_); }

    // Return the number of Points found within "rect" that have been copied to "out_points" array.
    // If "out_points" is null or "max_count" is < zero then will return a negative number.
    // If this exceeds "max_count" then only the best points are copied (those with the lowest rank).
    // Note: the copied points are not sorted by rank.
    // Time complexity: 
    //    - Average O(glogn)
    //    - Worst   O(gn) - when rect contains most/all points.
    // Where g = (#found_in_rect - "max_count") or g=1 if never find "max_count" points inside rect.
    // Memory complexity: O(1)
    int32_t find_best_points(const Rect rect, int32_t max_count, Point* out_points);

private: // methods

    // Recursive method that inserts the median value Point into the tree.
    // If "split_x" is true then "indices" refer to x-dimensional indices, and "other_indices"
    // refer to y-dimensional.  "start" is the first index in the indice vectors that is processed
    // during the call, and "end" is the last index that is processed.
    // Return the node that was just created or nullptr if one wasn't. 
    Node* insert_median(indices_t& indices, indices_t& other_indices, indices_t& temp_indices, 
                        const Point* points, index_t start, index_t end, bool split_x);

    // Recursive overload for calculating number of points using a DFS.
    size_t size(const Node* node);

    // Recursive overload that implements the DFS for best points in "rect".
    void find_best_points(const Rect rect, Node* node, Point* out_points, int32_t max_count,
                          int32_t& total_inside, index_t& worst_idx, int32_t& worst_rank, bool split_on_x);

    // Check if point inside "node" is inside "rect", and if so then copy it to "out_points".
    void process_node(const Node* node, const Rect rect, int32_t max_count, Point* out_points, 
                      index_t& worst_idx, int32_t& worst_rank, int32_t& total_found_inside);

    // De-allocate memory for specified node and all of its children.
    void delete_all(Node* node);

    // Look through first "num_points" Points in "points" and copy the worst point's rank and index to output params.
    void find_worst_rank(const Point* points, int32_t num_points, int32_t& worst_rank, index_t& worst_idx);

    // Return true if p1 is greater than p2. If compare_x is true then compares x dimension, else y dim.
    bool is_greater(const Point* p1, const Point* p2, bool compare_x);

    // Return true p1 has same dimension as p2. If compare_x is true then compares x dimension, else y dim.
    bool is_same(const Point* p1, const Point* p2, bool compare_x);

private: // fields

    Node* root_;

}; // End of KdTree

}; // End of namespace

#endif