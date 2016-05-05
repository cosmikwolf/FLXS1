#include <ArduinoJson.h>

#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN					  2
File saveData;

void setup(){
	Serial.begin(115200);
	delay(1000);
	Serial.println("Initializing Test Script");

	SPI.begin();
	SPI.setMOSI(11);
	SPI.setSCK(13);

  if (!SD.begin(SD_CS_PIN)){
    Serial.println("SD Card initialization failed!");
    //return;
  }

  if (SD.exists("jsonTest.txt")) {
  	SD.remove("jsonTest.txt");
    Serial.println("jsonTest.txt existed. deleted and created new.");
  } 

  saveData = SD.open("jsonTest.txt", FILE_WRITE);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["sensor"] = "gps";
  root["time"] = 1351824120;

  JsonArray& data = root.createNestedArray("data");
  data.add(double_with_n_digits(48.756080, 6));
  data.add(double_with_n_digits(2.302038, 6));

  root.printTo(saveData);

  saveData.close();

  Serial.println("Created file and Saved test JSON data");
  delay(1000);

//SAVING 
  saveData = SD.open("jsonTest.txt");
  if (saveData) {
    Serial.println("jsonTest.txt:");
    
    // read from the file until there's nothing else in it:
    while (saveData.available()) {
    	Serial.write(saveData.read());
    }
    // close the file:
    saveData.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening jsonTest.txt");
  }


// READING AND JUST PRINTING
  saveData = SD.open("jsonTest.txt");

  JsonObject& root2 = jsonBuffer.parseObject(saveData.read());
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  } 



// READING THEN PARSING
  Serial.println("printing values that have been parsed!!");
  const char* sensor = root2["sensor"];
  long time = root["time"];
  double latitude = root2["data"][0];
  double longitude = root2["data"][1];

  // Print values.
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);


}


void loop(){

}


