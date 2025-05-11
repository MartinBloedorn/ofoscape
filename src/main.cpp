#include "ofMain.h"
#include "ofApp.h"

#include "ofxMidi.h"

#include "ofoManager.h"
#include "ofoProject.h"

//========================================================================
int main( )
{
    ofo::Project project;
    ofo::Manager manager;

    // Disable automatically resolving to bin/data path
    ofDisableDataPath();

    if (!project.load(manager, "../data/project.json")) {
        ofLogError() << "Failed to load project.";
        return 1;
    }

    ofxMidiIn().listInPorts();

    //Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 6);
    settings.setSize(project.getConfiguredResolution().first, project.getConfiguredResolution().second);
    settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

    auto window = ofCreateWindow(settings);

    // Calls a patched function, that includes a call to ofExitCallback() present 
    // in its other versions, as well as the int-return value.
    return ofRunApp(window, make_shared<ofApp>(std::move(manager)));
}
