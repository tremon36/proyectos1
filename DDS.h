#include <stdio.h>
#include <math.h>
#include <iostream>
#include <memory>

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
        LA = 8,
        LAHARP = 9,
        SI = 10
    };

    // Some example functions that can be used for the DDS
    
    static float sine(float t) { //  Se asume que la frecuencia de la funcion es do grave.
        const float original_f = 261.63; // F do grave
        return sin(2*M_PI*original_f*t);
    }

    static float sawtooth(float t) {
        return t * 261.63f; // T = 1 / fdograve
    }

    static float inverse_sawtooth(float t) {
        return (1.0f/261.63f-t) * 261.63f;
    }

    static float triangle(float t) {
        if(t < 1/(2.0f * 261.63f)) return t * 2.0f * 261.63f;
        else return (1.0f/(261.63f)-t) * 2.0f * 261.63f;
    }

    static float square(float t) {
        if(t < 1/(2.0f * 261.63f)) return 0.0f;
        else return 1.0f;
    }

    virtual ~DDS() {};
    virtual int update() = 0;
    virtual void setNote(notas nota) = 0;
    virtual void enableRealtimeVolumeControl(bool enable) = 0;
    virtual void setVolumePercent(float percent) = 0;


    protected:

        bool volume_control_enabled = 0;
        float volume;
        int current_note;
};


class LutDDS : public DDS {

    public:

    
    LutDDS(float (*ft)(float t),float step, int npoints, float period,  float original_volume) {

        this->points = npoints;

        // Generate LUT according to period, function and number of points

        this->LUT = new uint16_t[npoints]; // using smart pointers here would just make the code either slower (std::vector) or unreadable
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

    LutDDS(float (*ft)(float t), float step, int npoints) : LutDDS(ft,step,npoints,1.0f/261.63f,1.0f) {}; // Constructor where frequency of the function is asumed to be 261.63Hz, and original volume to be 1.0 (MAX)

    ~LutDDS() {
        delete LUT;
    }

    int update() {
        // PWM write position @TODO
        int result = 0;
        if(!volume_control_enabled) {
            //std::cout << "pos = " << position << "->" << this->LUT[position] << std::endl;
            result =  this->LUT[position];
        } else {
            //std::cout << "pos = " << position << "->" << std::min((int)(this->LUT[position] * volume),pwm_max) << std::endl;
            result = std::min((int)(this->LUT[position] * volume),pwm_max);
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

    void setVolumePercent(float percent) {
        this->volume = percent;
    }


    private:

    int points;
    int steps_lut[11];
    int amount;
    int position;
    uint16_t* LUT;

};

class RealtimeDDS : public DDS {

    public:

    RealtimeDDS(float (*ft)(float t), float step, float ft_min, float ft_max) {
        this->step = step;
        this->f = ft;
        this->ft_max = ft_max;
        this->ft_min = ft_min;
    }

    int update() {

        // PWM write position @TODO
        //std::cout << "t = " << current_time;
        int result = 0;
        if(!volume_control_enabled) {
            //std::cout << "; generated -> " << (int) ((f(current_time) - ft_min) * pwm_max / (ft_max-ft_min)) << std::endl;
            result =  (int) ((f(current_time) - ft_min) * pwm_max / (ft_max-ft_min));
        } else {
            //std::cout << "; generated -> " << std::min(((int) ((f(current_time) - ft_min) * pwm_max / (ft_max-ft_min) * volume)),pwm_max)<< std::endl;
            result =  std::min(((int) ((f(current_time) - ft_min) * pwm_max / (ft_max-ft_min) * volume)),pwm_max);
        }

        current_time += step * multiplier;
        if(current_time>=period) {
            current_time-= period;
        }   

        return result; 
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
    float ft_max = 1;
    float ft_min = 0;


};

template<int n_channels>
class Mixer {

    public:

        Mixer() {};

        void setChannelSynth(int channel_number, DDS* synthetizer) {
            synths[channel_number] = synthetizer;
        }

        void setChannelVolume(int channel_number, float volume) {
            
            channel_volumes[channel_number] = volume;
            synths[channel_number]->enableRealtimeVolumeControl(true);
            synths[channel_number]->setVolumePercent(volume);
        }

        void update() {
            int val = 0;
            int acc = 0;
            for(int i = 0; i < n_channels; i++) {
                val = synths[i]->update();
                std::cout << "synth " << i << " -> " << val << std::endl;
                acc += val;
            }
            std::cout << "mixer ->" << acc << std::endl;
        }


    private:
        DDS* synths[n_channels];
        float channel_volumes[n_channels];
};



