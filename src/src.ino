#include <Adafruit_NeoPixel.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>



/*
 * setting for rgb led 8 bit
 */
 const int PIN = 7;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);
const int onoff =0;
const int gps = 1;
const int sd = 2;
const int tracking =3;
const int battery = 4;

 /*
  * setting for sd card
  */
File myFile;
const int chipSelect = 4;

/*
 * setting for gps module
 */
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

// If using hardware serial (e.g. Arduino Mega), comment out the
// above SoftwareSerial line, and enable this line instead
// (you can change the Serial number to match your wiring):

//HardwareSerial mySerial = Serial1;


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

static const byte ASCII[][5] = // 아스키코드의 문자열을 나열합니다.
{
// {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
//,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
//,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
//,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
//,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
//,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
//,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
//,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
//,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
//,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
//,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
//,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
//,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
//,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
//,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
//,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
// {0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
//,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
//,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
//,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
//,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
//,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
//,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
//,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
//,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
//,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
//,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
//,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
//,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
//,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
//,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
//,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
//,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
//,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
//,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
//,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
//,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
//,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
//,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
//,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
//,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
//,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
//,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
//,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
//,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
//,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
//,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
//,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
//,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
//,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
//,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
//,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
//,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
//,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
//,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
//,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
//,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
//,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
//,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
//,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
//,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
//,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
//,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
//,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
//,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
 {0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f →
};



void setup()  
{

  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
 
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  mySerial.println(PMTK_Q_RELEASE);

  strip.begin();
  strip.show(); 
  initialize_sd();
  set_green_pin(0);
  
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}


void loop()                     
{
  read_gps_data();
}

void initialize_sd()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    set_red_pin(2);
    return;
  }
  Serial.println("initialization done.");
  set_green_pin(2);
}

/*
 * code for rgb led 8bit strip
 */

void set_red_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(255,0,0);
  strip.setPixelColor(pinNum,c);
  strip.show();
}
void set_blue_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(0,0,255);
  strip.setPixelColor(pinNum,c);
  strip.show();
}
void set_green_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(0,255,0);
  strip.setPixelColor(pinNum,c);
  strip.show();
}
void set_yellow_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(255,255,0);
  strip.setPixelColor(pinNum,c);
  strip.show();
}


uint32_t timer = millis();
void read_gps_data()
{
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
    String hour = String(GPS.hour+9);
    // 현재 사용하는 라이브러리가 GMT표준시간을 출력합니다.
    // 우리나라는 GMT표준시간보다 9시간 빠르므로 9시간 더해주면 됩니다.
    // 추가로 출력시 24시가 넘어가면 24시 이상값이 나오므로 맞게 편집해 주셔야 합니다.
    String minu = String(GPS.minute);
    String sec = String(GPS.seconds);
    
    String time = hour + ":" + minu + ":" + sec;
    char *fTime = new char[time.length() + 1];
    strcpy(fTime, time.c_str());
    // 시분초를 HH:MM:SS형태로 바꿔주고, String값을 char*로 바꿔주어 LCD에 출력할 수 있도록 합니다.
   
    delete [] fTime;
    // 시분초를 LCD에 출력해 줍니다.
    
    String day = String(GPS.day);
    String month = String(GPS.month);
    String year = String(GPS.year);
    
    String date = "20" + year + "/" + month + "/" + day;
    char *fDate = new char[date.length() + 1];
    strcpy(fDate, date.c_str());
    // 년월일를 YY/MM/DD형태로 바꿔주고, String값을 char*로 바꿔주어 LCD에 출력할 수 있도록 합니다.

    delete [] fDate;
    // 년월일을 LCD에 출력해 줍니다.
    
    
    if (GPS.fix) {
      set_green_pin(1);
      char lat[20];
      char loc[20];
      dtostrf(GPS.latitude, 9, 4, lat);
      dtostrf(GPS.longitude, 9, 4, loc);
      // dtostrf는 GPS.latitude와 GPS.longitude의 값(float값)을 Char* 로 바꿔줍니다.

      lat[4] = lat[3];
      lat[3] = lat[2];
      lat[2] = ' ';

      
      loc[5] = loc[4];
      loc[4] = loc[3];
      loc[3] = ' ';
     
      
      // GPS.latitude와 GPS.longitude값을 받아오면 소숫점 자리가 4번째 자리에 찍혀 있습니다.
      // latitude = 3728,8640, longitude = 12700.8960
      // 이것을 소수점 두번째 자리에 찍는 작업입니다.    
    
      // latitude값과 longitude값을 LCD에 출력합니다.
    }
    else {
      set_blue_pin(1);
    }
  }
  
}



