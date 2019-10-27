height = ((130 * 2) /3)+19*2;
width = 34;
length = 15;


echo(height);
echo(width);
echo(length);


difference() {
    union() {
        cube([width +5, length +3, height +2], center=true);
    }
    
    union() {
        translate([0, 2, 0])
        cube([width, length+2, height+4], center=true);
    }
}

r1w = 2;
r1l = -length + 4+1;

r2l = -length + 12.5;

color("blue") {
    translate([((width - r1w) /2) , (r1l)/2, 0])
    cube([r1w, 2, height], center=true);

    translate([((-width + r1w) /2) , (r1l)/2, 0])
    cube([r1w, 2, height], center=true);

    translate([((width - r1w) /2) , (r2l)/2, 0])
    cube([r1w, 2, height], center=true);

    translate([((-width + r1w) /2) , (r2l)/2, 0])
    cube([r1w, 2, height], center=true);


}

