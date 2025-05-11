#pragma once

#include "ofJson.h"
#include "ofxOsc.h"
#include "ofxMidi.h"

#include "ofoFilters.h"

#include <string>

#define OFO_INPUT_EPS_F 0.00001f

namespace ofo
{

// TODO: there seems to be an OF build issue with addListener when using ofParameter<double>
class Value : public ofParameter<float>
{
    std::optional<FilterIirSimple> mlpf, mhpf;

    struct MappingRange {
        double inLo = 0.0;
        double inHi = 1.0;
        double outLo = 0.0;
        double outHi = 1.0;
    } mRange;

    double mSetpoint = 0.0;

public:
    virtual bool setup(const ofJson& json);

    void update(double val);
    void update();

    int getAsInt() {
        return static_cast<int>(round(get()));
    }
};

class Input : public Value
{
    struct OscSettings {
        std::string path;
        int         index = -1;
    };

    struct MidiSettings {
        int channel = -1;
        int cc = -1;
    };

    std::optional<OscSettings>  mOsc;
    std::optional<MidiSettings> mMidi;

public:
    bool setup(const ofJson& json) override;

    using Value::update;
    void update(const ofxOscMessage& message);
    void update(const ofxMidiMessage& message);

    bool isOscInput() const {
        return mOsc.has_value();
    }
};

}
