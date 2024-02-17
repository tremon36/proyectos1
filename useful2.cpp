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

    DDS(int n_points, int* steps, int step_amount) {
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

int M[7] = {1,2,3,4,5,6,7};

int main(void) {
    int a;
    DDS dds(100,M,7);
    dds.setNote(dds.MI);
    for(int i = 0; i < 120; i++) {
        dds.update();
    }

    return 0;
}
