// This routine converts a list of GladOS quotes into a format usable
// by the Arduino sketch.
// Step 1:  Perform a word-wrap on the strings, wrapping them all at
//          maxLineLength.  This fits nicely on the screen I selected for
//          this project.  Wrapping here GREATLY reduces the C code size
//          and complexity.
// Step 2:  Wrap everything up in PROGMEM syntax.
//          First: Write out a group of PROGMEM constants with a name
//          Second: Create an PROGMEN array of those c is an array of those constants
//          Third: Spit out a few utility constants to help assist with calculations.
var maxLineLength = 21;


function makeWordWrap(s) {
  // The first character isn't part of the displayed text
  // So don't include it in the wrapping code.
  var firstChar = s.charAt(0);
  var line = s.substring(1, s.length);

  var out = "";
  var bits = line.split(" ");

  var line = "";

  bits.forEach(function(value, index, array1) {

    if (value.length > maxLineLength) {
      throw (new Error("Symbol too long: " + value));
    }

    console.log(value.length, line.length);
    if (value.length + line.length >= maxLineLength) {
      // Force the wrap
      while (out.length % maxLineLength !== 0) {
        out += " ";
      }
      line = "";
    }

    line += value + " ";
    out += value + " ";
  });

  return firstChar + out;
}


function convert() {
  var totalLines = 0;
  var out = [];
  var s;
  var longestString = 0;

  out.push("//This file is generated semi-automatically from glados_quotes.html");
  out.push("//Do not hand edit");
  out.push("");

  var text = document.getElementById("input").value;

  // Normalize EOL characters.
  var text = text.replace(/\r\n/g, "\r");
  var text = text.replace(/\n/g, "\r");

  var lines = text.split("\r");

  for (var i = 0; i < lines.length; i++) {
    var line = lines[i].trim();
    if (line === "") {
      continue;
    }

    if (line.charAt(0) === "/") {
      // It's a comment, so pass it through un-altered.
      s = line;
    } else {
      longestString = Math.max(longestString, line.length);

      line = line.replace(/"/g, '\\"');
      line = makeWordWrap(line);

      s = 'const char glados_line#L#[] PROGMEM  = "#S#";';
      s = s.replace("#L#", totalLines);
      s = s.replace("#S#", line);
      totalLines++;
    }
    out.push(s);
  }

  //out.push("const int longestGladosString = " + longestString + ";");
  out.push("const int GladosStrings = " + totalLines + ";");

  out.push("const char* const glados_table[] PROGMEM = {");

  for (var i = 0; i < totalLines; i++) {
    var s = 'glados_line#L#,';
    s = s.replace("#L#", i);
    out.push(s);
  }
  out.push("NULL };");

  document.getElementById("output").value = out.join("\r");
}