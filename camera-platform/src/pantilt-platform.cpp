#include "pantilt-platform.h"

PanTiltPlatform::PanTiltPlatform(int panServoPin, int tiltServoPin)
{
    tiltServoPin_ = tiltServoPin;
    panServoPin_ = panServoPin;
    tiltCorrection_ = 0;
    panCorrection_ = 0;
    panAngle_ = 0;
    tiltAngle_ = 0;
}

PanTiltPlatform::~PanTiltPlatform()
{
}

void PanTiltPlatform::begin()
{
    tiltServo.attach(tiltServoPin_);
    panServo.attach(panServoPin_);
}

void PanTiltPlatform::home()
{
    setPanAngle(0);
    setTiltAngle(0);
}

void PanTiltPlatform::setPanAngle(float panAngle)
{
    if (panAngle == panAngle_) {
        return;
    }
    panAngle_ = panAngle;
    //panServo.write(map(panAngle_ + panCorrection_, -90, 90, 0, 180));
    panServo.writeMicroseconds(
        map(panAngle_ + panCorrection_, -90, 90, 500, 2500));
}

void PanTiltPlatform::setTiltAngle(float tiltAngle)
{
    if (tiltAngle == tiltAngle_) {
        return;
    }
    tiltAngle_ = tiltAngle;
    //tiltServo.write(map(tiltAngle_ + tiltCorrection_, 90, -90, 0, 180));
    tiltServo.writeMicroseconds(
        map(tiltAngle_ + tiltCorrection_, -90, 90, 2500, 500));
}

void PanTiltPlatform::pan(float angle)
{
    float newAngle = panAngle_ + angle;

    if (newAngle > 90)
        newAngle = 90;
    if (newAngle < -90)
        newAngle = -90;

    setPanAngle(newAngle);
}

void PanTiltPlatform::tilt(float angle)
{
    float newAngle = panAngle_ + angle;

    if (newAngle > 90)
        newAngle = 90;
    if (newAngle < -90)
        newAngle = -90;

    setTiltAngle(newAngle);
}

void PanTiltPlatform::setPanCorrection(float correction)
{
    panCorrection_ = correction;
}

void PanTiltPlatform::setTiltCorrection(float correction)
{
    tiltCorrection_ = correction;
}
