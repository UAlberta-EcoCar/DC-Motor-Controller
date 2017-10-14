/*
 *  Copyright (c) 2012 Neal Horman - http://www.wanlink.com
 *
 *  License: MIT open source (http://opensource.org/licenses/MIT)
 *      Summary;
 *      Use / modify / distribute / publish it how you want and
 *      if you use it, or don't, you can't hold me liable for how
 *      it does or doesn't work.
 *      If it doesn't work how you want, don't use it, or change
  *      it so that it does work.
 */
 #include <mbed.h>
 #include "rtos.h"
 #include "Adafruit_SSD1306.h"
 #include "TextLCD.h"
 #include "PID.hpp"
 #include "current_PID.hpp"

 #define I2C_FREQ 1096000

 // an I2C sub-class that provides a constructed default
 class I2CPreInit : public I2C
 {
 public:
     I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl)
     {
         frequency(I2C_FREQ);
         start();
     };
 };

 AnalogIn volt_pin(A0);

 Thread OLEDprint_thread;
 Thread analog_read_thread;
 Thread lcd_print_thread;
 Thread refresh_thread;      //thread runs every 1ms to perform PID calculations and update the output duty cycle
 Thread serial_thread;       //thread runs every 1s to send data through serial
 AnalogIn current_sensor(PTC1); //current sensor for feedback
 PwmOut output(PTD5);           //pwm output to motor driver
 Serial serial(USBTX, USBRX);   // tx, rx

 //current PID control; Parameters: feedback, slope (mA/v), intercept (mA), P, I, D, max sum, P saturation, I saturation, D saturation
 current_PID current_control(&current_sensor, 277.778, 0, 0, 1, 0, 10000000, 20000, 65535, 0);
 //note: the duty cycle is a 16 bit unsigned int, saturation values for P, I and D and be between 0-65535

 I2CPreInit gI2C(PTB3, PTB2);
 I2C gI2C_mbed(PTB3, PTB2);
 Adafruit_SSD1306_I2c display(gI2C, PTA5);
 TextLCD_I2C lcd(&gI2C_mbed, 0x4E, TextLCD::LCD20x4);


 float voltage;
 uint16_t ADC_OUT = 0;
 uint8_t horizontal_display_location[127];
 float current_value;
 int i;
 double sum;
 double current;
 double duty_cycle;

 void analog_read(void);
 void OLEDprint(void);
 void historigram_initialize(void);
 void run_historigram(void);
 void lcd_print(void);
 void refresh_duty_cycle();  //function for refresh_thread
 void send_serial();         //function for serial_thread


void analog_read(){
    while(1){
        float output = volt_pin.read();
        output = output*3.3;
        voltage = output;
    }
}

void OLEDprint(){
    historigram_initialize();
    while(1){
        run_historigram();
        display.display();
        Thread::wait(100);
    }
}

void lcd_print(){
    lcd.cls();
    lcd.printf("Voltage:    Average:\n");
    lcd.printf("DUTY:       Current:");
    while(1){
        lcd.setAddress( 0, 1);
        lcd.printf("%7.3f    %7.3f", voltage, sum);
        lcd.setAddress( 0, 3);
        lcd.printf("%7.3f    %7.3f", duty_cycle, current);
        Thread::wait(2000);
    }
}

void historigram_initialize(void){

	display.clearDisplay();
	sum = 0;
	i = 0;

	for( i = 0;  i < 127 ; i++ ){

		horizontal_display_location[i] = 63;

	}

	current_value = 63;

}

void run_historigram(void){

		//Shifts the graph over to make it real time
		//The black line clears the old value in that position
        current_value = (3.3 - voltage) * 19.090909;
		for(i = 0; i < 126 ; i++){

			if ( horizontal_display_location[i] != horizontal_display_location[i+1] ){
				horizontal_display_location[i] = horizontal_display_location[i+1];
				display.drawLine(i, 0, i, 63, BLACK);
			}

		}
		if ( horizontal_display_location[126] != ((uint8_t) round(current_value)) ){
			horizontal_display_location[126] = (uint8_t) round(current_value);
			display.drawLine(126, 0, 126, 63, BLACK);
		}

		//display the output values onto the graph
		for(i=126; i >= 0 ; i--){

			display.drawPixel(i, horizontal_display_location[i], WHITE);

		}
		display.drawLine(127, 0, 127, 63, BLACK);
		display.drawPixel(127, (uint8_t) round(current_value) , WHITE);

		//Print output voltage
		display.setTextColor(BLACK, WHITE);
		display.setTextSize(1);
		display.setTextCursor(0,0);
		display.printf("VOLTAGE\n");
		display.printf("%f\n", voltage);

		//Averaging the values of the graph
		sum = 0;
		for(i=0; i <= 126; i++){

			sum += horizontal_display_location[i];

		}
		sum += (uint8_t) round(current_value);
		sum = sum/128;
		sum = 63 - sum;
		sum = sum * 5;
		sum = sum / 63;

		//Print Average Values
		display.printf("AVERAGE\n");
		display.printf("%f\n", sum);

		//Draws a Scale
		display.drawLine(100, 0, 100, 63, WHITE);
		//5V tick
		display.drawLine(99, 0, 101, 0, WHITE);
		display.drawChar(91, 0, '5', WHITE, BLACK, 1);
		//4V tick
		display.drawLine(99, 13, 101, 13, WHITE);
		display.drawChar(91, 13, '4', WHITE, BLACK, 1);
		//3V tick
		display.drawLine(99, 25, 101, 25, WHITE);
		display.drawChar(91, 25, '3', WHITE, BLACK, 1);
		//2V tick
		display.drawLine(99, 38, 101, 38, WHITE);
		display.drawChar(91, 38, '2', WHITE, BLACK, 1);
		//1V tick
		display.drawLine(99, 50, 101, 50, WHITE);
		display.drawChar(91, 50, '1', WHITE, BLACK, 1);
		//0V tick
		display.drawLine(99, 63, 101, 63, WHITE);

		display.display();

}

//thread for doing PID calculations and refreshing the duty cycle, runs every 1ms
void refresh_duty_cycle() {
    while(true) {
    duty_cycle = current_control.PID_calc() / 65536.0;  //calculate duty cycle
    output.write(duty_cycle);                           //applies the new duty cycle to the PWM output
    wait(0.001);
  }
}

//thread for sending data through serial, runs every 1s
void send_serial() {
  while(true) {
    current = current_control.get_current();
    serial.printf("current: %f, the duty cycle is %f, the error is %d\r\n", current, duty_cycle, current_control.get_error());
    wait(1);
  }
}

 int main()
 {

     serial.baud(9600);                        //sets baudrate

     current = 0;
     duty_cycle = 0;
     voltage = 0;
     OLEDprint_thread.start(OLEDprint);
     OLEDprint_thread.set_priority(osPriorityAboveNormal1);
     analog_read_thread.start(analog_read);
     lcd_print_thread.start(lcd_print);

     refresh_thread.set_priority(osPriorityRealtime); //gives PID calculation the priority

     //start threads
     refresh_thread.start(refresh_duty_cycle);
     serial_thread.start(send_serial);

     output.period_us(300);                      //sets period in us
     current_control.set_reference_current(300); //sets desired current in mA


     while(1)
     {

     }
 }
