#ifndef PID_HPP
#define PID_HPP

#include "mbed.h"
#include "rtos.h"
#include <limits.h>

//constants
#define max_p 255
#define max_i 255
#define max_d 255
#define num_samples 32 //number of ADC samples per cycle


////////////////////
//PID CLASS
////////////////////


class PID {

  public:

    //variables
    int32_t duty_cycle;    //output to the actuator (eg motor)
  	float p, i, d;
    uint16_t ADC_value;
  	int32_t sum;           //sum for the Integral term
  	uint16_t reference;    //reference
  	int32_t error;         //error with respect to the reference, used for calculating the slope
  	int32_t slope;         //used for the Derivative term
  	int32_t previous_cycle;//previous error value, used to calculate the Derivative term
  	AnalogIn *feedBack;    //ADC object used for feedback, eg a current sensor
    uint32_t max_sum;
    uint32_t p_saturation;
    uint32_t i_saturation;
    uint32_t d_saturation;

  	//constructors
    PID();
  	PID(AnalogIn *feedBack, float p, float i, float d, uint32_t max_sum, uint32_t p_saturation, uint32_t i_saturation, uint32_t d_saturation); //feedBack is a pointer to an ADC object, for current sensing

  	//methods
  	uint16_t PID_calc(); //needs to be called every cycle
  	void set_reference(uint16_t reference);
  	void set_feedBack(AnalogIn *feedBack);
  	void set_p(float p);
  	void set_i(float i);
  	void set_d(float d);
  	int32_t get_error();
  	uint16_t analog_read();

};


#endif
