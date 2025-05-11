#include <string>
#include <vector>
#include <optional>

#include "ofMain.h"
#include "ofApp.h"

#include "ofxMidi.h"

#include "ofoManager.h"
#include "ofoProject.h"

struct Arguments {
    std::string project = "project.json";
    bool showMidiInputs = false;
    bool showHelp = false;
    bool verbose = false;
};

void showHelp()
{
    std::cout << "Usage: ofoscape [PROJECT] <options>\n";
    std::cout << "Load and execute a project.\n";
    std::cout << "\n";
    std::cout << "-h, --help    Display this help and exit.\n";
    std::cout << "-v            Enable verbose logging.\n";
    std::cout << "--listMidi    Print available MIDI input ports before starting.\n";
}

Arguments parseArguments(int argc, char* argv[])
{
    Arguments args;
    std::vector<std::string> list;

    // start at 1 and skip own binary name
    for (int i = 1; i < argc; ++i)
        list.emplace_back(argv[i]);

    if (!list.size()) {
        ofLogWarning() << "No project provided; using default path: " << args.project;
        return args;
    }

    auto has = [&list](std::string key) {
        return std::find(list.begin(), list.end(), key) != list.end();
    };

    args.project = list[0];
    args.verbose = has("-v");
    args.showHelp = has("-h") || has("--help");
    args.showMidiInputs = has("--listMidi");

    return args;
}

int main(int argc, char* argv[])
{
    auto args = parseArguments(argc, argv);

    if (args.showHelp) {
        showHelp();
        return 0;
    }
    if (args.verbose) {
        ofSetLogLevel(OF_LOG_VERBOSE);
    }

    // Disable automatically resolving to bin/data path
    ofDisableDataPath();

    ofo::Project project;
    ofo::Manager manager;

    if (args.showMidiInputs) {
        ofxMidiIn().listInPorts();
    }
    if (!project.load(manager, args.project)) {
        ofLogError() << "Failed to load: " << args.project;
        return 1;
    }

    //Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 6);
    settings.setSize(project.getConfiguredResolution().first, project.getConfiguredResolution().second);
    settings.windowMode = OF_WINDOW;

    auto window = ofCreateWindow(settings);

    // Calls a patched function, that includes a call to ofExitCallback() present 
    // in its other versions, as well as the int-return value.
    return ofRunApp(window, make_shared<ofApp>(std::move(manager)));
}
