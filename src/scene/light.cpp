#include <cmath>

#include "light.h"

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
	ray r = ray(P, d);
	isect i;
	if (scene->intersect(r, i)) {
		return vec3f(0, 0, 0);
	}

	return vec3f(1, 1, 1);
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
	double distance = (P - position).length();
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
	vec3f d = (position - P).normalize(); // Direction from the point to light (instead of from the light to point)
	ray r = ray(P, d);
	isect i;
	if (scene->intersect(r, i)) {
		vec3f intersection = r.at(i.t);
		double lightDistance = (P - position).length();
		double objectDisstance = (intersection - position).length();
		if (objectDisstance < lightDistance) {
			return vec3f(0, 0, 0);
		}
	}

    return vec3f(1,1,1);
}
