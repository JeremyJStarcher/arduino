CLEARANCE = 0.1;

CAP_HEIGHT = 3;
CAP_SIZE = 15;
SPACING = 2;

LAYER_HEIGHT = 0.2;

// 1 = BUTTONS
// 2 = KEY LEGENDS
MODE = 2; // DRAW KEY CAPS


module cross(h=4) {
    cube([4+2*CLEARANCE,1.3+2*CLEARANCE,h], center=true);
    cube([1.1+2*CLEARANCE,4+2*CLEARANCE,h], center=true);
}

module rounded_cube(x, y, z, r) {
    translate([0,0,-0.5])
    minkowski() {
        cube([x-2*r,y-2*r,z-1], center=true);
        cylinder(r=r, h=1, $fn=30);
    }
}




module key(x, y, legend) {
    xlate = [
        x * (CAP_SIZE+SPACING),
        -y * (CAP_SIZE+SPACING),
        0
        ];
    
    if (MODE == 1) {
        translate(xlate)
        button(legend);
    } else {
        translate(xlate)
        cap(legend);
    }

}

module cap(legend) {
    color("red")
    scale([-1, 1, 1])
    linear_extrude(height = LAYER_HEIGHT) {
       text(text = legend, size = CAP_SIZE * .8, valign = "center", halign="center");
    }
}


module buttonBody() {
    translate([
        0,
        0,
        CAP_HEIGHT/2
        ]) 
    {
        rounded_cube(CAP_SIZE, CAP_SIZE, CAP_HEIGHT, 2);
        difference() {
            translate([0,0,3])
//            rounded_cube(7.5, 5.5, 6, 1);
              rounded_cube(7.0, 5.0, 6, 1);
    
            translate([0,0,10/2+2.5])
            cross(h=10);
        }
    }
}

module button(legend) {
    difference() {
        buttonBody();
        cap(legend);
    }
}


if (true) {
    key(0, 0, "0");
    key(1, 0, "1");
    key(2, 0, "2");
    key(3, 0, "3");
    key(0, 1, "4");
    key(1, 1, "5");
    key(2, 1, "6");
    key(3, 1, "7");
    key(0, 2, "8");
    key(1, 2, "9");
    key(2, 2, "A");
    key(3, 2, "B");
    key(0, 3, "C");
    key(1, 3, "D");
    key(2, 3, "E");
    key(3, 3, "F");
    border();
} else {
    key(0, 0, "J");
}




module border() {
    xx = [-1, 4];
    
    module setdot(x, y, z) {
        xlate = [
            x * (CAP_SIZE+SPACING),
            -y * (CAP_SIZE+SPACING),
            0
            ];

            translate(xlate)
            linear_extrude(height = z)
            circle(r = 2);
    }

    module outer() 
    {
        for (x = xx, y = xx)
        setdot(x, y, LAYER_HEIGHT);
    }
    
    module inner() {
        for (x = xx * .9, y = xx * .9)
        setdot(x, y, 10 );
    }
    
    difference() {
    color("blue") hull() outer();
        translate([0, 0, -5])
    color("red") hull() inner();
    }
    
}
