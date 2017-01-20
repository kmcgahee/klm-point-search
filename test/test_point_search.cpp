
#include "catch.h"
#include "point_tree.h"

// This is in no way meant to be a full set of tests, just ones I used while building my kd-tree.
// The actual point search is tested by comparing results against reference.dll
TEST_CASE("adding nodes to tree") 
{
    PointTree::KdTree tree;

    // Enough points to get a split on both x and y dimensions.
    const int32_t num_points = 7;
    Point points[num_points];
    for (int32_t i = 0; i < num_points; ++i)
    {
        points[i].id = i;
        points[i].rank = i;
        points[i].x = (float)i;
        points[i].y = (float)-i;
    }

    PointTree::Node* root = tree.build_tree(points, points + num_points);

    SECTION("tree root is correctly return")
    {
        REQUIRE(root != nullptr);
    }
    SECTION("tree contains correct number of nodes")
    {
        REQUIRE(tree.size() == num_points);
    }
    SECTION("all points are balanced")
    {
        // Points should be added at each level by median value, starting with x dimension.
        REQUIRE(root->point.id == 3);
        REQUIRE(root->left->point.id == 1); // median y value of lower set
        REQUIRE(root->right->point.id == 5); // median y value of upper set
        // On last level right child is smaller ID because y coordinates are negative.
        REQUIRE(root->left->left->point.id == 2);
        REQUIRE(root->left->right->point.id == 0); 
        REQUIRE(root->right->left->point.id == 6);
        REQUIRE(root->right->right->point.id == 4); 
    }

    return;
}
