#include <Servo.h>

Servo servo;

const int TEMPERATURE_PIN = A0;

const int DELAY_PER_LOOP = 100;
const int TEMPERATURE_MA_COUNT = 240;
const float SERVO_POS_INCREMENTER = 0.05;
const float TEMPERATURE_TOLERANCE = 0.4;
// 1 to the right, -1 to the left
const int INC_TEMPERATURE_DIR = -1;

float targetTemperatures[24] = {
  72.0, 72.0, 72.0, 72.0,
  72.0, 72.0, 72.0, 72.0,
  72.0, 72.0, 72.0, 72.0,
  72.0, 72.0, 72.0, 72.0,
  72.0, 72.0, 72.0, 72.0,
  72.0, 72.0, 72.0, 72.0
};
long startTime; // seconds
float servoPos = 80;
float temperatureArray[TEMPERATURE_MA_COUNT] = {};
float MATemperature;
int run = 1;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < TEMPERATURE_MA_COUNT; i++) {
    temperatureArray[i] = 70.0;
  }
  MATemperature = 70.0;

  // pin 8, min/max pulse width 900/2100
  servo.attach(8, 900, 2100);
  servo.write(servoPos);
  pinMode(TEMPERATURE_PIN, INPUT);

  startTime = millis() / 1000;
}

void loop() {
  Serial.print("Run: ");
  Serial.print(run);
  // gather temperature
  int read = analogRead(TEMPERATURE_PIN);
  float voltage = read * 5.0 / 1023.0;
  float temperatureInC = (voltage - 0.5) * 100.0;
  float temperatureInF = temperatureInC * 1.8 + 32;
  Serial.print(" Instant F: ");
  Serial.print(temperatureInF);

  if (run == TEMPERATURE_MA_COUNT) {
    run = 0;
  }
  temperatureArray[run] = temperatureInF;
  MATemperature = 0;
  for (int i = 0; i < TEMPERATURE_MA_COUNT; i++) {
    MATemperature += temperatureArray[i];
  }
  MATemperature /= TEMPERATURE_MA_COUNT;

  Serial.print(" Temperature (F): ");
  Serial.print(MATemperature);

  // adjust servo if necessary
  long time = startTime + millis() / 1000;
  long hour = (time / 60 / 60) % 24;
  float currTargetTemperature = targetTemperatures[hour];
  if (MATemperature > currTargetTemperature + TEMPERATURE_TOLERANCE) {
    servoPos -= SERVO_POS_INCREMENTER * INC_TEMPERATURE_DIR;
    Serial.print(" Temperature high");
  } else if (MATemperature < currTargetTemperature + TEMPERATURE_TOLERANCE) {
    servoPos += SERVO_POS_INCREMENTER * INC_TEMPERATURE_DIR;
    Serial.print(" Temperature low ");
  } else {
    Serial.print(" Temperature good");
  }
  if (servoPos >= 160) {
    Serial.print(" Max ");
    servoPos = 160;
  } else if (servoPos <= 0) {
    Serial.print(" Min ");
    servoPos = 0;
  } else {
    Serial.print(" Good");
  }
  Serial.print(" Moving servo to ");
  Serial.println(servoPos);
  servo.write(round(servoPos));

  run++;
  delay(DELAY_PER_LOOP);
}
