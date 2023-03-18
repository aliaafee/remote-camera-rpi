#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ID 0x8

#include "pin-config.h"

#include "pantilt-platform.h"
PanTiltPlatform cameraPlatform(PAN_SERVO_PIN, TILT_SERVO_PIN);

int currentCommand[3];

int panAngle;
int tiltAngle;

void receiveEvent(int howMany)
{
  int i = 0;
  digitalWrite(LED_BUILTIN, HIGH);
  while (Wire.available())
  {
    currentCommand[i] = Wire.read();
    i++;
    if (i == 3)
    {
      int command2 = currentCommand[1] - 90;
      int command3 = currentCommand[2] - 90;

      panAngle = command2;
      tiltAngle = command3;

      i = 0;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
  panAngle = 0;
  tiltAngle = 0;

  Wire.begin(DEVICE_ID);

  Wire.onReceive(receiveEvent);

  cameraPlatform.begin();
  cameraPlatform.home();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  delay(100);

  cameraPlatform.setPanAngle(panAngle);
  cameraPlatform.setTiltAngle(tiltAngle);
}