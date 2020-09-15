#include <Arduino.h>
#include "./hardware.h"

Intensity def = intensity_none;
short pwm = 0;

Toy lnip = {"lnip", "L Nipple", 0, def, pwm, 1, .5, .25, 2  };
Toy rnip = {"rnip", "R Nipple", 0, def, pwm, 1, .75, .5, 16};
Toy button = {"btn", "button", 0, def, pwm, 1, .89, .75, 26};
Toy t4 = {"t4", "Toy 4", 0, def, pwm, 1, .89, .75, 25};

//Toy toys[] = {lnip, rnip, button};
Toy toys[] = {rnip, lnip, button, t4};
