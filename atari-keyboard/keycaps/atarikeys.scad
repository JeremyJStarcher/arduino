include <./KeyV2/includes.scad>

RENDER_FOR_SLA=true;

$keytop_thickness = 2;


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

module graphicsKey(row, legend, svg) {
    difference() {
        u(1)
        legend(legend, [0,0], full_size)
        oem_row(row)  key();

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
        oem_row(4) key();

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
      grid_row(1) key();
    }

    h = 6;
    w = total_key_width();


    // Draw the re-enforced walls
    color("cyan")
    difference() {
      hull()
      intersection() {
        key1();
        translate([-50, -50, 0]) cube([100, 100, h]);
      }

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
    oem_row(4)  key();
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
    u(1)
    legend("/", [0,1], half_size)
    legend("?", [0,-1], half_size)
    oem_row(4) key();
}

module rshift() {
    translate_u(1.75 +10.5, 0)
    u(1.75)
    legend("SHIFT", [0,0], long_size)
    oem_row(4) key();
}


/// ROW 3


module key_control() {
    translate_u(-.5, 1)
    u(2)
    stabilized()
    legend("CONTROL", [0,0], long_size)
    oem_row(3) key();
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
    u(1)
    legend("+", [0,1], half_size)
    legend("\\", [1,-1], half_size)
    legend("←", [-1,-1], half_size)
    oem_row(3) key();
}

module key_star() {
    translate_u(-.5+1.5+11, 1)
    u(1)
    legend("*", [0,1], half_size)
    legend("^", [1,-1], half_size)
    legend("→", [-1,-1], half_size)
    oem_row(3) key();
}


module key_caps() {
    translate_u(-.5+1.5+12 +.25, 1)
    u(1.25)
    legend("CAPS", [0,0], long_size)
    oem_row(3) key();
}

// ← ↑ ↓ →

// ROW 2

module key_tab() {
    translate_u(-.5, 2)
    u(1.75)
    legend("TAB", [0,1], long_size)
    legend("CLR", [1,-1], long_size)
    legend("SET", [-1,-1], long_size)
    oem_row(2) key();
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
    u(1)
    legend("-", [0,1], half_size)
    legend("_", [1,-1], half_size)
    legend("↑", [-1,-1], half_size)
    oem_row(2) key();
}

module key_equal() {
    translate_u(-.5+1.5+11, 2)
    u(1)
    legend("=", [0,1], half_size)
    legend("|", [1,-1], half_size)
    legend("↓", [-1,-1], half_size)
    oem_row(2) key();
}


module key_return() {
    translate_u(-.5+1.5+12.25, 2)
    u(1.5)
    legend("RETURN", [0,0], long_size)
    oem_row(2) key();
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
    u(1)
    legend("1", [0,1], half_size)
    legend("!", [0,-1], half_size)
    oem_row(1) key();
}

module key_2() {
    translate_u(-.5+1.25+1, 3)
    u(1)
    legend("2", [0,1], half_size)
    legend("\"", [0,-1], half_size)
    oem_row(1) key();
}

module key_3() {
    translate_u(-.5+1.25+2, 3)
    u(1)
    legend("3", [0,1], half_size)
    legend("#", [0,-1], half_size)
    oem_row(1) key();
}

module key_4() {
    translate_u(-.5+1.25+3, 3)
    u(1)
    legend("4", [0,1], half_size)
    legend("$", [0,-1], half_size)
    oem_row(1) key();
}

module key_5() {
    translate_u(-.5+1.25+4, 3)
    u(1)
    legend("5", [0,1], half_size)
    legend("%", [0,-1], half_size)
    oem_row(1) key();
}

module key_6() {
    translate_u(-.5+1.25+5, 3)
    u(1)
    legend("6", [0,1], half_size)
    legend("&", [0,-1], half_size)
    oem_row(1) key();
}

module key_7() {
    translate_u(-.5+1.25+6, 3)
    u(1)
    legend("7", [0,1], half_size)
    legend("'", [0,-1], half_size)
    oem_row(1) key();
}

module key_8() {
    translate_u(-.5+1.25+7, 3)
    u(1)
    legend("8", [0,1], half_size)
    legend("@", [0,-1], half_size)
    oem_row(1) key();
}

module key_9() {
    translate_u(-.5+1.25+8, 3)
    u(1)
    legend("9", [0,1], half_size)
    legend("(", [0,-1], half_size)
    oem_row(1) key();
}

module key_0() {
    translate_u(-.5+1.25+9, 3)
    u(1)
    legend("0", [0,1], half_size)
    legend(")", [0,-1], half_size)
    oem_row(1) key();
}


module key_lt() {
    translate_u(-.5+1.25+10, 3)
    u(1)
    legend("<", [0,1], half_size)
    legend("CLR", [0,-1], half_size)
    oem_row(1) key();
}

module key_gt() {
    translate_u(-.5+1.25+11, 3)
    u(1)
    legend(">", [0,1], half_size)
    legend("INS", [0,-1], half_size)
    oem_row(1) key();
}

module key_bs() {
    translate_u(-.5+1.25+12.25, 3)
    u(2)
    stabilized()
    legend("BACK SP", [0,1], half_size)
    legend("DELETE", [0,-1], half_size)
    oem_row(1) key();
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
    u(1)
    legend("↑", [0,0], full_size)
    oem_row(2) key();
}

module key_left() {
    translate_u(13, -1)
    u(1)
    legend("←", [0,0], full_size)
    oem_row(3) key();
}

module key_down() {
    translate_u(14, -1)
    u(1)
    legend("↓", [0,0], full_size)
    oem_row(3) key();
}

module key_right() {
    translate_u(15, -1)
    u(1)
    legend("→", [0,0], full_size)
    oem_row(3) key();
}


module key_fn() {
    translate_u(3, -1)
    u(1)
    legend("FN", [0,0], long_size)
     key();
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
