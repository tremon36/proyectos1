#include <stdio.h>
#include <math.h>
#include <iostream>
#include <memory>
#include "DDS.h"


int main(void) {
    int a;
    //std::unique_ptr<DDS> dds = std::make_unique<LutDDS>(sine,0.5f,0.001f,10000,1.0f);
    //std::unique_ptr<DDS> dds = std::make_unique<RealtimeDDS>(DDS::triangle,1.0f/50000.0f,0.0f,1.0f); // 50 Khz ticker
    std::unique_ptr<DDS> dds = std::make_unique<LutDDS>(DDS::sine,1.0f/50000.0f,10000);
    
    dds->setNote(DDS::SI);
    dds->enableRealtimeVolumeControl(true);
    dds->setVolumePercent(0.5f);

    for(float time = 0; time < 1.0f/261.63f; time += 1.0f/50000.0f) {
        dds->update();
        //std::cout << "t = " << time << " -> " << DDS::triangle(time) << std::endl;
    }
    return 0;
}
