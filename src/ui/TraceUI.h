//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_AttenuationConstantSlider;
	Fl_Slider*			m_AttenuationLinearSlider;
	Fl_Slider*			m_AttenuationQuadricSlider;
	Fl_Slider*			m_AmbientLightSlider;
	Fl_Slider*			m_ThresholdSlider;
	Fl_Slider*			m_nSupersampleSizeSlider;
	Fl_Slider*			m_nFocallengthSlider;
	Fl_Slider*			m_nApertureSizeSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;
	Fl_Light_Button*	m_supersample;
	Fl_Light_Button*	m_jitter;
	Fl_Light_Button*	m_adaptive;
	Fl_Light_Button*	m_dof;
	Fl_Light_Button*	m_MotionBlur;
	Fl_Light_Button*	m_useGlossyButton;
	
	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double		getAttenuation_Constant();
	double		getAttenuation_Linear();
	double		getAttenuation_Quadric();
	double		getAmbientLight();
	double		getThreshold();
	int			getuseGlossy();
	int			getSupersampleSize();
	int			getSupersample();
	int			getjitter();
	int			getadaptive();
	int			getDOF();
	int			getMotionBlur();
	int			getFocallength();
	int			getApertureSize();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nAttenuation_Constant;
	double		m_nAttenuation_Linear;
	double		m_nAttenuation_Quadric;
	double		m_nAmbientLight;
	double		m_nThreshold;
	int			m_useGlossy;
	int			m_nSupersampleSize;
	int			Supersample;
	int			jitter;
	int			adaptive;
	int			DOF;
	int			MotionBlur;
	int        Focallength;
	int			ApertureSize;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_background_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_Attenuation_ConstantSlides(Fl_Widget* o, void* v);
	static void cb_Attenuation_LinearSlides(Fl_Widget* o, void* v);
	static void cb_Attenuation_QuadricSlides(Fl_Widget* o, void* v);
	static void cb_AmbientLightSlides(Fl_Widget* o, void* v);
	static void cb_ThresholdSlides(Fl_Widget* o, void* v);
	static void cb_useGlossy(Fl_Widget* o, void* v);
	static void cb_SupersampleSize(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
	static void cb_supersample(Fl_Widget* o, void* v);
	static void cb_dof(Fl_Widget* o, void* v);
	static void cb_MotionBlur(Fl_Widget* o, void* v);
	static void cb_adaptive(Fl_Widget* o, void* v);
	static void cb_jitter(Fl_Widget* o, void* v);
	static void cb_Focallength(Fl_Widget* o, void* v);
	static void cb_ApertureSize(Fl_Widget* o, void* v);
};

#endif
