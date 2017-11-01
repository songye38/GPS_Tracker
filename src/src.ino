#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
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
const int chipSelect = 4;



SoftwareSerial mySerial(3, 2);


Adafruit_GPS GPS(&mySerial);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


void setup()  
{
  Serial.begin(9600);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);
  strip.begin();
  strip.show(); 
  set_green_pin(0);
  initialize_sd();
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
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

uint32_t timer = millis();
void loop()                     // run over and over again
{
    read_gps_write_to_sd();
}

int check_gps()
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
   
    if (GPS.fix) {
      set_green_pin(1);
      return 1;
    }
    else 
    {
      set_blue_pin(1);
      return 0;
    }
  }
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


void initialize_sd()
{
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    set_red_pin(2);
    return;
  }
  else 
  {
     Serial.println("initialization done.");
     set_green_pin(2);
  }
}

void read_gps_write_to_sd()
{
  String dataString = "";
  String latitude;
  String logitude;
  String date;
  String time;
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
    
    time = hour + ":" + minu + ":" + sec;
//    dataString += time;
//    dataString +=",";
    //strcpy(fTime, time.c_str());
    // 시분초를 HH:MM:SS형태로 바꿔주고, String값을 char*로 바꿔주어 LCD에 출력할 수 있도록 합니다.
    
    //delete [] fTime;
    // 시분초를 LCD에 출력해 줍니다.
    
    String day = String(GPS.day);
    String month = String(GPS.month);
    String year = String(GPS.year);
    
    date = "20" + year + "/" + month + "/" + day;
   
    char *fDate = new char[date.length() + 1];
//     date = fDate;
//     dataString += date;
//     dataString +=",";
     
    //strcpy(fDate, date.c_str());
    // 년월일를 YY/MM/DD형태로 바꿔주고, String값을 char*로 바꿔주어 LCD에 출력할 수 있도록 합니다.

 
    delete [] fDate;
    // 년월일을 LCD에 출력해 줍니다.

    if (GPS.fix) {
      set_green_pin(1);
      dataString += time;
      dataString +=",";
      dataString += date;
      dataString +=",";
//      char lat[20];
//      char loc[20];
//      dtostrf(GPS.latitude, 9, 4, lat);
//      dtostrf(GPS.longitude, 9, 4, loc);
      // dtostrf는 GPS.latitude와 GPS.longitude의 값(float값)을 Char* 로 바꿔줍니다.

//      lat[4] = lat[3];
//      lat[3] = lat[2];
//      lat[2] = ' ';
//            
//      loc[5] = loc[4];
//      loc[4] = loc[3];
//      loc[3] = ' ';

      
      // GPS.latitude와 GPS.longitude값을 받아오면 소숫점 자리가 4번째 자리에 찍혀 있습니다.
      // latitude = 3728,8640, longitude = 12700.8960
      // 이것을 소수점 두번째 자리에 찍는 작업입니다.      
       dataString += GPS.latitude;
       dataString +=",";
       dataString += GPS.longitude;

    Serial.println("..........................");
    Serial.println("..........................");
    Serial.println(dataString);   //이렇게 해주면 값은 제대로 찍혀 나온다.. 다만 sd카드에 저장이 안된다
    Serial.println("..........................");
    Serial.println("..........................");


      File dataFile = SD.open("file.csv", FILE_WRITE);
      if (dataFile) 
      {
        set_green_pin(3);
        dataFile.println(dataString);
        dataFile.close();
      }
      else 
      {
        set_red_pin(3);
      }
    }
    else 
    {
      set_blue_pin(1);
    }
  }
}








