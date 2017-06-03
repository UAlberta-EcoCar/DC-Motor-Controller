#include "mbed.h"
#include "rtos.h"
#include "PID.hpp"
#include <stdint.h>

///////////////////////
//PID class definitions
///////////////////////


//Constructor Definitions

PID::PID() {

	p = 0;
	i = 0;
	d = 0;
	sum = 0;
	reference = 0;
	error = 0;
	slope = 0;
	duty_cycle = 0;
	previous_cycle = 0;
	num_samples = 128;

}

PID::PID(AnalogIn *feedBack, uint16_t num_samples, uint16_t p, uint16_t i, uint16_t d) {

	this->feedBack = feedBack;
	this->p = p;
	this->i = i;
	this->d = d;
	this->num_samples = num_samples;
	sum = 0;
	reference = 0;
	error = 0;
	slope = 0;
	duty_cycle = 0;
	previous_cycle = 0;

}



//Method Definitions

uint16_t PID::PID_calc(){

	error = reference - analog_read();
	sum += error;
	slope = error - previous_cycle;
	previous_cycle = error;
	duty_cycle = error*p + sum*i + slope*d;

	if(duty_cycle >= 65536) {
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

}

void PID::set_feedBack(AnalogIn *feedBack){

	this->feedBack = feedBack;

}

void PID::set_p(uint16_t p){

	this->p = p;

}

void PID::set_i(uint16_t i){

	this->i = i;

}

void PID::set_d(uint16_t d){

	this->d = d;

}

int32_t PID::get_error(){

	return error;

}
