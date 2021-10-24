// This is an auto-generated file created by 'convert.py'
//
module atari_keyboard() {

// Key: RESET
if (PRINT_KEY == "" || PRINT_KEY == "\U000052\U000045\U000053\U000045\U000054") translate([11.899999999999999, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000052\U000045\U000042\U00004f\U00004f\U000054","","","\U000052\U000045\U000053\U000045\U000054","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: MENU
if (PRINT_KEY == "" || PRINT_KEY == "\U00004d\U000045\U00004e\U000055") translate([45.22, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004d\U000045\U00004e\U000055","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: TURBO
if (PRINT_KEY == "" || PRINT_KEY == "\U000054\U000055\U000052\U000042\U00004f") translate([69.02, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000054\U000055\U000052\U000042\U00004f","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: START
if (PRINT_KEY == "" || PRINT_KEY == "\U000053\U000054\U000041\U000052\U000054") translate([159.46, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000053\U000054\U000041\U000052\U000054","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: SELECT
if (PRINT_KEY == "" || PRINT_KEY == "\U000053\U000045\U00004c\U000045\U000043\U000054") translate([183.26, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000053\U000045\U00004c\U000045\U000043\U000054","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: OPTION
if (PRINT_KEY == "" || PRINT_KEY == "\U00004f\U000050\U000054\U000049\U00004f\U00004e") translate([207.06, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004f\U000050\U000054\U000049\U00004f\U00004e","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: HELP
if (PRINT_KEY == "" || PRINT_KEY == "\U000048\U000045\U00004c\U000050") translate([230.85999999999999, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000048\U000045\U00004c\U000050","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: INV
if (PRINT_KEY == "" || PRINT_KEY == "\U000049\U00004e\U000056") translate([254.66, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000049\U00004e\U000056","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: BREAK
if (PRINT_KEY == "" || PRINT_KEY == "\U000042\U000052\U000045\U000041\U00004b") translate([278.46, -0.0, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000042\U000052\U000045\U000041\U00004b","","","","","","","" ];
  difference() {
  u(1.25) uh(1.25)
      key();
  render_txt(23.799999999999997, 23.799999999999997, legend_map);
} // difference
}

// Key: ESC
if (PRINT_KEY == "" || PRINT_KEY == "\U000045\U000053\U000043") translate([11.899999999999999, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000045\U000053\U000043","","","","","","","" ];
  difference() {
  u(1.25) uh(1.0)
      key();
  render_txt(23.799999999999997, 19.04, legend_map);
} // difference
}

// Key: 1
if (PRINT_KEY == "" || PRINT_KEY == "\U000031") translate([33.31999999999999, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000021","","","","","","\U000031","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 2
if (PRINT_KEY == "" || PRINT_KEY == "\U000032") translate([52.36, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000022","","","","","","\U000032","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 3
if (PRINT_KEY == "" || PRINT_KEY == "\U000033") translate([71.39999999999999, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000023","","","","","","\U000033","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 4
if (PRINT_KEY == "" || PRINT_KEY == "\U000034") translate([90.44, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000024","","","","","","\U000034","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 5
if (PRINT_KEY == "" || PRINT_KEY == "\U000035") translate([109.47999999999999, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000025","","","","","","\U000035","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 6
if (PRINT_KEY == "" || PRINT_KEY == "\U000036") translate([128.52, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000026","","","","","","\U000036","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 7
if (PRINT_KEY == "" || PRINT_KEY == "\U000037") translate([147.56, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000027","","","","","","\U000037","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 8
if (PRINT_KEY == "" || PRINT_KEY == "\U000038") translate([166.6, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000040","","","","","","\U000038","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 9
if (PRINT_KEY == "" || PRINT_KEY == "\U000039") translate([185.64000000000001, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000028","","","","","","\U000039","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 0
if (PRINT_KEY == "" || PRINT_KEY == "\U000030") translate([204.68, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000029","","","","","","\U000030","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: <
if (PRINT_KEY == "" || PRINT_KEY == "\U00003c") translate([223.72, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000043\U00004c\U000052","","","","","","\U00003c","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: >
if (PRINT_KEY == "" || PRINT_KEY == "\U00003e") translate([242.76, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000049\U00004e\U000053","","","","","","\U00003e","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: BS
if (PRINT_KEY == "" || PRINT_KEY == "\U000042\U000053") translate([271.32, -28.56, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U000044\U000045\U00004c","","","","","","\U000042\U000053","","","","" ];
  difference() {
  u(2) uh(1.0)
    stabilized()
      key();
  render_txt(38.08, 19.04, legend_map);
} // difference
}

// Key: TAB
if (PRINT_KEY == "" || PRINT_KEY == "\U000054\U000041\U000042") translate([16.66, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "\U000043\U00004c\U000052","","\U000053\U000045\U000054","","","","","\U000054\U000041\U000042","","","","" ];
  difference() {
  u(1.75) uh(1.0)
      key();
  render_txt(33.32, 19.04, legend_map);
} // difference
}

// Key: Q
if (PRINT_KEY == "" || PRINT_KEY == "\U000051") translate([42.84, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000051","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-q.svg", center=true); 
}

// Key: W
if (PRINT_KEY == "" || PRINT_KEY == "\U000057") translate([61.879999999999995, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000057","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-w.svg", center=true); 
}

// Key: E
if (PRINT_KEY == "" || PRINT_KEY == "\U000045") translate([80.91999999999999, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000045","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-e.svg", center=true); 
}

// Key: R
if (PRINT_KEY == "" || PRINT_KEY == "\U000052") translate([99.96, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000052","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-r.svg", center=true); 
}

// Key: T
if (PRINT_KEY == "" || PRINT_KEY == "\U000054") translate([118.99999999999999, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000054","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-t.svg", center=true); 
}

// Key: Y
if (PRINT_KEY == "" || PRINT_KEY == "\U000059") translate([138.04, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000059","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-y.svg", center=true); 
}

// Key: U
if (PRINT_KEY == "" || PRINT_KEY == "\U000055") translate([157.08, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000055","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-u.svg", center=true); 
}

// Key: I
if (PRINT_KEY == "" || PRINT_KEY == "\U000049") translate([176.12, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000049","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-i.svg", center=true); 
}

// Key: O
if (PRINT_KEY == "" || PRINT_KEY == "\U00004f") translate([195.16, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004f","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-o.svg", center=true); 
}

// Key: P
if (PRINT_KEY == "" || PRINT_KEY == "\U000050") translate([214.2, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000050","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-p.svg", center=true); 
}

// Key: -
if (PRINT_KEY == "" || PRINT_KEY == "\U00002d") translate([233.24, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "\U002191","\U00005f","","","","","","\U00002d","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: =
if (PRINT_KEY == "" || PRINT_KEY == "\U00003d") translate([252.28, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "\U002193","\U00007c","","","","","","\U00003d","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: RETURN
if (PRINT_KEY == "" || PRINT_KEY == "\U000052\U000045\U000054\U000055\U000052\U00004e") translate([276.08, -47.599999999999994, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000052\U000045\U000054\U000055\U000052\U00004e","","","","","","","" ];
  difference() {
  u(1.5) uh(1.0)
      key();
  render_txt(28.56, 19.04, legend_map);
} // difference
}

// Key: <span class="ctrl">CONTROL</span>
if (PRINT_KEY == "" || PRINT_KEY == "\U00003c\U000073\U000070\U000061\U00006e\U000020\U000063\U00006c\U000061\U000073\U000073\U00003d\U000022\U000063\U000074\U000072\U00006c\U000022\U00003e\U000043\U00004f\U00004e\U000054\U000052\U00004f\U00004c\U00003c\U00002f\U000073\U000070\U000061\U00006e\U00003e") translate([19.04, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000043\U00004f\U00004e\U000054\U000052\U00004f\U00004c","","","","","","","" ];
  difference() {
  u(2) uh(1.0)
    stabilized()
      key();
  render_txt(38.08, 19.04, legend_map);
} // difference
}

// Key: A
if (PRINT_KEY == "" || PRINT_KEY == "\U000041") translate([47.599999999999994, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000041","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-a.svg", center=true); 
}

// Key: S
if (PRINT_KEY == "" || PRINT_KEY == "\U000053") translate([66.64, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000053","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-s.svg", center=true); 
}

// Key: D
if (PRINT_KEY == "" || PRINT_KEY == "\U000044") translate([85.67999999999999, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000044","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-d.svg", center=true); 
}

// Key: F
if (PRINT_KEY == "" || PRINT_KEY == "\U000046") translate([104.71999999999998, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000046","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-f.svg", center=true); 
}

// Key: G
if (PRINT_KEY == "" || PRINT_KEY == "\U000047") translate([123.75999999999999, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000047","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-g.svg", center=true); 
}

// Key: H
if (PRINT_KEY == "" || PRINT_KEY == "\U000048") translate([142.8, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000048","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-h.svg", center=true); 
}

// Key: J
if (PRINT_KEY == "" || PRINT_KEY == "\U00004a") translate([161.84, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004a","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-j.svg", center=true); 
}

// Key: K
if (PRINT_KEY == "" || PRINT_KEY == "\U00004b") translate([180.88, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004b","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-k.svg", center=true); 
}

// Key: L
if (PRINT_KEY == "" || PRINT_KEY == "\U00004c") translate([199.92, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004c","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-l.svg", center=true); 
}

// Key: ;
if (PRINT_KEY == "" || PRINT_KEY == "\U00003b") translate([218.96, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U00003a","","","","","","\U00003b","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-semi.svg", center=true); 
}

// Key: +
if (PRINT_KEY == "" || PRINT_KEY == "\U00002b") translate([238.0, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "\U002190","\U00005c","","","","","","\U00002b","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: *
if (PRINT_KEY == "" || PRINT_KEY == "\U00002a") translate([257.03999999999996, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "\U002192","\U00005e","","","","","","\U00002a","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: CAPS
if (PRINT_KEY == "" || PRINT_KEY == "\U000043\U000041\U000050\U000053") translate([278.46, -66.64, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000043\U000041\U000050\U000053","","","","","","","" ];
  difference() {
  u(1.25) uh(1.0)
      key();
  render_txt(23.799999999999997, 19.04, legend_map);
} // difference
}

// Key: [L]SHIFT
if (PRINT_KEY == "" || PRINT_KEY == "\U00005b\U00004c\U00005d\U000053\U000048\U000049\U000046\U000054") translate([23.799999999999997, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00005b\U00004c\U00005d\U000053\U000048\U000049\U000046\U000054","","","","","","","" ];
  difference() {
  u(2.5) uh(1.0)
    stabilized()
      key();
  render_txt(47.599999999999994, 19.04, legend_map);
} // difference
}

// Key: Z
if (PRINT_KEY == "" || PRINT_KEY == "\U00005a") translate([57.11999999999999, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00005a","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-z.svg", center=true); 
}

// Key: X
if (PRINT_KEY == "" || PRINT_KEY == "\U000058") translate([76.16, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000058","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-x.svg", center=true); 
}

// Key: C
if (PRINT_KEY == "" || PRINT_KEY == "\U000043") translate([95.19999999999999, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000043","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-c.svg", center=true); 
}

// Key: V
if (PRINT_KEY == "" || PRINT_KEY == "\U000056") translate([114.24, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000056","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-v.svg", center=true); 
}

// Key: B
if (PRINT_KEY == "" || PRINT_KEY == "\U000042") translate([133.28, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000042","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-b.svg", center=true); 
}

// Key: N
if (PRINT_KEY == "" || PRINT_KEY == "\U00004e") translate([152.32, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004e","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-n.svg", center=true); 
}

// Key: M
if (PRINT_KEY == "" || PRINT_KEY == "\U00004d") translate([171.36, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00004d","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-m.svg", center=true); 
}

// Key: ,
if (PRINT_KEY == "" || PRINT_KEY == "\U00002c") translate([190.4, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U00005b","","","","","","\U00002c","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-comma.svg", center=true); 
}

// Key: .
if (PRINT_KEY == "" || PRINT_KEY == "\U00002e") translate([209.44, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U00005d","","","","","","\U00002e","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
color("white") front_placement()  rotate([90, 0, 0])
 linear_extrude(height = 0.5)  resize([$font_size, $font_size]) 
  import ("../svg/ctrl-period.svg", center=true); 
}

// Key: /
if (PRINT_KEY == "" || PRINT_KEY == "\U00002f") translate([228.48, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","\U00003f","","","","","","\U00002f","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: [R]SHIFT
if (PRINT_KEY == "" || PRINT_KEY == "\U00005b\U000052\U00005d\U000053\U000048\U000049\U000046\U000054") translate([254.66, -85.67999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U00005b\U000052\U00005d\U000053\U000048\U000049\U000046\U000054","","","","","","","" ];
  difference() {
  u(1.75) uh(1.0)
      key();
  render_txt(33.32, 19.04, legend_map);
} // difference
}

// Key: ↑
if (PRINT_KEY == "" || PRINT_KEY == "\U002191") translate([285.59999999999997, -90.44, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U002191","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: FN
if (PRINT_KEY == "" || PRINT_KEY == "\U000046\U00004e") translate([23.799999999999997, -104.72, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U000046\U00004e","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: 
if (PRINT_KEY == "" || PRINT_KEY == "") translate([149.94, -104.72, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","","","","","","","","" ];
  difference() {
  u(6.25) uh(1.0)
    stabilized()
      key();
  render_txt(119.0, 19.04, legend_map);
} // difference
}

// Key: ←
if (PRINT_KEY == "" || PRINT_KEY == "\U002190") translate([266.55999999999995, -109.47999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U002190","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: ↓
if (PRINT_KEY == "" || PRINT_KEY == "\U002193") translate([285.59999999999997, -109.47999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U002193","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}

// Key: →
if (PRINT_KEY == "" || PRINT_KEY == "\U002192") translate([304.64, -109.47999999999999, 0])
key_profile(key_profile, row) legend("",size=5)
{
  legend_map = [ "","","","","\U002192","","","","","","","" ];
  difference() {
  u(1.0) uh(1.0)
      key();
  render_txt(19.04, 19.04, legend_map);
} // difference
}
} // Atari Keyboard
