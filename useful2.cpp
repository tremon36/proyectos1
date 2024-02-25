#include <stdio.h>
#include <math.h>
#include <iostream>
#include <memory>
#include "DDS.h"

 float sine(float t) { //  Se asume que la frecuencia de la funcion es do grave.
        const float original_f = 1.0f; // F do grave
        return sin(2*M_PI*original_f*t);
    }


int main(void) {
    int a;
    //std::unique_ptr<DDS> dds = std::make_unique<LutDDS>(sine,0.5f,0.001f,10000,1.0f);
    //std::unique_ptr<DDS> dds = std::make_unique<RealtimeDDS>(DDS::triangle,1.0f/50000.0f,0.0f,1.0f); // 50 Khz ticker
    DDS* dds =new LutDDS(sine,1.0f/100.0f,500,1.0f,1.0f);//(DDS::sine,1.0f/50000.0f,10000);
    DDS* dds2 =new LutDDS(sine,1.0f/100.0f,500,1.0f,1.0f);
    dds->setNote(DDS::DO);
    dds2->setNote(DDS::SI);

    Mixer<2> mixer;
    mixer.setChannelSynth(0,dds);
    mixer.setChannelSynth(1,dds2);

    mixer.setChannelVolume(0,0.25f);
    mixer.setChannelVolume(1,0.5f);

    for(float time = 0; time < 1.0f; time += 1.0f/100.0f) {
        mixer.update();
        //std::cout << "t = " << time << " -> " <<  << std::endl;
    }
    return 0;
}
