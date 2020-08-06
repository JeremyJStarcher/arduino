#include <Arduino.h>
#include "./hardware.h"

Intensity def = intensity_none;
short pwm = 0;

Toy lnip = {"led", "LED (works)", 0, def, pwm, 1, .5, .25, 2};
Toy rnip = {"rnip", "R Nipple (n/c)", 0, def, pwm, 1, .75, .5, -1};
Toy button = {"btn", "button", 0, def, pwm, 1, .89, .75, 15};

Toy toys[] = {lnip, rnip, button};
