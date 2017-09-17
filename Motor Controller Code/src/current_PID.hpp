#ifndef current_PID_HPP
#define current_PID_HPP

#include "PID.hpp"
#include "mbed.h"
#include "rtos.h"
#include <math.h>

class current_PID : public PID {

  public:

    double current_slope;
    double current_intercept;
    double current_reference;

    //methods

    void set_reference_current(double current); //sets reference current (in mA)

    //constructors

    current_PID(AnalogIn *feedBack,double current_slope,double current_intercept, uint16_t num_samples, float p, float i, float d);
    //note: slope represents sensitivity in mA/V, intercept is mA

};


#endif
