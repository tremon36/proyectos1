#include <stdint.h>  // standard library for integers (used in the next libraries)

#include "driverlib\systick.c"  // standard library for the SysTick (functions)
#include "driverlib\systick.h"  // standard library for the SysTick (header)
#include "wiring_private.h"  // library to access the PWM with configurable frequency

// Function to generate PWM signals
//
// PWMWrite(PWMpin,max_value,value,freq)
//
// PWMpin: pin for the PWM signal. It needs to be declared as output and needs
// to have PWM functionality. max_value: maximum value of the PWM counter value:
// value that is compared with the signal of the PWM counter. duty
// cycle=value/max_value freq: frequency of the PWM signal

#define TickerPeriod 24000000  // PERIOD OF THE SYSTICK COUNTER. THE CLOCK FREQUENCY IS 120 MHz. IT
            // COUNTS CLOCK TICKS. When it reaches 0, it activates the interrupt
            // The maximum value of the counter is 2^24-1=16777215 (7.14 Hz)
#define SENSOR_0 PE_0
#define SENSOR_1 PE_1

float read_s0 = 0;
float read_s1 = 0;


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
 
}

void loop() {
  
}

void Ticker() {
  read_s0 = analogRead(SENSOR_0);
  read_s1 = analogRead(SENSOR_1);
  Serial.print("s0: ");
  Serial.print(read_s0);
  Serial.print("   s1: ");
  Serial.println(read_s1);
}
