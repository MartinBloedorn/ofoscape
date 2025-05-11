#include "ofoProject.h"
#include "ofoInput.h"
#include "ofoScenes.h"
#include "ofoManager.h"

using namespace ofo;

bool Project::load(Manager& manager, const of::filesystem::path& filename)
{
    ofLogNotice() << "Loading project: " << filename;
    auto json = ofLoadJson(filename);
    
    // TODO: this doesn't seem to catch parsing errors.
    if (json.empty()) {
        ofLogError() << "Unable to load project JSON: " << filename;
        return false;
    }

    if (!loadProjectSettings(manager, json)) {
        ofLogError() << "loadProjectSettings failed.";
        return false;
    }
    if (!loadInputs(manager, json)) {
        ofLogError() << "loadInputs failed.";
        return false;
    }
    if (!loadScenes(manager, json, filename)) {
        ofLogError() << "loadScenes failed.";
        return false;
    }

    return true;
}

std::pair<int, int> Project::getConfiguredResolution() const
{
    return mConfiguredResolution;
}

bool Project::loadProjectSettings(Manager& manager, ofJson& json)
{
    std::string host, midi;
    int port;

    try {
        int hres = json["resolution"][0];
        int vres = json["resolution"][1];
        manager.setWindowSize(hres, vres);
        mConfiguredResolution = std::make_pair(hres, vres);

        host = json["io"]["osc"]["host"];
        port = json["io"]["osc"]["port"];

        midi = json["io"]["midi"]["port"];
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading JSON: " << e.what();
        return false;
    }

    manager.setOscSettings(host, port);
    manager.setMidiPort(midi);

    return true;
}

bool Project::loadInputs(Manager& manager, ofJson& json)
{
    std::vector<std::shared_ptr<Input>> inputs;
    bool ok = true;

    for (auto& [key, value] : json["inputs"].items()) 
    {
        inputs.push_back(std::make_shared<Input>());
        if (!(ok = inputs.back()->setup(value))) break;
    }

    if (ok) manager.setInputs(inputs);
    return ok;
}

bool Project::loadScenes(Manager& manager, ofJson& json, const std::filesystem::path& root)
{
    std::vector<std::shared_ptr<Scene>> scenes;
    bool ok = true;

    std::optional<std::reference_wrapper<const ofJson>> macros;
    if (json.contains("macros")) {
        macros.emplace(std::cref(json["macros"]));
    }

    for (auto& [key, value] : json["scenes"].items())
    {
        scenes.push_back(std::make_shared<Scene>(mConfiguredResolution.first, mConfiguredResolution.second));
        if (!(ok = scenes.back()->setup(value, macros, root))) break;
    }

    if (ok) manager.setScenes(scenes);
    return ok;
}
