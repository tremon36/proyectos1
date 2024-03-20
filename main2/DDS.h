#ifndef _DDS_H
#define _DDS_H

#include <stdio.h>
#include <math.h>

class DDS {

    public: 

    const int pwm_max = 1000;

    enum notas {
        DO = 0,
        DOSHARP = 1,
        RE = 2,
        RESHARP = 3,
        MI = 4,
        FA = 5, 
        FASHARP = 6, 
        SOL = 7,
        SOLSHARP = 8,
        LA = 9,
        LASHARP = 10,
        SI = 11
    };

    // Some example functions that can be used for the DDS
    
    static float sine(float t) { //  Se asume que la frecuencia de la funcion es do grave.
        const float original_f = 523.251; // F do grave
        return sin(2*M_PI*original_f*t);
    }

    static float sawtooth(float t) {
        return t * 523.251f; // T = 1 / fdograve
    }

    static float inverse_sawtooth(float t) {
        return (1.0f/523.251f-t) * 523.251f;
    }

    static float triangle(float t) {
        if(t < 1/(2.0f * 523.251f)) return t * 2.0f * 523.251f;
        else return (1.0f/(523.251f)-t) * 2.0f * 523.251f;
    }

    static float square(float t) {
        if(t < 1/(2.0f * 523.251f)) return 0.0f;
        else return 1.0f;
    }

    virtual ~DDS() {};
    virtual int update() = 0;
    virtual void setNote(notas nota) = 0;
    virtual void enableRealtimeVolumeControl(bool enable) = 0;
    virtual void setVolumePercent(int percent) = 0;


    protected:

        bool volume_control_enabled = 0;
        int volume;
        int current_note;
};


class LutDDS : public DDS {

    public:

    
    LutDDS(float (*ft)(float t),float step, int npoints, float period,  float original_volume) {

        this->points = npoints;

        // Generate LUT according to period, function and number of points

        this->LUT = new uint16_t[npoints]; // using smart pointers here would just make the code either slower (std::vector) or unreadable
        float min_fn = 9000000.0f;
        float max_fn = -9000000.0f;

        for(int i = 0; i < npoints; i++) { // get maximum and minimum of function for normalization
            float aux = 0.0f;
            aux = ft(period * i/npoints);
            if(aux < min_fn) min_fn = aux;
            if(aux > max_fn) max_fn = aux;
        }

        for(int i = 0; i < npoints; i++) {
            this->LUT[i] = (int) ((ft(period * i/npoints) - min_fn) * (floor(pwm_max * original_volume)) / (max_fn - min_fn)); //function normalization and lut fill
        }

        // generate steps LUT

        for(int i = 0; i < sizeof(steps_lut) / sizeof(int); i++) {
            this->steps_lut[i] = (int) ((npoints * step  / period) * pow(1.06f,i));
        }

    }

    LutDDS(float (*ft)(float t), float step, int npoints) : LutDDS(ft,step,npoints,1.0f/523.251f,1.0f) {}; // Constructor where frequency of the function is asumed to be 523.251Hz, and original volume to be 1.0 (MAX)

    ~LutDDS() {
        delete LUT;
    }


    int update() {
        int result = 0;
        if(!volume_control_enabled) {
            result = this->LUT[position];
        } else {
            result = min((int)(LUT[position] * volume / 100),pwm_max);
        }

        position = position + steps_lut[current_note];
        if(position >= points) position -= points;

        return result;
             
    }

    void setNote(notas nota) {
        this->current_note = nota;
    }

    void enableRealtimeVolumeControl(bool enable) {
        this->volume_control_enabled = true;
    }

    void setVolumePercent(int percent) {
        this->volume = percent;
    }


    private:

    int points;
    int steps_lut[12];
    int amount;
    int position;
    uint16_t* LUT;

};

template<int n_channels>
class Mixer {

    public:

        Mixer() {};

        void setChannelSynth(int channel_number, DDS* synthetizer) {
            synths[channel_number] = synthetizer;
        }

        void setChannelVolume(int channel_number, int volume) {
            
            channel_volumes[channel_number] = volume;
            synths[channel_number]->enableRealtimeVolumeControl(true);
            synths[channel_number]->setVolumePercent(volume);
        }

        int update() {
            int val = 0;
            int acc = 0;
            for(int i = 0; i < n_channels; i++) {
                val = synths[i]->update();
                acc += val;
            }
            
            return acc;
        }


    private:
        DDS* synths[n_channels];
        int channel_volumes[n_channels];
};

#endif
