#include <Arduino.h>

#ifndef TOY_HARWARE_h
#define TOY_HARWARE_h

enum Intensity {intensity_none, intensity_high, intensity_medium, intensity_low};

struct mytoy {
  String id;
  String name;
  long long expires;
  Intensity intensity;
  short pwm;
  float high;
  float medium;
  float low;
  byte digitalPin;
};

typedef mytoy Toy;

  extern Toy toys[];
#define TOY_COUNT 3

#endif
