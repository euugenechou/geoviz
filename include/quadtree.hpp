#ifndef __QUADTREE_HPP__
#define __QUADTREE_HPP__

#include <string>
#include <vector>

// Coordinate-based object.
typedef struct Point {
    std::string sample;
    double lat;
    double lng;

    // Pretty-prints a Point.
    void print(void);
} Point;

// Axis-aligned bounding box with half-dimension and center.
typedef struct AABB {
    double north;
    double south;
    double east;
    double west;

    // Returns true an AABB contains Point p.
    // @p: Point to check if the AABB contains.
    bool contains_point(const std::shared_ptr<Point> &p);

    // Returns true an AABB and AABB x intersect each other.
    // @x: The AABB to compute intersection with.
    bool intersects_aabb(AABB x);
} AABB;

// Point QuadTree class.
class QuadTree {
private:
    // Constant static capacity for all QuadTrees.
    // TODO: Make dynamic?
    static const size_t capacity = 1024;

    // Boundary of a QuadTree.
    AABB boundary;

    // Vector of points in a QuadTree node.
    std::vector<std::shared_ptr<Point>> points;

    // Children. All are unique_ptr since these children belong solely to the parent.
    std::unique_ptr<QuadTree> north_west;
    std::unique_ptr<QuadTree> north_east;
    std::unique_ptr<QuadTree> south_west;
    std::unique_ptr<QuadTree> south_east;

    // Internal function to subdivide a QuadTree when inserting a new Point.
    void subdivide(void);

public:
    // Constructor/destructor.
    // @n: North bound.
    // @s: South bound.
    // @e: East bound.
    // @w: West bound.
    QuadTree(double n, double s, double e, double w);
    ~QuadTree(void) = default;

    // Inserts the sample using the lat/lng as a Point into the QuadTree.
    // @sample: Sample name.
    // @lat: Sample's latitude.
    // @lng: Sample's longitude.
    bool insert(const std::string &sample, double lat, double lng);

    // Queries the given range for a vector of Points in the queried range.
    // Each Point in the vector is a shared_ptr since they are moved around.
    // @n: North bound.
    // @s: South bound.
    // @e: East bound.
    // @w: West bound.
    std::vector<std::shared_ptr<Point>> query(double n, double s, double e, double w);

    // Prints out the contents of a QuadTree.
    void print(size_t depth = 0);
};

#endif
