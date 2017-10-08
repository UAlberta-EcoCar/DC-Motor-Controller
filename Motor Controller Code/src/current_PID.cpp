#include "current_PID.hpp"

//constructor definitions

current_PID::current_PID(AnalogIn *feedBack, double current_slope, double current_intercept, float p, float i, float d, uint32_t max_sum, uint32_t p_saturation, uint32_t i_saturation, uint32_t d_saturation) {

  if(p > max_p)
		p = max_p;
	else if(p < 0)
		p = 0;
	this->p = p;

	if(i > max_i)
		i = max_i;
	else if(i < 0)
		i = 0;
	this->i = i;

	if(d > max_d)
		d = max_d;
	else if(d < 0)
		d = 0;
	this->d = d;

  this->feedBack = feedBack;
  this->max_sum = max_sum;
  this->p_saturation = p_saturation;
	this->i_saturation = i_saturation;
	this->d_saturation = d_saturation;
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

double current_PID::get_current() {

  return (double)((ADC_value / 65536.0 * 3.3) * current_slope + current_intercept);

}
