#include <cmath>
#include <assert.h>

#include "Box.h"

double min(double a, double b) {
    return (a < b) ? a : b;
}

double max(double a, double b) {
    return (a > b) ? a : b;
}

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
    vec3f p = r.getPosition();
    vec3f d = r.getDirection();
    
    // Unlike square where the plane normal must originate from (0, 0, 0),
    // There are 6 planes, with their respective normal situating in either:
    //  (0.5, 0, 0),    (-0.5, 0, 0),
    //  (0, 0.5, 0),    (0, -0.5, 0),
    //  (0, 0, 0.5),    (0, 0, -0.5)
    double tx0 = (-p[0] - 0.5) / d[0];
    double tx1 = (-p[0] + 0.5) / d[0];
    double ty0 = (-p[1] - 0.5) / d[1];
    double ty1 = (-p[1] + 0.5) / d[1];
    double tz0 = (-p[2] - 0.5) / d[2];
    double tz1 = (-p[2] + 0.5) / d[2];

    double txmin = min(tx0, tx1);
    double txmax = max(tx0, tx1);
    double tymin = min(ty0, ty1);
    double tymax = max(ty0, ty1);
    double tzmin = min(tz0, tz1);
    double tzmax = max(tz0, tz1);

    double tmin = txmin;    // These 2 variables stores the t value of intersecting either the x-, y- or z- min/max plane
    double tmax = txmax;    

    // If we do NOT consider bounding area (i.e. no extent check), then we can determin valid and invalid intersections using:
    // (1) For a valid intersection, a ray must first intersect with the plane closest to it (min plane), then the plane furtherest to it (max plane)
    // (2) Conversely for an invalid intersection, a ray will first intersect with a max plane then a min plane (this only occurs when out-of-bound,)


    // Test 1: x- and y-planes intersection
    // Check for invalid intersection
    if ((tymax < tmin) || (tmax < tymin)) return false;
    // Check for valid intersection
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    // Test 2: ?- and z-planes intersection (? because after Test 1, tmin/tmax either stores x- or y-plane's values)
    // Cehck for invalid intersection
    if ((tzmax < tmin) || (tmax < tzmin)) return false;
    // Check for valid intersection
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;
    
    // Ray intersect passes all tests, there must exist valid intersection
    i.obj = this;
    i.t = tmin;
    if (tmin == tx0) i.N = vec3f(-1.0, 0.0, 0.0);
    else if (tmin == tx1) i.N = vec3f(1.0, 0.0, 0.0);
    else if (tmin == ty0) i.N = vec3f(0.0, -1.0, 0.0);
    else if (tmin == ty1) i.N = vec3f(0.0, 1.0, 0.0);
    else if (tmin == tz0) i.N = vec3f(0.0, 0.0, -1.0);
    else i.N = vec3f(0.0, 0.0, 1.0);

    return true;
}
