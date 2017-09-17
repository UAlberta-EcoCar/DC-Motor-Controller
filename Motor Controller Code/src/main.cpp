#include <mbed.h>
//#include <rtos.h>
#include "USBSerial.h"
#include "PID.hpp"
#include "current_PID.hpp"

#include "rtos.h"

//void refresh_duty_cycle();
double duty_cycle = 0;
uint16_t current = 0;
//Ticker refresh;
AnalogIn current_sensor(PTC1); //current sensor for feedback
PwmOut output(PTD5);           //pwm output to motor driver
//PID current_control(&current_sensor, 2,5,0,1000000,30000,60000,0); //Parameters: feedback, samples, P, I, D
current_PID current_control(&current_sensor, 277.778, 0, 10, 30, 0, 10000000, 30000, 80000, 0);
USBSerial serial;              //serial for debugging

int main(){

  //TESTING CODE (with my small-scale setup at home)
  //AnalogIn current_sensor(PTC1); //current sensor for feedback
  //PwmOut output(PTD5);           //pwm output to motor driver
  output.period_us(200);         //sets period
  //USBSerial serial;              //serial for debugging
  //PID current_control(&current_sensor, 0.01,0.25,0,100000,10000,40000,0); //Parameters: feedback, samples, P, I, D
  current_control.set_reference_current(200); //corresponds to 200mA 14299

  //Ticker refresh;
  //refresh.attach(&refresh_duty_cycle,0.001);

  while(true) {

    duty_cycle = current_control.PID_calc() / 65535.0;
    output.write(duty_cycle);
    current = (uint16_t)((14299 - current_control.get_error())*0.01325);
    serial.printf("current: %d, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
    wait_ms(1);

  }


  return 0;

}

/*void refresh_duty_cycle() {
  duty_cycle = current_control.PID_calc() / 65535.0;
  output.write(duty_cycle);
  //current = (uint16_t)((22640 - current_control.get_error())*0.01325);
  //serial.printf("current: %d, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
} */
