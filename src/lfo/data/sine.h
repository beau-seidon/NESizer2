/*
  NESIZER

  (c) 2014 - 2015 Johan Fjeldtvedt

  Sine table for LFOs

 */


#pragma once

#include <avr/pgmspace.h>

const int8_t sine_table[] PROGMEM = {
  0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126,
  127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12,
  0, -12, -25, -37, -49, -60, -71, -81, -90, -98, -106, -112, -117, -122, -125, -126,
  -127, -126, -125, -122, -117, -112, -106, -98, -90, -81, -71, -60, -49, -37, -25, -12
};
