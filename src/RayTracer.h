#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

enum SHADER_TYPE {
	PHONG,
	TOON
};

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );
	vec3f traceRayPhong(Scene* scene, const ray& r, const vec3f& thresh, int depth);
	vec3f traceRayToon(Scene* scene, const ray& r, const vec3f& thresh, int depth);

	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );
	bool loadBackgroundImage( char* fn );
	bool loadTextureImage( char* fn );
	bool loadNormalImage( char* fn );
	bool loadHeightField(char* fn, char* grey);

	void setToggledTexture(bool toggled) { scene->setToggledTexture(toggled); }

	void setToggledNormal(bool toggled) { scene->setToggledNormal(toggled); }

	void setShader(int s);

	bool sceneLoaded();

private:
	unsigned char* buffer;
	unsigned char* bg;
	int buffer_width, buffer_height;
	int bgWidth, bgHeight;
	int bufferSize;
	Scene* scene;
	
	int shader;

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
