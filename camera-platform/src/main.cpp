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

int requestCount = 0;

void requestEvent()
{
  byte bval;

  switch (requestCount)
  {
  case 0:
    bval = 255;
    break;
  case 1:
    bval = panAngle + 90;
    break;
  case 2:
    bval = tiltAngle + 90;
    break;
  }

  Wire.write(bval);

  requestCount += 1;
  if (requestCount > 2)
    requestCount = 0;
}

void setup()
{
  panAngle = 0;
  tiltAngle = 0;

  Wire.begin(DEVICE_ID);

  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

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