STL_DIR="stls"

function make_stl {
   echo "$1"
   flatpak run org.openscad.OpenSCAD -D "key=\"$1\"" -o "$STL_DIR/$1.stl" "one_atari_key.scad" > /dev/null
}

mkdir "$STL_DIR"

#make_stl layout

make_stl key_lshift
make_stl key_z
make_stl key_x
make_stl key_c
make_stl key_v
make_stl key_b
make_stl key_n
make_stl key_m
make_stl key_comma
make_stl key_dot
make_stl key_slash
make_stl rshift
make_stl key_control
make_stl key_a
make_stl key_s
make_stl key_d
make_stl key_f
make_stl key_g
make_stl key_h
make_stl key_j
make_stl key_k
make_stl key_l
make_stl key_semi
make_stl key_plus
make_stl key_star
make_stl key_caps
make_stl key_tab
make_stl key_q
make_stl key_w
make_stl key_e
make_stl key_r
make_stl key_t
make_stl key_y
make_stl key_u
make_stl key_i
make_stl key_o
make_stl key_p
make_stl key_dash
make_stl key_equal
make_stl key_return
make_stl key_esc
make_stl key_1
make_stl key_2
make_stl key_3
make_stl key_4
make_stl key_5
make_stl key_6
make_stl key_7
make_stl key_8
make_stl key_9
make_stl key_0
make_stl key_lt
make_stl key_gt
make_stl key_bs
make_stl key_reset
make_stl key_menu
make_stl key_turbo
make_stl key_start
make_stl key_select
make_stl key_option
make_stl key_help
make_stl key_inverse
make_stl key_break
make_stl key_spacebar

make_stl key_up
make_stl key_down
make_stl key_left
make_stl key_right

make_stl key_fn
 
