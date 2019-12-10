#include <Arduino.h>

#define MOTOR_STEPS 200

#define DIR 55
#define STEP 54
#define SLEEP 38

#include "A4988.h"
#define MS1 10
#define MS2 11
#define MS3 12

A4988 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MS1, MS2, MS3);

String cmd;
int homeDegrees;
int currentDegrees;

void setup() {
    Serial.begin(56000);
    Serial.println("started...");
    stepper.begin(8.5); // default RPM
    stepper.setEnableActiveState(LOW);
    stepper.enable();
    stepper.setMicrostep(16);
}

void loop() {
    while (Serial.available() > 0) {
        cmd = Serial.readString();
        if (foundCommand(cmd, "rpm")) {
            float rpm = param(cmd).toFloat();
            Serial.print("setting rpm to: ");
            Serial.println(rpm);
            stepper.setRPM(rpm);
        } else if (foundCommand(cmd, "deg")) {
            int degrees = param(cmd).toInt();
            Serial.print("turning ");
            Serial.print(degrees);
            Serial.println(" degrees");
            rotate(degrees);
        } else if (foundCommand(cmd, "rev")) {
            float turns = param(cmd).toFloat();
            int degrees = (int)(turns * 360);
            Serial.print("turning ");
            Serial.print(turns);
            Serial.println(" times");
            rotate(degrees);
        } else if (foundCommand(cmd, "sto")) {
            Serial.println("stopping");
            stepper.stop();
        } else if (foundCommand(cmd, "fix")) {
            Serial.println("this is now home");
            homeDegrees = currentDegrees;
        } else if (foundCommand(cmd, "hom")) {
            int degrees;
            Serial.println("going home");
            if (currentDegrees > homeDegrees) {
                degrees = (currentDegrees - homeDegrees) * -1;
            } else {
                degrees = homeDegrees - currentDegrees;
            }
            rotate(degrees);
        }  else if (foundCommand(cmd, "pos")) {
            Serial.print("current: ");
            Serial.print(currentDegrees);
            Serial.print(" home: ");
            Serial.println(homeDegrees);
        }
    }
}

bool foundCommand(String incoming, String desired) {
    return incoming.indexOf(desired) > -1;
}

String param(String command) {
    return command.substring(3);
}

void rotate(int degrees) {
    stepper.rotate(degrees);
    currentDegrees = (currentDegrees + degrees) % 360;
}
