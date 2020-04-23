#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:


	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
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

	PointLight(Scene* scene, const vec3f& pos, const vec3f& color);
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual void setAttenuationCoefficient(double constant, double linear, double quadratic);
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	double m_const_atten_coeff;
	double	m_linear_atten_coeff;
	double	m_quadratic_atten_coeff;

};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene* scene, const vec3f& pos, const vec3f& color)
		: Light(scene, color), color(color) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
private:
	vec3f color;
};


class SpotLight
	: public Light
{
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& edge, const vec3f& dir, const vec3f& color)
		: Light(scene, color), position(pos), edgeplace(edge), direction(dir), m_const_atten_coeff(0.0), m_linear_atten_coeff(0.0), m_quadratic_atten_coeff(0.0) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
	vec3f getEdgeplace(const vec3f& P) const;
	vec3f getPosition(const vec3f& P) const;
	void  setAttenuationCoefficient(double constant, double linear, double quadratic);

protected:
	vec3f 		position;
	vec3f 		direction;
	vec3f 		edgeplace;
	double m_const_atten_coeff;
	double	m_linear_atten_coeff;
	double	m_quadratic_atten_coeff;
};



#endif // __LIGHT_H__
