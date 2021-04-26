#include "quadtree.hpp"
#include <iostream>

void Point::print(void) {
    std::cout.precision(15);
    std::cout << sample << "{"
              << "lat: " << lat << ", "
              << "lng: " << lng << "}" << std::endl;
}

bool AABB::contains_point(const std::shared_ptr<Point> &p) {
    return south <= p->lat && p->lat <= north && west <= p->lng && p->lng <= east;
}

bool AABB::intersects_aabb(AABB y) {
    if (north < y.south) return false; // If AABB x.north < AABB y.south, no intersection.
    if (south > y.north) return false; // If AABB x.south < AABB y.north, no intersection.
    if (east < y.west) return false;   // If AABB x.east < AABB y.west, no intersection.
    if (west > y.east) return false;   // If AABB x.west < AABB y.east, no intersection.

    // They intersect if all the conditions above are held.
    return true;
}

QuadTree::QuadTree(double n, double s, double e, double w) {
    // QuadTree initially has no points.
    size = 0;

    // Set up the boundary AABB.
    boundary = {
        .north = n,
        .south = s,
        .east  = e,
        .west  = w,
    };

    // Null out each of the contained node pointers.
    for (int i = 0; i < QUADTREE_CAPACITY; i += 1) {
        points[i] = nullptr;
    }

    // Null out the children.
    north_west = nullptr;
    north_east = nullptr;
    south_west = nullptr;
    south_east = nullptr;
}

void QuadTree::subdivide(void) {
    // Just to shorten the actual arguments later.
    double north     = boundary.north;
    double south     = boundary.north;
    double west      = boundary.west;
    double east      = boundary.east;
    double halfnorth = (boundary.north + boundary.south) / 2;
    double halfeast  = (boundary.east + boundary.west) / 2;

    // Make the QuadTrees.
    north_west = std::make_unique<QuadTree>(north, halfnorth, halfeast, west);
    north_east = std::make_unique<QuadTree>(north, halfnorth, east, halfeast);
    south_west = std::make_unique<QuadTree>(halfnorth, south, halfeast, west);
    south_east = std::make_unique<QuadTree>(halfnorth, south, east, halfeast);
}

bool QuadTree::insert(std::string sample, double lat, double lng) {
    // Create the point from the specified lat/lng and name.
    std::shared_ptr<Point> p = std::make_shared<Point>(Point {
        .sample = sample,
        .lat    = lat,
        .lng    = lng,
    });

    // Don't insert points that don't belong in the QuadTree.
    if (!boundary.contains_point(p)) {
        return false;
    }

    // Add point here if there is space and there aren't subdivisions.
    if (size < QUADTREE_CAPACITY && !north_west) {
        points[size++] = p;
        return true;
    }

    // Otherwise, subdivide and add point to some accepting node.
    if (!north_west) {
        subdivide();
    }

    // Try inserting into any of the new quadtrees.
    if (north_west->insert(sample, lat, lng)) return true;
    if (north_east->insert(sample, lat, lng)) return true;
    if (south_west->insert(sample, lat, lng)) return true;
    if (south_east->insert(sample, lat, lng)) return true;

    // If all else fails.
    return false;
}

std::vector<std::shared_ptr<Point>> QuadTree::query_range(double n, double s, double e, double w) {
    // Vector to hold points within bounds.
    std::vector<std::shared_ptr<Point>> bounded;

    // Set up the range AABB.
    AABB range = {
        .north = n,
        .south = s,
        .east  = e,
        .west  = w,
    };

    // Don't bother if range doesn't intersect the QuadTree.
    if (!boundary.intersects_aabb(range)) {
        return bounded;
    }

    // Check points in current quadtree.
    for (int i = 0; i < size; i += 1) {
        if (range.contains_point(points[i])) {
            bounded.push_back(points[i]);
        }
    }

    // If there are children, add points from them.
    if (north_west) {
        std::vector<std::shared_ptr<Point>> nwpts = north_west->query_range(n, s, e, w);
        std::vector<std::shared_ptr<Point>> nepts = north_east->query_range(n, s, e, w);
        std::vector<std::shared_ptr<Point>> swpts = south_west->query_range(n, s, e, w);
        std::vector<std::shared_ptr<Point>> septs = south_east->query_range(n, s, e, w);

        // Append points from children QuadTrees into the vector.
        bounded.insert(bounded.end(), nwpts.begin(), nwpts.end());
        bounded.insert(bounded.end(), nepts.begin(), nepts.end());
        bounded.insert(bounded.end(), swpts.begin(), swpts.end());
        bounded.insert(bounded.end(), septs.begin(), septs.end());
    }

    return bounded;
}

void QuadTree::print(int depth) {
    for (int i = 0; i < size; i += 1) {
        for (int j = 0; j < depth; j += 1) {
            std::cout << "|  ";
        }
        points[i]->print();
    }
    if (north_west) north_west->print(depth + 1);
    if (north_east) north_east->print(depth + 1);
    if (south_west) south_west->print(depth + 1);
    if (south_east) south_east->print(depth + 1);
}
