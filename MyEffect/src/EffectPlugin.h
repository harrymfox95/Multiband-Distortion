//
//  EffectPlugin.h
//  MyEffect Plugin Header File
//
//  Used to declare objects and data structures used by the plugin.
//

#pragma once

#include "apdi/Plugin.h"
#include "apdi/Helpers.h"
using namespace APDI;

#include "EffectExtra.h"

//#define ARRAY_SIZE 100000

class MyEffect : public APDI::Effect
{
public:
    MyEffect(const Parameters& parameters, const Presets& presets); // constructor (initialise variables, etc.)
    ~MyEffect();                                                    // destructor (clean up, free memory, etc.)

    void setSampleRate(float sampleRate){ stk::Stk::setSampleRate(sampleRate); }
    float getSampleRate() const { return stk::Stk::sampleRate(); };
    
    void process(const float** inputBuffers, float** outputBuffers, int numSamples);
    
    void presetLoaded(int iPresetNum, const char *sPresetName);
    void optionChanged(int iOptionMenu, int iItem);
    void buttonPressed(int iButton);
    void handleParameters();

private:
    // Declare shared member variables here
    float fInGain;
    float fOutGain;
    float fCutoff;
    float fCutoffHigh;
    float fCutoffLow;
    float fSignal0;
    float fSignal1;
    float fGainLow;
    float fGainMid;
    float fGainHigh;
    int iDistortType;
    LPF filter0;
    LPF filter1;
};
