#include "MQTT.h"

/* Testcomment */

void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192,168,43,35 };
MQTT client(server, 1883, callback);
byte bytebuffer[100];

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    if (message.equals("RED")) {
        RGB.color(255, 0, 0);

        String message = "RED";
        client.publish("sparkcore/debug",message);
    }
    else if (message.equals("GREEN")) {
        RGB.color(0, 255, 0);

        String message = "GREEN";
        message.getBytes(bytebuffer, 6);
        client.publish("sparkcore/debug",bytebuffer, 6);
    }

    else if (message.equals("BLUE")) {
        RGB.color(0, 0, 255);

        String message = "BLUE";
        message.getBytes(bytebuffer, 5);
        client.publish("sparkcore/debug",bytebuffer, 5);
    }

    else
        RGB.color(255, 255, 255);
    delay(1000);
}

void setup() {
    /* Init button */
    pinMode(D0, INPUT);
    pinMode(A1, INPUT);

    RGB.control(true);

    // connect to the server
    client.connect("spark_client");

    // publish/subscribe
    if (client.isConnected()) {
        String message = "SparkCore Connected";
        message.getBytes(bytebuffer, 20);
        client.publish("sparkcore/debug",bytebuffer, 20);
        client.subscribe("sparkcore/RGBled");
        client.publish("sparkcore/button",bytebuffer, 20);
    }
}

int pirValue;
void loop() {
    if (client.isConnected())
        client.loop();

    /* Check button value */
    if (digitalRead(D0)) {
        client.publish("sparkcore/button", "test_measurement2 test_key=123");
        while(digitalRead(D0));
        delay(10);
    }

    /* Read analog value */
    /* pirValue = analogRead(A1); */
    /* delay(500); */
    /* client.publish("analogread", String(pirValue)); */
}
