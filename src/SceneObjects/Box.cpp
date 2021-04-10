#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
    
    vec3f d = r.getDirection();
    vec3f p = r.getPosition();

    // Algorithm modified from the given hints
    double tfar = 999999999;
    double tnear = -tfar;
    vec3f nnear = vec3f(0, 0, 0);
    vec3f nfar = vec3f(0, 0, 0);

    // Perform square intersection test on all 3 axes
    // Uses the given Square.cpp for reference
    for (int i = 0; i < 3; ++i) {
        // Check for invalid rays (i.e. Rays parallel to both planes in the given axis)
        if (d[i] == 0.0) {
            // A ray that is parallel to one plane does not mean it is also parallel to other planes (because 6 planes are oriented in 3 different axes)
            // An extent check is needed to confirm if this ray must not intersect with planes oriented in other axes
            if (p[i] < -0.5 || p[i] > 0.5) {
                return false;
            }
        }

        // Attempt to intersect both the near and far plane in the given axis
        // t1 stands for the near plane and t2 stands for the far plane
        // n1 and n2 stands for their respective normal
        vec3f n1 = vec3f(0, 0, 0);
        vec3f n2 = vec3f(0, 0, 0);

        double t1 = (-p[i] - 0.5) / d[i];
        double t2 = (-p[i] + 0.5) / d[i];
        n1[i] = -1.0;   // Since the planes are axis-oriented, their normal must have direction in that given axis
        n2[i] = 1.0;    // Trivially, the near plane should have a normal pointing TOWARDS us, while the far plane should have a normal pointing AWAY FROM us

        // Ensures t1, n1 represent the near plane and vice versa for t2, n2
        if (t1 > t2) {
            double temp = t1;
            t1 = t2;
            t2 = temp;
            temp = n1[i];
            n1[i] = n2[i];
            n2[i] = temp;
        }

        // The far plane should be as close to us as possible (otherwise it would go to the scene's FAR constant (i.e. infinite distance)
        if (t2 < tfar) {
            tfar = t2;
            nfar = n2;
        }

        // Conversely, the same logic goes for the near plane
        if (t1 > tnear) {
            tnear = t1;
            nnear = n1;
        }
        
        // Final check for t value
        // (1) tfar < tnear is impossible by definition, so this is an invalid case
        // (2) tfar < RAY_EPSILON essentially means t < 0, which means the intersection is behind the COP, so this is an invalid case
        if (tfar < tnear || tfar < RAY_EPSILON) {
            return false;
        }
    }

    i.obj = this;
    i.t = tnear;
    i.N = nnear;
    return true;
}
