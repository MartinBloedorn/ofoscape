#include "ofoScenes.h"
#include "ofoUtils.h"
#include "ofoInput.h"

using namespace ofo;

Scene::Scene(int width, int height)
    : mWidth(width)
    , mHeight(height)
{
}

bool Scene::parseBuffers(const ofJson& json, const std::filesystem::path& root)
{
    auto parseChannels = [root](const ofJson& bjson, Buffer& buffer) {
        for (size_t i = 0; i < buffer.iChannels.size(); ++i) 
        {
            auto identifier = std::string("iChannel") + std::to_string(i);
            if (bjson.contains(identifier)) {
                buffer.iChannels[i].path = ofo::file::resolveRelative(bjson[identifier]["path"], root);
                buffer.iChannels[i].type = bjson[identifier]["type"] == "texture" ? 
                                                      ChannelType::Texture
                                                    : ChannelType::None;
            }
        }
    };

    try {
        for (Buffer& buffer : mBuffers) {
            if (json.contains(buffer.id)) 
            {
                buffer.path = ofo::file::resolveRelative(json[buffer.id]["path"], root);
                parseChannels(json[buffer.id], buffer);
            }
        }
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading JSON: " << e.what();
        return false;
    }

    return true;
}

bool Scene::parseVariables(const ofJson& json)
{
    // variables are optional:
    if (!json.contains("variables")) return true;

    auto parseVariable = [](const ofJson& vjson) -> Variable {
        Variable var;
        var.name = vjson["name"];
        var.source = vjson["source"];
        return var;
    };

    try {
        for (const auto& jvar : json["variables"]) {
            mVariables.push_back(std::move(parseVariable(jvar)));
        }
    }
    catch (const std::exception& e) {
        ofLogError() << "Error parsing variables: " << e.what();
        return false;
    }

    return true;
}

bool Scene::parse(const ofJson& json, const std::filesystem::path& root)
{
    try {
        mName   = json["name"];
        //mShader = json["shader"];
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading JSON: " << e.what();
        return false;
    }

    if (!parseBuffers(json, root) || !parseVariables(json))
        return false;

    ofLogNotice() << "Created scene: " << mName << "(" << mWidth << "," << mHeight << ")";
    return true;
}

std::optional<std::reference_wrapper<ofTexture>> Scene::loadTexture(const std::filesystem::path& path)
{
    if (ofImage img; img.load(path)) {
        img.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
        img.setImageType(OF_IMAGE_COLOR); // Or OF_IMAGE_COLOR_ALPHA
        mLoadedTextures.emplace_back(std::move(img));
        return std::ref(mLoadedTextures.back().getTexture());
    } 

    ofLogWarning() << "Failed to load image: " << path;
    return std::nullopt;
}

bool Scene::load()
{
    if (isLoaded()) return true;

    //mFbo.allocate(mWidth, mHeight);//, GL_RGBA);
    //if (!mFbo.isAllocated()) {
    //    ofLogError() << "Failed to allocate FBO for scene: " << mName;
    //    return false;
    //}

    // Lazy instantiating ofxShadertoy; otherwise it tries too early to tap into events.
    mShadertoy = ofxShadertoy{};

    for (const auto& buffer : mBuffers) {
        if (buffer.path.empty())
            continue;

        if (!mShadertoy->load(buffer.path.generic_string(), buffer.index)) {
            ofLogError() << "Failed to load ShaderToy engine for scene: " << mName;
            return false;
        }

        for (size_t i = 0; i < buffer.iChannels.size(); ++i) {
            auto& channel = buffer.iChannels[i];

            switch (channel.type) {
            case ChannelType::None:
                break;

            case ChannelType::Texture:
                if (auto texture = loadTexture(channel.path); texture.has_value()) {
                    mShadertoy->setTexture(static_cast<int>(i), *texture, buffer.index);
                }
                else {
                    ofLogError() << "Failed to load texture: " << channel.path;
                    return false;
                }
                break;

            case ChannelType::Keyboard:
            default:
                ofLogWarning() << "Ignoring unsupported iChannel type in scene: " << mName;
                continue;
            }
        }
    }

    mShadertoy->setAdvanceTime(true);
    mShadertoy->setDimensions(mWidth, mHeight);

    ofLogNotice() << "Loaded scene: " << mName;
    return true;
}

void Scene::unload()
{
    if (!isLoaded()) return;
    //mFbo.destroy();
    mShadertoy->unload();
    mShadertoy.reset();
}

bool ofo::Scene::isLoaded() const
{
    return mShadertoy.has_value(); // && mFbo.isAllocated();
}

void Scene::bindInputs(const std::vector<std::shared_ptr<Input>>& inputs)
{
    const auto findInput = [inputs](const std::string& name) -> std::shared_ptr<Input> {
        for (const auto& input : inputs) {
            if (input->name() == name)
                return input;
        }
        return nullptr;
    };

    for (auto& var : mVariables) {
        if (auto input = findInput(var.source))
            var.input = input;
        else
            ofLogWarning() << "For variable '" << var.name << "'; undefined source: " << var.source;
    }
}

void Scene::update()
{
    // TODO: this shouldn't be needed!
    if (isLoaded())
        mShadertoy->update();
}

void Scene::render(ofFbo& fbo)
{
    if (!isLoaded()) return;
    
    fbo.begin();
    //mShadertoy->draw(0, 0, mWidth, mHeight);
    mShadertoy->begin();
    for (auto& var : mVariables)
        if (var.input)
            mShadertoy->setUniform1f(var.name, var.input->parameter());

    ofDrawRectangle(0, 0, mWidth, mHeight);

    mShadertoy->end();
    fbo.end();
}

//ofTexture& Scene::getTexture()
//{
//    return mFbo.getTexture();
//}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// SceneManager
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Visualizer::Visualizer()
    : mGui(std::make_shared<ofxPanel>()) 
{
}

bool Visualizer::loadScene(std::shared_ptr<Scene> scene, Slot slot)
{
    if (!scene || scene->isLoaded()) 
        return false;
    if (scene == mScenes[slot])
        return true;
    if (mScenes[slot])
        mScenes[slot]->unload();

    return (mScenes[slot] = scene)->load();
}

void Visualizer::setInputs(const std::vector<std::shared_ptr<Input>>& inputs)
{
    mInputs = inputs;
}

void Visualizer::setCrossfade(float crossfade)
{
    mCrossfade = std::clamp(crossfade, 0.0f, 1.0f);
}

void Visualizer::update()
{
    for (auto& scene : mScenes)
        if (scene) scene->update();
}

void Visualizer::setup()
{
    setupOutputShader();

    for (auto& fbo : mFbos) {
        fbo.allocate(ofGetWidth(), ofGetHeight());//, GL_RGBA);
        if (!fbo.isAllocated()) {
            ofLogError() << "Failed to allocate FBO.";
        }
    }

    mGui->setup("scene");
    mGui->add(mCrossfade.set("crossfade", 0.5, 0.0, 1.0));

    for (auto input : mInputs)
        mGui->add(input->parameter());
}

void Visualizer::draw()
{
    for (size_t i = 0; i < mScenes.size(); ++i)
        if(mScenes[i]/* && mCrossfade > 0.0*/)
            mScenes[i]->render(mFbos[i]);

    mOutputShader.begin();
    mOutputShader.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 1.0);
    mOutputShader.setUniform1f("crossfade", mCrossfade);

    for (size_t i = 0; i < mScenes.size(); ++i)
        if (mScenes[i] && mScenes[i]->isLoaded())
            mOutputShader.setUniformTexture(std::string("deck") + (i ? "B" : "A"), mFbos[i].getTexture(), (int)i + 0);
    
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    mOutputShader.end();

    mGui->draw();
}

void Visualizer::shutdown()
{
    mGui->clear();
    mGui.reset();

    for (auto& scene : mScenes)
        if (scene) scene->unload();
}

void Visualizer::setupOutputShader()
{
    mOutputShader.setupShaderFromSource(
        GL_VERTEX_SHADER,
        string(
            "#version 330\n"
            "precision mediump float;"
            "uniform mat4 modelViewProjectionMatrix;"
            "uniform mat4 tCameraMatrix;"
            "in vec4 position;"
            "void main(){"
            "    gl_Position = modelViewProjectionMatrix * position;"
            "}\n"));
    mOutputShader.setupShaderFromSource(
        GL_FRAGMENT_SHADER,
        string(
            "#version 330\n"
            "precision mediump float;\n"
            "uniform sampler2D deckA;\n"
            "uniform sampler2D deckB;\n"
            "uniform float crossfade;\n"
            "uniform vec3 iResolution;\n"
            "void main() {\n"
            "    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
            "    vec4 colorA = texture(deckA, uv);\n"
            "    vec4 colorB = texture(deckB, uv);\n"
            "    vec4 finalColor = mix(colorA, colorB, crossfade);\n"
            "    gl_FragColor = finalColor;\n"
            "}\n"
            ));
        //string(
        //    "#version 330\n"
        //    "precision mediump float;\n"
        //    "uniform sampler2D deckA;\n"
        //    "uniform vec3 iResolution;\n"
        //    "void main() {\n"
        //    "    gl_FragColor = texture(slotA, gl_FragCoord.xy / iResolution.xy);\n"
        //    "}\n"
        //));
    if (!mOutputShader.linkProgram()) {
        ofLogError() << "Failed linking Visualizer's output shader.";
    }

    mOutputShader.bindDefaults();
}
