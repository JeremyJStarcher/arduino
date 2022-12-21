$fn = 60;

lead_spacing = 7.8;
lead_diameter = 0.55;
part_diameter = 1.75;
part_length = 3.4;

qty = 10;

translate([-lead_spacing/2, -lead_diameter/2/2, -lead_diameter/2/2])
cube([lead_spacing, lead_diameter/2, lead_diameter/2]);

rotate([0, 90, 0])
cylinder(r = part_diameter/2, h = part_length, center=true);