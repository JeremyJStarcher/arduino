include <./KeyV2/includes.scad>

RENDER_FOR_SLA=true;

$stem_support_type =  "disable"; // "tines"
$inset_legend_depth = .75;

full_size = 9;
long_size = 4;
half_size = 4;

// example key
* dcs_row(1) legend("⇪", size=9) key();

// example row
 * for (x = [0:1:4]) {
  translate_u(0,-x) dcs_row(x) key();
} 

// example layout
* preonic_default("dcs") key();


tilt=[0, 0, 0];


/// ROW 4

module lshift() {
    translate_u(0, 0)
    rotate(tilt)   
    u(2.25)
    legend("SHIFT", [0,0], long_size)
    oem_row(4)  key();
}

module key_z() {
    translate_u(1.75, 0)
    rotate(tilt)   
    u(1)
    legend("Z", [0,0], full_size)
    oem_row(4)  key();
}

module key_x() {
    translate_u(1.75 +1, 0)
    rotate(tilt)   
    u(1)
    legend("X", [0,0], full_size)
    oem_row(4) key();
}

module key_V() {
    translate_u(1.75 +2, 0)
    rotate(tilt)   
    u(1)
    legend("V", [0,0], full_size)
    oem_row(4) key();
}


module key_c() {
    translate_u(1.75 +2, 0)
    rotate(tilt)   
    u(1)
    legend("C", [0,0], full_size)
    oem_row(4) key();
}

module key_v() {
    translate_u(1.75 +3, 0)
    rotate(tilt)   
    u(1)
    legend("V", [0,0], full_size)
    oem_row(4) key();
}

module key_b() {
    translate_u(1.75 +4, 0)
    rotate(tilt)   
    u(1)
    legend("B", [0,0], full_size)
    oem_row(4) key();
}

module key_n() {
    translate_u(1.75 +5, 0)
    rotate(tilt)   
    u(1)
    legend("N", [0,0], full_size)
    oem_row(4) key();
}


module key_m() {
    translate_u(1.75 +6, 0)
    rotate(tilt)   
    u(1)
    legend("M", [0,0], full_size)
    oem_row(4) key();
}


module key_comma() {
    translate_u(1.75 +7, 0)
    rotate(tilt)   
    u(1)
    legend(",", [0,1], half_size)
    legend("[", [0,-1], half_size)
    oem_row(4) key();
}

module key_dot() {
    translate_u(1.75 +8, 0)
    rotate(tilt)   
    u(1)
    legend(".", [0,1], half_size)
    legend("]", [0,-1], half_size)
    oem_row(4) key();
}

module key_slash() {
    translate_u(1.75 +9, 0)
    rotate(tilt)   
    u(1)
    legend("/", [0,1], half_size)
    legend("?", [0,-1], half_size)
    oem_row(4) key();
}

module rshift() {
    translate_u(1.75 +10.5, 0)
    rotate(tilt)   
    u(1.75)
    legend("SHIFT", [0,0], long_size)
    oem_row(4) key();
}


/// ROW 3


module key_control() {
    translate_u(-.5, 1)
    rotate(tilt)   
    u(2)
    legend("CONTROL", [0,0], long_size)
    oem_row(3) key();
}

module key_a() {
    translate_u(-.5+1.5, 1)
    rotate(tilt)   
    u(1)
    legend("A", [0,0], full_size)
    oem_row(3) key();
}

module key_s() {
    translate_u(-.5+1.5+1, 1)
    rotate(tilt)   
    u(1)
    legend("S", [0,0], full_size)
    oem_row(3) key();
}

module key_d() {
    translate_u(-.5+1.5+2, 1)
    rotate(tilt)   
    u(1)
    legend("D", [0,0], full_size)
    oem_row(3) key();
}

module key_f() {
    translate_u(-.5+1.5+3, 1)
    rotate(tilt)   
    u(1)
    legend("F", [0,0], full_size)
    oem_row(3) key();
}

module key_g() {
    translate_u(-.5+1.5+4, 1)
    rotate(tilt)   
    u(1)
    legend("G", [0,0], full_size)
    oem_row(3) key();
}

module key_h() {
    translate_u(-.5+1.5+5, 1)
    rotate(tilt)   
    u(1)
    legend("H", [0,0], full_size)
    oem_row(3) key();
}

module key_j() {
    translate_u(-.5+1.5+6, 1)
    rotate(tilt)   
    u(1)
    legend("J", [0,0], full_size)
    oem_row(3) key();
}

module key_k() {
    translate_u(-.5+1.5+7, 1)
    rotate(tilt)   
    u(1)
    legend("K", [0,0], full_size)
    oem_row(3) key();
}

module key_l() {
    translate_u(-.5+1.5+8, 1)
    rotate(tilt)   
    u(1)
    legend("L", [0,0], full_size)
    oem_row(3) key();
}

module key_semi() {
    translate_u(-.5+1.5+9, 1)
    rotate(tilt)   
    u(1)
    legend(";", [0,1], half_size)
    legend(":", [0,-1], half_size)
    oem_row(3) key();
}


module key_plus() {
    translate_u(-.5+1.5+10, 1)
    rotate(tilt)   
    u(1)
    legend("+", [0,1], half_size)
    legend("\\", [1,-1], half_size)
    legend("←", [-1,-1], half_size)
    oem_row(3) key();
}

module key_star() {
    translate_u(-.5+1.5+11, 1)
    rotate(tilt)
    u(1)
    legend("*", [0,1], half_size)
    legend("^", [1,-1], half_size)
    legend("→", [-1,-1], half_size)
    oem_row(3) key();
}


module key_caps() {
    translate_u(-.5+1.5+12 +.25, 1)
    rotate(tilt)   
    u(1.25)
    legend("CAPS", [0,0], long_size)
    oem_row(3) key();
}

// ← ↑ ↓ →

// ROW 2

module key_tab() {
    translate_u(-.5, 2)
    rotate(tilt)   
    u(1.75)
    legend("TAB", [0,1], long_size)
    legend("CLR", [1,-1], long_size)
    legend("SET", [-1,-1], long_size)
    oem_row(2) key();
}

module key_q() {
    translate_u(-.5+1.5, 2)
    rotate(tilt)   
    u(1)
    legend("Q", [0,0], full_size)
    oem_row(2) key();
}

module key_w() {
    translate_u(-.5+1.5+1, 2)
    rotate(tilt)   
    u(1)
    legend("W", [0,0], full_size)
    oem_row(2) key();
}

module key_e() {
    translate_u(-.5+1.5+2, 2)
    rotate(tilt)   
    u(1)
    legend("E", [0,0], full_size)
    oem_row(2) key();
}

module key_r() {
    translate_u(-.5+1.5+3, 2)
    rotate(tilt)   
    u(1)
    legend("R", [0,0], full_size)
    oem_row(2) key();
}

module key_t() {
    translate_u(-.5+1.5+4, 2)
    rotate(tilt)   
    u(1)
    legend("T", [0,0], full_size)
    oem_row(2) key();
}

module key_y() {
    translate_u(-.5+1.5+5, 2)
    rotate(tilt)   
    u(1)
    legend("Y", [0,0], full_size)
    oem_row(2) key();
}

module key_u() {
    translate_u(-.5+1.5+6, 2)
    rotate(tilt)   
    u(1)
    legend("U", [0,0], full_size)
    oem_row(2) key();
}

module key_i() {
    translate_u(-.5+1.5+7, 2)
    rotate(tilt)   
    u(1)
    legend("I", [0,0], full_size)
    oem_row(2) key();
}

module key_o() {
    translate_u(-.5+1.5+8, 2)
    rotate(tilt)   
    u(1)
    legend("O", [0,0], full_size)
    oem_row(2) key();
}

module key_p() {
    translate_u(-.5+1.5+9, 2)
    rotate(tilt)   
    u(1)
    legend("P", [0,0], full_size)
    oem_row(2) key();
}



// ← ↑ ↓ →
module key_dash() {
   translate_u(-.5+1.5+10, 2)
    rotate(tilt)
    u(1)
    legend("-", [0,1], half_size)
    legend("_", [1,-1], half_size)
    legend("↑", [-1,-1], half_size)
    oem_row(2) key();
}

module key_equal() {
    translate_u(-.5+1.5+11, 2)
    rotate(tilt)
    u(1)
    legend("=", [0,1], half_size)
    legend("|", [1,-1], half_size)
    legend("↓", [-1,-1], half_size)
    oem_row(2) key();
}


module key_return() {
    translate_u(-.5+1.5+12.25, 2)
    rotate(tilt)   
    u(1.5)
    legend("RETURN", [0,0], long_size)
    oem_row(2) key();
}

//////////////////////////////////////

module key_esc() {
    translate_u(-.5, 3)
    rotate(tilt)   
    u(1.25)
    legend("ESC", [0,0], long_size)
    oem_row(1) key();
}

module key_1() {
    translate_u(-.5+1.25, 3)
    rotate(tilt)   
    u(1)
    legend("1", [0,1], half_size)
    legend("!", [0,-1], half_size)
    oem_row(1) key();
}

module key_2() {
    translate_u(-.5+1.25+1, 3)
    rotate(tilt)   
    u(1)
    legend("2", [0,1], half_size)
    legend("\"", [0,-1], half_size)
    oem_row(1) key();
}

module key_3() {
    translate_u(-.5+1.25+2, 3)
    rotate(tilt)   
    u(1)
    legend("3", [0,1], half_size)
    legend("#", [0,-1], half_size)
    oem_row(1) key();
}

module key_4() {
    translate_u(-.5+1.25+3, 3)
    rotate(tilt)   
    u(1)
    legend("4", [0,1], half_size)
    legend("$", [0,-1], half_size)
    oem_row(1) key();
}

module key_5() {
    translate_u(-.5+1.25+4, 3)
    rotate(tilt)   
    u(1)
    legend("5", [0,1], half_size)
    legend("%", [0,-1], half_size)
    oem_row(1) key();
}

module key_6() {
    translate_u(-.5+1.25+5, 3)
    rotate(tilt)   
    u(1)
    legend("6", [0,1], half_size)
    legend("&", [0,-1], half_size)
    oem_row(1) key();
}

module key_7() {
    translate_u(-.5+1.25+6, 3)
    rotate(tilt)   
    u(1)
    legend("7", [0,1], half_size)
    legend("'", [0,-1], half_size)
    oem_row(1) key();
}

module key_8() {
    translate_u(-.5+1.25+7, 3)
    rotate(tilt)   
    u(1)
    legend("8", [0,1], half_size)
    legend("@", [0,-1], half_size)
    oem_row(1) key();
}

module key_9() {
    translate_u(-.5+1.25+8, 3)
    rotate(tilt)   
    u(1)
    legend("9", [0,1], half_size)
    legend("(", [0,-1], half_size)
    oem_row(1) key();
}

module key_0() {
    translate_u(-.5+1.25+9, 3)
    rotate(tilt)   
    u(1)
    legend("0", [0,1], half_size)
    legend(")", [0,-1], half_size)
    oem_row(1) key();
}


module key_lt() {
    translate_u(-.5+1.25+10, 3)
    rotate(tilt)   
    u(1)
    legend("<", [0,1], half_size)
    legend("CLR", [0,-1], half_size)
    oem_row(1) key();
}

module key_gt() {
    translate_u(-.5+1.25+11, 3)
    rotate(tilt)   
    u(1)
    legend(">", [0,1], half_size)
    legend("INS", [0,-1], half_size)
    oem_row(1) key();
}

module key_bs() {
    translate_u(-.5+1.25+12.25, 3)
    rotate(tilt)   
    u(2)
    legend("BACKSPACE", [0,1], half_size)
    legend("DELETE", [0,-1], half_size)
    oem_row(1) key();
}

/////////////////

module key_reset() {
    translate_u(0, 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("RESET", [0,0], half_size)
    grid_row(1) key();
}

module key_menu() {
    translate_u(0+1.5, 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("MENU", [0,0], half_size)
    grid_row(1) key();
}

module key_turbo() {
    translate_u(0+(1.5 *2), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("TURBO", [0,0], half_size)
    grid_row(1) key();
}

module key_start() {
    translate_u(0+(1.5 *3), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("START", [0,0], half_size)
    grid_row(1) key();
}

module key_select() {
    translate_u(0+(1.5 *4), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("SELECT", [0,0], half_size)
    grid_row(1) key();
}

module key_option() {
    translate_u(0+(1.5 *5), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("OPTION", [0,0], half_size)
    grid_row(1) key();
}

module key_help() {
    translate_u(0+(1.5 *6), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("HELP", [0,0], half_size)
    grid_row(1) key();
}

module key_inverse() {
    translate_u(0+(1.5 *7), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("INV", [0,0], half_size)
    grid_row(1) key();
}

module key_break() {
    translate_u(0+(1.5 *8), 5)
    rotate(tilt)   
    u(1.25)
    uh(1.25)
    legend("BREAK", [0,0], half_size)
    grid_row(1) key();
}



module fullkeyboard() {
    lshift();
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

fullkeyboard();


* key_a();
