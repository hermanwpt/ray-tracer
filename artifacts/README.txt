This readme file explains the less apparent bonus requirements implemented in our Trace project.
For other bonus requirements, you can examine the related artifacts to see their effects.

1B		Accelerate shadow attenuation
When calculating shadow attenuation at each intersection point, our Trace project does NOT check against every objects.
Instead, it only checks against objects that are colinear with the intersction point and each light source.
This is done by, for each light source:
	0)	Set the shadow attenuation to (1, 1, 1)
	1)	Define a ray that starts from the intersection point and points at the light source
	2)	Invokes Scene::intersect() to find the ray's next intersection point
	3)	If the material of the new intersection point is OPAQUE, return (0, 0, 0) and end calculation PREMATURELY
	4)	Else, multiply the current attenuation by the material's transmissive coefficient, then repeat from Step (2)
	5)	Return the final attenuation when ONLY objects that are between the original intersection point and the light source are checked
	
4B		Realistic shading model
For this bonus requirement, we implemented a LESS realistic model instead (which can be used to fulfill the same requirement according to the project website)
We implemented a CARTOONISH shading model, with the following features:
	-	Black border around the silhouette of each object, with thickness varying with camera distance (objects closer to the camera have thicker border)
	-	High constrast color (although the scene will appear brighter in general, originally bright colors becomes disproportionately brighter than originally dim colors)
	-	Intensity thresholding (e.g. Highlights do not appear as white gradient circles, but rather an entire circle with the same shade of white)
	
SpotLight
- Please load spot_light.ray

WarnModel
- Please load warn_model.ray

Texture Mapping
- Please load earth.ray, load Texture worldmap.bmp in asset folder and toggle TextureMapping button.

Bump Mapping
- Please load earth.ray, load Normal worldmap_normal.bmp in asset folder and toggle BumpMapping button.
- It is possible to load Texture worldmap.bmp at the same time and see its effect.

Height Field
- Please load blank.ray, load Height Field hf_512_.bmp, hf_512_grey_.bmp and render.
- The running time of this function is very long. It is therefore recommended to use smaller size when rendering this.
