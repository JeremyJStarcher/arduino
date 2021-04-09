$fs = .1;

x_pos = (210 - 97)/2;
y_pos = (134 - 52)/2;
b_h = 18; // board height

inner_tolerance =0.5;
wall_r = (97 - 93);
pin_to_wall_inner = wall_r +inner_tolerance;

board_thick = 1.6;
wall_thick = 2;
pin_to_wall_outer = pin_to_wall_inner+wall_thick;


module all() {
      children();
      mirror([1, 0, 0]) children();
      mirror([0, 1, 0]) children();
      mirror([0, 1, 0])
        mirror([1, 0, 0]) children();
}

module setPegs() {
        h = b_h- board_thick - wall_thick;
        module studs() {
        
        all() {
        translate([x_pos, y_pos, +wall_thick]) 
            cylinder(h = h, r = 4, center=true);
        }
    }

    module h() {
        all() {
        translate([x_pos, y_pos, +wall_thick]) 
            cylinder(h = h*3, r = 1.5, center=true);
        }
    }
    
    
    difference() {
        studs();
        h();
    }
}


module outer() {
    x = x_pos+pin_to_wall_outer;
    y = y_pos+pin_to_wall_outer;  

    module p() {
        color("red")
        translate([0, 0, +wall_r/2])
        cylinder(h = b_h, r =  wall_r, center=true);
        color("green") 
        sphere(r = wall_r);
    }

  hull()
  all() {
    translate([x, y, 0]) p();
  }
}

module inner() {
    x = x_pos+pin_to_wall_inner;
    y = y_pos+pin_to_wall_inner;  

    module p() {
        translate([0, 0, +wall_r/2])
        cylinder(h = b_h, r =  wall_r, center=true);
        sphere(r = wall_r);
    }

    color("blue")
    hull()
    all() {
        translate([x, y, 0]) p();
    }
}

module cutout() {
    translate([
        +x_pos+pin_to_wall_outer,
        -y_pos+30,
        +wall_thick*3
    ])
    cube([20, 30, 20], center=true);
}

difference() {
    outer();
    translate([0, 0, wall_thick])
    inner();
    cutout();
    
}

setPegs();