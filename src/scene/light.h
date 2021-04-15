#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {
		constantAtten = 0.0;
		linearAtten = 0.0;
		quadAtten = 0.25;
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	void setConstantAtten(double x);
	void setLinearAtten(double x);
	void setQuadAtten(double x);

protected:
	vec3f position;
	double constantAtten;
	double linearAtten;
	double quadAtten;
};

// FROM spotlight.ray in demo
//spot_light {
//position = (0, 2, 0);
//direction = (0, -2, 0);
//edgeplace = (30, 0, 0);
//colour = (0.8, 0.8, 0.8);
//}
//spot_light{
//	position = (1, 2, -1);
//	direction = (-1, -2, 1);
//	edgeplace = (20, 0, 0);
//	colour = (0.8, 0.8, 0.8);
//}

class SpotLight
	: public Light
{
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& color, const vec3f& d, const double& ang)
		: Light(scene, color), position(pos), direction{ d }, angle{ ang } {
		constantAtten = 0.0;
		linearAtten = 0.0;
		quadAtten = 0.25;
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f position;
	vec3f direction;
	double angle;
	double constantAtten;
	double linearAtten;
	double quadAtten;
};

//class WarnModelLight
//	: public Light
//{
//public:
//	SpotLight(Scene* scene, const vec3f& pos, const vec3f& color, const vec3f& d, const vec3f& ep)
//		: Light(scene, color), position(pos), direction{ d }, edgeplace{ ep } {
//		angle = edgeplace[0];
//		constantAtten = 0.0;
//		linearAtten = 0.0;
//		quadAtten = 0.25;
//	}
//	virtual vec3f shadowAttenuation(const vec3f& P) const;
//	virtual double distanceAttenuation(const vec3f& P) const;
//	virtual vec3f getColor(const vec3f& P) const;
//	virtual vec3f getDirection(const vec3f& P) const;
//
//protected:
//	vec3f position;
//	vec3f direction;
//	vec3f edgeplace;
//	double angle;
//	double constantAtten;
//	double linearAtten;
//	double quadAtten;
//};

#endif // __LIGHT_H__
