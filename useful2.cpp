#include <stdio.h>
#include <math.h>
#include "lut.h"
#include <cstring>
#include <iostream>

class DDS {
    public: 

    enum notas {
        DO = 0,
        RE = 1,
        MI = 2,
        FA = 3, 
        SOL = 4,
        LA = 5,
        SI = 6
    };

    virtual void update() = 0;
    //virtual void setNote(notas nota) = 0;
};


class LutDDS : public DDS {

    public:

    
    LutDDS(int n_points, int* steps, int step_amount) {
        steps_lut = new int[step_amount];
        points = n_points;
        memcpy(steps_lut,steps,step_amount * sizeof(int));
        position = 0;
        current_note = 0;
    }

    void destroy() {
        delete(steps_lut);
    }

    void update() {
        // PWM write position @TODO
        std::cout << "pos = " << position << std::endl;

        position = position + steps_lut[current_note];
        if(position >= points) position -= points;

        
    }

    void setNote(notas nota) {
        current_note = nota;
    }


    private:

    int points;
    int *steps_lut;
    int amount;
    int position;
    int current_note;

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
        std::cout << "; generated -> " << f(current_time) << std::endl;

        current_time += step; // @TODO add multiplier for each note
        if(current_time>=period) {
            current_time-= period;
        }


        
    }

    private:

    float (*f)(float t); // function of the dds
    float period;
    float step;
    float current_time = 0;


};

int M[7] = {1,2,3,4,5,6,7};

float sine(float t) {
    const float original_f = 1; // 1Hz original period
    return sin(2*M_PI*original_f*t);
}

int main(void) {
    int a;
    //DDS* dds = new LutDDS(100,M,7);
    DDS* dds = new RealtimeDDS(sine,2.546479,0.2);
    //dds->setNote(DDS::MI);
    for(int i = 0; i < 120; i++) {
        dds->update();
    }

    return 0;
}
