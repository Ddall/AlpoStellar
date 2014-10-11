/**
  * CLES FACIL
  * Ballon 2014
  * Datalogger - Pressure, Temp, Hygro (I2C). 
  * Outputs to csv file
*/
#include <SD.h>
#include <Wire.h>

// PIN DEFINITION
#define CS_PIN 53
#define RESET_PIN 13
#define TEMP_A_PIN 0
#define TEMP_B_PIN 1
#define PRESSURE_PIN 2
#define humidityOFF 36

#define REPEAT_TEMP1_PIN 8
#define REPEAT_TEMP2_PIN 9
#define REPEAT_PRESSURE_PIN 10

// CONST DEFINITION
#define REFRESH_RATE 1000

// VARIABLE DEFINITION
int hygroTemp = 0;
float hygroTempFloat = 0;
float hygro = 0;

int analogTemp1 = 0;
int analogTemp2 = 0;
int pressure = 0;

int tmpHygro = 0;
int tmpTemp1 = 0;
int tmpTemp2 = 0;

/**
  * SETUP
  */
void setup(){
  // RESET
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  
  // ANALOG SENSORS <<-- USEFULL?
  // pinMode(TEMP_A_PIN, INPUT);
  // pinMode(TEMP_B_PIN, INPUT);
  // pinMode(PRESSURE_PIN, INPUT);
  
  // HYGRO
  Wire.begin(); // INIT I2C
  pinMode(humidityOFF, OUTPUT);
  digitalWrite(humidityOFF, 0);
  
  // END SENSORS --
  
  Serial.begin(9600);
  Serial.println("Initializing Card");
  // START SD
  pinMode(CS_PIN, OUTPUT);
  if (!SD.begin(CS_PIN)){
      Serial.println("Card Failure");
      return;
      reset(); 
  }
  
  Serial.println("Card Ready");
  
  //Write Log File Header
  File logFile = SD.open("log.csv", FILE_WRITE);
  if (!logFile){
    Serial.println("Couldn't open log file");
	return;
    reset(); // TODO CHANGE TO RESET
  }
  
} // END SETUP

// EXEC
void loop(){
   
  // READ HYGRO
  Wire.requestFrom(0x27, 4);    // request 4 bytes from slave device 0x27
  if(4 <= Wire.available()){
    tmpHygro = Wire.read();
    tmpHygro = (tmpHygro & 0b00111111) << 8;
    tmpHygro |= Wire.read();
    tmpTemp1 = Wire.read(); // receive a byte as character
    tmpTemp2 = Wire.read();
    
    // GET DATA
    hygroTemp = (tmpTemp1 << 6) | (tmpTemp2 >> 2);
    hygroTempFloat = float(hygroTemp / (pow(2, 14) - 2) * 165 - 40);
    hygro = float(tmpHygro / (pow(2, 14) - 2) * 100);
  }

  // ANALOG SENSORS
  analogTemp1 = analogRead(TEMP_A_PIN);
  analogTemp2 = analogRead(TEMP_B_PIN);
  pressure = analogRead(PRESSURE_PIN);
  
  
  /**
    * OUTPUT TO CSV
    * hygroTemp ; hygroTempFloat ; hygro ; analogTemp1 ; analogTemp2 ; pressure
  */
  String dataString = "\"" + String(hygroTemp) + "\",\"" + String(hygroTempFloat) +"\",\""+ String(hygro) + "\",\"" + String(analogTemp1) + "\",\"" + String(analogTemp2) + "\",\"" + String(pressure) + "\"";
  
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  if (logFile){
    logFile.println(dataString);
    logFile.close();

    Serial.println(dataString); 
    
  }else{
    Serial.println("Couldn't open log file");
    return;
    reset();
  }
  
  // RESET HYGRO
  digitalWrite(humidityOFF, 1);
  delay(100);
  digitalWrite(humidityOFF, 0);
  
  
  // REPEAT ANALOGS
  analogWrite(REPEAT_TEMP1_PIN, analogTemp1);
  analogWrite(REPEAT_TEMP2_PIN, analogTemp2);
  analogWrite(REPEAT_PRESSURE_PIN, pressure);
  
  // REFRESH RATE
  delay(REFRESH_RATE);
} // END LOOP


void reset(){
  digitalWrite(RESET_PIN, HIGH);
}

