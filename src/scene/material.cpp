#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shadePhong( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	// Emissive and ambient terms
	vec3f ia = scene->getAmbientIntensity();
	vec3f intensity = ke + vec3f((1 - kt[0]) * ka[0] * ia[0], (1 - kt[1]) * ka[1] * ia[1], (1 - kt[2]) * ka[2] * ia[2]);
	vec3f intersection = r.at(i.t); // Note to self: Equivalent to r.getPosition() + r.getDirection() * i.t;
	
	// Diffuse and specular term
	for (list<Light*>::const_iterator p = scene->beginLights(); p != scene->endLights(); ++p) {
		vec3f atten = ((*p)->distanceAttenuation(intersection) * (*p)->shadowAttenuation(intersection)).clamp();
		vec3f ip = (*p)->getColor(intersection);
		
		// Diffuse term
		double diffuseDot = i.N.dot((*p)->getDirection(intersection));
		if (diffuseDot < 0) diffuseDot = 0;

		vec3f diffusekd = kd;
		
		if (scene->mappingTexture()) {
			diffusekd = ((MaterialSceneObject*)(i.obj))->mapTexture(intersection, scene->getTexture(), scene->getTextureWidth(), scene->getTextureHeight());
		}

		if (scene->hasNormalImg()) {
			diffuseDot = (((MaterialSceneObject*)(i.obj))->mapBumpTexture(r, i, intersection, scene->getBumpMap(), scene->getBumpMapWidth(), scene->getBumpMapHeight())).dot((*p)->getDirection(intersection));
			if (diffuseDot < 0) diffuseDot = 0;
		}

		intensity += vec3f(atten[0] * (1 - kt[0]) * diffusekd[0] * diffuseDot * ip[0], atten[1] * (1 - kt[1]) * diffusekd[1] * diffuseDot * ip[1], atten[2] * (1 - kt[2]) * diffusekd[2] * diffuseDot * ip[2]);

		// Specular term
		vec3f reflectedRay = (2 * (*p)->getDirection(intersection).dot(i.N) * i.N - (*p)->getDirection(intersection)).normalize();
		double specularDot = (-r.getDirection()).dot(reflectedRay);
		if (specularDot < 0) specularDot = 0;
		specularDot = pow(specularDot, shininess * 128);

		intensity += vec3f(atten[0] * ks[0] * specularDot * ip[0], atten[1] * ks[1] * specularDot * ip[1], atten[2] * ks[2] * specularDot * ip[2]);
	}
	
	return intensity.clamp();
}

vec3f Material::shadeToon(Scene* scene, const ray& r, const isect& i) const
{
	// Distributed ray tracing
	// Determine whether the currently shaded pixel belongs to the OUTER silhouette of the current object by comparing with neighboring rays
	// Neighboring rays should intersect with the same object as the main ray r, otherwise it is a silhouette and should be outlined
	isect tempIntersect;
	for (int j = -1; j <= 1; j += 2) {
		for (int k = -1; k <= 1; k += 2) {
			for (int l = -1; l <= 1; l += 2) {
				ray tempRay = ray(r.getPosition(), (r.getDirection() + vec3f(0.003 * j, 0.003 * k, 0.003 * l)).normalize());
				tempIntersect.obj = NULL;
				(scene->intersect(tempRay, tempIntersect));
				if (tempIntersect.obj == NULL) return vec3f(0, 0, 0);
				if (tempIntersect.obj != i.obj) if (tempIntersect.getMaterial().kd != i.getMaterial().kd) return vec3f(0, 0, 0);
			}
		}
	}

	// Emissive and ambient terms
	vec3f ia = scene->getAmbientIntensity();
	vec3f intensity = ke + vec3f((1 - kt[0]) * ka[0] * ia[0], (1 - kt[1]) * ka[1] * ia[1], (1 - kt[2]) * ka[2] * ia[2]);
	vec3f intersection = r.at(i.t);

	// Diffuse and specular term
	for (list<Light*>::const_iterator p = scene->beginLights(); p != scene->endLights(); ++p) {
		vec3f atten = ((*p)->distanceAttenuation(intersection) * (*p)->shadowAttenuation(intersection)).clamp();
		vec3f ip = (*p)->getColor(intersection);

		// Diffuse term
		double diffuseDot = i.N.dot((*p)->getDirection(intersection));
		if (diffuseDot < 0) diffuseDot = 0;
		if (diffuseDot >= 1) diffuseDot = 1;
		else if (diffuseDot >= 0.8) diffuseDot = 0.8;
		else if (diffuseDot >= 0.6) diffuseDot = 0.6;
		else if (diffuseDot >= 0.4) diffuseDot = 0.4;
		else if (diffuseDot >= 0.2) diffuseDot = 0.2;

		intensity += vec3f(sqrt(atten[0] * (1 - kt[0]) * kd[0] * diffuseDot * ip[0]) * 0.7, sqrt(atten[1] * (1 - kt[1]) * kd[1] * diffuseDot * ip[1]) * 0.7, sqrt(atten[2] * (1 - kt[2]) * kd[2] * diffuseDot * ip[2]) * 0.7);

		// Specular term
		vec3f reflectedRay = (2 * (*p)->getDirection(intersection).dot(i.N) * i.N - (*p)->getDirection(intersection)).normalize();
		double specularDot = (-r.getDirection()).dot(reflectedRay);
		if (specularDot < 0.95) specularDot = 0;
		if (specularDot >= 0.95) specularDot = 1;
		specularDot = pow(specularDot, shininess * 128);

		intensity += vec3f(atten[0] * ks[0] * 0.7 * specularDot * ip[0], atten[1] * ks[1] * 0.7 * specularDot * ip[1], atten[2] * ks[2] * 0.7 * specularDot * ip[2]);
	}

	return intensity.clamp();
}
