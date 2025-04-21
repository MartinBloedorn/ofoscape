#pragma once

#include "ofxOsc.h"
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

    // TEMP: remove this slot bool, I guess
    bool loadScene(const string& name, bool slotA = true);

    std::shared_ptr<Input> input(const std::string& name) const;

    void setup();
    void update();
    void draw();
    void shutdown();

private:
    std::pair<int, int> mWindowSize{ 0 ,0 };

    std::vector<std::shared_ptr<Input>> mInputs;
    std::vector<std::shared_ptr<Scene>> mScenes;

    std::string mOscHost;
    int mOscPort = -1;

    Visualizer mVisualizer;

    ofxOscReceiver mOscReceiver;

    void processOscMessages(const ofxOscMessage& message);
    std::shared_ptr<Scene> getScene(const std::string& name);
};

}