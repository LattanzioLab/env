//Soil moisture, soil temperature, air temperature, and air relative humidity sensor for research on salamander ecology and physiology.
//License: CC-BY-NC-SA ; see https://github.com/LattanzioLab/env/tree/main/ManderMeter and main LattanzioLab repository for more info. 

/*Load required libraries. Note: your library use may vary depending on sensors selected; 
this configuration assumes an analog 2-prong soil moisture sensor, DHT22 air temperature/moisture sensor, and DS18B20 waterproof 
temperature sensor. The screen is a 1.8" TFT display, and the microcontroller is an Arduino Pro Mini. A rocker switch provides
power connection to the battery (STL files designed for a 9V battery, but any supply 3.3V or higher will do). A small arcade
pushbutton switch on the front of the device allows the user to reset initial readings and take new readings. */
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include "Adafruit_ILI9341.h"   

// TFT inputs
#define TFT_CS        10
#define TFT_RST        9 
#define TFT_DC         8

// TFT dimensions
#define FIRST_ROW_Y 16
#define FIRST_ROW_X 16

// Soil Temperature Sensor (DS18B20) is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4

#define DHTPIN 2     // Digital pin connected to the DHT Air temperature/humidity sensor
#define DHTTYPE DHT22   // Models could be: DHT22  (AM2302), AM2321 


// Setup a oneWire instance to communicate with Soil Temperature Sensor
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature library
DallasTemperature sensors(&oneWire);
// Store device address
DeviceAddress insideThermometer;

//Set pin for pushbutton to reset loop/take new readings
int buttonPin = 6;

//Set pin for soil moisture sensor readings
int soilPower = 7;

//Define TFT pins
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE); //defining DHT sensor

void setup() {
  Serial.begin(9600); //use serial connection for troubleshooting; OK to still initialize w/o it. 
  sensors.begin();
  dht.begin();
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  sensors.setResolution(insideThermometer, 12);
  pinMode(buttonPin,INPUT);
  //insideThermometer = { 0x28, 0x1D, 0x87, 0x95, 0xF0, 0x01, 0x3C, 0xA1 }; //Can use this to specify address for thermometer if needed

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S display, black tab
  tft.fillScreen(ST7735_BLACK); 

}
// function to print the temperature to the screen
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  //Commented material used to troubleshoot; lets the user know via Serial Monitor whether the temperature sensor is disconnected
  //if(tempC == DEVICE_DISCONNECTED_C) 
  //{
  //  Serial.println("Error: Could not read temperature data");
  //  return;
  //}
  tft.print(tempC);
  //Uncomment the next line, and comment out the previous line, if you wish to show temperature data in F, not C. 
  //tft.print(DallasTemperature::toFahrenheit(tempC));
}
//main loop
void loop() {
  //I prefer visual organization for showing data; modify or delete the function printRectangle() as desired for your needs. 
  printRectangle();
  delay(20);

  sensors.requestTemperatures(); // Send the command to get temperatures from DS18B20
  //Get data from DHT22:
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //get data from soil moisture sensor
  int capread = analogRead(A3);
  //float capval = avgRead(capread); //If you want average readings, uncomment this line. 
  float per = avgRead() - 295;
  float per2 = 1 - (per / 728);
  float perH20 = per2 * 100;
  //if (perH20 > 100) perH20 = 100; 
  //float test = (avgRead() - 295) / 1023.0;
  

  //Setting up the display colors, text properties, and pixel locations to provide readings:
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setRotation(2);
  tft.setCursor(30,150);
  tft.print(F("ManderMeter!")); 
  tft.setCursor(18,30);
  tft.setTextSize(2);
  tft.print(t); tft.print(F(" Ta")); // Ta = Air Temperature (Celsius)
  tft.println();
  tft.setCursor(18,55);
  tft.print(h); tft.print(F("% H")); // %H = Relative Humidity (%)
  tft.setCursor(18,95);
  printTemperature(insideThermometer); tft.print(F(" Ts")); // Ts = Soil Temperature (Celsius)
  tft.setCursor(18,120);
  tft.print(avgRead()); tft.print(F("% Sm")); // Sm = Soil Moisture (%)
  tft.setTextSize(2);

  //This While loop tells the system to now wait until the arcade button is pressed. Any small delay will do. 
  while (digitalRead(buttonPin) == LOW) {delay(10);}
  //once the button is pressed, fill the screen black and restart the void loop function, again stopping at the while loop. 
  tft.fillScreen(ST7735_BLACK);
}

//function to average 50 readings from an analog sensor, assuming the max reading is 1023 counts. 
float avgRead(){
  int x;
  int numberOfReadings = 50;
  float value = 0.0; 
  float percnt = 0.0;
  float wholeperc = 0.0;
  for(x = 0 ; x < numberOfReadings; x++){
    delay(10);
    value = value + analogRead(A3);
    delay(2);
  }
  value = value / 50;
  percnt = 1 - value / 1023;
  wholeperc = percnt * 100;
  return(wholeperc);
}

//Function to produce the text display rectangles. 
void printRectangle()
{
 tft.setRotation(2);
 //Top rectangle
 tft.fillRect(5, 20, 120, 60, ILI9341_WHITE);
 tft.fillRect(10, 25, 110, 50, ILI9341_DARKGREY);

//Bottom rectangle
 tft.fillRect(5, 85, 120, 60, ILI9341_GREEN);
 tft.fillRect(10, 90, 110, 50, ILI9341_DARKGREY);
}