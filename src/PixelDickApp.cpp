#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PixelDickApp : public AppNative {
public:
    void prepareSettings(Settings* settings);
    void setup();
    void mouseDown(MouseEvent event);
    void keyDown(KeyEvent event);
    void update();
    void draw();
};

void PixelDickApp::prepareSettings(Settings* settings) {
    settings->enableHighDensityDisplay();
    settings->prepareWindow(Window::Format().fullScreenButton());
}

void PixelDickApp::setup() {
}

void PixelDickApp::mouseDown(MouseEvent event) {
}

void PixelDickApp::keyDown(KeyEvent event) {
    switch (event.getCode()) {
        case app::KeyEvent::KEY_f:
            setFullScreen(!isFullScreen());
            if (isFullScreen()) {
                hideCursor();
            } else {
                showCursor();
            }
           break;
        default:
            break;
    }
}

void PixelDickApp::update() {
}

void PixelDickApp::draw() {
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE(PixelDickApp, RendererGl)
