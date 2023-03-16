//
//  EffectExtra.h
//  Additional Plugin Code
//
//  This file is a workspace for developing new DSP objects or functions to use in your plugin.
//

float hardTransferFn(float fSignal)
{
    if (fSignal > 1)
    {
        fSignal = 1;
    }
    
    else if (fSignal < -1)
    {
        fSignal = -1;
    }
    
    return fSignal;
}

float softTransferFn(float fSignal)
{
    float a = 10;
    
    if (fSignal > 0 && fSignal <= 1){
        fSignal = a/(a-1) * (1-pow(a, -fSignal));
    }
    else if (fSignal >= -1 && fSignal <= 0){
        fSignal = a/(a-1) * (-1 + pow(a, fSignal));
    }
    
    return fSignal;
}

float quarterCircleTransferFn(float fSignal)
{
    
    if (fSignal >0 && fSignal <=1)
    {
        fSignal = pow(1 - pow(fSignal - 1, 2), 0.5);
    }

    else if (fSignal >= -1 && fSignal <= 0)
    {
        fSignal = -pow(1 - pow(fSignal + 1, 2), 0.5);
    }

    return fSignal;
}

float asymmetricTransferFn(float fSignal)
{
    
    float a = 1 / (-0.5 + 1);
    
    if (fSignal >0 && fSignal <=1)
    {
        fSignal = fSignal;
    }
    
    if (fSignal >= -1 && fSignal <= 0)
    {
        fSignal = fSignal + (pow(-fSignal, a)) / a;
    }
    
    return fSignal;
}
