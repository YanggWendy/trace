#include <cmath>

#include "light.h"

#include "../ui/TraceUI.h"
#include <iostream>
using namespace std;

extern TraceUI* traceUI;

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
	: Light(scene, color),position(pos)
{}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0


	//double const_atten_coeff = traceUI->getAttenuation_Constant();
	//double linear_atten_coeff = traceUI->getAttenuation_Linear();
	//double quadratic_atten_coeff = traceUI->getAttenuation_Quadric();
	
	
	double d_square = (P - position).length_squared();
	double d = sqrt(d_square);
	double coef = m_const_atten_coeff + m_linear_atten_coeff * d + m_quadratic_atten_coeff * d_square;
	//double coef = const_atten_coeff + linear_atten_coeff * d + quadratic_atten_coeff * d_square;
	//cout << " m_const_atten_coeff = " << m_const_atten_coeff << endl;
	//cout << " m_linear_atten_coeff = " << m_linear_atten_coeff << endl;
	//cout << " m_quadratic_atten_coeff = " << m_quadratic_atten_coeff << endl;
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
	vec3f cur_P = P;
	isect isec_P;
	vec3f result = getColor(P);
	ray r = ray(cur_P, d);
	while (scene->intersect(r, isec_P))
	{
		if ((distance -= isec_P.t) < RAY_EPSILON) return result;
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


void  PointLight::setAttenuationCoefficient(double constant, double linear, double quadratic)
{
	m_const_atten_coeff = constant;
	m_linear_atten_coeff = linear;
	m_quadratic_atten_coeff = quadratic;
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}