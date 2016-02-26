/*
 * Not written for photon
 */

#define numMeasurements 20
#define THRESHOLD 15

int pirValue;
int pirValues[20];
int counter = 0;
int mean;

void setup() {
  int i;
  Serial.begin(9600);
  analogReference(INTERNAL);
  pinMode(A1, INPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  delay(2000);
  for(i = 0; i < numMeasurements; i++) {
    pirValues[i] = analogRead(A1);
    delay(100);
  }
}

int getMean() {
  int i, sum;
  for(i = 0, sum = 0; i < numMeasurements; i++) {
    sum += pirValues[i];
  }
  return sum / numMeasurements;
}

void loop() {
  pirValue = analogRead(A1);
  delay(100);
  Serial.print(pirValue);
  Serial.print('\n');
  
  pirValues[counter % numMeasurements] = pirValue;
  mean = getMean();
  if(pirValue - THRESHOLD > mean) {
    digitalWrite(7, HIGH);
    delay(4000);
    digitalWrite(7, LOW);
  }

  if(pirValue + THRESHOLD < mean) {
    digitalWrite(6, HIGH);
    delay(4000);
    digitalWrite(6, LOW);
  }
  
  counter++;
}
