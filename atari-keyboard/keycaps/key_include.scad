
print_target = "fdm"; // "fdm"; // "resin"

// How far to rotate the keys when they are generated
atari_rotation = print_target == "fdm" ? 0 : 45;

// how far below the "zero line" to start making the supports.
sink = -9;
// Diamater of supports at one end
support_r1 = 0.5;
// Diamater of supports at other end
support_r2 = 0.5;

// Font size of one-letter per key
// full_size = 9;
full_size = 5;

// Font size of words written on a key
long_size = 4;
// Font size if there are multiple lines on a key.
half_size = 5; // 4.5;
grid_key_size = 4;
arrow_size = 6;


base_height = 0.4;

$keytop_thickness = 2;


// Supports for the stem, as it often comes off during printing. Reccommended for most machines
// $stem_support_type = "tines"; // [tines, brim, disabled]

$stem_support_type = "brim"; //  "disable"; // "disable"; // "tines"
$inset_legend_depth = 1.5;
// $clearance_check = true;

// $font = "Liberation Mono:style=Bold";
$font = "Liberation Sans:style=Bold";

