#include <cmath>

#include "light.h"

#define PI 3.14159265359

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P); // Direction from the point to light (instead of from the light to point)
	vec3f atten = getColor(P);
	ray r = ray(P, d);
	isect i;

	// Keep doing intersection test from P to the light source (denoted by ray r) until there is no more intersection
	// Whenever r intersects with a TRANSPARENT object, multiply the current attenuation by that object's transmissive coefficient

	while (scene->intersect(r, i)) {
		Material m = i.getMaterial();
		if (m.kt.iszero()) return vec3f(0, 0, 0);
		if (isnan(i.t)) return vec3f(0, 0, 0); // A bug with the given intersect function causes this, I can do nothing but give up shading this pixel
		atten = vec3f(atten[0] * m.kt[0], atten[1] * m.kt[1], atten[2] * m.kt[2]);
		r = ray(r.at(i.t), d);
	}

	return atten;
}

vec3f DirectionalLight::softShadowAttenuation(const vec3f& P) const
{
	vec3f tempAtten = vec3f(0, 0, 0);

	// I don't even know what I'm doing at this point
	// I'm basically forcing the raytracer to trace more than one shadow rays by defining 8 imaginary light sources per light source
	// I guess this counts as a kind of distributed forward raytracing???
	for (int j = -1; j <= 1; j += 2) {
		for (int k = -1; k <= 1; k += 2) {
			for (int l = -1; l <= 1; l += 2) {
				DirectionalLight tempLight = DirectionalLight(scene, orientation + vec3f(0.01 * j * rand() / RAND_MAX, 0.01 * k * rand() / RAND_MAX, 0.01 * l * rand() / RAND_MAX), color);
				tempAtten += tempLight.shadowAttenuation(P);
			}
		}
	}
	// tempAtten += shadowAttenuation(P);	// Also attenuating the original shadow looks odd
	return tempAtten / 8;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double distance = (P - position).length() / 5;
	double atten = 1.0 / (constantAtten + linearAtten * distance + quadAtten * (distance * distance));
	return (1.0 < atten) ? 1.0 : atten;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}

void PointLight::setConstantAtten(double x)
{
	constantAtten = x;
}

void PointLight::setLinearAtten(double x)
{
	linearAtten = x;
}

void PointLight::setQuadAtten(double x)
{
	quadAtten = x;
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P); // Direction from the point to light (instead of from the light to point)
	vec3f atten = getColor(P);
	ray r = ray(P, d);
	isect i;

	// Keep doing intersection test from P to the light source (denoted by ray r) until there is no more intersection
	// Whenever r intersects with a TRANSPARENT object, multiply the current attenuation by that object's transmissive coefficient
	while (scene->intersect(r, i)) {
		Material m = i.getMaterial();
		if (m.kt.iszero()) return vec3f(0, 0, 0);
		if (isnan(i.t)) return vec3f(0, 0, 0); // A bug with the given intersect function causes this, I can do nothing but give up shading this pixel
		atten = vec3f(atten[0] * m.kt[0], atten[1] * m.kt[1], atten[2] * m.kt[2]);
		r = ray(r.at(i.t), d);
	}

	return atten;
}

vec3f PointLight::softShadowAttenuation(const vec3f& P) const
{
	vec3f tempAtten = vec3f(0, 0, 0);

	// I don't even know what I'm doing at this point
	// I'm basically forcing the raytracer to trace more than one shadow rays by defining 8 imaginary light sources per light source
	// I guess this counts as a kind of distributed forward raytracing???
	for (int j = -1; j <= 1; j += 2) {
		for (int k = -1; k <= 1; k += 2) {
			for (int l = -1; l <= 1; l += 2) {
				PointLight tempLight = PointLight(scene, position + vec3f(0.1 * j * rand() / RAND_MAX, 0.1 * k * rand() / RAND_MAX, 0.1 * l * rand() / RAND_MAX), color);
				tempAtten += tempLight.shadowAttenuation(P);
			}
		}
	}
	// tempAtten += shadowAttenuation(P);	// Also attenuating the original shadow looks odd
	return tempAtten / 8;
}

double SpotLight::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double distance = (P - position).length() / 5;
	double atten = 1.0 / (constantAtten + linearAtten * distance + quadAtten * (distance * distance));
	atten = (1.0 < atten) ? 1.0 : atten;

	double range = -getDirection(P) * direction;
	if (range >= cos(angle / 180 * PI)) {
		return atten;
	}
	return 0;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}

vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	vec3f d = getDirection(P); // Direction from the point to light (instead of from the light to point)
	vec3f atten = getColor(P);
	ray r = ray(P, d);
	isect i;

	// Keep doing intersection test from P to the light source (denoted by ray r) until there is no more intersection
	// Whenever r intersects with a TRANSPARENT object, multiply the current attenuation by that object's transmissive coefficient
	while (scene->intersect(r, i)) {
		Material m = i.getMaterial();
		if (m.kt.iszero()) return vec3f(0, 0, 0);
		if (isnan(i.t)) return vec3f(0, 0, 0); // A bug with the given intersect function causes this, I can do nothing but give up shading this pixel
		atten = vec3f(atten[0] * m.kt[0], atten[1] * m.kt[1], atten[2] * m.kt[2]);
		r = ray(r.at(i.t), d);
	}

	return atten;
}

double WarnModel::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	vec4f p_in_object = matrix * vec4f({ P[0], P[1], P[2] });
	double x = p_in_object[0], y = p_in_object[1];

	switch (shape) {
	case 0:
		if (abs(x) > size || abs(y) > size)
			return 0;
		break;
	case 1:
		if (y < -size / 2 || y > tan(PI / 3) * x + size || y > -tan(PI / 3) * x + size)
			return 0;
		break;
	case 2:
		if (x * x + y * y > size || (x * x + y * y < size / 2))
			return 0;
		break;
	case 3:
			return 0;
		break;
	}

	double distance = (P - position).length();
	double atten = 1.0 / (constantAtten + linearAtten * distance + quadAtten * (distance * distance));
	return (1.0 < atten) ? 1.0 : atten;
}

vec3f WarnModel::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f WarnModel::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}

vec3f WarnModel::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	vec3f d = getDirection(P); // Direction from the point to light (instead of from the light to point)
	vec3f atten = getColor(P);
	ray r = ray(P, d);
	isect i;

	// Keep doing intersection test from P to the light source (denoted by ray r) until there is no more intersection
	// Whenever r intersects with a TRANSPARENT object, multiply the current attenuation by that object's transmissive coefficient
	while (scene->intersect(r, i)) {
		Material m = i.getMaterial();
		if (m.kt.iszero()) return vec3f(0, 0, 0);
		if (isnan(i.t)) return vec3f(0, 0, 0); // A bug with the given intersect function causes this, I can do nothing but give up shading this pixel
		atten = vec3f(atten[0] * m.kt[0], atten[1] * m.kt[1], atten[2] * m.kt[2]);
		r = ray(r.at(i.t), d);
	}

	return atten;
}