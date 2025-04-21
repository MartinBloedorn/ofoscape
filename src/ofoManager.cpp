#include "ofoManager.h"
#include "ofoInput.h"
#include "ofoScenes.h"

using namespace ofo;

Manager::Manager()
{
}

void Manager::setWindowSize(int width, int height)
{
    mWindowSize = std::make_pair(width, height);
}

void Manager::setInputs(const std::vector<std::shared_ptr<Input>>& inputs)
{
    mInputs = inputs;
    mVisualizer.setInputs(inputs);
}

void Manager::setScenes(const std::vector<std::shared_ptr<Scene>>& scenes)
{
    mScenes = scenes;

    for (auto& scene : mScenes)
        scene->bindInputs(mInputs);
}

void Manager::setOscSettings(const std::string& host, int port)
{
    mOscHost = host;
    mOscPort = port;
}

void Manager::setup()
{
    mVisualizer.setup();

    // TODO: this is annoying ... move config parsing AFTER window/context creation?
    if (!mOscReceiver.setup(mOscHost, mOscPort)) 
        ofLogError() << "Failed to setup OSC connection at " << mOscHost << "::" << mOscPort;
    else
        ofLogNotice() << "Set up OSC at " << mOscHost << "::" << mOscPort;
}

bool Manager::loadScene(const string& name, bool slotA)
{
    for (auto& scene : mScenes) {
        if (scene->name() == name) {
            return mVisualizer.loadScene(scene, slotA ? Visualizer::Slot_A : Visualizer::Slot_B);
        }
    }

    ofLogWarning() << "Unable to load scene: " << name;
    return false;
}

std::shared_ptr<Input> Manager::input(const std::string& name) const
{
    for (auto& input : mInputs)
        if (input->name() == name)
            return input;
    
    return std::shared_ptr<Input>();
}

void Manager::update()
{
    mVisualizer.update();

    //if (!mOscReceiver.isListening()) return;

    while (mOscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        mOscReceiver.getNextMessage(m);

        processOscMessages(m);

        for (auto& input : mInputs) {
            input->update(m);
        }
    }
}

void Manager::draw()
{
    mVisualizer.draw();
}

void Manager::shutdown()
{
    mVisualizer.shutdown();

    for (auto& scene : mScenes)
        scene->unload();
}

void Manager::processOscMessages(const ofxOscMessage& message)
{
    if (message.getAddress() == "/ofo/scene/crossfade" && message.getNumArgs() == 1) {
        mVisualizer.setCrossfade(message.getArgAsFloat(0));
        return;
    }

    if (message.getAddress() == "/ofo/scene/load" && message.getNumArgs() == 2) {
        auto name = message.getArgAsString(0);
        auto deck = message.getArgAsInt(1);
        mVisualizer.loadScene(getScene(name), deck ? Visualizer::Slot_B : Visualizer::Slot_A);
    }
}

std::shared_ptr<Scene> ofo::Manager::getScene(const std::string& name)
{
    for (auto& scene : mScenes) {
        if (scene->name() == name) {
            return scene;
        }
    }

    return std::shared_ptr<Scene>();
}
