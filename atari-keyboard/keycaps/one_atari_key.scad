use <atarikeys.scad>

key = "key_z";

print_one();

ztilt=[-45, 0, 0];
module tilt() {
    rotate(ztilt)
    children();
}
 
module print_one() {
    if (key == "key_lshift" || key == "layout") {
        tilt() key_lshift();
    }
    if (key == "key_z" || key == "layout") {
        tilt() key_z();
    }
    if (key == "key_x" || key == "layout") {
        tilt() key_x();
    }
    if (key == "key_c" || key == "layout") {
        tilt() key_c();
    }
    if (key == "key_v" || key == "layout") {
        tilt() key_v();
    }
    if (key == "key_b" || key == "layout") {
        tilt() key_b();
    }
    if (key == "key_n" || key == "layout") {
        tilt() key_n();
    }
    if (key == "key_m" || key == "layout") {
        tilt() key_m();
    }
    if (key == "key_comma" || key == "layout") {
        tilt() key_comma();
    }
    if (key == "key_dot" || key == "layout") {
        tilt() key_dot();
    }
    if (key == "key_slash" || key == "layout") {
        tilt() key_slash();
    }
    if (key == "rshift" || key == "layout") {
        rshift();
    }
    if (key == "key_control" || key == "layout") {
        tilt() key_control();
    }
    if (key == "key_a" || key == "layout") {
        tilt() key_a();
    }
    if (key == "key_s" || key == "layout") {
        tilt() key_s();
    }
    if (key == "key_d" || key == "layout") {
        tilt() key_d();
    }
    if (key == "key_f" || key == "layout") {
        tilt() key_f();
    }
    if (key == "key_g" || key == "layout") {
        tilt() key_g();
    }
    if (key == "key_h" || key == "layout") {
        tilt() key_h();
    }
    if (key == "key_j" || key == "layout") {
        tilt() key_j();
    }
    if (key == "key_k" || key == "layout") {
        tilt() key_k();
    }
    if (key == "key_l" || key == "layout") {
        tilt() key_l();
    }
    if (key == "key_semi" || key == "layout") {
        tilt() key_semi();
    }
    if (key == "key_plus" || key == "layout") {
        tilt() key_plus();
    }
    if (key == "key_star" || key == "layout") {
        tilt() key_star();
    }
    if (key == "key_caps" || key == "layout") {
        tilt() key_caps();
    }
    if (key == "key_tab" || key == "layout") {
        tilt() key_tab();
    }
    if (key == "key_q" || key == "layout") {
        tilt() key_q();
    }
    if (key == "key_w" || key == "layout") {
        tilt() key_w();
    }
    if (key == "key_e" || key == "layout") {
        tilt() key_e();
    }
    if (key == "key_r" || key == "layout") {
        tilt() key_r();
    }
    if (key == "key_t" || key == "layout") {
        tilt() key_t();
    }
    if (key == "key_y" || key == "layout") {
        tilt() key_y();
    }
    if (key == "key_u" || key == "layout") {
        tilt() key_u();
    }
    if (key == "key_i" || key == "layout") {
        tilt() key_i();
    }
    if (key == "key_o" || key == "layout") {
        tilt() key_o();
    }
    if (key == "key_p" || key == "layout") {
        tilt() key_p();
    }
    if (key == "key_dash" || key == "layout") {
        tilt() key_dash();
    }
    if (key == "key_equal" || key == "layout") {
        tilt() key_equal();
    }
    if (key == "key_return" || key == "layout") {
        tilt() key_return();
    }
    if (key == "key_esc" || key == "layout") {
        tilt() key_esc();
    }
    if (key == "key_1" || key == "layout") {
        tilt() key_1();
    }
    if (key == "key_2" || key == "layout") {
        tilt() key_2();
    }
    if (key == "key_3" || key == "layout") {
        tilt() key_3();
    }
    if (key == "key_4" || key == "layout") {
        tilt() key_4();
    }
    if (key == "key_5" || key == "layout") {
        tilt() key_5();
    }
    if (key == "key_6" || key == "layout") {
        tilt() key_6();
    }
    if (key == "key_7" || key == "layout") {
        tilt() key_7();
    }
    if (key == "key_8" || key == "layout") {
        tilt() key_8();
    }
    if (key == "key_9" || key == "layout") {
        tilt() key_9();
    }
    if (key == "key_0" || key == "layout") {
        tilt() key_0();
    }
    if (key == "key_lt" || key == "layout") {
        tilt() key_lt();
    }
    if (key == "key_gt" || key == "layout") {
        tilt() key_gt();
    }
    if (key == "key_bs" || key == "layout") {
        tilt() key_bs();
    }
    if (key == "key_reset" || key == "layout") {
        tilt() key_reset();
    }
    if (key == "key_menu" || key == "layout") {
        tilt() key_menu();
    }
    if (key == "key_turbo" || key == "layout") {
        tilt() key_turbo();
    }
    if (key == "key_start" || key == "layout") {
        tilt() key_start();
    }
    if (key == "key_select" || key == "layout") {
        tilt() key_select();
    }
    if (key == "key_option" || key == "layout") {
        tilt() key_option();
    }
    if (key == "key_help" || key == "layout") {
        tilt() key_help();
    }
    if (key == "key_inverse" || key == "layout") {
        tilt() key_inverse();
    }
    if (key == "key_break" || key == "layout") {
        tilt() key_break();
    }
    if (key == "key_spacebar" || key == "layout") {
        tilt() key_spacebar();
    }
    if (key == "key_up" || key == "layout") {
        tilt() key_up();
    }
    if (key == "key_down" || key == "layout") {
        tilt() key_down();
    }
    if (key == "key_left" || key == "layout") {
        tilt() key_left();
    }
    if (key == "key_right" || key == "layout") {
        tilt() key_right();
    }
    if (key == "key_fn" || key == "layout") {
        tilt() key_fn();
    }
}