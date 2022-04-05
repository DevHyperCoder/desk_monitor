#include <SoftwareSerial.h>

// Bluetooth Module
const int rx = 10;
const int tx = 11;

// Ultrasonic sensor
const int trig = 7, echo = 6;

// Too Close detection
const int tooClose = 5;
int treshold = 10; // in centimeters

SoftwareSerial BT(rx,tx);

void setup() {
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(trig,OUTPUT);
    pinMode(echo,INPUT);
    pinMode(tooClose,OUTPUT);

    // Begin USB Serial
    Serial.begin(9600);
    BT.begin(9600);

    Serial.println("Desk monitor");
    BT.println("Desk Monitor");
}

void loop()
{
    // Reading bluetooth data if available
    if (BT.available()) {
        auto data = BT.readString();

        // tres:45

        if (data.startsWith("tres:")) {
            String num = data.substring(5);
            treshold = num.toInt();
            BT.println("Set treshold to "+treshold);
        }
    }


    Serial.println("Measuring data");

    auto distances = 0;
    auto numMeasure = 0;

    for(auto i =0; i<10; i++) {
        auto distance = getDistance();

        if (distance >= 200) {
            flashLed(100); // Indicate bad value.
            continue;
        }

        // Too Close detection
        if (distance <= treshold) {
            digitalWrite(tooClose,HIGH);
            BT.println("TOO CLOSE: "+distance);
        } else {
            digitalWrite(tooClose,LOW);
        }

        distances += distance;
        numMeasure ++;
        delay(100);
    }

    /*Serial.println("Distances: "+distances);*/
    /*Serial.println("numMeasure: "+numMeasure);*/

    auto distance = distances / numMeasure;

    Serial.println(distance);
    BT.println(distance);
}

void flashLed(int ms) {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(ms);
    digitalWrite(LED_BUILTIN,LOW);
    delay(ms);
}

/*
* Read distance (in centimeters)
*/
long getDistance() {
    // Clear trig pin
    digitalWrite(trig, LOW);
    delayMicroseconds(2);

    // Send pulse to trig
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    // Get pulse time for echo
    long duration = pulseIn(echo, HIGH);

    return duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
}
