#include <stdio.h>
#include <math.h>
#include "lut.h"
#include <iostream>

class DDS {

    public: 

    const int pwm_max = 65535;

    enum notas {
        DO = 0,
        DOSHARP = 1,
        RE = 2,
        RESHARP = 3,
        MI = 4,
        FA = 5, 
        FASHARP = 6, 
        SOL = 7,
        LA = 8,
        LAHARP = 9,
        SI = 10
    };

    virtual void update() = 0;
    virtual void setNote(notas nota) = 0;
    virtual void enableRealtimeVolumeControl(bool enable) = 0;
    virtual void setVolumePercent(float percent) = 0;

    // @TODO create some example functions


    protected:

        bool volume_control_enabled = 0;
        float volume;
        int current_note;
};


class LutDDS : public DDS {

    public:

    
    LutDDS(float (*ft)(float t),float period , float step, int npoints, float original_volume) {

        this->points = npoints;

        // Generate LUT according to period, function and number of points

        this->LUT = new int[npoints];
        float min_fn = std::numeric_limits<float>::max();
        float max_fn = std::numeric_limits<float>::min();

        for(int i = 0; i < npoints; i++) { // get maximum and minimum of function for normalization
            float aux = 0.0f;
            aux = ft(period * i/npoints);
            if(aux < min_fn) min_fn = aux;
            if(aux > max_fn) max_fn = aux;
        }

        for(int i = 0; i < npoints; i++) {
            this->LUT[i] = (int) ((ft(period * i/npoints) - min_fn) * (floorf32(pwm_max * original_volume)) / (max_fn - min_fn)); //function normalization and lut fill
        }

        // generate steps LUT

        for(int i = 0; i < 11; i++) {
            this->steps_lut[i] = (int) (npoints * step  / period) * powf32(1.06f,i);
        }

    }


    void update() {
        // PWM write position @TODO
        if(!volume_control_enabled) {
            std::cout << "pos = " << position << "->" << this->LUT[position] << std::endl;
        } else {
            std::cout << "pos = " << position << "->" << std::min((int)(this->LUT[position] * volume),pwm_max) << std::endl;
        }


        position = position + steps_lut[current_note];
        if(position >= points) position -= points;

        
    }

    void setNote(notas nota) {
        this->current_note = nota;
    }

    void enableRealtimeVolumeControl(bool enable) {
        this->volume_control_enabled = true;
    }

    void setVolumePercent(float percent) {
        this->volume = percent;
    }


    private:

    int points;
    int steps_lut[11];
    int amount;
    int position;
    int* LUT;

};

class RealtimeDDS : public DDS {

    public:

    RealtimeDDS(float (*ft)(float t),float period, float step) {
        this->period = period;
        this->step = step;
        this->f = ft;
    }

    void update() {

        // PWM write position @TODO
        std::cout << "t = " << current_time;
        if(!volume_control_enabled) {
            std::cout << "; generated -> " << (int) (f(current_time) * pwm_max) << std::endl;
        } else {
            std::cout << "; generated -> " << std::min((int)(f(current_time) * volume * pwm_max),pwm_max)<< std::endl;
        }

        current_time += step * multiplier;
        if(current_time>=period) {
            current_time-= period;
        }    
    }

    void setNote(notas nota) {
        this->multiplier = powf32(1.06f,nota);
    }

    void enableRealtimeVolumeControl(bool enable) {
        this->volume_control_enabled = true;
    }

    void setVolumePercent(float percent) {
        this->volume = percent;
    }

    private:

    float (*f)(float t); // function of the dds
    float period;
    float step;
    float current_time = 0;
    float multiplier = 0;
    bool volume_control_enabled = 0;
    float volume;


};


float sine(float t) { // will be used as the input function for the dds. Se asume que la frecuencia de la funcion es do grave.
    const float original_f = 1; // 1Hz original period
    return sin(2*M_PI*original_f*t);
}

int main(void) {
    int a;
    DDS* dds = new LutDDS(sine,1.0f,0.01f,100,0.5f);
    //LutDDS(float (*ft)(float t),float period , float step, int npoints, float volume_percent_original)
    //DDS* dds = new RealtimeDDS(sine,2.546479,0.2);
    dds->setNote(DDS::DO);
    dds->setVolumePercent(2.0f);
    dds->enableRealtimeVolumeControl(true);

    for(int i = 0; i < 120; i++) {
        dds->update();
    }

    return 0;
}
