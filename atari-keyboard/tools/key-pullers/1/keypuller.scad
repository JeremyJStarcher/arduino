// 2013 by pokey9000
// cc-by-nc-sa http://creativecommons.org/licenses/by-nc-sa/3.0/

use <write/Write.scad>

std_key_width=18;
puller_thickness=6;
handle_dia=28;
handle_thickness=8;
tine_length=18; // from tangent of handle to tine end
tine_thickness=1;
catch_thickness=.8;


//Optional features
//fillet=true; // uncomment for extra support where the tines meet the handle
//emboss_text="r/MechanicalKeyboards"; // uncomment for custom handle embossing 

//tines for a key puller
//z        completed width of the puller
//tine_th  thickness of the tines
//tine_ln  length from end of tine to the beginning of the handle
//catch_ln length of the key catch
//catch_wd amount the catch extends out by
//key_wd   width of a key (18mm is std)
module puller(z, tine_th, tine_ln, catch_ln, catch_wd, key_wd) {
	difference() {
		cube([(tine_th*2)+key_wd, tine_ln, z]);
		translate([tine_th, 0,0]) {
			cube([key_wd, tine_ln-catch_ln, z]);	
		}
		translate([tine_th+catch_wd, 0,0]) {
			cube([key_wd-(catch_wd*2), tine_ln, z]);	
		}
	}
}

puller_width=std_key_width+(tine_thickness*2);
final_tine_length=tine_length+(handle_dia/2);
difference() {
	union() {
		puller(puller_thickness, tine_thickness, final_tine_length, 1, catch_thickness, std_key_width);
		translate([puller_width/2,0,0]) 
			cylinder(h=puller_thickness, r=handle_dia/2);
		if(fillet==true) 
			cube([puller_width, handle_dia/2, puller_thickness]);
	}
	translate([puller_width/2,0,0]) 
		cylinder(h=puller_thickness, r=(handle_dia-handle_thickness)/2);
	if(emboss_text!="")
		translate([puller_width/2,0,0]) 
			writecylinder(emboss_text,[0,0,0],(handle_dia/2)-0.2 , puller_thickness);
}

