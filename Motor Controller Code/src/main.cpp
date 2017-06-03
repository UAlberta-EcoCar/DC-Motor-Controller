#include "mbed.h"
#include "rtos.h"
#include "PID.hpp"

int main(){

  AnalogIn current_sensor(PTC1);
  PID current_control(&current_sensor, 128, 0 , 0, 0);
  return 0;

}
