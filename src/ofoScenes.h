#pragma once

#include <optional>
#include <filesystem>

#include "ofxGui.h"
#include "ofJson.h"
#include "ofFbo.h"
#include "ofTexture.h"
#include "ofxShadertoy.h"

namespace ofo
{

class Input;

class Scene
{
public:
    Scene(int width = 0, int height = 0);

    bool setup(const ofJson& json, std::optional<std::reference_wrapper<const ofJson>> macros, const std::filesystem::path& root);

    bool load();
    void unload();
    bool isLoaded() const;

    const std::string& name() const {
        return mName;
    }

    void bindInputs(const std::vector<std::shared_ptr<Input>>& inputs);

    void update();
    void render(ofFbo& fbo);

private:
    std::string mName;
    int mWidth  = 0;
    int mHeight = 0;

    enum class ChannelType {
        None,
        Texture,
        Keyboard,
    };

    struct Channel {
        ChannelType           type = ChannelType::None;
        std::filesystem::path path;
    };

    struct Buffer {
        const std::string           id;
        const ofxShadertoy::Buffer  index;
        std::filesystem::path       path;
        std::array<Channel, 4>      iChannels;
    };

    std::array<Buffer, 6> mBuffers{
        Buffer{"image",     ofxShadertoy::Buffer::Image},
        Buffer{"bufferA",   ofxShadertoy::Buffer::BufferA},
        Buffer{"bufferB",   ofxShadertoy::Buffer::BufferB},
        Buffer{"bufferC",   ofxShadertoy::Buffer::BufferC},
        Buffer{"bufferD",   ofxShadertoy::Buffer::BufferD},
        Buffer{"sound",     ofxShadertoy::Buffer::Sound},
    };

    struct Variable
    {
        std::string name;
        std::string source;
        std::shared_ptr<Input> input;
    };

    std::vector<ofImage> mLoadedTextures;

    std::vector<Variable> mVariables;

    std::optional<ofxShadertoy> mShadertoy;

    bool parseBuffers(const ofJson& json, const std::filesystem::path& root);
    bool parseVariables(const ofJson& list);

    std::optional<std::reference_wrapper<ofTexture>> loadTexture(const std::filesystem::path& path);
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Visualizer
{
public:
    Visualizer();

    enum Slot {
        Slot_A = 0,
        Slot_B = 1
    };

    bool loadScene(std::shared_ptr<Scene> scene, Slot slot);
    void unloadScene(Slot slot);
    std::shared_ptr<Scene> loadedScene(Slot slot);
    
    void setInputs(const std::vector<std::shared_ptr<Input>>& inputs);
    
    void setCrossfade(float crossfade);
    float crossfade() const { return mCrossfade; }

    void update();
    void setup();
    void draw();
    void shutdown();

private:
    void setupOutputShader();

    // Heap-alloc needed: ofxPanel can't be copied or moved.
    std::shared_ptr<ofxPanel> mGui;

    ofParameter<float> mCrossfade{ 0.0 };
    std::vector<std::shared_ptr<Input>> mInputs;

    ofShader mOutputShader;

    std::array<std::shared_ptr<Scene>, 2> mScenes;
    std::array<ofFbo, 2> mFbos;
};

}
