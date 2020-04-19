#include <cmath>

#include "light.h"

//extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.

	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
	vec3f d = getDirection(P);

	// loop to get the attenuation
	vec3f cur_P = P;
	isect isec_P;
	vec3f result = getColor(P);
	ray r = ray(cur_P, d);
	while (scene->intersect(r, isec_P))
	{
		//if not transparent return black
		if (isec_P.getMaterial().kt.iszero()) return vec3f(0, 0, 0);
		//use current intersection point as new light source
		cur_P = r.at(isec_P.t);
		r = ray(cur_P, d);
		result = prod(result, isec_P.getMaterial().kt);
	}
	return result;
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
	return 1.0;
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


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return vec3f(1,1,1);
}
