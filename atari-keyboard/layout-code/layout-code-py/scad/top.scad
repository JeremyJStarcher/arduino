include <../scad-orig/KeyV2/includes.scad>
include <./generated-file.scad>

$stabilizer_type = "cherry_stabilizer";
key_profile = "disable";

row = 0;


module r(x, y, w, h, t, halign = "center") {
 text = "Align";
 font = "Liberation Sans";
 size = h / 3 /2;
 rise = 1;
    
     translate([x, y, 0])
     linear_extrude(height = rise) {
       text(
         text = t,
         font = font, 
         size = size,
         valign = "center",
         halign = halign
         );   
}
}

module render_txt(w, h, legends) {
    echo(legends);
    top_of_key() {
        r(0, 0, w, h, legends[4]);
        r(0, -h/4, w, h, legends[7]);
        r(0, +h/4, w, h, legends[1]);
        color("red")
        r(-w/6, h/4, w, h, legends[0], halign="right");

        color("white")
        r(+w/6, h/4, w, h, legends[2], halign="left");
        
    }
}



atari_keyboard();