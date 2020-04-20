#include <cmath>

#include "light.h"

#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

/*PointLight::PointLight(Scene* scene, const vec3f& pos, const vec3f& color)
	: Light(scene, color), position(pos)
{
	m_const_atten_coef = traceUI->getAttenuation_Constant();
	m_linear_atten_coef = traceUI->getAttenuation_Linear();
	m_quadratic_atten_coef = traceUI->getAttenuation_Quadric();
}*/

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
		if (isec_P.getMaterial().kt.iszero())
		{
			return vec3f(0, 0, 0);
		}
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

PointLight::PointLight(Scene* scene, const vec3f& pos, const vec3f& color)
	: Light(scene, color),
	position(pos),
	m_const_atten_coef(0.0),
	m_linear_atten_coef(0.0),
	m_quadratic_atten_coef(0.0)
{}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0


	//setAttenuationCoefficient(traceUI->getAttenuation_Constant(), traceUI->getAttenuation_Linear(), traceUI->getAttenuation_Quadric());
	double const_atten_coef = traceUI->getAttenuation_Constant();
	double linear_atten_coef = traceUI->getAttenuation_Linear();
	double quadratic_atten_coef = traceUI->getAttenuation_Quadric();
	
	
	double d_square = (P - position).length_squared();
	double d = sqrt(d_square);
	double coef = const_atten_coef + linear_atten_coef * d + quadratic_atten_coef * d_square;
	return coef == 0.0 ? 1.0 : 1.0 / max<double>(coef, 1.0);
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
	double distance = (position - P).length();
	vec3f d = getDirection(P);
	// loop to get the attenuation
	vec3f cur_P = position;
	isect isec_P;
	vec3f result = getColor(P);
	ray r = ray(cur_P, d);
	while (scene->intersect(r, isec_P))
	{
		if (abs(distance-isec_P.t)< RAY_EPSILON)
		{
			//if not transparent return black
			if (isec_P.getMaterial().kt.iszero())
			{
				return vec3f(0, 0, 0);
			}
			//use current intersection point as new light source
			cur_P = r.at(isec_P.t);
			//r = ray(cur_P, d);
			result = prod(result, isec_P.getMaterial().kt);
		}
		
	}
	return result;
}


void  PointLight::setAttenuationCoefficient(double constant, double linear, double quadratic)
{
	m_const_atten_coef = constant;
	m_linear_atten_coef = linear;
	m_quadratic_atten_coef = quadratic;
}