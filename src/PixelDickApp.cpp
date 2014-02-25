
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PixelDickApp : public AppNative {
public:
    void prepareSettings(Settings* settings);
    void setup();
    void update();
    void draw();
    void mouseDown(MouseEvent event);
    void keyDown(KeyEvent event);

private:
    gl::Texture createGridTexture(int stride, Color color, Color colorAlt);

    gl::Texture mCheckerBoardTexture;
};

void PixelDickApp::prepareSettings(Settings* settings) {
    settings->enableHighDensityDisplay();
    settings->prepareWindow(Window::Format().fullScreenButton());
    // TODO - go fullscreen?
}

void PixelDickApp::setup() {
    // use fixed physical size (density independent)
    int stride = 60 * getWindowContentScale();
    mCheckerBoardTexture = createGridTexture(stride, Color::black(), Color::white());
}

void PixelDickApp::update() {
}

void PixelDickApp::draw() {
    if (!mCheckerBoardTexture) {
        return;
    }

    gl::clear(Color::black());
    gl::draw(mCheckerBoardTexture, getDisplay()->getBounds());
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

#pragma mark - PRIVATE

gl::Texture PixelDickApp::createGridTexture(int stride, Color color, Color colorAlt) {
    Area scaledDisplayBounds = Area(Vec2f(0.0f, 0.0f), Vec2i(getDisplay()->getWidth() * getWindowContentScale(), getDisplay()->getHeight() * getWindowContentScale()));
    Surface8u surface = Surface8u(scaledDisplayBounds.getWidth(), scaledDisplayBounds.getHeight(), false);

    Surface::Iter outputIter(surface.getIter());
    while(outputIter.line()) {
        while(outputIter.pixel()) {
            Vec2i position = outputIter.getPos();
            int y = ((int)floor(position.y / stride)) % 2;
            int x = ((int)floor((position.x + y * stride) / stride)) % 2;
            Color c = x ? color : colorAlt;
            outputIter.r() = c.r * 255;
            outputIter.g() = c.g * 255;
            outputIter.b() = c.b * 255;
        }
    }

    return gl::Texture(surface);
}

CINDER_APP_NATIVE(PixelDickApp, RendererGl)
