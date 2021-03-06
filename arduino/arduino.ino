#include <SoftwareSerial.h>

void flashLed(int ms,int pin);
long getFilteredDistance();
void detect(int distance);

// Bluetooth Module
const int rx = 10;
const int tx = 11;

// Ultrasonic sensor
const int trig = 7, echo = 6;

// Too Close detection
const int tooClose = 5;
const int goodDistance = 4;
const int tresholdUpdate = 3;

int treshold = 10; // in centimeters

SoftwareSerial BT(rx,tx);

void setup() {
    pinMode(LED_BUILTIN,OUTPUT);

    pinMode(trig,OUTPUT);
    pinMode(echo,INPUT);

    pinMode(tooClose,OUTPUT);
    pinMode(goodDistance,OUTPUT);
    pinMode(tresholdUpdate,OUTPUT);

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

        if (data.startsWith("tres:")) {
            String num = data.substring(5);
            treshold = num.toInt();

            digitalWrite(tooClose,LOW);
            digitalWrite(goodDistance,LOW);
            flashLed(1000, tresholdUpdate);
        }
    }

    auto distance = getFilteredDistance();
    Serial.println(distance);
    BT.println(distance);
}

void flashLed(int ms,int pin) {
    digitalWrite(pin,HIGH);
    delay(ms);
    digitalWrite(pin,LOW);
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

void detect(int distance) {
    // Too Close detection
    if (distance <= treshold) {
        digitalWrite(tooClose,HIGH);
        digitalWrite(goodDistance,LOW);
    } else {
        digitalWrite(tooClose,LOW);
        digitalWrite(goodDistance,HIGH);
    }
}

long getFilteredDistance() {

    auto distances = 0;
    auto numMeasure = 0;

    for(auto i =0; i<10; i++) {
        flashLed(100, LED_BUILTIN);
        auto distance = getDistance();


        if (distance >= 200) {
            flashLed(600, LED_BUILTIN); // Indicate bad value.
            continue;
        }

        detect(distance);

        distances += distance;
        numMeasure ++;
    }

    auto distance = distances / numMeasure;
    return distance;
}
