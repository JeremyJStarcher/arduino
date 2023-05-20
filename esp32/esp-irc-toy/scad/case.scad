
// Higher definition curves
$fs = 0.01;

W=74+2;
L=62+2;
BOARD_THICK=1.6;
B=10-BOARD_THICK;
H=23.5;
WALL=2;
OVERSHOOT = 3;

MOUNT_ORIGIN = [-1.5, -2.25];
mounts = [
    [-31.5, -10.5],
    [26.4, -10.5],
    [26.4, 16],
    [-7.6, 16.5],
];

top_mounts = [
  //  [-31.5, -10.5],
 //   [26.4, -10.5],
    [26.4, 16],
    [-7.6, 16.5],
];


TERMINALS=[
    [15.2+4.3, 33.1],
    [28.2+4.3, 33.1],
    [40.8+4.3, 33.1],
    [53.8+4.3, 33.1],
];


module bottomCase() {    
    bottomWall(WALL, 0);
    bottomWall(WALL/2, OVERSHOOT);

    for(m = mounts) {
        translate([
        -m[0]+MOUNT_ORIGIN[0],
        m[1]+MOUNT_ORIGIN[1],
        -H/2-WALL]) {
            difference() {
                color("green")
                cylinder(h = B, r = 3);
                translate([0, 0, WALL])
                cylinder(h = B, r = 1);
            }
        }
    }
}

module topCase() {
    difference() {
        union() {
            topWall(WALL, OVERSHOOT+BOARD_THICK/2);
          
            union() {  
                mirror([1, 0, 0])      
                topMounts();
            }
        }
   
        union() {
            bottomCase();
            window();
            terminals();
            button();
            }
       }
  
    module window() {
        translate([
            +4.3+mounts[0][0]+1.52+MOUNT_ORIGIN[0],
            mounts[0][1]-13.7+MOUNT_ORIGIN[1],
        -WALL
        ])
        cube([50, 28, WALL*100]);
    }
    
    
    module terminals() {
       for(m = TERMINALS) {
            translate([
                m[0]+MOUNT_ORIGIN[0]+mounts[0][0]-3,
                m[1]+MOUNT_ORIGIN[1]+mounts[0][1]-.5,
                -H]) {
                cube([11, 28, WALL*100]);
            }
        }
    }

    module button() {
        translate([
            +4.3+4.6+MOUNT_ORIGIN[0]+mounts[0][0]-.5,
            34.6+MOUNT_ORIGIN[1]+mounts[0][1]-.5,
            -WALL]) {
            cube([10.25, 10.25, WALL*100], center=true);
        }
    }    
}
 
module topMounts() {
    for(m = top_mounts) {
        translate([
        -m[0]+MOUNT_ORIGIN[0],
        m[1]+MOUNT_ORIGIN[1],
        -BOARD_THICK]) {
            difference() {
                color("pink")
                cylinder(h = H-B, r = 3);
                translate([0, 0, -H/2-2])
               cylinder(h = H, r = .75);
            }
        }
    }
}


module bottomWall(wall, overshoot) {
    difference() {
        color("blue")
        outsideBox(wall);
        union() {
            topChopOff(overshoot);
            insideBox();
        }
    }
}

module topWall(wall, overshoot) {
    module top(){
        union() {
             bottomChopOff(overshoot);
             insideBox();
        }
    }
    difference() {
        union() {
            color("cyan")
            outsideBox(wall);
        }
        top();
    }
    
    intersection() {
            translate([33, 44, B])
            color("red")
           roundedcube([100, 50, H], center=true);
         outsideBox(wall);
       }
}


module bottomChopOff(h) {
        translate([0, 0, -H+B-h])
        cube([W+2+WALL*2,L+2+WALL*2, H+WALL*2], center=true);
}

module topChopOff(h) {
        translate([0, 0, B+h])
        cube([W+2+WALL*2,L+2+WALL*2, H+WALL*2], center=true);
}

module insideBox() {
        roundedcube([W,L, H], center=true, radius=3);
}


module outsideBox(thick) {
roundedcube([W+thick*2,L+thick*2, H+thick*2], center=true, radius=3);
}





module roundedcube(size = [1, 1, 1], center = false, radius = 0.5, apply_to = "all") {
	// If single value, convert to [x, y, z] vector
	size = (size[0] == undef) ? [size, size, size] : size;

	translate_min = radius;
	translate_xmax = size[0] - radius;
	translate_ymax = size[1] - radius;
	translate_zmax = size[2] - radius;

	diameter = radius * 2;

	module build_point(type = "sphere", rotate = [0, 0, 0]) {
		if (type == "sphere") {
			sphere(r = radius);
		} else if (type == "cylinder") {
			rotate(a = rotate)
			cylinder(h = diameter, r = radius, center = true);
		}
	}

	obj_translate = (center == false) ?
		[0, 0, 0] : [
			-(size[0] / 2),
			-(size[1] / 2),
			-(size[2] / 2)
		];

	translate(v = obj_translate) {
		hull() {
			for (translate_x = [translate_min, translate_xmax]) {
				x_at = (translate_x == translate_min) ? "min" : "max";
				for (translate_y = [translate_min, translate_ymax]) {
					y_at = (translate_y == translate_min) ? "min" : "max";
					for (translate_z = [translate_min, translate_zmax]) {
						z_at = (translate_z == translate_min) ? "min" : "max";

						translate(v = [translate_x, translate_y, translate_z])
						if (
							(apply_to == "all") ||
							(apply_to == "xmin" && x_at == "min") || (apply_to == "xmax" && x_at == "max") ||
							(apply_to == "ymin" && y_at == "min") || (apply_to == "ymax" && y_at == "max") ||
							(apply_to == "zmin" && z_at == "min") || (apply_to == "zmax" && z_at == "max")
						) {
							build_point("sphere");
						} else {
							rotate = 
								(apply_to == "xmin" || apply_to == "xmax" || apply_to == "x") ? [0, 90, 0] : (
								(apply_to == "ymin" || apply_to == "ymax" || apply_to == "y") ? [90, 90, 0] :
								[0, 0, 0]
							);
							build_point("cylinder", rotate);
						}
					}
				}
			}
		}
	}
}


//bottomCase();

mirror([1, 0, 0])
topCase();


module button() {
    BX = 9;
    BY = 9;
    bh1 = 10;
    
    
roundedcube([BX, BY, bh1], radius=2);

translate([-1, -1, 0])
roundedcube([11, 11, bh1-WALL-2]);

}

//button();
