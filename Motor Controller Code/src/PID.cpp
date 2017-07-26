#include "mbed.h"
#include "rtos.h"
#include "PID.hpp"
#include <stdint.h>
#include <math.h>

///////////////////////
//PID class definitions
///////////////////////


//Constructor Definitions

PID::PID() {
	error = 0;
}

PID::PID(AnalogIn *feedBack, float p, float i, float d, uint32_t max_sum, uint32_t p_saturation, uint32_t i_saturation, uint32_t d_saturation) {

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

}



//Method Definitions

uint16_t PID::PID_calc(){

	ADC_value = analog_read();
	error = (int32_t)reference - (int32_t)ADC_value;

	//Proportional Term
	if(error*p > max_p) //checks for saturation and adds error to the duty cycle
		duty_cycle = max_p;
	else if(error*p < -max_p)
		duty_cycle = -max_p;
	else
		duty_cycle = error*p;

	//Integral Term
	sum += error;
	if(sum > (int32_t)max_sum) //checks for max sum
		sum = max_sum;
	else if(sum < -(int32_t)max_sum)
	 	sum = -max_sum;
	if(sum*i > max_i) //checks for saturation and adds sum to the duty cycle
		duty_cycle += max_i;
	else if(sum*i < -max_i)
		duty_cycle += -max_i;
	else
		duty_cycle += sum*i;

	//Derivative Term
	slope = error - previous_cycle; //calculates slope, if there is no saturation
	previous_cycle = error;
	if(slope*d > max_d)
		duty_cycle += max_d;
	else if(slope*d < -max_d)
		duty_cycle += -max_d;
	else
		duty_cycle += slope;


	if(duty_cycle >= 65536) { //saturates duty cycle at 65535
		duty_cycle = 65535;
	}
	else if(duty_cycle < 0) { //saturates duty cycle at 0
		duty_cycle = 0;
	}

	return (uint16_t)duty_cycle;

}

uint16_t PID::analog_read(){

	uint32_t sum = 0;
	for(int i = 0; i < num_samples; i++) {
		sum += (*feedBack).read_u16();
	}
	return (uint16_t)(sum / num_samples);

}

void PID::set_reference(uint16_t reference){

	this->reference = reference;
	sum = 0;						//reset integral term for a new reference
	previous_cycle = 0;	//reset derivative term for a new reference

}

void PID::set_feedBack(AnalogIn *feedBack){

	this->feedBack = feedBack;

}

void PID::set_p(float p){

	this->p = p;

}

void PID::set_i(float i){

	this->i = i;

}

void PID::set_d(float d){

	this->d = d;

}

int32_t PID::get_error(){

	return error;

}
