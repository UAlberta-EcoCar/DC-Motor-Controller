#include "Historigram.h"

Historigram::Historigram(uint8_t i2c_address, I2C & i2c_bus) : display( i2c_bus, D6, i2c_address){
    height_px = 63;
    width_px = 127;
    max_value = 3.3;

    string default_name = "VOLTAGE";
    target_value_name = default_name;

}

Historigram::Historigram(char* target_name, uint8_t i2c_address, I2C & i2c_bus) : display(i2c_bus, D6, i2c_address){
    height_px = 63;
    width_px = 127;
    max_value = 3.3;

    target_value_name.assign(target_name );

}

Historigram::Historigram(uint8_t height_px, uint8_t width_px, double max_value, char* target_name, uint8_t i2c_address, I2C & i2c_bus) : display(i2c_bus, D6, i2c_address, height_px, width_px){
    //We're using 0 indexing
    height_px--;
    width_px--;

    target_value_name.assign(target_name );

}

void Historigram::historigram_initialize(void){

    getShift();
    getVerticalShifts();
    generateTickMarkValues();

    //OLED holds it buffer from previous program, that needs to be cleared.
	display.clearDisplay();

    //Some initializers.
	sum = 0;
    average = 0;
	i = 0;

    //initialize the data buffer. Only needs to be 1D since each vertical line holds 1 data point
	for( i = 0;  i < width_px ; i++ ){

		horizontal_display_location[i] = height_px;

	}

	current_value = height_px;

}

void Historigram::run_historigram(void){

		//Shifts the graph over to make it real time
		//The black line clears the old value in that position
        current_value = (max_value - target_value) * (height_px / max_value);  //converts output of ADC to 0-3.3V (or whatever is set for max value)
		for(i = 0; i < (width_px - 1) ; i++){

			if ( horizontal_display_location[i] != horizontal_display_location[i+1] ){
				horizontal_display_location[i] = horizontal_display_location[i+1];
				display.drawLine(i, 0, i, height_px, BLACK);
			}

		}
		if ( horizontal_display_location[width_px - 1] != ((uint8_t) round(current_value)) ){
			horizontal_display_location[width_px - 1] = (uint8_t) round(current_value);
			display.drawLine(width_px-1, 0, width_px-1, height_px, BLACK);
		}

		//display the output values onto the graph
		for(i=width_px-1; i >= 0 ; i--){

			display.drawPixel(i, horizontal_display_location[i], WHITE);

		}
		display.drawLine(width_px, 0, width_px, height_px, BLACK);
		display.drawPixel(width_px, (uint8_t) round(current_value) , WHITE);

		//Print output voltage
		display.setTextColor(BLACK, WHITE);
		display.setTextSize(1);
		display.setTextCursor(0,0);

		display.printf("%s\n", target_value_name.c_str());
		display.printf("%4.3f\n", target_value);

        findAverage();

		//Print Average Values
		display.printf("AVERAGE\n");
		display.printf("%4.3f\n", average);

        drawScale();

        display.display();

}

void Historigram::findAverage(){

    //Averaging the values of the graph
    sum = 0;
    for(i=0; i <= 126; i++){

        sum += horizontal_display_location[i];

    }
    sum += (uint8_t) round(current_value);

    average = sum/(width_px+1);
    average = height_px - average;
    average = average * max_value;
    average = average / height_px;

}

void Historigram::drawScale(){

    //Draws a Scale
    display.drawLine(shift, 0, 100, 63, WHITE);
    //5 tick
    display.drawLine(shift_minus1, 0, shift_plus1, 0, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setTextCursor(textMeasurementShift, 0);
    display.printf("%3.1f", tick5 );
    //4 tick
    display.drawLine(shift_minus1, shift1, shift_plus1, shift1, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setTextCursor(textMeasurementShift, shift1);
    display.printf("%3.1f", tick4 );
    //3 tick
    display.drawLine(shift_minus1, shift2, shift_plus1, shift2, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setTextCursor(textMeasurementShift, shift2);
    display.printf("%3.1f", tick3 );
    //2 tick
    display.drawLine(shift_minus1, shift3, shift_plus1, shift3, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setTextCursor(textMeasurementShift, shift3);
    display.printf("%3.1f", tick2 );
    //1 tick
    display.drawLine(shift_minus1, shift4, shift_plus1, shift4, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setTextCursor(textMeasurementShift, shift4);
    display.printf("%3.1f", tick1 );
    //0 tick
    display.drawLine(shift_minus1, height_px, shift_plus1, height_px, WHITE);


}
void Historigram::getShift(){
    shift = (uint8_t) round(0.79*width_px); //amount of shift the vertical line would be for vertical measurement
    shift_plus1 = shift + 1;
    shift_minus1 = shift - 1;

    textMeasurementShift = (uint8_t) round(0.63*width_px);
}
void Historigram::getVerticalShifts(){
    shift1 = (uint8_t) round(0.2*height_px);
    shift2 = (uint8_t) round(0.4*height_px);
    shift3 = (uint8_t) round(0.6*height_px);
    shift4 = (uint8_t) round(0.8*height_px);
}

void Historigram::updateDisplay(){
    display.display();
}

void Historigram::setI2CFreq(int freq , I2C & i2c_bus){

    i2c_bus.frequency( freq );
    i2c_bus.start();
}

void Historigram::generateTickMarkValues(void){

    tick5 = max_value;
    tick4 = max_value * 0.8;
    tick3 = max_value * 0.6;
    tick2 = max_value * 0.4;
    tick1 = max_value * 0.2;

    tick5 *= 10;
    tick4 *= 10;
    tick3 *= 10;
    tick2 *= 10;
    tick1 *= 10;

    tick5 = round(tick5);
    tick4 = round(tick4);
    tick3 = round(tick3);
    tick2 = round(tick2);
    tick1 = round(tick1);

    tick5 /= 10;
    tick4 /= 10;
    tick3 /= 10;
    tick2 /= 10;
    tick1 /= 10;

}
