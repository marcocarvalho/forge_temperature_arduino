#include <max6675.h>

//Include the SoftwareSerial library
#include "SoftwareSerial.h"

//Create a new software  serial
SoftwareSerial bluetooth(2, 3); //TX, RX (Bluetooth)
  
const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

char lastCommand[200];

int idx;
int avail;

String str;

String errTooLong = "+Err:001:too long";
String errNotACommand = "+Err:002:not a command";
String okVersion = "+Ok:1.0";
String okPing = "+Pong!";
String okHelp = "+Ok\nCommands: version? | ping? | temp? | stream?\nResponse begins with + and ends with carrie return character.";

int thermoDO = 6;
int thermoCS = 5;
int thermoCLK = 4;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

double lastReading = 0;

bool okCommand = true;

void setup() {
  //Initialize the software serial
  bluetooth.begin(9600);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("connected!");
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  idx = 0;
}

void loop() {
  // see if there's incoming serial data:
  avail = bluetooth.available();
  if (avail > 0) {
    Serial.print("Size: ");
    Serial.println(avail);
    // read the oldest byte in the serial buffer:
    while((incomingByte = bluetooth.read()) != -1){
      lastCommand[idx] = incomingByte;
      idx ++;
      if(idx > 190) {
        Serial.println(errTooLong);
        bluetooth.println(errTooLong);
        idx = 0;
        break;
      }
      delay(10);
    }
    lastCommand[idx] = '\0';

    Serial.write(lastCommand, idx - 1);
      
    if (lastCommand[idx - 1] == '?') {
      okCommand = false;
      str = String(lastCommand);
      if(str == "version?"){
        okCommand = true;
        bluetooth.println(okVersion);
        Serial.println(okVersion);
      }

      if(str  == "ping?"){
        okCommand = true;
        bluetooth.println(okPing);
        Serial.println(okPing);
      }

      if(str == "temp?"){
        okCommand = true;
        lastReading = thermocouple.readCelsius();
        bluetooth.print("+");
        bluetooth.print(lastReading);
        bluetooth.println("C");
        Serial.print("+");
        Serial.print(lastReading);
        Serial.println("C");
      }

      if(str == "stop?"){
        okCommand = true;
        bluetooth.println("+Ok");
        Serial.println("+Ok");  
      }

      if(str == "stream?"){
        okCommand = true;
        while(bluetooth.available() <= 0){
          lastReading = thermocouple.readCelsius();          
          bluetooth.print("+");
          bluetooth.print(lastReading);
          bluetooth.println("C");
          Serial.print("+");
          Serial.print(lastReading);
          Serial.println("C");
          delay(1000);
        }
      }

      if(str == "help?")
      {
        okCommand = true;
        bluetooth.println(okHelp);
        bluetooth.println(okHelp);
      }

      if(!okCommand){
        bluetooth.println(errNotACommand);
        Serial.println("+Err:002:not a command");
      }
      
      delay(100);
      lastCommand[0] = '\0';
      idx = 0;
    }
  }
}

