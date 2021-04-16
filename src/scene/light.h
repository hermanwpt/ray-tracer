#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual vec3f softShadowAttenuation(const vec3f& P) const = 0;
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
	virtual vec3f softShadowAttenuation(const vec3f& P) const;
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
	virtual vec3f softShadowAttenuation(const vec3f& P) const;
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
	virtual vec3f softShadowAttenuation(const vec3f& P) const { return vec3f{ 0,0,0 }; }
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

class WarnModel
	: public Light
{
public:
	WarnModel(Scene* scene, const vec3f& pos, const vec3f& color, const vec3f& d, const double& sz, const double& sp)
		: Light(scene, color), position(pos), direction{ d }, size{ sz } {
		shape = sp;
		constantAtten = 0.0;
		linearAtten = 0.0;
		quadAtten = 0.25;

		vec3f t = { 0, 1, 0 };
		vec3f X = direction.cross(t).normalize();
		vec3f Y = direction.cross(X).normalize();
		X = direction.cross(Y).normalize();

		mat4f translation({ 1, 0, 0, -position[0] },
						{ 0, 1, 0, -position[1] },
						{ 0, 0, 1, -position[2] },
						{ 0, 0, 0, 0 });

		mat4f rotation({ X[0], X[1], X[2], 0 },
						{ Y[0], Y[1], Y[2], 0 },
						{ direction[0], direction[1], direction[2], 0 },
						{ 0, 0, 0, 1 });

		mat4f projection({ 1, 0, 0, 0 },
						{ 0, 1, 0, 0 },
						{ 0, 0, 1, 0 },
						{ 0, 0, 1, 0 });

		matrix = projection * rotation * translation;
	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual vec3f softShadowAttenuation(const vec3f& P) const { return vec3f{ 0,0,0 }; }
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f position;
	vec3f direction;
	double size;
	int shape;

	mat4f matrix;
	
	double constantAtten;
	double linearAtten;
	double quadAtten;
};

#endif // __LIGHT_H__
