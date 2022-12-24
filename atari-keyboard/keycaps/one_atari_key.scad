use <key_desc.scad>

keymode = 4;

$part_mode = keymode;

  key = "key_o";
  print_one();

//   prepKey() key_o();
//   prepKey() key_i();
//   prepKey() key_comma();

module keyWithLegend() {
    $show_legend = true;
    children();
}

module keyWithoutLegend() {
    $show_legend = false;
    children();
}

module prepKey() {
    $set_location = key == "layout";

    if ($part_mode == "2") {
        difference() {
            union() {color("red") keyWithoutLegend() children(); }
            union() {color("white") keyWithLegend() children(); }
        }
    }

    if ($part_mode == "1") {
        keyWithLegend() children();
    }
}

module print_one() {
    if (key == "key_lshift" || key == "layout") {
        prepKey() key_lshift();
    }
    if (key == "key_z" || key == "layout") {
        prepKey() key_z();
    }
    if (key == "key_x" || key == "layout") {
        prepKey() key_x();
    }
    if (key == "key_c" || key == "layout") {
        prepKey() key_c();
    }
    if (key == "key_v" || key == "layout") {
        prepKey() key_v();
    }
    if (key == "key_b" || key == "layout") {
        prepKey() key_b();
    }
    if (key == "key_n" || key == "layout") {
        prepKey() key_n();
    }
    if (key == "key_m" || key == "layout") {
        prepKey() key_m();
    }
    if (key == "key_comma" || key == "layout") {
        prepKey() key_comma();
    }
    if (key == "key_period" || key == "layout") {
        prepKey() key_period();
    }
    if (key == "key_slash" || key == "layout") {
        prepKey() key_slash();
    }
    if (key == "key_rshift" || key == "layout") {
        prepKey() key_rshift();
    }
    if (key == "key_control" || key == "layout") {
        prepKey() key_control();
    }
    if (key == "key_a" || key == "layout") {
        prepKey() key_a();
    }
    if (key == "key_s" || key == "layout") {
        prepKey() key_s();
    }
    if (key == "key_d" || key == "layout") {
        prepKey() key_d();
    }
    if (key == "key_f" || key == "layout") {
        prepKey() key_f();
    }
    if (key == "key_g" || key == "layout") {
        prepKey() key_g();
    }
    if (key == "key_h" || key == "layout") {
        prepKey() key_h();
    }
    if (key == "key_j" || key == "layout") {
        prepKey() key_j();
    }
    if (key == "key_k" || key == "layout") {
        prepKey() key_k();
    }
    if (key == "key_l" || key == "layout") {
        prepKey() key_l();
    }
    if (key == "key_semi" || key == "layout") {
        prepKey() key_semi();
    }
    if (key == "key_plus" || key == "layout") {
        prepKey() key_plus();
    }
    if (key == "key_astrix" || key == "layout") {
        prepKey() key_astrix();
    }
    if (key == "key_caps" || key == "layout") {
        prepKey() key_caps();
    }
    if (key == "key_tab" || key == "layout") {
        prepKey() key_tab();
    }
    if (key == "key_q" || key == "layout") {
        prepKey() key_q();
    }
    if (key == "key_w" || key == "layout") {
        prepKey() key_w();
    }
    if (key == "key_e" || key == "layout") {
        prepKey() key_e();
    }
    if (key == "key_r" || key == "layout") {
        prepKey() key_r();
    }
    if (key == "key_t" || key == "layout") {
        prepKey() key_t();
    }
    if (key == "key_y" || key == "layout") {
        prepKey() key_y();
    }
    if (key == "key_u" || key == "layout") {
        prepKey() key_u();
    }
    if (key == "key_i" || key == "layout") {
        prepKey() key_i();
    }
    if (key == "key_o" || key == "layout") {
        prepKey() key_o();
    }
    if (key == "key_p" || key == "layout") {
        prepKey() key_p();
    }
    if (key == "key_dash" || key == "layout") {
        prepKey() key_dash();
    }
    if (key == "key_equal" || key == "layout") {
        prepKey() key_equal();
    }
    if (key == "key_return" || key == "layout") {
        prepKey() key_return();
    }
    if (key == "key_esc" || key == "layout") {
        prepKey() key_esc();
    }
    if (key == "key_1" || key == "layout") {
        prepKey() key_1();
    }
    if (key == "key_2" || key == "layout") {
        prepKey() key_2();
    }
    if (key == "key_3" || key == "layout") {
        prepKey() key_3();
    }
    if (key == "key_4" || key == "layout") {
        prepKey() key_4();
    }
    if (key == "key_5" || key == "layout") {
        prepKey() key_5();
    }
    if (key == "key_6" || key == "layout") {
        prepKey() key_6();
    }
    if (key == "key_7" || key == "layout") {
        prepKey() key_7();
    }
    if (key == "key_8" || key == "layout") {
        prepKey() key_8();
    }
    if (key == "key_9" || key == "layout") {
        prepKey() key_9();
    }
    if (key == "key_0" || key == "layout") {
        prepKey() key_0();
    }
    if (key == "key_lt" || key == "layout") {
        prepKey() key_lt();
    }
    if (key == "key_gt" || key == "layout") {
        prepKey() key_gt();
    }
    if (key == "key_bs" || key == "layout") {
        prepKey() key_bs();
    }
    if (key == "key_reset" || key == "layout") {
        prepKey() key_reset();
    }
    if (key == "key_menu" || key == "layout") {
        prepKey() key_menu();
    }
    if (key == "key_turbo" || key == "layout") {
        prepKey() key_turbo();
    }
    if (key == "key_start" || key == "layout") {
        prepKey() key_start();
    }
    if (key == "key_select" || key == "layout") {
        prepKey() key_select();
    }
    if (key == "key_option" || key == "layout") {
        prepKey() key_option();
    }
    if (key == "key_help" || key == "layout") {
        prepKey() key_help();
    }
    if (key == "key_power" || key == "layout") {
        prepKey() key_power();
    }
    if (key == "key_inv" || key == "layout") {
        prepKey() key_inv();
    }
    if (key == "key_break" || key == "layout") {
        prepKey() key_break();
    }
    if (key == "key_space" || key == "layout") {
        prepKey() key_space();
    }
    if (key == "key_c_up" || key == "layout") {
        prepKey() key_c_up();
    }
    if (key == "key_c_down" || key == "layout") {
        prepKey() key_c_down();
    }
    if (key == "key_c_left" || key == "layout") {
        prepKey() key_c_left();
    }
    if (key == "key_c_right" || key == "layout") {
        prepKey() key_c_right();
    }
    if (key == "key_fn" || key == "layout") {
        prepKey() key_fn();
    }
}