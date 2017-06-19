#include "current_PID.hpp"

//constructor definitions

current_PID::current_PID(AnalogIn *feedBack,double current_slope,double current_intercept, uint16_t num_samples, float p, float i, float d){

  this->feedBack = feedBack;
	this->p = p;
	this->i = i;
	this->d = d;
	this->num_samples = num_samples;
	sum = 0;
	ADC_value = 0;
	reference = 0;
	error = 0;
	slope = 0;
	duty_cycle = 0;
	previous_cycle = 0;
  this->current_slope = current_slope;
  this->current_intercept = current_intercept;

}


//method definitions

void current_PID::set_reference_current(double current) { //sets reference current (in mA)

  reference = (uint16_t)round(((current - current_intercept) / current_slope) * (65536 / 3.3));

}
