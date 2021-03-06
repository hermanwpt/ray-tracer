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
	Fl_Slider*			m_thresholdSlider;
	Fl_Slider*			m_SSAASlider;
	Fl_Slider*			m_apertureSizeSlider;
	Fl_Slider*			m_focalLengthSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Light_Button*	m_SSAAButton;
	Fl_Light_Button*	m_adaptiveSSAAButton;
	Fl_Light_Button*	m_jitterButton;
	Fl_Light_Button*	m_textureMappingButton;
	Fl_Light_Button*	m_bumpMappingButton;
	Fl_Light_Button*	m_glossButton;
	Fl_Light_Button*	m_softShadowButton;
	Fl_Light_Button*	m_DOFButton;
	Fl_Light_Button*	m_montionBlurButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	int			getSSAASize();
	int			getApertureSize();

	double		getThreshold();
	double		getFocalLength();

	bool		isSSAA();
	bool		isAdaptiveSSAA();
	bool		isJitter();
	bool		isTextureMapping();
	bool		isBumpMapping();
	bool		isGloss();
	bool		isSoftShadow();
	bool		isDOF();
	bool		isMotionBlur();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	int			m_nSSAASize;
	int			m_nApertureSize;

	double		m_nThreshold;
	double		m_nFocalLength;

	bool		m_nSSAA;
	bool		m_nAdaptiveSSAA;
	bool		m_nJitter;
	bool		m_nTextureMapping;
	bool		m_nBumpMapping;
	bool		m_nGloss;
	bool		m_nSoftShadow;
	bool		m_nDOF;
	bool		m_nMotionBlur;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_background_image(Fl_Menu_* o, void* v);
	static void cb_load_texture_image(Fl_Menu_* o, void* v);
	static void cb_load_normal_image(Fl_Menu_* o, void* v);
	static void cb_load_height_field(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_set_shader_phong(Fl_Menu_* o, void* v);
	static void cb_set_shader_toon(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_thresholdSlides(Fl_Widget* o, void* v);
	static void cb_SSAASlides(Fl_Widget* o, void* v);
	static void cb_apertureSizeSlides(Fl_Widget* o, void* v);
	static void cb_focalLengthSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);

	static void cb_SSAAButton(Fl_Widget* o, void* v);
	static void cb_adaptiveSSAAButton(Fl_Widget* o, void* v);
	static void cb_jitterButton(Fl_Widget* o, void* v);
	static void cb_textureMappingButton(Fl_Widget* o, void* v);
	static void cb_bumpMappingButton(Fl_Widget* o, void* v);
	static void cb_glossButton(Fl_Widget* o, void* v);
	static void cb_softShadowButton(Fl_Widget* o, void* v);
	static void cb_DOFButton(Fl_Widget* o, void* v);
	static void cb_motionBlurButton(Fl_Widget* o, void* v);
};

#endif
