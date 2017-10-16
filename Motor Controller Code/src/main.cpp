 #include <mbed.h>
 #include "rtos.h"
 #include "Historigram.h"
 #include "TextLCD.h"
 #include "PID.hpp"
 #include "current_PID.hpp"

 //i2c addresses for the OLED and the LCD
 #define OLED_I2C_Address 0x78
 #define LCD_I2C_Address 0x4E   //Make sure you goto TextLCD_Config.h and select the correct settings.
 //Frequency I2C will run at.
 #define I2C_FREQ 921600

 #define AVERAGE_BUFFER_SIZE 8 //Size of the buffer used for averaging

 // I/O
 AnalogIn volt_pin(A0);    //Testing pin, change this pin to what the historigram will be monitoring
 AnalogIn current_sensor(PTC1); //current sensor for feedback
 PwmOut output(PTD5);           //pwm output to motor driver
 Serial serial(USBTX, USBRX);   // tx, rx

 //Threads that will be running at the same time
 Thread OLEDprint_thread;
 Thread analog_read_thread;
 Thread lcd_print_thread;
 Thread refresh_thread;      //thread runs every 1ms to perform PID calculations and update the output duty cycle
 Thread serial_thread;       //thread runs every 1s to send data through serial

//Creating the objects for the OLED and the LCD, The OLED is running a historigram
 char historigram_target[] = "VOLTAGE";
 I2C i2c_OLED(PTB1, PTB0);
 Historigram historigram(historigram_target, OLED_I2C_Address , i2c_OLED);
 I2C i2c_LCD(PTB1, PTB0);
 TextLCD_I2C lcd(&i2c_LCD, LCD_I2C_Address, TextLCD::LCD20x4);

 //current PID control; Parameters: feedback, slope (mA/v), intercept (mA), P, I, D, max sum, P saturation, I saturation, D saturation
 current_PID current_control(&current_sensor, 277.778, 0, 0, 1, 0, 10000000, 20000, 65535, 0);
 //note: the duty cycle is a 16 bit unsigned int, saturation values for P, I and D and be between 0-65535

 //Global variables.
 double voltage;  //This is the voltage read from the testing pin ( currently A0 )
 double average; //average of the above. Uses AVERAGE_BUFFER_SIZE
 double current;
 double duty_cycle;

 //prototypes for the threads that'll be running
 void analog_read(void); // Reads the testing pin (currently A0)
 void OLEDprint(void);  //This program has the historigram
 void lcd_print(void);  //utilized to print more data in real time. Should give it a dedicated averaging function. currently using average of historigram values
 void refresh_duty_cycle();  //function for refresh_thread
 void send_serial();         //function for serial_thread

 int main()
 {
     serial.baud(9600);                        //sets baudrate
     serial_thread.start(send_serial);
     serial_thread.set_priority(osPriorityBelowNormal1);

     historigram.setI2CFreq(I2C_FREQ, i2c_OLED);  //sets i2c frequency

     current = 0;
     duty_cycle = 0;
     voltage = 0;
     average = 0;

     OLEDprint_thread.start(OLEDprint);
     OLEDprint_thread.set_priority(osPriorityAboveNormal1);
     analog_read_thread.start(analog_read);
     lcd_print_thread.start(lcd_print);

     refresh_thread.set_priority(osPriorityRealtime); //gives PID calculation the priority

     //start threads
     refresh_thread.start(refresh_duty_cycle);

     while(1)
     {

     }
 }

 void lcd_print(){
     lcd.cls();
     lcd.printf("Voltage:    Average:\n");
     lcd.printf("DUTY:       Current:");
     while(1){
         lcd.setAddress( 0, 1);
         lcd.printf("%7.3f    %7.3f", voltage, average);
         lcd.setAddress( 0, 3);
         lcd.printf("%7.3f    %7.3f", duty_cycle, current);
         Thread::wait(2000);
     }
 }

void analog_read(){
    int i = 0;
    double bufferForAvg[AVERAGE_BUFFER_SIZE] = {0};
    while(1){

        float output = volt_pin.read();
        output = output*3.3;
        voltage = output;

        bufferForAvg[i] = voltage;
        i++;

        if (i >= AVERAGE_BUFFER_SIZE){

            i = 0;
            double sum = 0;
            for (int j = 0; j < AVERAGE_BUFFER_SIZE ; j++) {
                sum = sum + bufferForAvg[j];
            }

            average = sum / AVERAGE_BUFFER_SIZE;

        }
        Thread::wait(0.1);


    }
}

void OLEDprint(){
    historigram.historigram_initialize();
    while(1){
        historigram.target_value = voltage;
        historigram.run_historigram();
        Thread::wait(500);
    }
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
