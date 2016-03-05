#define NUMMEASUREMENTS 100
#define MINMEASUREMENTS 3
#define THRESHOLD 30
#define PIR A0
#define DEBUGSERIAL
#define TIMEOUT 7

// TODO IMPORTANT A0 max 3.3V protect somehow? resistor?

int pirValue;
int pirValues[NUMMEASUREMENTS];
int counter = 0;
int mean;
int measurements;
int timeout_counter;

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

  // Signal ready with green LED
  RGB.color(0, 255, 0);
  delay(100);
  RGB.color(0, 0, 0);
}

void loop() {
  read(true);

  if (pirValue - THRESHOLD > mean) { // If high spike
    RGB.color(0, 0, 255);
    while(pirValue - THRESHOLD > mean) { // Ride high wave
      measurements++; // Require minimum measurements
      delay(100);
      read(false);
    }
    // Wait for transition from high to low
    timeout_counter = 0;
    while(pirValue + THRESHOLD > mean && pirValue - THRESHOLD < mean) {
      delay(100);
      read(false);
      timeout_counter++;
      if (timeout_counter > TIMEOUT) {
        break;
      }
    }
    // Ride following lower wave 
    // TODO require lower wave or not? Test in real world
    while(pirValue + THRESHOLD < mean) {
      delay(100);
      read(false);
    }
    if (measurements >= MINMEASUREMENTS) {
      // TODO add mqtt code here for example
      Particle.publish("motion", "blue");
    }
    RGB.color(0, 0, 0);
    measurements = 0;
    delay(1000);
  }
  else if (pirValue + THRESHOLD < mean) { // If low spike
    RGB.color(0, 255, 0);
    while(pirValue + THRESHOLD < mean) { // Ride low wave
      measurements++; // Require minimum measurements
      delay(100);
      read(false);
    }
    // Wait for transition from low to high
    timeout_counter = 0;
    while(pirValue + THRESHOLD > mean && pirValue - THRESHOLD < mean) {
      delay(100);
      read(false);
      timeout_counter++;
      if (timeout_counter > TIMEOUT) {
        break;
      }
    }
    // Ride following higher wave
    // TODO require wave or not? Test in real world
    while(pirValue - THRESHOLD > mean) {
      delay(100);
      read(false);
    }
    if (measurements >= MINMEASUREMENTS) {
      // TODO add mqtt code here for example
      Particle.publish("motion", "green");
    }
    RGB.color(0, 0, 0);
    measurements = 0;
    delay(1000);
  }

  delay(100);
}

void read(boolean updateMean) {
  pirValue = analogRead(PIR);
  pirValues[counter % NUMMEASUREMENTS] = pirValue;
  if (updateMean) {
    mean = getMean();
  }
  counter++;
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
