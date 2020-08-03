#include <Arduino.h>
#include "./hardware.h"

Toy lnip = {"led", "LED (works)", 0, 0, 2};
Toy rnip = {"rnip", "R Nipple (n/c)", 0, 0, -1};
Toy button = {"btn", "button (n/c)", 0, 0, -1};
Toy feep = {"feep", "Noise Maker", 0, 0, 15};

Toy toys[] = {lnip, rnip, button, feep};
