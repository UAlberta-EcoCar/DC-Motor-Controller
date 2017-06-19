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

	p = 0;
	i = 0;
	d = 0;
	sum = 0;
	ADC_value = 0;
	reference = 0;
	error = 0;
	slope = 0;
	duty_cycle = 0;
	previous_cycle = 0;
	num_samples = 128;

}

PID::PID(AnalogIn *feedBack, uint16_t num_samples, float p, float i, float d) {

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

}



//Method Definitions

uint16_t PID::PID_calc(){

	ADC_value = analog_read();
	error = (int32_t)reference - (int32_t)ADC_value;

	//saturates the integral sum, to prevent overflow
	if((error > 0) && (sum > INT_MAX - error)) {
		sum = INT_MAX; //saturates sum
	}
	else if((error < 0) && (sum < INT_MIN - error)) {
		sum = INT_MIN; //saturates sum
	}
	else {
		sum += error; //add error to the sum, if there is no saturation
	}

	//saturates the derivative term  (slope = error - previous_cycle)
	if((previous_cycle < 0) && (error > INT_MAX + previous_cycle)) {
		slope = INT_MAX; //saturates derivative
	}
	else if((previous_cycle > 0) && (error < INT_MIN + previous_cycle)) {
		slope = INT_MIN; //saturates derivative
	}
	else {
		slope = error - previous_cycle; //calculates slope, if there is no saturation
	}

	previous_cycle = error;

	duty_cycle = round(error*p + sum*i + slope*d); //note: overflow protected

	if(duty_cycle >= 65536) { //saturates duty cycle at 65535
		duty_cycle = 65535;
	}
	else if(duty_cycle < 0) {
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
