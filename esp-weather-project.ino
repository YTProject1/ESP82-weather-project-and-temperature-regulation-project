#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>


// WiFi credentials
char ssid[] = ".";
char pass[] = "";

// DHT11 settings
#define DHTPIN D5     // DHT11 connected to GPIO14
#define DHTTYPE DHT11 // DHT11 sensor
DHT dht(DHTPIN, DHTTYPE);

// Relay settings
#define RELAY_PIN D6  // Relay connected to GPIO12

// Blynk virtual pins
#define VPIN_TEMPERATURE V1
#define VPIN_HUMIDITY V2
#define VPIN_RELAY V3 // Button control for relay

BlynkTimer timer;
bool relayManualControl = false; // Flag for manual control mode

// Function to send sensor data
void sendSensorData() {
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to Blynk app
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity);

  // Relay control logic (only if not in manual mode)
  if (!relayManualControl) {
    if (temperature > 30) { // Example: turn on relay if temperature > 30°C
      digitalWrite(RELAY_PIN, HIGH); // Activate relay
      Blynk.logEvent("high_temp", "Temperature above 30°C! Fan ON");
    } else if (humidity < 40) { // Example: turn on relay if humidity < 40%
      digitalWrite(RELAY_PIN, HIGH); // Activate relay
      Blynk.logEvent("low_humidity", "Humidity below 40%! Humidifier ON");
    } else {
      digitalWrite(RELAY_PIN, LOW); // Deactivate relay
    }
  }

  // Print data to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}

// Blynk function to manually control the relay
BLYNK_WRITE(VPIN_RELAY) {
  relayManualControl = true; // Enable manual control mode
  int relayState = param.asInt(); // Get value from Blynk button (0 or 1)

  if (relayState == 1) {
    digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
    Serial.println("Relay turned ON manually!");
  } else {
    digitalWrite(RELAY_PIN, LOW); // Turn relay OFF
    Serial.println("Relay turned OFF manually!");
  }
}

void setup() {
  // Debug console
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Ensure relay is off initially

  // Connect to WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set up a timer to send sensor data every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();    // Run Blynk
  timer.run();    // Run timer
}
