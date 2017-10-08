#ifndef current_PID_HPP
#define current_PID_HPP

#include "PID.hpp"
#include "mbed.h"
//#include "rtos.h"
#include <math.h>

class current_PID : public PID {

  public:

    double current_slope;     //slope of the current sensor voltage/current curve, in mA/V
    double current_intercept; //intercept of the current sensor voltage/current curve, in mA
    double current_reference; //desired current setpoint

    //methods

    void set_reference_current(double current); //sets reference current (in mA)
    double get_current();

    //constructors

    current_PID(AnalogIn *feedBack, double current_slope, double current_intercept, float p, float i, float d, uint32_t max_sum, uint32_t p_saturation, uint32_t i_saturation, uint32_t d_saturation);
    //note: slope represents sensitivity in mA/V, intercept is mA

};


#endif
