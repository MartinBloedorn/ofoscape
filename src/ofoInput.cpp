#include "ofoInput.h"

using namespace ofo;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

bool Value::setup(const ofJson& json)
{
    try {
        std::string name = json["name"];

        if (json.contains("mapping") && json["mapping"].size() == 4) {
            auto m = json["mapping"];
            mRange.inLo  = m[0];
            mRange.inHi  = m[1];
            mRange.outLo = m[2];
            mRange.outHi = m[3];
        }

        if (json.contains("lpf")) {
            mlpf = FilterIirSimple(json["lpf"], FilterIirSimple::LowPass_1OD);
        }
        if (json.contains("hpf")) {
            mhpf = FilterIirSimple(json["hpf"], FilterIirSimple::HighPass_1OD);
        }

        setMin(mRange.outLo);
        setMax(mRange.outHi);
        setName(name);
        set(mRange.outLo);
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading value " << getName() << ": " << e.what();
        return false;
    }

    return true;
}

void Value::update(double val)
{
    mSetpoint = val;

    if (mlpf) {
        val = mlpf->filter(val);
    }
    if (mhpf) {
        val = mhpf->filter(val);
    }

    set(ofMap(val, mRange.inLo, mRange.inHi, mRange.outLo, mRange.outHi, true));
}

void Value::update()
{
    update(mSetpoint);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

bool Input::setup(const ofJson& json)
{
    if (!Value::setup(json))
        return false;

    try {
        auto source = json["source"];

        // OSC:
        if (source.contains("path")) {
            mOsc = OscSettings{};
            mOsc->path  = source["path"];
            mOsc->index = source["index"];
        }
        // MIDI:
        else if (source.contains("channel")) {
            mMidi = MidiSettings{};
            mMidi->channel = std::clamp(int(source["channel"]), 1, 16);
            mMidi->cc      = std::clamp(int(source["cc"]), 0, 127);
        }
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading variable " << getName() << ": " << e.what();
        return false;
    }

    return mOsc.has_value() != mMidi.has_value();
}

void Input::update(const ofxOscMessage& message)
{
    if (!mOsc) return;

    if (mOsc->index < message.getNumArgs() 
        && message.getAddress() == mOsc->path) {
        Value::update(message.getArgAsDouble(mOsc->index));
    }
}

void Input::update(const ofxMidiMessage& message)
{
    if (!mMidi) return;

    if (message.status == MIDI_CONTROL_CHANGE
        && message.channel == mMidi->channel
        && message.control == mMidi->cc) {
        Value::update(message.value);
    }
}
