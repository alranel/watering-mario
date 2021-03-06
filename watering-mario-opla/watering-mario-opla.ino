/* 
  Sketch generated by the Arduino IoT Cloud Thing "Mario"
  https://create.arduino.cc/cloud/things/f4fb20c7-c0ff-4075-a198-460fd416a6ea 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  int raw_moisture;
  bool watering;
  CloudPercentage moisture;
  String log_message;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "arduino_secrets.h"
#include "thingProperties.h"

#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_OplaUI.h>

const int moistPin = A5;
const float waterAmount = 2;  // liters
const float waterSpeed = 0.045; // liters/sec
const float waterTime = waterAmount / waterSpeed;  // seconds
unsigned long startedWatering;

MKRIoTCarrier opla;
CycleWidgetsApp app;
Gauge2_Widget moistureWidget;
Bool_Widget wateringToggleWidget;

void setup() {
  Serial.begin(9600);
  delay(1500);

  // Make sure the pump is not running
  stopWatering();

  // Connect to Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();

  // Configure widgets
  moistureWidget.attachValue(moisture);
  moistureWidget.setTitle("MOISTURE");
  moistureWidget.setRange(0, 100);
  moistureWidget.setDigits(0);
  moistureWidget.setSuffix(" %");
  moistureWidget.setReadOnly(true);

  wateringToggleWidget.attachValue(watering);
  wateringToggleWidget.setTitle("PUMP");
  wateringToggleWidget.onValueChange(onWateringChange);
  
  // Initialize Oplà
  CARRIER_CASE = true;
  opla.begin();

  // Initialize the widget application
  app.begin(opla);
  app.addWidget(moistureWidget);
  app.addWidget(wateringToggleWidget);
}

void loop() {
  ArduinoCloud.update();
  app.loop();
  
  // Read the sensor and convert its value to a percentage 
  // (0% = dry; 100% = wet)
  raw_moisture = analogRead(moistPin);
  moisture = map(raw_moisture, 780, 1023, 100, 0);

  // Set the LED color according to the moisture percentage
  if (moisture > 40) {
    opla.leds.setPixelColor(1, 50, 0 , 0);  // green
  } else if (moisture > 10) {
    opla.leds.setPixelColor(1, 50, 50 , 0); // yellow
  } else {
    opla.leds.setPixelColor(1, 0, 50 , 0);  // red
  }
  opla.leds.show();
  
  // Stop watering after the configured duration
  if (watering && (millis() - startedWatering) >= waterTime*1000) {
    stopWatering();
  }
  
  delay(200);
}

// This function is triggered whenever the server sends a change event,
// which means that someone changed a value remotely and we need to do
// something.
void onWateringChange() {
  if (watering) {
    startWatering();
  } else {
    stopWatering();
  }
}

void startWatering () {
  if (!watering) log_message = "Start watering";
  watering = true;
  startedWatering = millis();
  opla.Relay2.open();
}

void stopWatering () {
  if (watering) log_message = "Stop watering";
  watering = false;
  opla.Relay2.close();
}





