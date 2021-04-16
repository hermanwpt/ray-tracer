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

// Bump Mapping
// http://www.cs.ucy.ac.cy/courses/EPL426/courses/eBooks/ComputerGraphicsPrinciplesPractice.pdf page.600
// https://web.eecs.umich.edu/~sugih/courses/eecs487/lectures/26-BumpMap+ProcTex.pdf
// https://en.wikipedia.org/wiki/Normal_mapping

// https://learnopengl.com/Advanced-Lighting/Normal-Mapping
vec3f Sphere::mapBumpTexture(const ray& ray, const isect& isect, const vec3f& intersection, unsigned char* bumpMap, const int& width, const int& height) const
{
	// Sp, Se, Sn
	vec3f sp = { 0, 1, 0 };
	vec3f se = { 0, 0, 1 };
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

	int index = (y * width + x) * 3;
	vec3f normal { static_cast<double>(bumpMap[r]), static_cast<double>(bumpMap[g]), static_cast<double>(bumpMap[b]) };

	// Calculate TBN
	vec3f T = (sp.cross(sn)).normalize();
	vec3f B = (T.cross(sn)).normalize();

	mat4f TBN{ vec4f{ T[0], T[1], T[2], 0 },
					vec4f{ B[0], B[1], B[2], 0 },
					vec4f{ sn[0], sn[1], sn[2], 0},
					vec4f{ 0.0, 0.0, 0.0, 1.0} };

	// OPENGL: normal = normal * 2.0 - 1.0;
	// Adjust normal values from the bump map to [-1,1]
	normal[0] = (normal[0] / 255.0f) * 2 - 1;
	normal[1] = (normal[1] / 255.0f) * 2 - 1;
	normal[2] = (normal[2] / 255.0f) * 2 - 1;

	// OPENGL: normal = normalize(fs_in.TBN * normal); 
	return (TBN * normal).normalize();
}
