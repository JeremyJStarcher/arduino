// HI

module bulk() {
translate([1, 14, 0])
import("aperture_science_keychain.stl", convexity=3);


w=108;
l=60;
h=3;

color("red")
translate([-w/2, -l/2, 0])
cube([w, l, h]);

 text = "Align";
 font = "Liberation Sans";
 

     linear_extrude(height = h+1) {
      translate([0, -13, 0])
    text(text = "Jeremy Starcher", font = font, size = 10, halign = "center");
 
         }

    linear_extrude(height = h+1) {
      translate([0, -25, 0])
    text(text = "Failed Subject Recovery", font = font, size = 7, halign = "center");
 
         }
     }
     
     difference() {
     bulk();
         translate([-38, 15, 0])
         cube([29, 26, 10], center=true);
     }