/*
 * ArduinoNunchuk.cpp - Improved Wii Nunchuk library for Arduino
 *
 * Copyright 2011-2013 Gabriel Bianconi, http://www.gabrielbianconi.com/
 *
 * Project URL: http://www.gabrielbianconi.com/projects/arduinonunchuk/
 *
 * Based on the following resources:
 *   http://www.windmeadow.com/node/42
 *   http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *   http://wiibrew.org/wiki/Wiimote/Extension_Controllers
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include "ArduinoNunchuk.h"

#define ADDRESS 0x52

bool ArduinoNunchuk::init()
{
  byte error;
  Serial.print(F("- init Nunchuck :"));
  Wire.begin();
  Wire.beginTransmission(ADDRESS);
  error = Wire.endTransmission();
   if (error != 0) // absent
  {
    Serial.println(F(" absent !"));
    bNunchuck = false;
  }
  else
  {
    Serial.println(F(" present"));
    ArduinoNunchuk::_sendByte(0x55, 0xF0);
    ArduinoNunchuk::_sendByte(0x00, 0xFB);
    bNunchuck = true;
    ArduinoNunchuk::update();
  }
  return(bNunchuck);
}

void ArduinoNunchuk::update()
{
  int count = 0;
  int values[6];

  Wire.requestFrom(ADDRESS, 6);

  while (Wire.available())
  {
    values[count] = Wire.read();
    count++;
  }

  ArduinoNunchuk::analogX = values[0];
  ArduinoNunchuk::analogY = values[1];
  ArduinoNunchuk::accelX = (values[2] << 2) | ((values[5] >> 2) & 3);
  ArduinoNunchuk::accelY = (values[3] << 2) | ((values[5] >> 4) & 3);
  ArduinoNunchuk::accelZ = (values[4] << 2) | ((values[5] >> 6) & 3);
  ArduinoNunchuk::zButton = !((values[5] >> 0) & 1);
  ArduinoNunchuk::cButton = !((values[5] >> 1) & 1);

  ArduinoNunchuk::_sendByte(0x00, 0x00);
}

void ArduinoNunchuk::_sendByte(byte data, byte location)
{
  Wire.beginTransmission(ADDRESS);

  Wire.write(location);
  Wire.write(data);

  Wire.endTransmission();

  delay(10);
}