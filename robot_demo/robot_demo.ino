#define NRF_CE 38
#define NRF_CSN 39
#define JOYSTICK_DEADZONE 5

#define US_PING 14
#define US_ECHO 15

#include "NEET_RF24.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

NEET_RF24 radio(NRF_CE, NRF_CSN, 25);

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);

Servo s;

ControlInput input;

unsigned long last_reading = 0;

void setup(){
  Serial.begin(115200);
  if (!radio.begin()){
    Serial.println("Radio not started");
  } else {
    Serial.println("Radio started");
  }
  AFMS.begin();

  s.attach(10);

  pinMode(US_PING, OUTPUT);
  pinMode(US_ECHO, INPUT);
}

void loop(){
  // If radio input is valid
  if (radio.rxUpdate()){
    input = radio.rxGetInput();
    int left = 0;
    int right = 0;

    if (abs(input.j1PotY) > JOYSTICK_DEADZONE){
      left = input.j1PotY;
    }
    if (abs(input.j2PotX) > JOYSTICK_DEADZONE){
      right = input.j2PotX;
    }
    drive(left + right * 0.4, left - right * 0.4);

    if (millis() < last_reading || (millis() - last_reading) > 100){
      radio.rxSendTelemetry("Distance is: " + String(readUltrasonic()) + " cm\n           ");
      last_reading = millis();
    }

    s.write(map(input.pot, -127, 127, 0, 180));
  } else {
    drive(0, 0);
  }
}

void drive(int left, int right){
  right = -right;
  motor_left->setSpeed(abs(left));
  motor_right->setSpeed(abs(right));

  if (left > 0){
    motor_left->run(FORWARD);
  } else if (left < 0){
    motor_left->run(BACKWARD);
  } else {
    motor_left->run(RELEASE);
  }

  if (right > 0){
    motor_right->run(FORWARD);
  } else if (right < 0){
    motor_right->run(BACKWARD);
  } else {
    motor_right->run(RELEASE);
  }
}

float readUltrasonic(){
  digitalWrite(US_PING, LOW);
  delayMicroseconds(2);
  digitalWrite(US_PING, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_PING, LOW);
  unsigned long delay_us = pulseIn(US_ECHO, HIGH);
  float distance_cm = delay_us * 0.034 / 2;

  return distance_cm; 
}