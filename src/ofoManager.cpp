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

void Manager::setMidiPort(const std::string& port)
{
    mMidiPort = port;
}

void Manager::setup()
{
    mVisualizer.setup();

    // TODO: this is annoying ... move config parsing AFTER window/context creation?
    if (!mOscReceiver.setup(mOscHost, mOscPort)) 
        ofLogError() << "Failed to setup OSC connection at " << mOscHost << "::" << mOscPort;
    else
        ofLogNotice() << "Set up OSC at " << mOscHost << "::" << mOscPort;

    // Searching for port name; built-in function won't work for some reason.
    if (mMidiPort == "")
        return;

    auto list = mMidiInput.getInPortList();
    int port = -1;

    for (int i = 0; i < list.size(); ++i) {
        if (list[i].find(mMidiPort) != std::string::npos) {
            port = i; 
            break;
        }
    }

    if (port < 0)
        return;

    if (!mMidiInput.openPort(port))
        ofLogError() << "Failed to open MIDI port: " << mMidiPort;
    else
        ofLogNotice() << " Opened MIDI port: " << mMidiPort;

    mMidiInput.setVerbose(true);
}

bool Manager::loadScene(SceneLoadDirection direction, std::optional<Visualizer::Slot> slot)
{
    if (!mLastLoadedScene) 
        return false;

    auto it = std::find_if(mScenes.begin(), mScenes.end(), [this](auto& s) {
            return s && s->name() == mLastLoadedScene->name();
        });

    // TODO: this won't work for prev - sequentially, it'll try to load a scene in the previous deck.
    it = direction == Next ? ++it : --it;

    if (it >= mScenes.end() || it < mScenes.begin()) {
        ofLogWarning() << "Unable to load " 
            << (direction == Next ? "next" : "previous")
            << " scene from: " << mLastLoadedScene->name();
        return false;
    }

    return loadScene(*it, slot.value_or(mVisualizer.crossfade() > 0.5 ? Visualizer::Slot_A 
                                                                      : Visualizer::Slot_B));
}

bool Manager::loadScene(const string& name, Visualizer::Slot slot)
{
    for (auto& scene : mScenes) {
        if (scene->name() == name) {
            return loadScene(scene, slot);
        }
    }

    ofLogWarning() << "Unable to load scene: " << name;
    return false;
}

bool Manager::loadScene(const std::shared_ptr<Scene>& scene, Visualizer::Slot slot)
{
    bool r = mVisualizer.loadScene(scene, slot);
    mLastLoadedScene = r ? scene : mLastLoadedScene;
    return r;
}

bool Manager::reloadScenes()
{
    bool result = true;

    for (auto slot : std::array<Visualizer::Slot, 2>{ Visualizer::Slot_A, Visualizer::Slot_B }) {
        auto scene = mVisualizer.loadedScene(slot);
        mVisualizer.unloadScene(slot);
        result &= mVisualizer.loadScene(scene, slot);
    }

    return result;
}

std::shared_ptr<Input> Manager::input(const std::string& name) const
{
    for (auto& input : mInputs)
        if (input->getName() == name)
            return input;
    
    return std::shared_ptr<Input>();
}

void Manager::update()
{
    mVisualizer.update();

    while (mOscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        mOscReceiver.getNextMessage(m);

        processOscMessages(m);

        for (auto& input : mInputs) {
            input->update(m);
        }
    }

    while (mMidiInput.hasWaitingMessages()) {
        ofxMidiMessage m;
        mMidiInput.getNextMessage(m);

        for (auto& input : mInputs) {
            input->update(m);
        }
    }

    for (auto& input : mInputs) {
        input->update();
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
        loadScene(name, deck ? Visualizer::Slot_B : Visualizer::Slot_A);
    }

    if (message.getAddress() == "/ofo/scene/next") {
        loadScene(SceneLoadDirection::Next);
    }
    if (message.getAddress() == "/ofo/scene/prev") {
        loadScene(SceneLoadDirection::Previous);
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
