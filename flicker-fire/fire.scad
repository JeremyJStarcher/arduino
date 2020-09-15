dome_r = 15;
led_r = 3;

$fs = 0.01;
//$fn = 100;

module body() {
    difference() {
        sphere(r = dome_r);

        translate([0, 0, -dome_r])
        cylinder(h = dome_r, r = dome_r);
        }
}

module led_void() {
    h = 5;
    cylinder(h = h, r = led_r);
    translate([0, 0, h]) 
    sphere(r = led_r);
}

module voids() {
    translate([0, 0, -1]) 
    led_void();
    for (i = [0:360/5:360]) {
        rotate(i)
        translate([0, dome_r/2, -1]) {
           led_void();
        }
    }    
}

difference() {
    body();
    voids();
}
