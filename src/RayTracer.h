#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <iostream>
using namespace std;
#include <list>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene,  ray& r, const vec3f& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );
	void loadBackground(char* file);
	bool sceneLoaded();
	vec3f getBackgroundImage(double x, double y);
	void getFiveRays(ray& center, ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight, double width, double height);
	vec3f adaptiveSuperSampling(ray& center, ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight, int depth);


private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;
	int background_width;
	int background_height;
	unsigned char* backgroundImage;
	bool m_useBackground;
	list<Material> media_History_stack;
};

#endif // __RAYTRACER_H__
