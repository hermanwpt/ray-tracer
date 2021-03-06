#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "../scene/scene.h"

class Sphere
	: public MaterialSceneObject
{
public:
	Sphere( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}
    
	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual vec3f mapTexture(const vec3f& intersection, unsigned char* texture, const int& width, const int& height) const override;
	virtual vec3f mapBumpTexture(const ray& ray, const isect& isect, const vec3f& intersection, unsigned char* bumpMap, const int& width, const int& height) const override;

    virtual BoundingBox ComputeLocalBoundingBox()
    {
        BoundingBox localbounds;
		localbounds.min = vec3f(-1.0f, -1.0f, -1.0f);
		localbounds.max = vec3f(1.0f, 1.0f, 1.0f);
        return localbounds;
    }
};
#endif // __SPHERE_H__
