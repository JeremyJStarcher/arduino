KBD_WIDTH = 69;
KBD_HEIGHT = 79;
KBD_THICK = 3;

SCR_WIDTH = 35;
SCR_HEIGHT = 42+6+5+10;
SCR_THICK = 8;

B_THICKNESS = 20;
B_WIDTH = 53;
B_HEIGHT = 72;

BORDER = 4;
SEP = 2;

BOX_WIDTH = KBD_WIDTH + SCR_WIDTH + (BORDER*2) + SEP;
BOX_HEIGHT = KBD_HEIGHT + (BORDER*2);
BOX_THICKNESS = B_THICKNESS + SCR_THICK + 3;

BOARD_X= ((KBD_WIDTH - B_WIDTH) /2) +BORDER;
BOARD_Y= ((KBD_HEIGHT - B_HEIGHT) /2) +BORDER;

CABLE_WIDTH = 21;

module boardVoid() {
    translate([
        BOARD_X,
        BOARD_Y,
        -1
    ])
    cube([B_WIDTH, B_HEIGHT, B_THICKNESS+2]);    
}


module kbdVoid() {
    translate([BORDER, BORDER, BOX_THICKNESS - KBD_THICK])
    cube([KBD_WIDTH, KBD_HEIGHT, KBD_THICK+2]);    
}

module scrVoid() {
    translate([
        BORDER + KBD_WIDTH + SEP,
                ((KBD_HEIGHT - SCR_HEIGHT) /2) +BORDER,

        BOX_THICKNESS - SCR_THICK])
    cube([SCR_WIDTH, SCR_HEIGHT, SCR_THICK+2]);    
}

module boardPegs() {
    s = B_THICKNESS /2;
    module p(x, y) {
        color("red")
        translate([BOARD_X+x,BOARD_Y+y,B_THICKNESS-s])
        cube([4, 4, s]); 
    }
 
    p(0, 0);
    p(0, B_HEIGHT-s);
    p(B_WIDTH-s, B_HEIGHT-s);
    p(B_WIDTH-s, 0);    
}



module mainBody() {
    difference() {
        union() {
            roundedcube([BOX_WIDTH, BOX_HEIGHT, BOX_THICKNESS], radius=BORDER);
        }
        union() {
            boardVoid();
            kbdVoid();
            scrVoid();
        }
    }
}

module bodyVoid() {
    color("red")
    translate([
        BORDER+(KBD_WIDTH/2)-(CABLE_WIDTH/2),
        BORDER*3,
        BOX_THICKNESS - KBD_THICK]) {
            cube([CABLE_WIDTH, KBD_HEIGHT, 5+2]);
            translate([0, 0, -BOX_THICKNESS+2])
            cube([CABLE_WIDTH, KBD_HEIGHT, 5+2]);
        }
        
x = BORDER + KBD_WIDTH + SEP;
y = +((KBD_HEIGHT + SCR_HEIGHT) /2) +BORDER;
z =    -BOX_THICKNESS/2 - SCR_THICK;
  
    color("cyan")
    translate([x, -15 +y, -z])
    rotate([-45, 0, 0])
    cube([SCR_WIDTH, SCR_HEIGHT/4, SCR_THICK+2]);    
    color("green")
    translate([x-15,  +y-65, +23 +z])
    cube([SCR_WIDTH+15, SCR_HEIGHT, SCR_THICK+10]);
   

   bb = BORDER*4;
        translate([-1, bb, 0])
  cube([CABLE_WIDTH, BOX_HEIGHT -bb*2, 20]);     
}

if (0)
difference() {
    union() {
        mainBody();
        //boardPegs();
    }
    union() {
        bodyVoid();
    }
}

difference() {
    roundedcube([BOX_WIDTH, BOX_HEIGHT, 3], radius=1);
    union() {
        translate([53, 17, 0])
        cylinder(r=4, h = 10);
        
        
        translate([20, BOX_HEIGHT-17, 0])
        cylinder(r=4, h = 10);
 
        translate([BOX_WIDTH-20, BOX_HEIGHT-17, 0])
        cylinder(r=4, h = 10);
        
        }
}

// Higher definition curves
$fs = 0.01;

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

