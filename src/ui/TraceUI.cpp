//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_Attenuation_ConstantSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuation_Constant = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_Attenuation_LinearSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuation_Linear = double(((Fl_Slider*)o)->value());
}


void TraceUI::cb_Attenuation_QuadricSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAttenuation_Quadric = double(((Fl_Slider*)o)->value());
}


void TraceUI::cb_AmbientLightSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLight = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_ThresholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThreshold = double(((Fl_Slider*)o)->value());
}





void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getAttenuation_Constant()
{
	return m_nAttenuation_Constant;
}

double TraceUI::getAttenuation_Linear()
{
	return m_nAttenuation_Linear;
}
double TraceUI::getAttenuation_Quadric()
{
	return m_nAttenuation_Quadric;
}
double TraceUI::getAmbientLight()
{
	return m_nAmbientLight;
}
double TraceUI::getThreshold()
{
	return m_nThreshold;
}


// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_mainWindow = new Fl_Window(100, 40, 320, 300, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install Attenuation,Constant
		m_AttenuationConstantSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation,Constant");
		m_AttenuationConstantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AttenuationConstantSlider->type(FL_HOR_NICE_SLIDER);
		m_AttenuationConstantSlider->labelfont(FL_COURIER);
		m_AttenuationConstantSlider->labelsize(12);
		m_AttenuationConstantSlider->minimum(0);
		m_AttenuationConstantSlider->maximum(1);
		m_AttenuationConstantSlider->step(0.01);
		m_AttenuationConstantSlider->value(m_nAttenuation_Constant);
		m_AttenuationConstantSlider->align(FL_ALIGN_RIGHT);
		m_AttenuationConstantSlider->callback(cb_Attenuation_ConstantSlides);


		// install slider Attenuation,Linear
		m_AttenuationLinearSlider = new Fl_Value_Slider(10, 105, 180, 20, "Attenuation,Linear");
		m_AttenuationLinearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AttenuationLinearSlider->type(FL_HOR_NICE_SLIDER);
		m_AttenuationLinearSlider->labelfont(FL_COURIER);
		m_AttenuationLinearSlider->labelsize(12);
		m_AttenuationLinearSlider->minimum(0);
		m_AttenuationLinearSlider->maximum(1);
		m_AttenuationLinearSlider->step(0.01);
		m_AttenuationLinearSlider->value(m_nAttenuation_Linear);
		m_AttenuationLinearSlider->align(FL_ALIGN_RIGHT);
		m_AttenuationLinearSlider->callback(cb_Attenuation_LinearSlides);

		// install slider size
		m_AttenuationQuadricSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation,Quadric");
		m_AttenuationQuadricSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AttenuationQuadricSlider->type(FL_HOR_NICE_SLIDER);
		m_AttenuationQuadricSlider->labelfont(FL_COURIER);
		m_AttenuationQuadricSlider->labelsize(12);
		m_AttenuationQuadricSlider->minimum(0);
		m_AttenuationQuadricSlider->maximum(1);
		m_AttenuationQuadricSlider->step(0.01);
		m_AttenuationQuadricSlider->value(m_nAttenuation_Quadric);
		m_AttenuationQuadricSlider->align(FL_ALIGN_RIGHT);
		m_AttenuationQuadricSlider->callback(cb_Attenuation_QuadricSlides);


		// install slider Ambient Light
		m_AmbientLightSlider = new Fl_Value_Slider(10, 155, 180, 20, "Ambient Light");
		m_AmbientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AmbientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_AmbientLightSlider->labelfont(FL_COURIER);
		m_AmbientLightSlider->labelsize(12);
		m_AmbientLightSlider->minimum(0);
		m_AmbientLightSlider->maximum(1);
		m_AmbientLightSlider->step(0.01);
		m_AmbientLightSlider->value(m_nAmbientLight);
		m_AmbientLightSlider->align(FL_ALIGN_RIGHT);
		m_AmbientLightSlider->callback(cb_AmbientLightSlides);


		// install slider Threshold
		m_ThresholdSlider = new Fl_Value_Slider(10, 180, 180, 20, "Threshold");
		m_ThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ThresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_ThresholdSlider->labelfont(FL_COURIER);
		m_ThresholdSlider->labelsize(12);
		m_ThresholdSlider->minimum(0);
		m_ThresholdSlider->maximum(1);
		m_ThresholdSlider->step(0.01);
		m_ThresholdSlider->value(m_nThreshold);
		m_ThresholdSlider->align(FL_ALIGN_RIGHT);
		m_ThresholdSlider->callback(cb_ThresholdSlides);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}