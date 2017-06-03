#ifndef PID_HPP
#define PID_HPP

#include "mbed.h"
#include "rtos.h"


////////////////////
//PID CLASS
////////////////////


class PID {

  private:

  	int32_t duty_cycle;
  	uint16_t p, i, d;
  	int32_t sum;
  	uint16_t reference;
  	int32_t error;
  	int32_t slope;
  	int32_t previous_cycle;
  	uint16_t num_samples; //max number of samples is 65536
  	AnalogIn *feedBack;

  public:

  	//constructors
  	PID();
  	PID(AnalogIn *feedBack, uint16_t num_samples, uint16_t p, uint16_t i, uint16_t d); //feedBack is a pointer to an ADC object, for current sensing

  	//methods
  	uint16_t PID_calc(); //needs to be called every cycle
  	void set_reference(uint16_t reference);
  	void set_feedBack(AnalogIn *feedBack);
  	void set_p(uint16_t p);
  	void set_i(uint16_t i);
  	void set_d(uint16_t d);
  	int32_t get_error();
  	uint16_t analog_read();

};


#endif
