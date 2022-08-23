include <./KeyV2/includes.scad>

// How far to rotate the keys when they are generated
atari_rotation = 25;

$keytop_thickness = 2;


sink = -7;
support_r1 = 0.5;
support_r2 = 0.5;


$stem_support_type =  "disable"; // "disable"; // "tines"
$inset_legend_depth = 1.5;
// $clearance_check = true;

$font = "Liberation Sans:style=Bold";

full_size = 9;
long_size = 4;
half_size = 4.5;

// example key
* dcs_row(1) legend("⇪", size=9) key();

// example row
 * for (x = [0:1:4]) {
  translate_u(0,-x) dcs_row(x) key();
}

// example layout
* preonic_default("dcs") key();



module preKey(size, w2, h2) {
    w = w2 == undef ? total_key_width() : w2;
    h = h2 == undef ? total_key_height() : h2;

    thick = size == undef ? $wall_thickness : size;

    spacing = 3;


    rotate([atari_rotation, 0, 0]) {
        children();
        // % cube([w, h, 1], center = true);
    }

    module supportHeight(a, offset) {
        hull() {
            rotate([atari_rotation, 0, 0])
            translate([offset, a, 0])
            cylinder(h = 0.25, r = support_r1);

            translate([offset, a, sink])
            cylinder(h = 0.25, r = support_r2);
        }
        foot(offset, a);
    }

    module supportWidth(a, offset) {
        hull() {
            rotate([atari_rotation, 0, 0])
            translate([a, offset, 0])
            cylinder(h = 0.25, r = support_r1);

            translate([a, offset, sink])
            cylinder(h = 0.25, r = support_r1);
        }

        foot(a, offset);
    }

     for (a = [ -h/2 : spacing : h/2 ]) {
        supportHeight(a, -w/2);
        supportHeight(a, -w/2 + thick/2);

        supportHeight(a, w/2);
        supportHeight(a, w/2 - thick/2);
    }

     for (a = [ -w/2 : spacing : w/2 ]) {
        supportWidth(a, -h/2);
        supportWidth(a, -h/2 + thick/2);

        supportWidth(a, h/2);
        supportWidth(a, h/2 - thick/2);
    }

    echo($stabilizers);
    for(s = $stabilizers) {
       support_stabilizers(s[0], s[1]);
    }
    support_stabilizers(0, 0);
}

module foot(x, y) {
    translate([x, y, sink])
    cylinder(h = 0.25, r = 1);
}

module support_stabilizers(x, y) {
    color("magenta")
    for (m = [0:1:1])
    mirror([m, 0, 0]) {
    hull()
    {
        translate([x, y, sink])
        cylinder(h = 0.25, r = support_r2);

        translate([3 +x, y, 2])
        cylinder(h = 0.25, r = support_r2/2);
    }
    translate([0, 0, 0])
    foot(x, y);
    }
}


module graphicsKey(row, legend, svg) {
    difference() {
        u(1)
        legend(legend, [0,0], full_size)
        oem_row(row)
        preKey()
        key();

        front_of_key()
        scale([0.75, 1, 0.75]) {
          boundBox();
          color("white") children();
        }
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

        front_of_key()
        scale([0.75, 1, 0.75]) {
          boundBox();
          color("white") children();
        }
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

        front_of_key()
        scale([0.75, 1, 0.75]) {
          boundBox();
          color("white") children();
        }
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

    h = 6;
    w = total_key_width();


    // Draw the re-enforced walls
    color("cyan")
    difference() {
      hull()
      intersection() {
        key1();
        translate([-50, -50, 0])
        cube([100, 100, h]);
      }


     rotate([atari_rotation, 0, 0])
      cube([w, w, 100], center=true);
    }

    // Then draw the button itself (which will redraw the thin walls)
    key1();
}


/// ROW 4

module key_lshift() {
    translate_u(0, 0)
    u(2.25)
    stabilized()
    legend("SHIFT", [0,0], long_size)
    oem_row(4)
    preKey()
    key();
}

module key_z() {
    translate_u(1.75, 0)
    graphicsKey(4, "Z", "ctrl-z") {
        gLine12();
        gLine3();
    }
}

module key_x() {
    translate_u(1.75 +1, 0)
    graphicsKey(4, "X", "ctrl-x") {
        gLine9();
        gLine3();
        gLine12();
    }
}


module key_c() {
    translate_u(1.75 +2, 0)
    graphicsKey(4, "C", "ctrl-c") {
        gLine9();
        gLine12();
    }
}

module key_v() {
    translate_u(1.75 +3, 0)
    graphicsKey(4, "V", "ctrl-v") {
        gLineLeft();
    }
}

module key_b() {
    translate_u(1.75 +4, 0)
    graphicsKey(4, "B", "ctrl-b") {
        gLineRight();
    }
}

module key_n() {
    translate_u(1.75 +5, 0)
    graphicsKey(4, "N", "ctrl-n") {
        gLineBottom();
    }
}


module key_m() {
    translate_u(1.75 +6, 0)
    graphicsKey(4, "M", "ctrl-m") {
        gLineTop();
    }
}


module key_comma() {
    translate_u(1.75 +7, 0)
    graphicsKey2(4, ",", "[", "ctrl-comma") {
        gCharacter("♥");
    }
}

module key_dot() {
    translate_u(1.75 +8, 0)
    graphicsKey2(4, ".", "]", "ctrl-period"){
        gCharacter("♦");
    }
}

module key_slash() {
    translate_u(1.75 +9, 0)
    graphicsKey2(4, "/", "?", "ctrl-period");
}

module rshift() {
    translate_u(1.75 +10.5, 0)
    u(1.75)
    legend("SHIFT", [0,0], long_size)
    oem_row(4)
    preKey()
    key();
}


/// ROW 3


module key_control() {
    translate_u(-.5, 1)
    u(2)
    stabilized()
    legend("CONTROL", [0,0], long_size)
    oem_row(3)
    preKey()
    key();
}

module key_a() {
    translate_u(-.5+1.5, 1)
    graphicsKey(3, "A", "ctrl-a") {
        gLine6();
        gLine12();
        gLine3();
    }
}

module key_s() {
    translate_u(-.5+1.5+1, 1)
    graphicsKey(3, "S", "ctrl-s") {
        gLine6();
        gLine12();
        gLine3();
        gLine9();
    }
}

module key_d() {
    translate_u(-.5+1.5+2, 1)
    graphicsKey(3, "D", "ctrl-d") {
        gLine6();
        gLine12();
        gLine9();
    }
}

module key_f() {
    $key_bump = true;

    translate_u(-.5+1.5+3, 1)
    graphicsKey(3, "F", "ctrl-f") {
        hull() {
        translate([3, -1, 3])
        cube([1, gCut, 1]);

        translate([-4, -1, -4])
        cube([1, gCut, 1]);
        }
    }
}

module key_g() {
    translate_u(-.5+1.5+4, 1)
    graphicsKey(3, "G", "ctrl-g") {
        hull() {
        translate([-4, -1, 3])
        cube([1, gCut, 1]);

        translate([3, -1, -4])
        cube([1, gCut, 1]);
        }
    }
}

module key_h() {
    translate_u(-.5+1.5+5, 1)
    graphicsKey(3, "H", "ctrl-h") {
        hull() {
            translate([4, -1, 4])
            cube([.1, gCut, .1]);

            translate([-4, -1, -4])
            cube([.1, gCut, .1]);

            translate([3, -1, -4])
            cube([1, gCut, 1]);
        }
    }
}

module key_j() {
    $key_bump = true;

    translate_u(-.5+1.5+6, 1)
    graphicsKey(3, "J", "ctrl-j") {
        hull() {
            translate([-4, -1, 4])
            cube([.1, gCut, .1]);

            translate([4, -1, -4])
            cube([.1, gCut, .1]);

            translate([-4, -1, -4])
            cube([1, gCut, 1]);
        }
    }
}

module key_k() {
    translate_u(-.5+1.5+7, 1)
    graphicsKey(3, "K", "ctrl-k") {
        translate([0, -1, 0])
        cube([4, gCut, 4]);
    }
 }

module key_l() {
    translate_u(-.5+1.5+8, 1)
    graphicsKey(3, "L", "ctrl-l") {
        translate([-4, -1, 0])
        cube([4, gCut, 4]);
    }
}

module key_semi() {
    translate_u(-.5+1.5+9, 1)
    graphicsKey2(3, ";", ":", "ctrl-semi"){
        gCharacter("♠️");
    }
}

module key_plus() {
    translate_u(-.5+1.5+10, 1)
    graphicsKey3(3, "+", "\\", "←", "ctrl-comma");
}

module key_star() {
    translate_u(-.5+1.5+11, 1)
    graphicsKey3(3, "*", "^", "→", "ctrl-comma");
}


module key_caps() {
    translate_u(-.5+1.5+12 +.25, 1)
    u(1.25)
    legend("CAPS", [0,0], long_size)
    oem_row(3)
    preKey()
    key();
}

// ← ↑ ↓ →

// ROW 2

module key_tab() {
    translate_u(-.5, 2)
    u(1.75)
    legend("TAB", [0,1], long_size)
    legend("CLR", [1,-1], long_size)
    legend("SET", [-1,-1], long_size)
    oem_row(2)
    preKey()
    key();
}

module key_q() {
    translate_u(-.5+1.5, 2)
    graphicsKey(2, "Q", "ctrl-q") {
       gLine3();
       gLine6();
    }
}

module key_w() {
    translate_u(-.5+1.5+1, 2)
    graphicsKey(2, "W", "ctrl-w") {
       gLine3();
       gLine6();
       gLine9();
    }
}

module key_e() {
    translate_u(-.5+1.5+2, 2)
    graphicsKey(2, "E", "ctrl-e") {
       gLine6();
       gLine9();
    }
}

module key_r() {
    translate_u(-.5+1.5+3, 2)
    graphicsKey(2, "R", "ctrl-e") {
       gLine9();
       gLine3();
   }
}

module key_t() {
    translate_u(-.5+1.5+4, 2)
    graphicsKey(2, "T", "ctrl-t") {
        translate([0, 1, 0])
        rotate([90, 0, 0])
        cylinder(h = gCut, r = 3);
    }
}

module key_y() {
    translate_u(-.5+1.5+5, 2)
    graphicsKey(2, "Y", "ctrl-y") {
        translate([-4, -1, -4])
        cube([4, gCut, 8]);
    }
}

module key_u() {
    translate_u(-.5+1.5+6, 2)
    graphicsKey(2, "U", "ctrl-u") {
        translate([-4, -1, -4])
        cube([8, gCut, 4]);
    }
}

module key_i() {
    translate_u(-.5+1.5+7, 2)
    graphicsKey(2, "I", "ctrl-i") {
        translate([0, -1, -4])
        cube([4, gCut, 4]);
    }
}

module key_o() {
    translate_u(-.5+1.5+8, 2)
    graphicsKey(2, "I", "ctrl-i") {
        translate([-4, -1, -4])
        cube([4, gCut, 4]);
    }
}

module key_p() {
    translate_u(-.5+1.5+9, 2)
    graphicsKey(2, "P", "ctrl-p") {
        gCharacter("♣️");
    }
}


// ← ↑ ↓ →
module key_dash() {
    translate_u(-.5+1.5+10, 2)
    graphicsKey3(2, "-", "_", "↑", "ctrl-comma");
}

module key_equal() {
    translate_u(-.5+1.5+11, 2)
    graphicsKey3(2, "=", "|", "↓", "ctrl-comma");
}


module key_return() {
    translate_u(-.5+1.5+12.25, 2)
    u(1.5)
    legend("RETURN", [0,0], long_size)
    oem_row(2)
    preKey()
    key();
}

//////////////////////////////////////

module key_esc() {
    translate_u(-.5, 3)
    u(1.25)
    legend("ESC", [0,0], long_size)
    oem_row(1) key();
}

module key_1() {
    translate_u(-.5+1.25, 3)
    graphicsKey2(1, "1", "!", "ctrl-comma");
}

module key_2() {
    translate_u(-.5+1.25+1, 3)
    graphicsKey2(1, "2", "\"", "ctrl-comma");
}

module key_3() {
    translate_u(-.5+1.25+2, 3)
    graphicsKey2(1, "3", "#", "ctrl-comma");
}

module key_4() {
    translate_u(-.5+1.25+3, 3)
    graphicsKey2(1, "4", "$", "ctrl-comma");
}

module key_5() {
    translate_u(-.5+1.25+4, 3)
    graphicsKey2(1, "5", "%", "ctrl-comma");
}

module key_6() {
    translate_u(-.5+1.25+5, 3)
    graphicsKey2(1, "6", "&", "ctrl-comma");
}

module key_7() {
    translate_u(-.5+1.25+6, 3)
    graphicsKey2(1, "7", "'", "ctrl-comma");
}

module key_8() {
    translate_u(-.5+1.25+7, 3)
    graphicsKey2(1, "8", "&", "ctrl-comma");
}

module key_9() {
    translate_u(-.5+1.25+8, 3)
    graphicsKey2(1, "9", "(", "ctrl-comma");
}

module key_0() {
    translate_u(-.5+1.25+9, 3)
    graphicsKey2(1, "0", ")", "ctrl-comma");
}


module key_lt() {
    translate_u(-.5+1.25+10, 3)
    graphicsKey2(1, "<", "CLR", "ctrl-comma");
}

module key_gt() {
    translate_u(-.5+1.25+11, 3)
    graphicsKey2(1, ">", "INS", "ctrl-comma");
}

module key_bs() {
    translate_u(-.5+1.25+12.25, 3)
    graphicsKey2(1, "BACK SP", "DELETE", "ctrl-comma");
}

/////////////////


module key_reset() {
    translate_u(0, 5)
    gridKey("RESET");
}

module key_menu() {
    translate_u(0+1.5, 5)
    gridKey("MENU");
}

module key_turbo() {
    translate_u(0+(1.5 *2), 5)
    gridKey("TURBO");
}

module key_start() {
    translate_u(0+(1.5 *3), 5)
    gridKey("START");
}

module key_select() {
    translate_u(0+(1.5 *4), 5)
    gridKey("SELECT");
}

module key_option() {
    translate_u(0+(1.5 *5), 5)
    gridKey("OPTION");
}

module key_help() {
    translate_u(0+(1.5 *6), 5)
    gridKey("HELP");
}

module key_inverse() {
    translate_u(0+(1.5 *7), 5)
    gridKey("INV");
}

module key_break() {
    translate_u(0+(1.5 *8), 5)
    gridKey("BREAK");
}

/////////////////////////////////////
module key_spacebar() {
  $inverted_dish = $dish_type != "disable";
  $dish_type = $dish_type != "disable" ? "sideways cylindrical" : "disable";

  translate_u(7, -1)
  6_25u() stabilized(mm=50) key();
}


module key_up() {
    translate_u(14, 0)
    graphicsKey(2, "↑", "ctrl-y");
}

module key_left() {
    translate_u(13, -1)
    graphicsKey(2, "←", "ctrl-y");
}

module key_down() {
    translate_u(14, -1)
    graphicsKey(2, "↓", "ctrl-y");
 }

module key_right() {
    translate_u(15, -1)
    graphicsKey(2, "→", "ctrl-y");
 }


module key_fn() {
    translate_u(3, -1)
    graphicsKey(1, "FN", "ctrl-y");
}


module fullkeyboard() {
    if (true) {
        key_lshift();
        key_z();
        key_x();
        key_c();
        key_v();
        key_b();
        key_n();
        key_m();
        key_comma();
        key_dot();
        key_slash();
        rshift();
    }

    if (true) {
        key_control();
        key_a();
        key_s();
        key_d();
        key_f();
        key_g();
        key_h();
        key_j();
        key_k();
        key_l();
        key_semi();
        key_plus();
        key_star();
        key_caps();
    }

    if (true) {
        key_tab();
        key_q();
        key_w();
        key_e();
        key_r();
        key_t();
        key_y();
        key_u();
        key_i();
        key_o();
        key_p();
        key_dash();
        key_equal();
        key_return();
    }


    if (true) {
        key_esc();
        key_1();
        key_2();
        key_3();
        key_4();
        key_5();
        key_6();
        key_7();
        key_8();
        key_9();
        key_0();
        key_lt();
        key_gt();
        key_bs();
    }

    if (true) {
        key_reset();
        key_menu();
        key_turbo();
        key_start();
        key_select();
        key_option();
        key_help();
        key_inverse();
        key_break();
    }

    if (true) {
        key_spacebar();

        key_up();
        key_down();
        key_left();
        key_right();

        key_fn();
    }
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
