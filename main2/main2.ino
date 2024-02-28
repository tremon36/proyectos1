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

LutDDS dds(DDS::sine,1.0f / 30000.0f,30000);
float read_s0=0;
float read_s1=0;

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


  dds.setNote(DDS::DO);
  dds.enableRealtimeVolumeControl(true);

}

DDS::notas getNote(uint16_t read) {
  if(read > 1950) return DDS::DO;
  if(read > 1810) return DDS::DOSHARP;
  if(read > 1650) return DDS::RE;
  if(read > 1500) return DDS::RESHARP;
  if(read > 1440) return DDS::MI;
  if(read > 1320) return DDS::FA;
  if(read > 1240) return DDS::FASHARP;
  if(read > 1190) return DDS::SOL;
  if(read > 1120) return DDS::SOLSHARP;
  if(read > 1070) return DDS::LA;
  if(read > 1000) return DDS::LASHARP; 
  else return DDS::SI;
}

int getNote2(uint16_t read) {
  if(read > 1840) return 100;
  if(read > 1570) return 75;
  if(read > 1400) return 50;
  if(read > 1300) return 25;
  else return 25;
}

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
  dds.setNote(getNote((int)read_s0));
  dds.setVolumePercent(getNote2((int)read_s1));
  PWMWrite(PM_2,1000,dds.update(),120000);
  //Serial.print("s0: ");
  //Serial.println(getNote((int)read_s0));
 //Serial.print("   s1: ");
 //Serial.println(getNote((int)read_s1));
}
