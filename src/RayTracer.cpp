// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include "fileio/bitmap.h"
#include <random>
#include <stack>
extern unsigned char* readBMP(char* fname, int& width, int& height);

#include <iostream>
using namespace std;

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );


    scene->getCamera()->rayThrough( x,y,r );
	
	uniform_real_distribution<double> rand(0, 2);
	static default_random_engine re;

	double grid_size = traceUI->getSupersampleSize();
	double step_x = 1.0 / (grid_size * buffer_width * 2);
	double step_y = 1.0 / (grid_size * buffer_height * 2);

	if (traceUI->getSupersample()) {
		double factor_x = 0.0;
		double factor_y = 0.0;
		if (traceUI->getjitter()) {
			factor_x = rand(re) - 1.0;
			factor_y = rand(re) - 1.0;
		}
		vec3f result(0.0, 0.0, 0.0);

		x = x - (grid_size / 2) * step_x + step_y * 0.5;
		y = y - (grid_size / 2) * step_x + step_y * 0.5;
		for (int Y = 1; Y <= grid_size; Y++) {
			for (int X = 1; X <= grid_size; X++) {
				scene->getCamera()->rayThrough(x + factor_x * step_x, y + factor_y * step_y, r);
				result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
				x += step_x;
			}
			y += step_y;
		}
		double number = 1.0 / (grid_size * grid_size);
		result *= number;
		return result;
	}

	//adaptive supper sampling
	/*if (traceUI->getadaptive()) {

		ray up_Left(r); 
		ray up_Right(r);
		ray down_Left(r); 
		ray down_Right(r);

		double width = double(1) / double(buffer_width);
		double height = double(1) / double(buffer_height);
		getFiveRays(r, up_Left, up_Right, down_Left, down_Right, width, height);
		return adaptiveSuperSampling(r, up_Left, up_Right, down_Left, down_Right, 0);
	}*/

	if (traceUI->getDOF()) {
		vec3f focalPoint = r.at(traceUI->getFocallength() + 1);
		vec3f result;
		result = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
		for (int i = 0; i < 50; i++) {
			double factor_X = rand(re) - 1.0;
			double factor_Y = rand(re) - 1.0;

			vec3f new_Point = scene->getCamera()->getWorldCoords(x + factor_X * 0.05 * traceUI->getApertureSize(), y + factor_X * 0.05 * traceUI->getApertureSize());
			vec3f new_Dir = (focalPoint - new_Point).normalize();
			ray new_ray(new_Point, new_Dir);
			result += traceRay(scene, new_ray, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
		}
		for (int i = 0; i < 3; i++) {
			result[i] /= 51.0f;
		
		}
		return result;
	}

	if (traceUI->getMotionBlur()) {

		/*vec3f result;
		for (int i = 0; i < 50; i++) {
			scene->motionBlurObjectsFactorTransform();
			result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
		}
		scene->motionBlurObjectsChange();
		result /= 50;
		return result;*/
		vec3f result(0.0, 0.0, 0.0);
		for (int i = 0; i < 50; i++) {
			scene->motionBlurObjectsDeltaTransform();
			result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
		}
		scene->motionBlurObjectRestore();
		result /= 50;
		return result;

	}


	return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
	
	
	/*ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
	scene->getCamera()->rayThrough(x, y, r);
	return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();*/
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, ray& r, 
	const vec3f& thresh, int depth )
{

	isect i;
	vec3f shade;

	if( scene->intersect( r, i ) ) {
		vec3f I;
		vec3f P = r.at(i.t);
		I += i.getMaterial().shade(scene, r, i);// shade
		if (depth >= traceUI->getDepth())
		{
			return I;
		}
		//cout << "intersect" << endl;

		vec3f norm = i.N;
		vec3f dir = -r.getDirection();
		vec3f R = ((2 * (norm.dot(dir)) * norm) - dir).normalize();
		ray new_ray(P,R);


		//reflection part 
		if (!i.getMaterial().kr.iszero())
		{

			if (I.length() / (depth + 1) < traceUI->getThreshold())
			{
				return I;
			}


			vec3f shade = traceRay(scene, new_ray, thresh, depth + 1);
			//cout << "m_useGlossy=" << traceUI->getuseGlossy() << endl;
		
			if (traceUI->getuseGlossy())
			{
				uniform_real_distribution<double> rand(0, 2);
				static default_random_engine re;

				vec3f u = (R ^ norm).normalize();
				vec3f v = (u ^ R).normalize();
				for (int i = 0; i < 50; i++) {
					float factor_U = rand(re) - 1.0;
					float factor_V = rand(re) - 1.0;

					vec3f newReflec = (R + u * factor_U * 0.05 + v * factor_V * 0.05).normalize();
					ray newRay(P, newReflec);
					shade += traceRay(scene, newRay, thresh, depth+ 1);
				}
				for (int j = 0; j < 3; j++)
				{
					shade[j] /= 51.0f;
				}
				shade = prod(shade, i.getMaterial().kr);
				I += shade;
			}
			else { I += prod(i.getMaterial().kr, traceRay(scene, new_ray, thresh, depth + 1)); }
		}
		
		

		if (!i.getMaterial().kt.iszero())
		{
			// take account total refraction effect
			bool TotalRefraction = false;
			// opposite ray
			ray oppR(P, r.getDirection()); //without refraction

			// marker to simulate a stack
			//bool toAdd = false, toErase = false;


				// refractive index
			double index_i, index_t;

			// For ray go out of an object
			if (i.N * r.getDirection() > RAY_EPSILON)
			{
				if (media_History_stack.empty()) 
				{
					index_i = i.getMaterial().index;

					index_t = 1.0;
				}
				else 
				{
					index_i = media_History_stack.front().index;
					media_History_stack.pop_front();
					if (media_History_stack.empty()) 
					{
						index_t = 1.0;
					}
					else 
					{
						index_t = media_History_stack.front().index;
					}


				}
			
				norm = -i.N;
			}
			// For ray get in the object
			else
			{

				if (media_History_stack.empty())
				{
					index_t = i.getMaterial().index;
					index_i = 1.0;
					media_History_stack.push_front(i.getMaterial());
				}
				else 
				{
					index_i = media_History_stack.front().index;
					media_History_stack.push_front(i.getMaterial());
					index_t = i.getMaterial().index;
				}
				norm = i.N;
			}


			double index_Ratio = index_i / index_t;
			double cos_i = max(min(norm * ((-r.getDirection()).normalize()), 1.0), -1.0); //SYSNOTE: min(x, 1.0) to prevent cos_i becomes bigger than 1
			double sin_i = sqrt(1 - cos_i * cos_i);
			double sin_t = sin_i * index_Ratio;

			if (sin_t >1.0)
			{
				TotalRefraction = true;
			}
			else
			{
				TotalRefraction = false;
				double cos_t = sqrt(1 - sin_t * sin_t);
				vec3f Tdir = (index_Ratio * cos_i - cos_t) * norm - index_Ratio * (-r.getDirection());
				oppR = ray(P, Tdir);
				if (I.length() / (depth + 1) < traceUI->getThreshold())
				{
					return I;
				}
				I += prod(i.getMaterial().kt, traceRay(scene, oppR, thresh, depth + 1));
			}
		}


		I = I.clamp();
		return I;
	
	}
	else {
		/*bool ok = true;
		if (depth > 0) {
			ok = scene->getCamera()->resetRayCoords(r);
		}*/
		if (m_useBackground)
		{
			vec3f x = scene->getCamera()->getU();
			vec3f y = scene->getCamera()->getV();
			vec3f z = scene->getCamera()->getLook();
			double distance_x = r.getDirection() * x;
			double distance_y = r.getDirection() * y;
			double distance_z = r.getDirection() * z;
			return getBackgroundImage(distance_x / distance_z + 0.5, distance_y / distance_z + 0.5);
		}
		
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		 return vec3f(0,0,0);
		//return I;
	}
}

void RayTracer::loadBackground(char* file)
{
	unsigned char* data = NULL;
	data = readBMP(file, background_width, background_height);
	if (data) {
		if (backgroundImage) delete[] backgroundImage;
		m_useBackground = true;
		backgroundImage = data;
	}

}


vec3f RayTracer::getBackgroundImage(double x, double y) {
	if (!m_useBackground) return vec3f(0, 0, 0);
	int xGrid = int(x * background_width);
	int yGrid = int(y * background_height);
	if (xGrid < 0 || xGrid >= background_width || yGrid < 0 || yGrid >= background_height)
	{
		return vec3f(0, 0, 0);
	}
	double pixel1 = backgroundImage[(yGrid * background_width + xGrid) * 3] / 255.0;
	double pixel2 = backgroundImage[(yGrid * background_width + xGrid) * 3 + 1] / 255.0;
	double pixel3 = backgroundImage[(yGrid * background_width + xGrid) * 3 + 2] / 255.0;
	return vec3f(pixel1, pixel2, pixel3);
}



RayTracer::RayTracer():backgroundImage(NULL), m_useBackground(false)
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
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
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

/*void RayTracer::getFiveRays(ray& center, ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight, double width, double height) {


	double* coordinates = center.getCoordinates();

	scene->getCamera()->rayThrough(coordinates[0] - width / double(2), coordinates[1] - height / double(2), bottomleft);
	scene->getCamera()->rayThrough(coordinates[0] + width / double(2), coordinates[1] - height / double(2), bottomRight);
	scene->getCamera()->rayThrough(coordinates[0] - width / double(2), coordinates[1] + height / double(2), topLeft);
	scene->getCamera()->rayThrough(coordinates[0] + width / double(2), coordinates[1] + height / double(2), topRight);
}


vec3f RayTracer::adaptiveSuperSampling(ray& center, ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight, int depth) {
	if (depth == 3)
		return vec3f(0.0, 0.0, 0.0);
	stack<Material> material;

	vec3f colorCenter = traceRay(scene, center, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
	vec3f colorTopLeft = traceRay(scene, topLeft, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
	vec3f colorTopRight = traceRay(scene, topRight, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
	vec3f colorBottomLeft = traceRay(scene, bottomleft, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
	vec3f colorBottomRight = traceRay(scene, bottomRight, vec3f(1.0, 1.0, 1.0), traceUI->getDepth() - 2).clamp();
	bool statements[4];
	statements[0] = (colorCenter - colorTopLeft).length() > 0.1;
	statements[1] = (colorCenter - colorTopRight).length() > 0.1;
	statements[2] = (colorCenter - colorBottomLeft).length() > 0.1;
	statements[3] = (colorCenter - colorBottomRight).length() > 0.1;

	double pixelWidth = double(1) / double(buffer_width);
	double pixelHeight = double(1) / double(buffer_height);

	double level = pow(2, depth + 1);
	double width = pixelWidth / double(level);
	double height = pixelHeight / double(level);

	ray copyCenter(center);

	vec3f result(0.0, 0.0, 0.0);
	int numOfDiff = 0;
	if (statements[0]) {
	
		center.getCoordinates()[0] = copyCenter.getCoordinates()[0] - width / double(2);
		center.getCoordinates()[1] = copyCenter.getCoordinates()[1] + height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorTopLeft = newColor;
	}

	if (statements[1]) {
	
		center.getCoordinates()[0] = copyCenter.getCoordinates()[0] + width / double(2);
		center.getCoordinates()[1] = copyCenter.getCoordinates()[1] + height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorTopRight = newColor;
	}

	if (statements[2]) {
	
		center.getCoordinates()[0] = copyCenter.getCoordinates()[0] - width / double(2);
		center.getCoordinates()[1] = copyCenter.getCoordinates()[1] - height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorBottomLeft = newColor;
	}

	if (statements[3]) {
	
		center.getCoordinates()[0] = copyCenter.getCoordinates()[0] + width / double(2);
		center.getCoordinates()[1] = copyCenter.getCoordinates()[1] - height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0) {
			colorBottomRight = newColor;
			
		}
	}


	result = (colorBottomRight + colorBottomLeft + colorTopLeft + colorTopRight) / double(4);
	return result;
}*/