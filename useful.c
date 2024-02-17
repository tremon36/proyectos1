#include <stdio.h>
#include <math.h>

const float period  = 0.25f;

int real_time_generation(float (*lambda_fn) (void* args) ) { //  function used to generate the signal in realtime
    static float realtime = 0.0f;
    printf("[t = %f] -> generated: %f\n",realtime,lambda_fn(&realtime)); // change to PWM_WRITE
    realtime += period;
}

float sine(void* args) {
    float f = 3.14159 / 8.0f;
    return sin(((float*)args)[0] * 2 * 3.14159 * f);
}

int main(void) {

    real_time_generation(sine);
    real_time_generation(sine);
    real_time_generation(sine);

    return 0;
}
