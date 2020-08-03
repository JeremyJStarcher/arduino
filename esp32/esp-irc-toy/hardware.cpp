#include <Arduino.h>
#include "./hardware.h"

Toy lnip = {"led", "LED (works)", 0, 0, 2};
Toy rnip = {"rnip", "Right Nipple (not present)", 0, 0, -1};
Toy button = {"btn", "button (not present)", 0, 0, -1};

Toy toys[] = {lnip, rnip, button};
