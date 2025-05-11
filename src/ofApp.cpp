#include "ofApp.h"

#include "ofoProject.h"
#include "ofoManager.h"
#include "ofoInput.h"

#include <filesystem>

struct Listener {
    void onChanged(float& val) {
        ofLogNotice() << val;
    }
};

static Listener listener;

ofApp::ofApp(ofo::Manager&& manager)
    : mManager(manager)
{ 
    ofSetLogLevel(OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(30);
    ofDisableArbTex();

    // Actual CWD is different than reported here (bin instead of bin/data). Why?
    ofLogNotice() << "CWD: " << std::filesystem::current_path();

    mManager.setup();

    //ofParameter<double> p;
    //p.addListener(&listener, &Listener::onChanged);

    if (auto x = mManager.input("x")) {
        //x->addListener(&listener, &Listener::onChanged);
        //(*x).addListener(&listener, &Listener::onChanged);
    }
    //if (auto y = mManager.input("y")) {
    //    y->addListener(&listener, &Listener::onChanged);
    //}

    //mManager.loadScene("infrastructure");
    //mManager.loadScene("tube");
    //mManager.loadScene("alien", false);
}

//--------------------------------------------------------------
void ofApp::update()
{
    mManager.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    mManager.draw();
}

void ofApp::exit()
{
    mManager.shutdown();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key) {
    case 'f':
        ofToggleFullscreen();
        break;
    case 'r':
        mManager.reloadScenes();
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
