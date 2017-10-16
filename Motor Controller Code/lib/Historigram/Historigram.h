#include <mbed.h>
#include "Adafruit_SSD1306.h"   //This library is dependant on the Adafruit_SSD1306 library for 128x64 OLED display.
#include <string>
using namespace std;

class Historigram {

    private:

        int sum;
        int i;
        uint8_t horizontal_display_location[128];
        uint8_t height_px; //default is 64px
        uint8_t width_px; //default is 128px
        double current_value;
        double max_value; //3.3V default

        void drawScale(void);
        void findAverage(void);
        void getShift(void); //Shift of the vertical axis (for visual measuring)
        void getVerticalShifts(void); //shifts for the markings on the vertical axis for measurements
        uint8_t shift;
        uint8_t shift_plus1;
        uint8_t shift_minus1;
        uint8_t shift1;
        uint8_t shift2;
        uint8_t shift3; //only 4 shifts since the first/last line are the hieght_px and 0
        uint8_t shift4;
        uint8_t textMeasurementShift; 

        void generateTickMarkValues(void);
        double tick1;
        double tick2;
        double tick3;
        double tick4;
        double tick5;

    public:

        double target_value;
        double average;
        void historigram_initialize(void);
        void run_historigram(void);
        Historigram(uint8_t i2c_address, I2C & i2c_bus);
        Historigram(char* target_value_name, uint8_t i2c_address, I2C & i2c_bus);
        Historigram(uint8_t hieght_px, uint8_t width_px, double max_value, char* target_value_name, uint8_t i2c_address, I2C & i2c_bus) ;
        void updateDisplay(void);    //Pushes frame buffer to display
        void setI2CFreq( int freq, I2C & i2c_bus ); //Frequency in Hz

        Adafruit_SSD1306_I2c display;

        string target_value_name;

};
