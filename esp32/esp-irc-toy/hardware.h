#include <Arduino.h>

#ifndef TOY_HARWARE_h
#define TOY_HARWARE_h

struct mytoy {
  String id;
  String name;
  long long expires;
  byte intensity;
  byte digitalPin;
};

typedef mytoy Toy;

extern Toy toys[];
#define TOY_COUNT 3

#endif
