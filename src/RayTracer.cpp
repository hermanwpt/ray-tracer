// The main ray tracer.

#include <Fl/fl_ask.h>

#include "fileio/bitmap.h"

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	switch (shader) {
		case PHONG: return traceRayPhong(scene, r, thresh, depth); break;
		case TOON: return traceRayToon(scene, r, thresh, depth); break;
		default: break;
	}
	return vec3f(0, 0, 0);	// This shouldn't occur
}

vec3f RayTracer::traceRayPhong(Scene* scene, const ray& r,
	const vec3f& thresh, int depth) {
	isect i;

	if (scene->intersect(r, i)) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		if (depth >= traceUI->getDepth() + 1) return vec3f(0.0, 0.0, 0.0);

		double threshold = traceUI->getThreshold();
		const Material& m = i.getMaterial();
		vec3f intersection = r.at(i.t);

		// Shadow rays
		vec3f intensity = m.shadePhong(scene, r, i);
		if ((intensity[0] < threshold) && (intensity[1] < threshold) && (intensity[2] < threshold)) return intensity;

		// Reflected ray
		if (!m.kr.iszero()) {
			vec3f reflectDir = (2 * (-r.getDirection()).dot(i.N) * i.N - (-r.getDirection())).normalize();
			vec3f ir = traceRay(scene, ray(intersection, reflectDir), thresh, depth + 1);
			intensity += vec3f(m.kr[0] * ir[0], m.kr[1] * ir[1], m.kr[2] * ir[2]);
		}

		// Refracted ray
		if (!m.kt.iszero()) {
			double ni, nt;
			vec3f Nr = i.N;	// Normal exclusively for refraction
			bool isTLR = false;
			// When ray is entering the object, the dot product between the ray and the normal should be positive
			// This is because the normal always point OUTWARDS FROM the surface
			if ((-r.getDirection()).dot(i.N) >= 0) {
				ni = 1.0;
				nt = m.index;
				// Conversely, when ray is exiting the object, the dot product between the ray and the noraml should be negative
			}
			else {
				ni = m.index;
				nt = 1.0;
				Nr = -Nr;
			}

			// Check if total internal reflection occurs
			if (ni > nt) {
				double thetac = asin(nt / ni);
				double thetai = acos((-r.getDirection()).dot(Nr));
				if (thetai > thetac) isTLR = true;
			}

			// Only refract if total internal reflection doesnt occur (which is addressed by reflected ray)
			if (!isTLR) {
				double sinnt = (ni / nt) * sin(acos((-r.getDirection().dot(Nr))));
				vec3f refractDir = (((-r.getDirection()).dot(Nr) * Nr - (-r.getDirection())).normalize() * (sqrt(1 + sinnt * sinnt) * sinnt) - Nr).normalize();
				if (sinnt <= NORMAL_EPSILON) refractDir = -Nr;
				vec3f ir = traceRay(scene, ray(intersection, refractDir), thresh, depth + 1);
				intensity += vec3f(m.kt[0] * ir[0], m.kt[1] * ir[1], m.kt[2] * ir[2]);
			}

		}

		return intensity.clamp();

	}
	else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		if (bg) {
			// This part essentially reverts Camera::rayThrough( double x, double y, ray &r ) to retrieve x and y,
			// which is the normalized screen coodrinates ranging from [0, 1] by performing perspective transformation
			// back to the image plane
			Camera* cam = scene->getCamera();

			// See "3D Transformations" lecture notes for notations
			vec3f u = cam->getU();			// The "X"
			vec3f v = cam->getV();			// The "Y"
			vec3f look = cam->getLook();	// The "Z"
			vec3f d = r.getDirection();

			double DX = d.dot(u);		// D = Focal length = |d|cos(theta) and X = |u|, so d.u gives DX
			double DY = d.dot(v);		// The same logic but Y = |v|, so d.v gives DY
			double DZ = d.dot(look);	// The same logic but Z = |look|, so d.look gives DZ

			int x = (DX / DZ) * bgWidth + bgWidth / 2 + 0.5;			// I don't know why + bgWidth / 2 is needed but only 1/4 of the background image is shown on the top-right quadrant
			int y = (DY / DZ) * bgHeight + bgHeight / 2 + 0.5;		// of the rendered image when I run the code without this term. The same goes for + bgHeight / 2. 

			// I actually don't know why I need to check the lower and upper bounds of texture space coordinates
			// They are not supposed to fall outside the valid range but I encountered segmentation error anyway
			// So I hardcoded this constraint to prevent crashing lol
			if ((x < 0) || (x >= bgWidth) || (y < 0) || (y >= bgHeight)) return vec3f(0, 0, 0);

			return vec3f(bg[3 * (y * bgWidth + x)] / 256.0, bg[3 * (y * bgWidth + x) + 1] / 256.0, bg[3 * (y * bgWidth + x) + 2] / 256.0);
		}
		else {
			return vec3f(0.0, 0.0, 0.0);
		}
	}
}

vec3f RayTracer::traceRayToon(Scene* scene, const ray& r,
	const vec3f& thresh, int depth) {
	isect i;

	if (scene->intersect(r, i)) {
		if (depth >= traceUI->getDepth() + 1) return vec3f(0.0, 0.0, 0.0);

		double threshold = traceUI->getThreshold();
		const Material& m = i.getMaterial();
		vec3f intersection = r.at(i.t);

		// Shadow rays
		vec3f intensity = m.shadeToon(scene, r, i);
		if ((intensity[0] < threshold) && (intensity[1] < threshold) && (intensity[2] < threshold)) return intensity;

		// Reflected ray
		if (!m.kr.iszero()) {
			vec3f reflectDir = (2 * (-r.getDirection()).dot(i.N) * i.N - (-r.getDirection())).normalize();
			vec3f ir = traceRay(scene, ray(intersection, reflectDir), thresh, depth + 1);
			intensity += vec3f(m.kr[0] * ir[0], m.kr[1] * ir[1], m.kr[2] * ir[2]);
		}

		// Refracted ray
		if (!m.kt.iszero()) {
			double ni, nt;
			vec3f Nr = i.N;	// Normal exclusively for refraction
			bool isTLR = false;
			// When ray is entering the object, the dot product between the ray and the normal should be positive
			// This is because the normal always point OUTWARDS FROM the surface
			if ((-r.getDirection()).dot(i.N) >= 0) {
				ni = 1.0;
				nt = m.index;
				// Conversely, when ray is exiting the object, the dot product between the ray and the noraml should be negative
			}
			else {
				ni = m.index;
				nt = 1.0;
				Nr = -Nr;
			}

			// Check if total internal reflection occurs
			if (ni > nt) {
				double thetac = asin(nt / ni);
				double thetai = acos((-r.getDirection()).dot(Nr));
				if (thetai > thetac) isTLR = true;
			}

			// Only refract if total internal reflection doesnt occur (which is addressed by reflected ray)
			if (!isTLR) {
				double sinnt = (ni / nt) * sin(acos((-r.getDirection().dot(Nr))));
				vec3f refractDir = (((-r.getDirection()).dot(Nr) * Nr - (-r.getDirection())).normalize() * (sqrt(1 + sinnt * sinnt) * sinnt) - Nr).normalize();
				if (sinnt <= NORMAL_EPSILON) refractDir = -Nr;
				vec3f ir = traceRay(scene, ray(intersection, refractDir), thresh, depth + 1);
				intensity += vec3f(m.kt[0] * ir[0], m.kt[1] * ir[1], m.kt[2] * ir[2]);
			}

		}

		return intensity.clamp();

	}
	else {
		if (bg) {
			// This part essentially reverts Camera::rayThrough( double x, double y, ray &r ) to retrieve x and y,
			// which is the normalized screen coodrinates ranging from [0, 1] by performing perspective transformation
			// back to the image plane
			Camera* cam = scene->getCamera();

			// See "3D Transformations" lecture notes for notations
			vec3f u = cam->getU();			// The "X"
			vec3f v = cam->getV();			// The "Y"
			vec3f look = cam->getLook();	// The "Z"
			vec3f d = r.getDirection();

			double DX = d.dot(u);		// D = Focal length = |d|cos(theta) and X = |u|, so d.u gives DX
			double DY = d.dot(v);		// The same logic but Y = |v|, so d.v gives DY
			double DZ = d.dot(look);	// The same logic but Z = |look|, so d.look gives DZ

			int x = (DX / DZ) * bgWidth + bgWidth / 2 + 0.5;			// I don't know why + bgWidth / 2 is needed but only 1/4 of the background image is shown on the top-right quadrant
			int y = (DY / DZ) * bgHeight + bgHeight / 2 + 0.5;		// of the rendered image when I run the code without this term. The same goes for + bgHeight / 2. 

			// I actually don't know why I need to check the lower and upper bounds of texture space coordinates
			// They are not supposed to fall outside the valid range but I encountered segmentation error anyway
			// So I hardcoded this constraint to prevent crashing lol
			if ((x < 0) || (x >= bgWidth) || (y < 0) || (y >= bgHeight)) return vec3f(0, 0, 0);

			return vec3f(bg[3 * (y * bgWidth + x)] / 256.0, bg[3 * (y * bgWidth + x) + 1] / 256.0, bg[3 * (y * bgWidth + x) + 2] / 256.0);
		}
		else {
			return vec3f(0.0, 0.0, 0.0);
		}
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	bg = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	shader = PHONG;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	if (bg) delete[] bg;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

bool RayTracer::loadBackgroundImage(char* fn)
{
	unsigned char* data;
	unsigned char* another_data;
	int	width, height;

	// Attempt to read file
	if ((data = readBMP(fn, width, height)) == NULL) {
		fl_alert("Can't load bitmap file");
		return false;
	}

	// Make sure the background image is of aspect ratio 1:1
	if (width != height) {
		fl_alert("Background image must have aspect ratio of 1:1");
		return false;
	}

	// Release old storage for the old background image
	if (bg) delete[] bg;

	// Update background image size
	bgWidth = width;
	bgHeight = height;

	bg = data;

	return true;
}

bool RayTracer::loadTextureImage(char* fn)
{
	unsigned char* data;
	unsigned char* another_data;
	int	width, height;

	// Attempt to read file
	if ((data = readBMP(fn, width, height)) == NULL) {
		fl_alert("Can't load bitmap file");
		return false;
	}

	// Update background image size
	scene->setTextureData(data, width, height);

	return true;
}

bool RayTracer::loadNormalImage(char* fn)
{
	unsigned char* data;
	unsigned char* another_data;
	int	width, height;

	// Attempt to read file
	if ((data = readBMP(fn, width, height)) == NULL) {
		fl_alert("Can't load bitmap file");
		return false;
	}

	// Update background image size
	scene->setNormalData(data, width, height);

	return true;
}

void RayTracer::setShader(int s)
{
	shader = s;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col = vec3f(0, 0, 0);

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	if (traceUI->isSSAA()) {
		int n = traceUI->getSSAASize();
		double subPixelSize = (1 / double(buffer_height)) / n;
		bool jitter = traceUI->isJitter();

		for (int a = 0; a < n; ++a) {
			for (int b = 0; b < n; ++b) {
				if (!jitter) col += trace(scene, x + a * subPixelSize, y + b * subPixelSize);
				else {
					double xJitter = (-rand() / RAND_MAX) * subPixelSize;
					double yJitter = (-rand() / RAND_MAX) * subPixelSize;
					col += trace(scene, x + a * subPixelSize + xJitter, y + b * subPixelSize + yJitter);
				}
			}
		}
		col = col / (n * n);
	} else if (traceUI->isAdaptiveSSAA()) {

	} else {
		col = trace(scene, x, y);
	}
	

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}
