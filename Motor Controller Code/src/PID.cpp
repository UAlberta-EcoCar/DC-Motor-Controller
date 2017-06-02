#include "mbed.h"
#include "rtos.h"
#include <stdint.h>

//***********************
// PID Class
//***********************

class PID {

private:

	uint16_t duty_cycle = 0;
	uint16_t p, i, d;
	int32_t sum = 0;
	uint16_t reference = 0;
	int32_t error = 0;
	AnalogIn *feedBack;

public:

	//constructors
	PID();
	PID(AnalogIn *feedBack, uint16_t p, uint16_t i, uint16_t d); //feedBack is a pointer to an ADC object, for current sensing

	//methods
	void set_reference(uint16_t reference);
	void set_feedBack(AnalogIn *feedBack);
	void set_p(uint16_t p);
	void set_i(uint16_t i);
	void set_d(uint16_t d);
	int32_t get_error();

};



//Constructor Definitions

PID::PID() {

	p = 0;
	i = 0;
	d = 0;

}

PID::PID(AnalogIn *feedBack, uint16_t p, uint16_t i, uint16_t d) {

	this->feedBack = feedBack;
	this->p = p;
	this->i = i;
	this->d = d;

}



//Method Definitions

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
