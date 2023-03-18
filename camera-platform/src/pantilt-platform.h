#ifndef PANTILT_PLATFORM_H
#define PANTILT_PLATFORM_H

#include <Arduino.h>
#include <Servo.h>

class PanTiltPlatform
{
public:
    PanTiltPlatform(int panServoPin, int tiltServoPin);
    ~PanTiltPlatform();

    void begin();
    void home();

    void setPanCorrection(float correction);
    void setTiltCorrection(float correction);

    void setPanAngle(float panAngle);
    void setTiltAngle(float tiltAngle);
    
    void pan(float angle);
    void tilt(float angle);
    //void loop();
private:
    int tiltServoPin_;
    int panServoPin_;
    float panCorrection_;
    float panAngle_;
    float tiltCorrection_;
    float tiltAngle_;
    Servo tiltServo;
    Servo panServo;
};

#endif