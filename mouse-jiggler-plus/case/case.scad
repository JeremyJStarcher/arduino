$fn = 45;


HEIGHT = 38;
LEN = 100;
WIDTH = 155;
CASE_R = 10;
TOP_R = 2;

LCD_X = +28;
LCD_Y = -22;
LCD_Z = 12;

OLED_X = -43;
OLED_Y = -22;
OLED_Z = 17;

LCD_W = 25.25;
LCD_H = 10.5;
LCD_L = 71.25;

LCD_H_X = 75 / 2;
LCD_H_Y = 32 / 2;

OLED_L = 28;
OLED_W = 20;
OLED_H = 4;

OLED_H_X = 24 / 2;
OLED_H_Y = 24 / 2;

MAIN_W = 60;
MAIN_L = 41;
MAIN_H = 4;

MAIN_X = -40;
MAIN_Y = 20;
MAIN_Z = 17;

MAIN_H_X = 56 / 2;
MAIN_H_Y = 34 / 2;

module boundingCorner() {
  color("green")
  translate([WIDTH/2 -CASE_R , LEN/2 - CASE_R, 0]) {
    cylinder(r = CASE_R, h=HEIGHT - TOP_R, center=true);
    translate([TOP_R, TOP_R, HEIGHT/2]) {
      color("BLUE")
      sphere(TOP_R);
    }
  }  
}

module outsideCase() {
  hull() {
    mirror([0, 0, 0])
    boundingCorner();
 
    mirror([1, 0, 0])
    boundingCorner();
 
    mirror([0, 1, 0])
    boundingCorner();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    boundingCorner();
  }  
}

module lcd_screwholes() {
     module h() {
       translate([LCD_H_X, LCD_H_Y, 0])
       cylinder(r = 4/2, h=20, center=true);
     }

    mirror([0, 0, 0])
    h();
     
    mirror([1, 0, 0])
    h();
 
    mirror([0, 1, 0])
    h();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h();    
}

module lcd_screwstuds() {
     module h() {
       translate([LCD_H_X, LCD_H_Y, 0])
       cylinder(r = 4, h=LCD_H-7.25, center=true);
     }

    mirror([0, 0, 0])
    h();
     
    mirror([1, 0, 0])
    h();
 
    mirror([0, 1, 0])
    h();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h();    
}

module lcd() {
    module screwholerim() {
      translate([LCD_H_X, LCD_H_Y, 0]) {
        cylinder(r = 5/2, h=3, center=true);
      }
    }
    
    color("green")
    cube([80.25, 36, 2], center=true);
    
    // Height from bottom of circuit board
    color("black")
    translate([0, 0, LCD_H/2])
    cube([LCD_L, LCD_W, LCD_H], center=true);
    
    color("white")
    translate([LCD_L /2 -2, 0, LCD_H/2 /2])
    cylinder(r = 6, h=LCD_H/2+5, center=true);
    
    mirror([0, 0, 0])
    screwholerim();
    
    mirror([1, 0, 0])
    screwholerim();
 
    mirror([0, 1, 0])
    screwholerim();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    screwholerim();
}

module oled() {
    module h_rim() {
      translate([OLED_H_X, OLED_H_Y, ]) {
        cylinder(r = 1.5, h=3, center=true);
      }
    }
    
    color("lightblue")
    cube([28, 27, 2], center=true);
    
    color("navy")
    translate([0, 0, OLED_H/2]) {
      cube([OLED_L, OLED_W, OLED_H], center=true);
      // Cut away space for the connectors.
      cube([OLED_L-9, OLED_W+12, OLED_H-2], center=true);
    }

    mirror([0, 0, 0])
    h_rim();
    
    mirror([1, 0, 0])
    h_rim();
 
    mirror([0, 1, 0])
    h_rim();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h_rim();
}

module oled_screwholes() {
     module h() {
      translate([OLED_H_X, OLED_H_Y, 0]) {
        cylinder(r = 1.5, h=10, center=true);
      }
     }

    mirror([0, 0, 0])
    h();
     
    mirror([1, 0, 0])
    h();
 
    mirror([0, 1, 0])
    h();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h();    
}

module whole() {
    outsideCase();
}

module top_half() {
  rotate([0, 180, 0]) { 
  difference() {
    intersection() {
      translate([-500, -500, 0])
      cube([1000, 1000, 1000]);
      whole();
    }
    
    cube([WIDTH - CASE_R, LEN  -CASE_R, HEIGHT -4], center=true);
    guts();

    // Touchpad mounting sensors.
    translate([50, 7, 0])
    cylinder(r = 5.25/2, h=100, center=true);

    translate([50, 30, 0])
    cylinder(r = 5.25/2, h=100, center=true);

    translate([10, 7, 0])
    cylinder(r = 5.25/2, h=100, center=true);

    translate([10, 30, 0])
    cylinder(r = 5.25/2, h=100, center=true);

   // Opening for USB cable.    
    translate([-72+27, 47, 10])
    rotate([90, 0, 0]) {
    scale([1.25, .5, 1])
    cylinder(r = 13/2, h=10, center=true); 
    }
  }
  
  translate([LCD_X, LCD_Y, LCD_Z+4])
    difference() {
      lcd_screwstuds();
      lcd_screwholes();
    }
  }
  
  translate([-MAIN_X, MAIN_Y, -MAIN_Z+1])
    difference() {
      main_screwstuds();
      main_screwholes();
    }
}

module mainBoard() {
    cube([MAIN_W, MAIN_L, MAIN_H], center=true);
}


module main_screwstuds() {
     module h() {
       translate([MAIN_H_X, MAIN_H_Y, 0])
       cylinder(r = 4, h=LCD_H -4, center=true);
     }

    // No room for this one because of how the board is soldered
    // mirror([0, 0, 0])
    // h();
     
    mirror([1, 0, 0])
    h();
 
    mirror([0, 1, 0])
    h();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h();    
}


module main_screwholes() {
     module h() {
       translate([MAIN_H_X, MAIN_H_Y, 0])
       cylinder(r = 2, h=40, center=true);
     }

    mirror([0, 0, 0])
    h();
     
    mirror([1, 0, 0])
    h();
 
    mirror([0, 1, 0])
    h();
 
    mirror([1, 0, 0])
    mirror([0, 1, 0])
    h();
}

module guts() {
    translate([LCD_X, LCD_Y, LCD_Z])
    {
      lcd();
      lcd_screwholes();  
    }

    translate([OLED_X, OLED_Y, OLED_Z]) {
      oled();
      oled_screwholes();
        
   }

  translate([MAIN_X, MAIN_Y, MAIN_Z]) {
    mainBoard();
    main_screwholes();
  }
}

module bottom_half() {
  difference() {
    intersection() {
      translate([-500, -500, -1000])
      cube([1000, 1000, 1000]);
      whole();
    }
    cube([WIDTH - CASE_R, LEN  -CASE_R, HEIGHT -4], center=true);
  }
}

// top_half();

rotate([180, 180, 180])
bottom_half();

