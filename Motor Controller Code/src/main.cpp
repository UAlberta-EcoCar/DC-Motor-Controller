#include "mbed.h"
#include "rtos.h"
#include "USBSerial.h"
#include "PID.hpp"

int main(){

  //TESTING CODE

  double duty_cycle = 0;
  uint16_t current = 0;
  AnalogIn current_sensor(PTC1);
  PwmOut output(PTD5);
  output.period_us(200);
  USBSerial serial;
  PID current_control(&current_sensor, 128, 5 , 5, 0);
  current_control.set_reference(15093); //corresponds to 200mA

  while(true) {
    duty_cycle = current_control.PID_calc() * 0.000015258789;
    //duty_cycle = 0.75;
    output.write(duty_cycle);
    current = (uint16_t)((15093 - current_control.get_error())*0.01325);
    serial.printf("current: %d, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
    wait_ms(1);
  }


  return 0;

}
