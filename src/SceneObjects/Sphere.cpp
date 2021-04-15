#include <cmath>

#include "Sphere.h"

#define PI 3.14159265359

bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.N = r.at( t1 ).normalize();
	} else {
		i.t = t2;
		i.N = r.at( t2 ).normalize();
	}

	return true;
}

// Inverse Mapping in lecture notes
vec3f Sphere::mapTexture(const vec3f& intersection, unsigned char* texture, const int& width, const int& height) const
{
	// Sp, Se, Sn
	vec3f sp = { 0, 1, 0 };
	vec3f se = { 1, 0, 0 };
	vec3f sn = transform->globalToLocalCoords(intersection).normalize();

	double u, v;

	// v
	double phi = acos(-sn * sp);
	v = phi / PI;

	// u
	if (v >= 1 || v <= 0) {
		u = 0;
	}
	else {
		double theta = acos((se * sn) / (sin(phi))) / (2 * PI);
		if ((sp.cross(se)) * sn > 0) {
			u = theta;
		}
		else {
			u = 1 - theta;
		}
	}

	// UV2TextureMap
	int x = min(int(u * width), width - 1);
	int y = min(int(v * height), height - 1);

	int r = (y * width + x) * 3, g = (y * width + x) * 3 + 1, b = (y * width + x) * 3 + 2;

	return vec3f{ texture[r] / 255.0f, texture[g] / 255.0f, texture[b] / 255.0f };
}
