#pragma once

#include "ofJson.h"

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace ofo {

class Input;
class Manager;

class Project {
public:
    Project() = default;

    bool load(Manager& manager, const of::filesystem::path& filename);
    std::pair<int, int> getConfiguredResolution() const;

private:
    bool loadProjectSettings(Manager& manager, ofJson& json);
    bool loadInputs(Manager& manager, ofJson& json);
    bool loadScenes(Manager& manager, ofJson& json, const std::filesystem::path& root);

    std::pair<int, int> mConfiguredResolution;
};

}