#include "MQTT.h"

#define NUMMEASUREMENTS 100
#define MINMEASUREMENTS 3
#define MINMEASUREMENTS2 1
#define THRESHOLD 15
#define THRESHOLD2 15
#define PIR A0
#define DEBUGSERIAL
#define TIMEOUT 7
#define CACHE_SIZE 1

// TODO IMPORTANT A0 max 3.3V protect somehow? resistor?

int pirValue;
int pirValues[NUMMEASUREMENTS];
int counter = 0;
int mean;
int measurements, measurements2;
int timeout_counter;
int cached_values = 0;
int cached_directions = 0;

void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192,168,43,35 };
MQTT client(server, 1883, callback);
byte bytebuffer[100];

void setup() {
#ifdef DEBUGSERIAL
  Serial.begin(9600);
#endif
  RGB.control(true); //enable on-board LED control
  // Signal initializing
  RGB.color(255, 0, 0);

  pinMode(PIR, INPUT);
  delay(2000);
  for(int i = 0; i < NUMMEASUREMENTS; i++) {
    pirValues[i] = analogRead(PIR);
    delay(100);
  }

  // connect to the server
  client.connect("spark_client");

  // Signal ready with green LED
  RGB.color(0, 255, 0);
  delay(100);
  RGB.color(0, 0, 0);
}

void loop() {
  read(true);

  if (pirValue - THRESHOLD > mean) { // If high spike
    while(pirValue - THRESHOLD > mean) { // Ride high wave
      measurements++; // Require minimum measurements
      delay(100);
      read(false);
    }
    // Wait for transition from high to low
    timeout_counter = 0;
    while(pirValue + THRESHOLD2 > mean && pirValue - THRESHOLD < mean) {
      delay(100);
      read(false);
      timeout_counter++;
      if (timeout_counter > TIMEOUT) {
        break;
      }
    }
    // Ride following lower wave 
    // TODO require lower wave or not? Test in real world
    while(pirValue + THRESHOLD2 < mean) {
      RGB.color(0, 0, 255); // Signal motion detected
      measurements2++;
      delay(100);
      read(false);
    }
    if (measurements >= MINMEASUREMENTS && measurements2 >= MINMEASUREMENTS2) {
      send_motion(1);
    }
    measurements = 0;
    measurements2 = 0;
    RGB.color(0, 0, 0);
  }
  else if (pirValue + THRESHOLD < mean) { // If low spike
    while(pirValue + THRESHOLD < mean) { // Ride low wave
      measurements++; // Require minimum measurements
      delay(100);
      read(false);
    }
    // Wait for transition from low to high
    timeout_counter = 0;
    while(pirValue + THRESHOLD > mean && pirValue - THRESHOLD2 < mean) {
      delay(100);
      read(false);
      timeout_counter++;
      if (timeout_counter > TIMEOUT) {
        break;
      }
    }
    // Ride following higher wave
    // TODO require wave or not? Test in real world
    while(pirValue - THRESHOLD2 > mean) {
      RGB.color(0, 255, 0); // Signal motion detected
      measurements2++;
      delay(100);
      read(false);
    }
    if (measurements >= MINMEASUREMENTS && measurements2 >= MINMEASUREMENTS2) {
      send_motion(-1);
    }
    measurements = 0;
    measurements2 = 0;
    RGB.color(0, 0, 0);
  }

  delay(100);
}

boolean send_motion(int direction) {
  cached_values++;
  cached_directions += direction;
  if (cached_values >= CACHE_SIZE) {
    // send motion detected
    if (!client.isConnected()) {
      client.connect("sensor1");
    }
    if (client.isConnected()) { // TODO what if not connected?
      String message = String(cached_directions);
      cached_values = 0;
      cached_directions = 0;
      message.getBytes(bytebuffer, 20);
      client.publish("sensor1/motion" , message);
      /* client.publish("analogprint",bytebuffer, 20); */
      return true;
    }
    RGB.color(255, 0, 0);
    delay(1000);
    return false;
  }
  return true;
}

void read(boolean updateMean) {
  pirValue = analogRead(PIR);
  if (updateMean) {
    pirValues[counter % NUMMEASUREMENTS] = pirValue;
    mean = getMean();
    counter++;
  }
#ifdef DEBUGSERIAL
  Serial.print(pirValue);
  Serial.print('\n');
#endif
}

int getMean() {
  int i, sum;
  for(i = 0, sum = 0; i < NUMMEASUREMENTS; i++) {
    sum += pirValues[i];
  }
  return sum / NUMMEASUREMENTS;
}

/* Not really used but still required */
void callback(char* topic, byte* payload, unsigned int length) {
}
