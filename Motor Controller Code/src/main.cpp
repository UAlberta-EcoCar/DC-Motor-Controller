#include "mbed.h"
#include "rtos.h"
#include "USBSerial.h"
#include "PID.hpp"

int main(){

  //TESTING CODE (with my small-scale setup at home)

  double duty_cycle = 0;
  uint16_t current = 0;
  AnalogIn current_sensor(PTC1); //current sensor for feedback
  PwmOut output(PTD5);           //pwm output to motor driver
  output.period_us(200);         //sets period
  USBSerial serial;              //serial for debugging
  PID current_control(&current_sensor, 128, 1 , 1, 0); //Parameters: feedback, samples, P, I, D
  current_control.set_reference(22640); //corresponds to 200mA

  while(true) {

    duty_cycle = current_control.PID_calc() / 65535.0;
    output.write(duty_cycle);
    current = (uint16_t)((22640 - current_control.get_error())*0.01325);
    serial.printf("current: %d, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
    wait_ms(1);

  }


  return 0;

}
