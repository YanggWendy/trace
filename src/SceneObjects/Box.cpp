#include <cmath>
#include <assert.h>

#include "Box.h"
#include "float.h"
#include <iostream>
using namespace std;

#define eps 1e-5
#define ZERO(t) (fabs(t)<1e-5)

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	i.obj = this;

	//numeric_limits<double> doubleLimit;
	//double Tfar = doubleLimit.infinity(), Tnear = -Tfar;
	double Tfar = DBL_MAX;
	double Tnear = -Tfar;
	vec3f Nnear, Nfar;

	vec3f& d = r.getDirection();
	vec3f& p = r.getPosition();
	const double lBound = -0.5, uBound = 0.5;

	for (int axis = 0; axis < 3; ++axis) {
		if (abs(d[axis]) < RAY_EPSILON) {
			//parallel to the planes
			if (p[axis] < lBound || p[axis] > uBound) {
				//the light ray is out side of the slab
				return false;
			}
		}
		//else there should be intersection with this pair of plane
		vec3f N1, N2;
		double t1 = (lBound - p[axis]) / d[axis];
		//cout << "t1=" << t1 << endl;
		N1[axis] = -1;
		double t2 = (uBound - p[axis]) / d[axis];
		//cout << "t2=" << t2 << endl;
		N2[axis] = 1;
		//make sure t1 is the intersection with near plane
		if (t1 > t2) {
			double t = t1;
			t1 = t2;
			t2 = t;
			double n = N1[axis];
			N1[axis] = N2[axis];
			N2[axis] = n;
		}

		//tell if this near intersection is better than Tnear
		if (t1 > Tnear) {
			Tnear = t1;
			Nnear = N1;
		}
		//the same for Tfar
		if (t2 < Tfar) {
			Tfar = t2;
			Nfar = N2;
		}
		//return false if the ray misses the box or is behind the source
		if (Tfar < Tnear || Tfar < RAY_EPSILON) {
			return false;
		}
	}
	i.setT(Tnear);
	i.setN(Nnear);
	return true;
}


/*vec3f& p = r.getPosition();
vec3f& d = r.getDirection();

double t = DBL_MAX;
double t_temp = DBL_MAX;
double t_compare = DBL_MAX;

if (abs(d[2]) > RAY_EPSILON)//compare upper and lower surfaces
{

	double a = -((p[2] + 0.5) / d[2]);
	if (a > RAY_EPSILON)
	{
		t_temp = -((p[2] + 0.5) / d[2]);
		//cout << "t_tem=2" << t_temp << endl;
	}
	double b = -((p[2] - 0.5) / d[2]);
	if (b > RAY_EPSILON)
	{
		t_compare = -((p[2] - 0.5) / d[2]);
		//cout << "t_compare=2" << t_compare << endl;
	}

	if (t_temp > t_compare)
	{
		t =  t_compare;
	}
	else { t = t_temp; }
	//cout << "t2=" << t << endl;
}


if (abs(d[1]) > RAY_EPSILON)//compare left and right surfaces
{
	t_temp = DBL_MAX;
	t_compare = DBL_MAX;
	double a = -((p[1] + 0.5) / d[1]);
	if (a > RAY_EPSILON)
	{
		t_temp = -((p[1] + 0.5) / d[1]);
		cout << "t_tem=1" << t_temp << endl;
	}
	double b = -((p[1] - 0.5) / d[1]);
	if (b > RAY_EPSILON)
	{
		t_compare = -((p[1] - 0.5) / d[1]);
		cout << "t_compare=1" << t_compare << endl;
	}
	 double t1 = 0.0;
	 if (t_temp > t_compare)
	 {
		 t1 = t_compare;
	 }
	 else { t1 = t_temp; }

	 if (t > t1)
	 {
		 t = t1;
	 }
	 cout << "t1=" << t << endl;
}


if (abs(d[0]) > RAY_EPSILON)//compare front and back surfaces
{
	t_temp = DBL_MAX;
	t_compare = DBL_MAX;
	double a = -((p[0] + 0.5) / d[0]);
	if (a > RAY_EPSILON)
	{
		t_temp = -((p[0] + 0.5) / d[0]);
	}
	double b = -((p[0] - 0.5) / d[0]);
	if (b > RAY_EPSILON)
	{
		t_compare = -((p[0] - 0.5) / d[0]);
	}
	double t1 = 0.0;

	if (t_temp > t_compare)
	{
		t1 = t_compare;
	}
	else { t1 = t_temp; }

	if (t > t1)
	{
		t = t1;
	}
	//cout << "t0=" << t << endl;
}

//cout << "t=" << t << endl;


if (t <= RAY_EPSILON) {
	return false;
}

vec3f& P = r.at(t);

if (P[0] < -0.5 || P[0] > 0.5) {
	return false;
}

if (P[1] < -0.5 || P[1] > 0.5) {
	return false;
}

if (P[2] < -0.5 || P[2] > 0.5) {
	return false;
}


i.obj = this;
i.t = t;
if (ZERO(P[2] - 0.5)) {
	i.N = vec3f(0.0, 0.0, 1.0);
}
else if (ZERO(P[2] + 0.5)) {
	i.N = vec3f(0.0, 0.0, -1.0);
}
else if (ZERO(P[1] - 0.5)) {
	i.N = vec3f(0.0, 1.0, 0.0);
}
else if (ZERO(P[1] + 0.5)) {
	i.N = vec3f(0.0, -1.0, 0.0);
}
else if (ZERO(P[0] - 0.5)) {
	i.N = vec3f(1.0, 0.0, 0.0);
}
else if (ZERO(P[0] + 0.5)) {
	i.N = vec3f(-1.0, 0.0, 0.0);
}

cout << "t=" << t << endl;

return true;*/