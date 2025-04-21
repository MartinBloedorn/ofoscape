#pragma once

#include "ofxOsc.h"
#include "ofJson.h"

#include <string>

#define OFO_INPUT_EPS_F 0.00001f

namespace ofo
{

class Input
{
public:
    Input();

    bool parse(const ofJson& json);
    void update(const ofxOscMessage& message);

    ofParameter<float> parameter() const {
        return mParam;
    }
    std::string name() const {
        return mParam.getName();
    }
    template<class ListenerClass, typename ListenerMethod>
    void addListener(ListenerClass* listener, ListenerMethod method, int prio = OF_EVENT_ORDER_AFTER_APP) {
        mParam.addListener(listener, method, prio);
    }
    template<class ListenerClass, typename ListenerMethod>
    void removeListener(ListenerClass* listener, ListenerMethod method, int prio = OF_EVENT_ORDER_AFTER_APP) {
        mParam.removeListener(listener, method, prio);
    }

private:
    ofParameter<float> mParam;

    struct {
        std::string path;
        int index = -1;
    } mOsc;

    struct MappingRange {
        float inLo = 0.0, inHi = 1.0, outLo = 0.0, outHi = 1.0;
    } mRange;

    struct {
        float old;
        float k = 0.0;
    } mSmoothing;    
};

}