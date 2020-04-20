#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.


		// Initial light get set to ke
	vec3f I = ke;
	vec3f norm = i.N;
	vec3f P = r.at(i.t);
	vec3f out_P = P + i.N * RAY_EPSILON;

	// Handle transparency by transmissive light
	vec3f transparency = vec3f(1, 1, 1) - kt;

	// Handle ambient light effect
	vec3f ambient = prod(ka, scene->getAmbient());

	I += prod(transparency, ambient);

	// iterate over ray
	for (Scene::cliter j = scene->beginLights(); j != scene->endLights(); j++) {
		// shadow and distance attenuation, the color part is handled in light
		// use out_P is important, to see the effect, select recur_depth and look at the red one
		vec3f attenuation = (*j)->distanceAttenuation(P) * (*j)->shadowAttenuation(out_P);

		vec3f light_dir = ((*j)->getDirection(P)).normalize();
		double angle = maximum(norm.dot(light_dir), 0.0);
		vec3f diffuse = prod(kd * angle, transparency);

		vec3f R = ((2 * (norm.dot(light_dir)) * norm) - light_dir).normalize();
		vec3f specular = ks * (pow(maximum(R * (-r.getDirection()), 0), shininess * 128.0));
		I += prod(attenuation, diffuse + specular);
	}
	I = I.clamp();

	return I;
}
