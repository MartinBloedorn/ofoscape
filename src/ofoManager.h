#pragma once

#include "ofxOsc.h"
#include "ofxMidi.h"
#include "ofoScenes.h"
#include "ofxShadertoy.h"

#include <memory>
#include <vector>

namespace ofo
{

class Input;
class Scene;
class Visualizer;

class Manager
{
public:
    Manager();

    void setWindowSize(int width, int height);
    void setInputs(const std::vector<std::shared_ptr<Input>>& inputs);
    void setScenes(const std::vector<std::shared_ptr<Scene>>& scenes);

    void setOscSettings(const std::string& host, int port);
    void setMidiPort(const std::string& port);
    
    enum SceneLoadDirection {
        Next,
        Previous
    };
    bool loadScene(SceneLoadDirection direction, std::optional<Visualizer::Slot> slot = std::nullopt);
    bool loadScene(const string& name, Visualizer::Slot slot);
    bool loadScene(const std::shared_ptr<Scene>& scene, Visualizer::Slot slot);
    bool reloadScenes();

    std::shared_ptr<Input> input(const std::string& name) const;

    void setup();
    void update();
    void draw();
    void shutdown();

private:
    std::pair<int, int> mWindowSize{ 0 ,0 };

    std::vector<std::shared_ptr<Input>> mInputs;
    std::vector<std::shared_ptr<Scene>> mScenes;

    std::shared_ptr<Scene> mLastLoadedScene;

    std::string mOscHost;
    int mOscPort = -1;

    std::string mMidiPort;

    Visualizer mVisualizer;

    ofxOscReceiver mOscReceiver;
    ofxMidiIn mMidiInput;

    void processOscMessages(const ofxOscMessage& message);
    std::shared_ptr<Scene> getScene(const std::string& name);
};

}
