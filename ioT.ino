#include "DHT.h"   
#include <Servo.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "string.h"
/////Air vnet ///////
Servo air_vent;
////////////////////
char str[30];
//////Pump/////////
int relayPin = 12;
///////////////////

///////DHT11/////////
const int DHTPIN = 4;      
const int DHTTYPE = DHT11;  
DHT dht(DHTPIN, DHTTYPE);
//////////////////////

////////Metan sensor/////////
int methaneSensorPin = A1;
/////////////////////////////

///////Temperature sensor//////
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float compost_temp;
///////////////////////////////

////////Humid sensor////////////
int Humid_sensor_pin = A0;
float compost_humid;
int analog;
int average_analog;
////////////////////////////////

void setup() {
  Serial.begin(9600);
  pinMode(methaneSensorPin, INPUT);
  pinMode(Humid_sensor_pin,INPUT);
  dht.begin(); 
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  air_vent.attach(9); 
  air_vent.write(0);
}
int data;
int count = 0;
int water_pump = 0;
int vent = 0;
void loop() {
  receive_data();
  if (count == 0){
    send_data();
    count = 10;
  }
  count--;
  delay(1000);
}

/////////////////////////////////////////////////////
void get_compost_Temp(){
  do{
    DS18B20.requestTemperatures();
    compost_temp = DS18B20.getTempCByIndex(0);
    if (compost_temp == (-127)){
        delay(100);
    }
  }
  while(compost_temp == (-127.0));
}
/////////////////////////////////////////////////

////////////////////////////////////////////////
void get_compost_humid(){
  for (int i = 0; i<= 9; i++){
      analog += analogRead(Humid_sensor_pin);
  }
  average_analog = analog/10;
  int humid_value = map(average_analog,0,1023,0,100);
  compost_humid = 100-humid_value;
  analog = 0;
}
////////////////////////////////////////////////

///////////////////////////////////////////////
double getMethanePPM() {
  float sensorValue = analogRead(methaneSensorPin);
  double ppm = 10.938 * exp(1.7742 * (sensorValue * 5.0 / 4095));
  if (ppm < 0) {
    ppm = 0;
  }
  return ppm;
}
//////////////////////////////////////////////

///////////////////////////////////////////////
void send_data(){
  get_compost_Temp();
  get_compost_humid();   
  float temp_ambient = dht.readTemperature(); 
  float humid_ambient = dht.readHumidity();
  Serial.print(temp_ambient);
  Serial.println();
  double metan = getMethanePPM();
  String data_compost_temp = "!1:TEMP:"+String(compost_temp)+"#";
  String data_compost_humid = "!1:HUMI:"+String(compost_humid)+"#";
  String data_metan = "!1:METAN:"+String(metan)+"#";
  String data_ambient_temp = "!1:AM_TEM:"+String(temp_ambient)+"#";
  Serial.print(data_compost_temp);
  Serial.print(data_compost_humid);
  Serial.print(data_metan);
  Serial.print(data_ambient_temp);
}
////////////////////////////////////////////////////
String inputstring = "";
bool receive_complete = false;
void receive_data(){
  while(Serial.available()){
    char inchar = (char)Serial.read();
    inputstring += inchar;
   
    if (inchar == '#'){
       receive_complete = true;
    }
    if (receive_complete == true){
      data = inputstring[inputstring.length()-2]-48;
      inputstring = "";
      receive_complete = false;
    }
    if (data == 0 && vent == 1){
        air_vent.write(180);
        vent = 0;
    }
    else if (data == 1 && vent ==0){
       air_vent.write(180);
       vent = 1;
    }
    else if (data == 3 && water_pump < 5) {
        digitalWrite(relayPin, HIGH);
    }
    else if (data == 2 || water_pump >= 5){
      digitalWrite(relayPin, LOW);
    }
    water_pump++;
    if (water_pump > 100){
      water_pump = 0;
    }
  }
}
