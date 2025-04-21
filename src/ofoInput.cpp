#include "ofoInput.h"

using namespace ofo;

Input::Input()
{
}

bool Input::parse(const ofJson& json)
{
    if (!json.contains("source") || !json.contains("name")) {
        ofLogError() << "Variable is incorrectly defined.";
        return false;
    }

    try {
        std::string name = json["name"];
        mOsc.path = json["source"]["path"];
        mOsc.index = json["source"]["index"];

        if (json.contains("mapping") && json["mapping"].size() == 4) {
            auto m = json["mapping"];
            mRange.inLo = m[0];
            mRange.inHi = m[1];
            mRange.outLo = m[2];
            mRange.outHi = m[3];
        }

        mSmoothing.k = std::clamp(json.value("smoothing", 0.0), 0.0, 0.99);

        mParam.setMin(mRange.outLo);
        mParam.setMax(mRange.outHi);
        mParam.setName(name);
        mParam = mRange.outLo;
    }
    catch (const std::exception& e) {
        ofLogError() << "Error reading variable " << name() << ": " << e.what();
        return false;
    }

    ofLogNotice() << "Loaded variable: " << name() //mParam.getName()
                  << " {" << mOsc.path << ", " << mOsc.index << "}"
                  << " [" << mRange.inLo  << ", " << mRange.inHi << ", "
                          << mRange.outLo << ", " << mRange.outHi << "]";

    return true;
}

void Input::update(const ofxOscMessage& message)
{
    if (mOsc.index < 0) return;

    const auto map = [this](float val) -> float {
        float rin = mRange.inHi - mRange.inLo;
        float rout = mRange.outHi - mRange.outLo;
        if (fabs(rin) < OFO_INPUT_EPS_F)
            return 1.0;

        return ((val - mRange.inLo) * rout) / rin + mRange.outLo;
    };

    // TODO: this lpf needs to be called continuously from an update method.
    const auto lp = [this](float val) {
        val = (1.0f - mSmoothing.k) * val + mSmoothing.k * mSmoothing.old;
        mSmoothing.old = val;
        return val;
    };

    if (mOsc.index < message.getNumArgs() && message.getAddress() == mOsc.path) {
        float val = message.getArgAsFloat(mOsc.index);
        mParam = lp(map(val));
    }
}

