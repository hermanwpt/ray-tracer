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

void TraceUI::cb_load_background_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Open File?", "*.bmp", "");
	if (newfile != NULL) {
		pUI->raytracer->loadBackgroundImage(newfile);
	}
}

void TraceUI::cb_load_texture_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	char* newfile = fl_file_chooser("Open File?", "*.bmp", "");
	if (newfile != NULL) {
		pUI->raytracer->loadTextureImage(newfile);
		pUI->m_textureMappingButton->activate();
		pUI->m_bumpMappingButton->activate();
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

void TraceUI::cb_set_shader_phong(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	pUI->raytracer->setShader(PHONG);
}

void TraceUI::cb_set_shader_toon(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);
	pUI->raytracer->setShader(TOON);
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

void TraceUI::cb_thresholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThreshold = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_SSAASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSSAASize = int(((Fl_Slider*)o)->value());
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

void TraceUI::cb_SSAAButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	((TraceUI*)(o->user_data()))->m_nSSAA = bool(((Fl_Light_Button*)o)->value());
	if (pUI->m_nSSAA) {
		pUI->m_SSAASlider->activate();
		pUI->m_adaptiveSSAAButton->deactivate();
		pUI->m_jitterButton->activate();
	} else {
		pUI->m_SSAASlider->deactivate();
		pUI->m_adaptiveSSAAButton->activate();
		pUI->m_jitterButton->deactivate();
	}
}

void TraceUI::cb_adaptiveSSAAButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	((TraceUI*)(o->user_data()))->m_nAdaptiveSSAA = bool(((Fl_Light_Button*)o)->value());
	if (pUI->m_nAdaptiveSSAA) {
		pUI->m_SSAASlider->activate();
		pUI->m_SSAAButton->deactivate();
		pUI->m_jitterButton->activate();
	}
	else {
		pUI->m_SSAASlider->deactivate();
		pUI->m_SSAAButton->activate();
		pUI->m_jitterButton->deactivate();
	}
}

void TraceUI::cb_jitterButton(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nJitter = bool(((Fl_Light_Button*)o)->value());
}

void TraceUI::cb_textureMappingButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->m_nTextureMapping = bool(((Fl_Light_Button*)o)->value());
	if (pUI->m_nTextureMapping) {
		pUI->raytracer->setToggledTexture(true);
		pUI->m_bumpMappingButton->deactivate();
	}
	else {
		pUI->raytracer->setToggledTexture(false);
		pUI->m_bumpMappingButton->activate();
	}
}

void TraceUI::cb_bumpMappingButton(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->m_nBumpMapping = bool(((Fl_Light_Button*)o)->value());
	if (pUI->m_nBumpMapping) {
		pUI->raytracer->setToggledTexture(true);
		pUI->m_textureMappingButton->deactivate();
	}
	else {
		pUI->raytracer->setToggledTexture(false);
		pUI->m_textureMappingButton->activate();
	}
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

int TraceUI::getSSAASize()
{
	return m_nSSAASize;
}

double TraceUI::getThreshold()
{
	return m_nThreshold;
}

bool TraceUI::isSSAA()
{
	return m_nSSAA;
}

bool TraceUI::isAdaptiveSSAA()
{
	return m_nAdaptiveSSAA;
}

bool TraceUI::isJitter()
{
	return m_nJitter;
}

bool TraceUI::isTextureMapping()
{
	return m_nTextureMapping;
}

bool TraceUI::isBumpMapping()
{
	return m_nBumpMapping;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image, 0, FL_MENU_DIVIDER },
		{ "&Load Background Image...", FL_ALT + 'b', (Fl_Callback *)TraceUI::cb_load_background_image, 0, }, 
		{ "&Load Texture Image...", FL_ALT + 't', (Fl_Callback*)TraceUI::cb_load_texture_image, 0, FL_MENU_DIVIDER },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Shader",		0, 0, 0, FL_SUBMENU },
		{ "&Phong Illumination Model",	FL_ALT + 'p', (Fl_Callback*)TraceUI::cb_set_shader_phong },
		{ "&Cartoonish",	FL_ALT + 'c', (Fl_Callback*)TraceUI::cb_set_shader_toon },
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
	m_nSSAASize = 1;
	m_nThreshold = 0.0;
	m_nSSAA = false;
	m_nAdaptiveSSAA = false;
	m_nJitter = false;
	m_nTextureMapping = false;
	m_nBumpMapping = false;
	loadedTexture = false;
	m_mainWindow = new Fl_Window(100, 40, 320, 200, "Ray <Not Loaded>");
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

		// install slider threshold
		m_thresholdSlider = new Fl_Value_Slider(10, 80, 180, 20, "Threshold");
		m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_thresholdSlider->labelfont(FL_COURIER);
		m_thresholdSlider->labelsize(12);
		m_thresholdSlider->minimum(0.0);
		m_thresholdSlider->maximum(1.0);
		m_thresholdSlider->step(0.01);
		m_thresholdSlider->value(m_nThreshold);
		m_thresholdSlider->align(FL_ALIGN_RIGHT);
		m_thresholdSlider->callback(cb_thresholdSlides);

		// install button SSAA
		m_SSAAButton = new Fl_Light_Button(10, 105, 60, 20, "SSAA");
		m_SSAAButton->user_data((void*)(this));	// record self to be used by static callback functions
		m_SSAAButton->labelfont(FL_COURIER);
		m_SSAAButton->labelsize(12);
		m_SSAAButton->value(m_nSSAA);
		m_SSAAButton->callback(cb_SSAAButton);

		// install button adaptive SSAA
		m_adaptiveSSAAButton = new Fl_Light_Button(80, 105, 120, 20, "Adaptive SSAA");
		m_adaptiveSSAAButton->user_data((void*)(this));	// record self to be used by static callback functions
		m_adaptiveSSAAButton->labelfont(FL_COURIER);
		m_adaptiveSSAAButton->labelsize(12);
		m_adaptiveSSAAButton->value(m_nAdaptiveSSAA);
		m_adaptiveSSAAButton->callback(cb_adaptiveSSAAButton);

		// install button jitter
		m_jitterButton = new Fl_Light_Button(210, 105, 60, 20, "Jitter");
		m_jitterButton->user_data((void*)(this));	// record self to be used by static callback functions
		m_jitterButton->labelfont(FL_COURIER);
		m_jitterButton->labelsize(12);
		m_jitterButton->value(m_nJitter);
		m_jitterButton->callback(cb_jitterButton);
		m_jitterButton->deactivate();

		// install slider SSAA size	
		m_SSAASlider = new Fl_Value_Slider(10, 130, 180, 20, "SSAA Sample Size");
		m_SSAASlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_SSAASlider->type(FL_HOR_NICE_SLIDER);
		m_SSAASlider->labelfont(FL_COURIER);
		m_SSAASlider->labelsize(12);
		m_SSAASlider->minimum(1);
		m_SSAASlider->maximum(5);
		m_SSAASlider->step(1);
		m_SSAASlider->value(m_nSSAASize);
		m_SSAASlider->align(FL_ALIGN_RIGHT);
		m_SSAASlider->callback(cb_SSAASlides);
		m_SSAASlider->deactivate();

		m_textureMappingButton = new Fl_Light_Button(10, 155, 130, 20, "Texture Mapping");
		m_textureMappingButton->user_data((void*)(this));	// record self to be used by static callback functions
		m_textureMappingButton->labelfont(FL_COURIER);
		m_textureMappingButton->labelsize(12);
		m_textureMappingButton->value(m_nTextureMapping);
		m_textureMappingButton->deactivate();
		m_textureMappingButton->callback(cb_textureMappingButton);

		m_bumpMappingButton = new Fl_Light_Button(150, 155, 110, 20, "Bump Mapping");
		m_bumpMappingButton->user_data((void*)(this));	// record self to be used by static callback functions
		m_bumpMappingButton->labelfont(FL_COURIER);
		m_bumpMappingButton->labelsize(12);
		m_bumpMappingButton->value(m_nBumpMapping);
		m_bumpMappingButton->deactivate();
		m_bumpMappingButton->callback(cb_bumpMappingButton);

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