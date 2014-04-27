
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Timeline.h"
#include "cinder/audio/Output.h"

#include <sstream>
#include <boost/format.hpp>

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

enum DisplayState {
    White = 0,
    Grid,
    Black,
    Gray,
    Red,
    Green,
    Blue,
};

class PixelDickApp : public AppNative {
public:
    void prepareSettings(Settings* settings);
    void setup();
    void update();
    void draw();
    void keyDown(KeyEvent event);

private:
    void setDisplayState(int state);
    gl::Texture createGridTexture(int stride, Color color, Color colorAlt);
    void cueHit();
	void displayChange();

    int mDisplayState;
    Color mColor;
	audio::SourceRef mAudioSource;
    gl::Texture mCheckerBoardTexture;
    ci::TimelineRef mTimeline;
    gl::TextureFontRef mFont;
    double mGridEpoch;
};

void PixelDickApp::prepareSettings(Settings* settings) {
    settings->enableHighDensityDisplay();
    settings->prepareWindow(Window::Format().fullScreenButton());
}

void PixelDickApp::setup() {
	mFont = gl::TextureFont::create(Font("Helvetica", 64 * 2), gl::TextureFont::Format().enableMipmapping());
    mAudioSource = audio::load(loadResource(RES_BLOOP));

    // tickle to create grid texture
    displayChange();

    getWindow()->getSignalDisplayChange().connect(std::bind(&PixelDickApp::displayChange, this));

    setDisplayState(DisplayState::White);
}

void PixelDickApp::update() {}

void PixelDickApp::draw() {
    if (!mCheckerBoardTexture) {
        return;
    }

    if (mDisplayState == DisplayState::Grid) {
        gl::color(Color::white());

        gl::draw(mCheckerBoardTexture, getWindowBounds(), getWindowBounds());

        // countdown
        double remainingSeconds = (4 * 60.0) - (getElapsedSeconds() - mGridEpoch);
        int minutes = (int)floor(remainingSeconds / 60.0f);
        int seconds = (int)floor(remainingSeconds) % 60;

        stringstream ss;
        ss << boost::format("%02d:%02d") % minutes % seconds;

        string s = ss.str();
        Vec2f stringSize = mFont->measureString(s) * 0.5f;

        gl::enableAlphaBlending();
            gl::color(1.0f, 0.125f, 1.0f, 1.0f);
            mFont->drawString(s, Vec2f((getWindowWidth() - stringSize.x) / 2.0f, stringSize.y + 12), gl::TextureFont::DrawOptions().scale(0.5f).pixelSnap(false));
        gl::disableAlphaBlending();
    } else {
        gl::color(mColor);
        gl::drawSolidRect(Rectf(getWindowBounds()));

        if (mDisplayState == DisplayState::White) {
            // TODO - instructions
        }
    }
}

void PixelDickApp::keyDown(KeyEvent event) {
    switch (event.getCode()) {
        case app::KeyEvent::KEY_RIGHT: {
            int state = mDisplayState < DisplayState::Blue ? mDisplayState+1 : DisplayState::White;
            setDisplayState(state);
            break;
        }
        case app::KeyEvent::KEY_LEFT: {
            int state = mDisplayState > DisplayState::White ? mDisplayState-1 : DisplayState::Blue;
            setDisplayState(state);
            break;
        }
        case app::KeyEvent::KEY_f:
            setFullScreen(!isFullScreen());
            if (isFullScreen()) {
                hideCursor();
            } else {
                showCursor();
            }
            break;
        case app::KeyEvent::KEY_ESCAPE:
            quit();
            break;
        default:
            break;
    }
}

#pragma mark - PRIVATE

void PixelDickApp::setDisplayState(int state) {
    // from
    switch (mDisplayState) {
        case DisplayState::Grid:
            // NB - this crashes
//            mTimeline->clear();
            timeline().remove(mTimeline);
            mTimeline = NULL;
            break;
        default:
            break;
    }

    // to
    switch (state) {
        case DisplayState::White:
            mColor = Color(1, 1, 1);
            break;
        case DisplayState::Grid:
            mTimeline = Timeline::create();
            timeline().add(mTimeline);
            mTimeline->add(boost::bind(&PixelDickApp::cueHit, this), mTimeline->getCurrentTime() + 4 * 60);
            mGridEpoch = getElapsedSeconds();
            break;
        case DisplayState::Black:
            mColor = Color(0, 0, 0);
            break;
        case DisplayState::Gray:
            mColor = Color::gray(0.5f);
            break;
        case DisplayState::Red:
            mColor = Color(1, 0, 0);
            break;
        case DisplayState::Green:
            mColor = Color(0, 1, 0);
            break;
        case DisplayState::Blue:
            mColor = Color(0, 0, 1);
            break;
        default:
            break;
    }

    mDisplayState = state;
}

gl::Texture PixelDickApp::createGridTexture(int stride, Color color, Color colorAlt) {
    Surface8u surface = Surface8u(stride * 2, stride * 2, false);

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

void PixelDickApp::cueHit() {
    audio::Output::play(mAudioSource);
    setDisplayState(mDisplayState + 1);
}

void PixelDickApp::displayChange() {
    // use size in points, density independent
    int stride = 60 * getWindowContentScale();
    mCheckerBoardTexture = createGridTexture(stride, Color::black(), Color::white());
    mCheckerBoardTexture.setWrap(GL_REPEAT, GL_REPEAT);
}

CINDER_APP_NATIVE(PixelDickApp, RendererGl)
