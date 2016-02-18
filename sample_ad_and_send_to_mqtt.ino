// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"



unsigned long millistime, tx_time = 0;
unsigned long systime;


//Setup MQTT
void callback(char* topic, byte* payload, unsigned int length);

MQTT client("op-en.se", 1883, callback);


//Handler for MQTT messages.
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    //Convert the text to an int and set the servo. 
  

}

unsigned int data[256],ts[256];
unsigned char di;

void sample()
{
    di++;
    data[di] = analogRead(15);
    ts[di] = millis();
    
    if (di == 256)
        di = 0;
    
}


Timer timer(5, sample);




void setup() {
    
    
    /* WiFi.setCredentials("Antons iPhone", "xxxxxx", WPA2, WLAN_CIPHER_AES); */
    /* WiFi.setCredentials("iPhone", "xxxxxx", WPA2, WLAN_CIPHER_AES); */
    
   
    
    //Servo is on pin D0
    //serv.attach(D0);
    //serv.write(0);
    
    //Set pin D5 High to power the sensor
    pinMode(D5, OUTPUT);
    digitalWrite(D5,HIGH);
    
    setADCSampleTime(ADC_SampleTime_3Cycles);
    // connect to the server
    client.connect(String(System.deviceID()));

    // publish/subscribe
    if (client.isConnected()) {
        //Anounce our presense
        client.publish("/device/mysensor","Connected");
        
        //Subscribe to the data
        client.subscribe("/device/mysensor");
    }
    
    Particle.syncTime();
    systime = Time.now();
    millistime = millis();
    
    //for (unsigned char i = 0; i<256;i++)
    //    data[i] = 0;

    timer.start();
    
    delay(5*256);
    
}






long lastlux = 0;


unsigned int last = 0,current = 0;

long lastsample=0;


#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();



void loop() {
    String value(""),msg(""),ts(""),topic("");
    double now;
    unsigned char i0,i1,i2,i3;
    
    //if (millis() - lastSync > ONE_DAY_MILLIS) {
        // Request time synchronization from the Particle Cloud
    //    Particle.syncTime();
     //   lastSync = millis();
//    }
    
    client.loop();
    
    long delta = millis() - tx_time;
    
    if (delta < 1000)
        return; 
    
    i0 = di;
    i1 = (i0-1)%256;
    i2 = (i0-2)%256;
    i3 = (i0-3)%256;
    
    long sum= data[i0] + data[i1] + data[i2] + data[i3];
    
    if (sum < 1)
        sum = 1;
   
    //long part1 = sum * 16384;
    //long part2 = 16384 - sum * 16384;
    long lux = (500 * (16384 - sum)) / (10 * sum);
    
    //At least one update per every 5 seconds and inbetween if more than 10 lux change. 
    if ( delta < 15000 and abs(lux - lastlux) < 10)
        return;
        
    lastlux = lux;
   
    now = systime + (millis()-millistime)/1000.0;
    //value = String(sum);
    //msg = "{ \"value\": " + value + ", \"time\": "+ (ts + now) +"}";
    //client.publish("test/mysensor/monica/raw",msg);
    
    
    
    value = String(lux);
    msg = "{ \"value\": " + value + ", \"time\": "+ (ts + now) +"}";
    topic = "test/mysensor/" + String(System.deviceID()) + "/lux";
    client.publish(topic,msg);
    
    //value = String(i0);
    //msg = "{ \"value\": " + value + ", \"time\": "+ (ts + now) +"}";
    //client.publish("test/mysensor/monica/index",msg);
    
    //unsigned char test = (i0-3)%256;
    
    //value = String( test );
    //msg = "{ \"value\": " + value + ", \"time\": "+ (ts + now) +"}";
    //client.publish("test/mysensor/monica/index3",msg);
    
    tx_time = millis();
        //delay(1000);
        
    

    // publish/subscribe
    if (!client.isConnected()) {
        client.connect(System.deviceID());
        
        // publish/subscribe
        if (client.isConnected()) {
            //Anounce our presense
            client.publish("/device/mysensor","reconnecting");
            
            //Subscribe to the data
            client.subscribe("/device/mysensor");
        }
        
    }
        
    
    
}
