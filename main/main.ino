#include <stdint.h>  // standard library for integers (used in the next libraries)

#include "driverlib\systick.c"  // standard library for the SysTick (functions)
#include "driverlib\systick.h"  // standard library for the SysTick (header)
#include "wiring_private.h"  // library to access the PWM with configurable frequency

#include "DDS.h"

// Function to generate PWM signals
//
// PWMWrite(PWMpin,max_value,value,freq)
//
// PWMpin: pin for the PWM signal. It needs to be declared as output and needs
// to have PWM functionality. max_value: maximum value of the PWM counter value:
// value that is compared with the signal of the PWM counter. duty
// cycle=value/max_value freq: frequency of the PWM signal

#define TickerPeriod 4000  // PERIOD OF THE SYSTICK COUNTER. THE CLOCK FREQUENCY IS 120 MHz. IT
            // COUNTS CLOCK TICKS. When it reaches 0, it activates the interrupt
            // The maximum value of the counter is 2^24-1=16777215 (7.14 Hz)
#define SENSOR_0 PE_0
#define SENSOR_1 PE_1

float read_s0 = 0;
float read_s1 = 0;
uint16_t* LUT;
int points;
int steps_lut[11];
int amount;
int position = 0;
int current_note;
float period;
int pwm_max = 1000;
float step;



        // Generate LUT according to period, function and number of points

    float sine(float t) { //  Se asume que la frecuencia de la funcion es do grave.
        const float original_f = 261.63f; // F do grave
        return sin(2*M_PI*original_f*t);
    }

void setup() {
  SysTickDisable();                // Disables SysTick during the configuration
  SysTickPeriodSet(TickerPeriod);  // Define the period of the counter. When it
                                   // reaches 0, it activates the interrupt
  SysTickIntRegister(&Ticker);        // The interrupt is associated to the SysTick ISR
  SysTickIntEnable();  // The SysTick interrupt is enabled
  SysTickEnable();     // The SysTick is enabled, after having been configured
  IntMasterEnable();   // All interrupts are enabled

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SENSOR_0,INPUT);
  pinMode(SENSOR_1,INPUT);
  
  Serial.println("hello");
  //dds = new LutDDS(DDS::sine,1.0f/30000.0f,1000);
  Serial.println("world");
  points = 100000;
  period = 1.0f/261.63f;
  step = 1/30000.0f;
  LUT = new uint16_t[points]; // using smart pointers here would just make the code either slower (std::vector) or unreadable
        float min_fn = 90000000.0f;
        float max_fn = -900000.0f;

        for(int i = 0; i < points; i++) { // get maximum and minimum of function for normalization
            float aux = 0.0f;
            aux = sine(period * i/points);
            if(aux < min_fn) min_fn = aux;
            if(aux > max_fn) max_fn = aux;
        }

        for(int i = 0; i < points; i++) {
            LUT[i] = (int) ((sine(period * i/points) - min_fn) * (floor(pwm_max)) / (max_fn - min_fn)); //function normalization and lut fill
        }

        // generate steps LUT

        for(int i = 0; i < 11; i++) {
            steps_lut[i] = (int) (points * step  / period) * pow(1.06f,i);
        }
        
}

DDS::notas getNote(uint16_t read) {
  if(read > 2760) return DDS::DO;
  if(read > 2350) return DDS::DOSHARP;
  if(read > 2120) return DDS::RE;
  if(read > 1900) return DDS::RESHARP;
  if(read > 1700) return DDS::MI;
  if(read > 1600) return DDS::FA;
  if(read > 1480) return DDS::FASHARP;
  if(read > 1350) return DDS::SOL;
  if(read > 1260) return DDS::LA;
  if(read > 1190) return DDS::LAHARP;
  else return DDS::SI;
}
int i = 0;

void loop() {
  //if(i < 1005)  Serial.println(dds->LUT[i]);
  //i++;
  //Serial.println(dds->update());
  //PWMWrite(PM_2,1000,dds->update(),120000);
  
}

void Ticker() {
  //Serial.print("world");
  read_s0 = analogRead(SENSOR_0);
  read_s1 = analogRead(SENSOR_1);
  position = position + steps_lut[current_note];
        if(position >= points) position -= points;
            
  PWMWrite(PM_2,1000,LUT[position],120000);
  //PWMWrite(PM_2,1000,500,120000);
  //Serial.print("s0: ");
 // Serial.println(getNote((int)read_s0));
 // Serial.print("   s1: ");
  //Serial.println(read_s1);
}
