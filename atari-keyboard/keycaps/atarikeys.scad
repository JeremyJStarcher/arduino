include <./KeyV2/includes.scad>
include <key_include.scad>

module preKey(size, w2, h2) {
    // Rotate and then generate the key
    rotate([atari_rotation, 0, 0]) {
        children();
    }

    // The supports work by placing to points and drawing a line
    // between them.  (IE, wrapping a hull around them.)
    // The first point goes on the "sink" plane, the other point goes
    // on the plane where the rotated key now sits on.

    // Size of the key -- may be overwritten
    w = w2 == undef ? total_key_width() : w2;
    h = h2 == undef ? total_key_height() : h2;


    // Thickness of the walls -- may be overwritten
    thick = size == undef ? $wall_thickness : size;

    // Spacing between supports. Found by trail and error.
    spacing = 3;

    // Shorthand values for easy of use.
    rr1 = support_r1/2;
    rr2 = support_r2/2;

    module supportHeight(a, offset) {
        hull() {
            rotate([atari_rotation, 0, 0])
            translate([offset, a, 0])
            cube([rr1+thick, rr1, 0.25]);

            translate([offset, a, sink])
            cube([rr2+thick, rr2, 0.25]);
        }
        foot(offset, a);
    }

    module supportWidth(a, offset) {
        hull() {
            rotate([atari_rotation, 0, 0])
            translate([a, offset, 0])
            cube([rr1, rr1 + thick, 0.25]);

            translate([a, offset, sink])
            cube([rr2, rr2 + thick, 0.25]);
        }

        foot(a, offset);
    }

if (false) {
     for (a = [ -h/2 : spacing : h/2 ]) {
        color("red") supportHeight(a, -w/2 + rr1);
        color("black") supportHeight(a, w/2 - rr1 -thick);
    }

     for (a = [ -w/2 : spacing : w/2 ]) {
        color("white") supportWidth(a, -h/2 + rr1);
        color("blue") supportWidth(a, h/2 - rr1 -thick);
    }

    for(s = $stabilizers) {
       support_stabilizers(s[0], s[1]);
    }
    support_stabilizers(0, 0);

    // Make the "base" for the keys to adhere to the bed.
    color("lavender")
    translate([-w/2, -h/2, sink])
    cube([w, h, base_height]);
    }
}

module foot(x, y) {
    translate([x, y, sink])
    cylinder(h = base_height, r = 1);
}

module support_stabilizers(x, y) {
    // Offset for the supports
    ox = 2.5;
    oy = 1.5;
    sup_len = 3;


    for (m = [0:1:1])
    mirror([m, 0, 0])
    hull()
    for (m1 = [0:1:1])
    mirror([0, m1, 0])
     {
        hull()
        {
            translate([x + ox, y+oy, sink])
            cube([support_r2, sup_len, 0.25]);

            translate([x + ox, y+oy, 2])
            cube([support_r2, sup_len, 0.25]);
        }

    // translate([ox, oy, 0])
    // foot(x, y);
    }
}

module frontGraphic() {
    rotate([atari_rotation, 0, 0])
    front_of_key()
    scale([0.75, 1, 0.75]) {
        boundBox();
        color("white") children();
    }
}

module graphicsKey(row, legend, svg) {
    difference() {
        u(1)
        legend(legend, [0,0], full_size)
        oem_row(row)
        preKey()
        key();

        frontGraphic()
        children();
    }
}

module graphicsKey2(row, legendBottom, legendTop, svg) {
    difference() {
        u(1)
        legend(legendBottom, [0,1], half_size)
        legend(legendTop, [0,-1], half_size)
        oem_row(4)
        preKey()
        key();

        frontGraphic()
        children();
    }
}

module graphicsKey3(row, legendBottom, legendTop, legendLeft, svg) {
    difference() {
        u(1)
        legend(legendBottom, [0,1], half_size)
        legend(legendTop, [0,-1], half_size)
        legend(legendLeft, [-1,-1], half_size)
        oem_row(4)
        preKey()
        key();

        frontGraphic()
        children();
    }
}


gCut = 2;
module boundBox() {
*    difference() {
        cube([10, gCut, 10], center=true);
        cube([8, gCut, 8], center=true);
        }
}

module gLine6() {
    translate([0, 0, -2])
    cube([2, gCut, 6], center=true);
}


module gLine12() {
    translate([0, 0, 2])
    cube([2, gCut, 6], center=true);
}

module gLine3() {
    translate([+2, 0, 0])
    cube([6, gCut, 2], center=true);
}

module gLine9() {
    translate([-2, 0, 0])
    cube([6, gCut, 2], center=true);
}

module gLineLeft() {
    translate([-3, 0, 0])
    cube([2, gCut, 8], center=true);
}

module gLineRight() {
    translate([3, 0, 0])
    cube([2, gCut, 8], center=true);
}

module gLineBottom() {
    translate([0, 0, -3])
    cube([8, gCut, 2], center=true);
}

module gLineTop() {
    translate([0, 0, 3])
    cube([8, gCut, 2], center=true);
}

module gCharacter(t) {
    translate([0, 1, 0])
    rotate([90, 0, 0])
    linear_extrude(height = gCut)
    text(text = t,  size = 8, valign = "center", halign="center");
}

module gridKey(legend) {
    preKey(size=1.75, w2=19.5 + 3, h2=19.5+3)
    gridKeyRender(legend);
}

module gridKeyRender(legend) {
    // The walls are WAY too thin by default and I couldn't find
    // a parameter setting that would thicken the walls on these kinds of
    // buttons, so we will manually re-enforce the button ourselves.
    //
    // Slows down processing time, but -- eh --

    usize = 1.25;
    uhsize = 1.25;

    module key1() {
        u(usize)
        uh(uhsize)
        legend(legend, [0,0], half_size)
        grid_row(1)
        key();
    }

    hh = 6;
    w = total_key_width();
    h = total_key_height();

    // Draw the re-enforced walls
    color("cyan")
    difference() {
      hull()
      intersection() {
        key1();
        translate([-50, -50, 0])
        cube([100, 100, hh]);
      }

       cube([w, h, hh*5], center=true);
    }

    // Then draw the button itself (which will redraw the thin walls)
    key1();
}


  fullkeyboard();

// key_z();


if (false) {
    key_spacebar();
    key_up();
    key_down();
    key_left();
    key_right();
    key_fn();
}
