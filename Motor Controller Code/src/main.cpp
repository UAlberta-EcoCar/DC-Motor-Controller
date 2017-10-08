#include <mbed.h>
#include <rtos.h>
#include "PID.hpp"
#include "current_PID.hpp"


//variables to run inside threads
double current = 0;
double duty_cycle = 0;

//threads
void refresh_duty_cycle();  //function for refresh_thread
void send_serial();         //function for serial_thread
Thread refresh_thread;      //thread runs every 1ms to perform PID calculations and update the output duty cycle
Thread serial_thread;       //thread runs every 1s to send data through serial

//IO
AnalogIn current_sensor(PTC1); //current sensor for feedback
PwmOut output(PTD5);           //pwm output to motor driver
Serial serial(PTB17, PTB16);   // tx, rx

//current PID control; Parameters: feedback, slope (mA/v), intercept (mA), P, I, D, max sum, P saturation, I saturation, D saturation
current_PID current_control(&current_sensor, 277.778, 0, 0, 1, 0, 10000000, 20000, 65535, 0);
//note: the duty cycle is a 16 bit unsigned int, saturation values for P, I and D and be between 0-65535


int main(){

  refresh_thread.set_priority(osPriorityRealtime); //gives PID calculation the priority

  //start threads
  refresh_thread.start(refresh_duty_cycle);
  serial_thread.start(send_serial);

  serial.baud(500000);                        //sets baudrate
  output.period_us(300);                      //sets period in us
  current_control.set_reference_current(300); //sets desired current in mA

  while(true) {
    //infinite loop
    }

    return 0;

}

//thread for doing PID calculations and refreshing the duty cycle, runs every 1ms
void refresh_duty_cycle() {
    while(true) {
    duty_cycle = current_control.PID_calc() / 65536.0;  //calculate duty cycle
    output.write(duty_cycle);                           //applies the new duty cycle to the PWM output
    wait(0.001);
  }
}

//thread for sending data through serial, runs every 1s
void send_serial() {
  while(true) {
    current = current_control.get_current();
    serial.printf("current: %f, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
    wait(1);
  }
}
