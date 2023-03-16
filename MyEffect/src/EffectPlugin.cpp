//
//  EffectPlugin.cpp
//  MyEffect Plugin Source Code
//
//  Used to define the bodies of functions used by the plugin, as declared in EffectPlugin.h.
//

#include "EffectPlugin.h"

////////////////////////////////////////////////////////////////////////////
// EFFECT - represents the whole effect plugin
////////////////////////////////////////////////////////////////////////////

// Called to create the effect (used to add your effect to the host plugin)
extern "C" {
    CREATE_FUNCTION createEffect(float sampleRate) {
        ::stk::Stk::setSampleRate(sampleRate);
        
        //==========================================================================
        // CONTROLS - Use this array to completely specify your UI
        // - tells the system what parameters you want, and how they are controlled
        // - add or remove parameters by adding or removing entries from the list
        // - each control should have an expressive label / caption
        // - controls can be of different types: ROTARY, BUTTON, TOGGLE, SLIDER, or MENU (see definitions)
        // - for rotary and linear sliders, you can set the range of values (make sure the initial value is inside the range)
        // - for menus, replace the three numeric values with a single array of option strings: e.g. { "one", "two", "three" }
        // - by default, the controls are laid out in a grid, but you can also move and size them manually
        //   i.e. replace AUTO_SIZE with { 50,50,100,100 } to place a 100x100 control at (50,50)
        
        const Parameters CONTROLS = {
            //  name,       type,              min, max, initial, size
            {   "Distortion Type",  Parameter::MENU, {"Hard Clip", "Soft Clip", "Quarter Circle", "Asymmetric"},{ 10,30,75,75 } },
            {   "Out Gain",  Parameter::ROTARY, 0.0, 0.5, 0.0, { 275,90,102,102 }   },
            {   "Low/Mid Split",  Parameter::ROTARY, 50.0, 300.0, 300.0, { 90,30,75,75 } },
            {   "Mid/High Split",  Parameter::ROTARY, 300.0, 20000.0, 20000.0, { 170,30,75,75 }},
            {   "Low Gain",  Parameter::ROTARY, 0.0, 10.0, 0.0, { 10,150,75,75 } },
            {   "Mid Gain",  Parameter::ROTARY, 0.0, 10.0, 0.0,  { 90,150,75,75 } },
            {   "High Gain",  Parameter::ROTARY, 0.0, 10.0, 0.0,  { 170,150,75,75 } },
//            {   "Param 8",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
//            {   "Param 9",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            };

        const Presets PRESETS = {
            { "Extreme Sports", { 0, 0.5, 150, 8000, 10, 10, 10, 0, 0, 0 } },
            { "Warm Strings", { 1, 0.2, 200, 8500, 5.5, 8, 7, 0, 0, 0 } },
            { "Drum Bus", { 2, 0.3, 150, 5500, 3,5, 7, 6.5, 0, 0, 0 } },
        };

        return (APDI::Effect*)new MyEffect(CONTROLS, PRESETS);
    }
}

// Constructor: called when the effect is first created / loaded
MyEffect::MyEffect(const Parameters& parameters, const Presets& presets)
: Effect(parameters, presets)
{
    // Initialise member variables, etc.
        
}



void MyEffect::handleParameters()
{
    
    iDistortType = parameters[0];
    fOutGain = parameters[1] * parameters[1] * parameters[1];
    fCutoffLow = parameters[2];
    fCutoffHigh = parameters[3];
    fGainLow = parameters[4] * parameters[4] * parameters[4];
    fGainMid = parameters[5] * parameters[5] * parameters[5];
    fGainHigh = parameters[6] * parameters[6] * parameters[6];
    
}

// Destructor: called when the effect is terminated / unloaded
MyEffect::~MyEffect()
{
    // Put your own additional clean up code here (e.g. free memory)
}

// EVENT HANDLERS: handle different user input (button presses, preset selection, drop menus)

void MyEffect::presetLoaded(int iPresetNum, const char *sPresetName)
{
    // A preset has been loaded, so you could perform setup, such as retrieving parameter values
    // using getParameter and use them to set state variables in the plugin
}

void MyEffect::optionChanged(int iOptionMenu, int iItem)
{
    // An option menu, with index iOptionMenu, has been changed to the entry, iItem
}

void MyEffect::buttonPressed(int iButton)
{
    // A button, with index iButton, has been pressed
}

// Applies audio processing to a buffer of audio
// (inputBuffer contains the input audio, and processed samples should be stored in outputBuffer)
void MyEffect::process(const float** inputBuffers, float** outputBuffers, int numSamples)
{
    float fIn0, fIn1, fOut0 = 0, fOut1 = 0, fOutLow0 = 0, fOutLow1 = 0, fOutMid0 = 0, fOutMid1 = 0, fOutHigh0 = 0, fOutHigh1 = 0, fOutFinalLow0 = 0, fOutFinalLow1 = 0, fOutFinalMid0 = 0, fOutFinalMid1 = 0, fOutFinalHigh0 = 0, fOutFinalHigh1 = 0;
    const float *pfInBuffer0 = inputBuffers[0], *pfInBuffer1 = inputBuffers[1];
    float *pfOutBuffer0 = outputBuffers[0], *pfOutBuffer1 = outputBuffers[1];
    
    handleParameters();
    filter0.setCutoff(fCutoffLow);
    filter1.setCutoff(fCutoffHigh);

    while(numSamples--)
    {
        
        // Get sample from input
        fIn0 = *pfInBuffer0++;
        fIn1 = *pfInBuffer1++;
        fSignal0 = fIn0;
        fSignal1 = fIn1;
        
        // Add your effect processing here
        
        if (iDistortType == 0) //Hard
        {
            //Left Channel
            fOutLow0 = filter0.tick(fSignal0) * fGainLow;
            fOutFinalLow0 = hardTransferFn(fOutLow0) ;

            fOutMid0 = filter1.tick((fSignal0 - filter0.tick(fSignal0))) * fGainMid;
            fOutFinalMid0 = hardTransferFn(fOutMid0) ;

            fOutHigh0 =(fSignal0 - filter0.tick(fSignal0) - filter1.tick((fSignal0 - filter0.tick(fSignal0)))) * fGainHigh;
            fOutFinalHigh0 = hardTransferFn(fOutHigh0) ;

            fOut0 = (fOutFinalLow0 + fOutFinalMid0 + fOutFinalHigh0) * fOutGain;
            
            //Right Channel
            fOutLow1 = filter0.tick(fSignal1) * fGainLow;
            fOutFinalLow1 =   hardTransferFn(fOutLow1) ;

            fOutMid1 = filter1.tick((fSignal1 - filter0.tick(fSignal1))) * fGainMid;
            fOutFinalMid1 = hardTransferFn(fOutMid1) ;

            fOutHigh1 =(fSignal1 - filter0.tick(fSignal1) - filter1.tick((fSignal1 - filter0.tick(fSignal1)))) * fGainHigh;
            fOutFinalHigh1 = hardTransferFn(fOutHigh1) ;

            fOut1 = (fOutFinalLow1 + fOutFinalMid1 + fOutFinalHigh1) * fOutGain;

        }

        else if (iDistortType == 1) //Soft
        {
            //Left Channel
            fOutLow0 = filter0.tick(fSignal0) * fGainLow;
            fOutFinalLow0 = softTransferFn(fOutLow0) ;
            
            fOutMid0 = filter1.tick((fSignal0 - filter0.tick(fSignal0))) * fGainMid;
            fOutFinalMid0 = softTransferFn(fOutMid0) ;
            
            fOutHigh0 =(fSignal0 - filter0.tick(fSignal0) - filter1.tick((fSignal0 - filter0.tick(fSignal0)))) * fGainHigh;
            fOutFinalHigh0 = softTransferFn(fOutHigh0) ;
            
            fOut0 = 0.5 * (fOutFinalLow0 + fOutFinalMid0 + fOutFinalHigh0) * fOutGain;
            
            //Right Channel
            fOutLow1 = filter1.tick(fSignal1) * fGainLow;
            fOutFinalLow1 = softTransferFn(fOutLow1) ;
            
            fOutMid1 = filter1.tick((fSignal1 - filter0.tick(fSignal1))) * fGainMid;
            fOutFinalMid1 = softTransferFn(fOutMid1) ;
            
            fOutHigh1 =(fSignal1 - filter0.tick(fSignal1) - filter1.tick((fSignal0 - filter0.tick(fSignal1)))) * fGainHigh;
            fOutFinalHigh1 = softTransferFn(fOutHigh1) ;
            
            fOut1 = 0.5 * (fOutFinalLow1 + fOutFinalMid1 + fOutFinalHigh1) * fOutGain;
        }
        
        else if (iDistortType == 2) //Quarter Circle
        {
            //Left Channel
            fOutLow0 = filter0.tick(fSignal0) * fGainLow;
            fOutFinalLow0 =   quarterCircleTransferFn(fOutLow0) ;
            
            fOutMid0 = filter1.tick((fSignal0 - filter0.tick(fSignal0))) * fGainMid;
            
            fOutFinalMid0 = quarterCircleTransferFn(fOutMid0) ;
            
            fOutHigh0 =(fSignal0 - filter0.tick(fSignal0) - filter1.tick((fSignal0 - filter0.tick(fSignal0)))) * fGainHigh;
            fOutFinalHigh0 = quarterCircleTransferFn(fOutHigh0) ;
            
            fOut0 = 0.5 * (fOutFinalLow0 + fOutFinalMid0 + fOutFinalHigh0) * fOutGain;
            
            //Right Channel
            fOutLow1 = filter1.tick(fSignal1) * fGainLow;
            fOutFinalLow1 =   quarterCircleTransferFn(fOutLow1) ;
            
            fOutMid1 = filter1.tick((fSignal1 - filter0.tick(fSignal1))) * fGainMid;
            
            fOutFinalMid1 = quarterCircleTransferFn(fOutMid1) ;
            
            fOutHigh1 =(fSignal0 - filter0.tick(fSignal0) - filter1.tick((fSignal0 - filter0.tick(fSignal0)))) * fGainHigh;
            fOutFinalHigh1 = quarterCircleTransferFn(fOutHigh1) ;
            
            fOut1 = 0.5 * (fOutFinalLow1 + fOutFinalMid1 + fOutFinalHigh1) * fOutGain;
        }

        
        else if(iDistortType == 3) //Asymmetric
        {
            //Left Channel
            fOutLow0 = filter0.tick(fSignal0) * fGainLow;
            fOutFinalLow0 =   asymmetricTransferFn(fOutLow0) ;
            
            fOutMid0 = filter1.tick((fSignal0 - filter0.tick(fSignal0))) * fGainMid;
            fOutFinalMid0 = asymmetricTransferFn(fOutMid0) ;
            
            fOutHigh0 =(fSignal0 - filter0.tick(fSignal0) - filter1.tick((fSignal0 - filter0.tick(fSignal0)))) * fGainHigh;
            fOutFinalHigh0 = asymmetricTransferFn(fOutHigh0) ;
            
            fOut0 = 0.5 * (fOutFinalLow0 + fOutFinalMid0 + fOutFinalHigh0) * fOutGain;
            
            //Right Channel
            fOutLow1 = filter0.tick(fSignal1) * fGainLow;
            fOutFinalLow1 =   asymmetricTransferFn(fOutLow1) ;
            
            fOutMid1 = filter1.tick((fSignal1 - filter0.tick(fSignal1))) * fGainMid;
            fOutFinalMid1 = asymmetricTransferFn(fOutMid1) ;
            
            fOutHigh1 =(fSignal1 - filter0.tick(fSignal1) - filter1.tick((fSignal1 - filter0.tick(fSignal1)))) * fGainHigh;
            fOutFinalHigh1 = asymmetricTransferFn(fOutHigh1) ;
            
            fOut1 = 0.5 * (fOutFinalLow1 + fOutFinalMid1 + fOutFinalHigh1) * fOutGain;
        }
        
        // Copy result to output
        *pfOutBuffer0++ = fOut0;
        *pfOutBuffer1++ = fOut1;
    }
}
