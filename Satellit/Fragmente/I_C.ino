// HIH_6130_1  - Arduino
// 
// Arduino                HIH-6130
// SCL (Analog 5) ------- SCL (term 3)
// SDA (Analog 4) ------- SDA (term 4)
//
// Note 2.2K pullups to 5 VDC on both SDA and SCL
//
// Pin4 ----------------- Vdd (term 8) 
//
// Illustrates how to measure relative humidity and temperature.
//
// copyright, Peter H Anderson, Baltimore, MD, Nov, '11
// You may use it, but please give credit.  
// Hier wird die Schaltung programmiert, die mit dem Command Mode arbeitet, die ohne wir dim GAA Skript programmiert.
    
#include <Wire.h> //I2C library

byte fetch_humidity_temperature(unsigned int *p_Humidity, unsigned int *p_Temperature);
void print_float(float f, int num_digits);

#define TRUE 1
#define FALSE 0

void setup(void)
{
   Serial.begin(9600);
   Wire.begin();
   pinMode(4, OUTPUT);
   digitalWrite(4, HIGH); // this turns on the HIH6130
   delay(5000);
   Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>");  // just to be sure things are working
}
    
void loop(void)
{
   byte _status;
   unsigned int H_dat, T_dat;
   float RH, T_C;
   
   while(1)
   {
      _status = fetch_humidity_temperature(&H_dat, &T_dat);
      
      switch(_status)
      {
          case 0:  Serial.println("Normal.");
                   break;
          case 1:  Serial.println("Stale Data.");
                   break;
          case 2:  Serial.println("In command mode.");
                   break;
          default: Serial.println("Diagnostic."); 
                   break; 
      }       
    
      RH = (float) H_dat * 6.10e-3;
      T_C = (float) T_dat * 1.007e-2 - 40.0;

      print_float(RH, 1);
      Serial.print("  ");
      print_float(T_C, 2);
      Serial.println();
      delay(1000);
   }
}

byte fetch_humidity_temperature(unsigned int *p_H_dat, unsigned int *p_T_dat)
{
      byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
      unsigned int H_dat, T_dat;
      address = 0x27;;
      Wire.beginTransmission(address); 
      Wire.endTransmission();
      delay(100);
      
      Wire.requestFrom((int)address, (int) 4);
      Hum_H = Wire.read();
      Hum_L = Wire.read();
      Temp_H = Wire.read();
      Temp_L = Wire.read();
      Wire.endTransmission();
      
      _status = (Hum_H >> 6) & 0x03;
      Hum_H = Hum_H & 0x3f;
      H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
      T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
      T_dat = T_dat / 4;
      *p_H_dat = H_dat;
      *p_T_dat = T_dat;
      return(_status);
}
   
void print_float(float f, int num_digits)
{
    int f_int;
    int pows_of_ten[4] = {1, 10, 100, 1000};
    int multiplier, whole, fract, d, n;

    multiplier = pows_of_ten[num_digits];
    if (f < 0.0)
    {
        f = -f;
        Serial.print("-");
    }
    whole = (int) f;
    fract = (int) (multiplier * (f - (float)whole));

    Serial.print(whole);
    Serial.print(".");

    for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
    {
         d = fract / pows_of_ten[n];
         Serial.print(d);
         fract = fract % pows_of_ten[n];
    }
}     
