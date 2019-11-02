var playAlive = (function() {
  "use strict";
  var out = [];

  var outNote = [];
  var outStartTime = [];

  var p = pitches;

  var eighnotelen = 3000;

  var music = [
    // Key of A major/ F# minor
    // F#, C#, G#

    //LINE 1: 1
    p.A4, 1 / 8,
    p.GS4, 1 / 8,
    p.FS4, 1 / 8,
    p.FS4, 1 / 8,

    //LINE 1:2
    p.GS4, 1 / 8,
    0, 1 / 8,
    0, 1 / 4,
    0, 1 / 2,

    //LINE 1:3
    0, 1 / 8,
    0, 1 / 8,
    0, 1 / 8,
    p.B3, 1 / 8,
    p.A4, 1 / 8,
    p.GS4, 1 / 8,
    p.E4, 1 / 8,
    p.E4, 1 / 8,

    //LINE 1:4
    0, 1 / 8,
    0, 1 / 8,
    p.GS4, 1 / 8,
    p.E4, 1 / 8,
    0, 1 / 8,
    0, 1 / 8,
    p.FS4, 1 / 8,
    p.B3, 1 / 8,

    //LINE 2: 1
    0, 1 / 1,

    //LINE 2:2
    p.FS4, 1 / 8,
    p.GS4, 1 / 8,
    p.A4, (1 / 4) * 1.5,
    p.FS4, 1 / 8,

    //LINE 2:3
    p.DS4, 1 / 4, //Eb 
    p.E4, 1 / 8,
    p.FS4, (1 / 4) * 1.5,
    p.B3, 1 / 8,
    p.B3, 1 / 8 + 1 / 8,

    //LINE 2:4 (continued)
    p.GS4, 1 / 4,
    0, 1 / 8,
    0, 1 / 2,

    //LINE 3:1
    0, 1 / 8,
    0, 1 / 8,
    0, 1 / 8,
    p.B3, 1 / 8,
    p.A4, 1 / 8,
    p.GS4, 1 / 8,
    p.FS4, 1 / 8,
    p.FS4, 1 / 8,

    //LINE 3:2
    p.GS4, 1 / 8,
    0, 1 / 8,
    0, 1 / 4,
    0, 1 / 2,

    //LINE 3:3
    0, 1 / 8,
    0, 1 / 8,
    0, 1 / 8,
    p.B3, 1 / 8,
    p.A4, 1 / 8,
    p.GS4, 1 / 8,
    p.FS4, 1 / 8,
    p.FS4, 1 / 8,

    //LINE 3:4
    0, 1 / 8,
    0, 1 / 8,
    p.GS4, 1 / 8,
    p.E4, 1 / 8,
    0, 1 / 8,
    0, 1 / 8,
    p.FS4, 1 / 8,
    p.B3, 1 / 8,

    //LINE 3:5
    0, 1 / 1,

    //LINE 4:1
    p.FS4, 1 / 4,
    p.GS4, 1 / 8,
    p.A4, (1 / 4) * 1.5,
    p.FS4, 1 / 4,

    //LINE 4:2
    p.DS4, 1 / 4, //* Eb
    p.E4, 1 / 8, //* E nat
    p.FS4, 1 / 4,
    p.B3, 1 / 8,
    p.C4, 1 / 8, //* C nat 
    p.D4, 1 / 8,

    //LINE 4:3
    p.GS4, 1 / 8,
    p.FS4, 1 / 4,
    p.E4, 1 / 8,
    p.D4, 1 / 8,
    0, 1 / 4,
    p.B3, 1 / 8,
    p.C4, 1 / 8, //* C nat 

    //LINE 4:4
    //KEY CHANGE
    // KEY OF G 
    // F#

    p.D4, 1 / 4,
    p.G4, 1 / 4,
    p.FS4, 1 / 8,
    p.E4, 1 / 8,
    p.E4, 1 / 8,
    p.D4, 1 / 8,

    //LINE 5:1
    p.E4, 1 / 8,
    p.D4, 1 / 8,
    p.D4, 1 / 4,
    p.B3, 1 / 8,
    p.C4, 1 / 8,

    //LINE 5:2
    p.D4, 1 / 4,
    p.G4, 1 / 4,
    p.A4, 1 / 8,
    p.G4, 1 / 8,
    p.FS4, 1 / 8,
    p.E4, 1 / 8,

    //LINE 5:3
    p.E4, 1 / 8,
    p.FS4, 1 / 8,
    p.G4, 1 / 4,
    p.G4, 1 / 4,
    p.A4, 1 / 8,
    p.B4, 1 / 8,

    //LINE 5:4
    p.C5, 1 / 8,
    p.C5, 1 / 8,
    p.B4, 1 / 4,
    p.A4, 1 / 4,
    p.G4, 1 / 8,
    p.A4, 1 / 8,

    //LINE 5:5
    p.B4, 1 / 8,
    p.B4, 1 / 8,
    p.A4, 1 / 8,
    p.G4, 1 / 8,
    p.G4, 1 / 4,
    p.E4, 1 / 8,
    p.D4, 1 / 8,

    //LINE 6:1
    p.E4, 1 / 8,
    p.G4, 1 / 8,
    p.G4, 1 / 8,
    p.FS4, 1 / 4,
    p.GS4, 1 / 8, //* G# 
    p.G4, 1 / 8 + 1 / 4,

    0, 0
  ];

  function queueNote(time, note, duration) {

    outNote.push(note);
    outStartTime.push(time);

    window.setTimeout(function() {
      stopTone();
    }, time - 50);


    window.setTimeout(function() {
      if (note !== 0) {
        startTone(note);
      }
    }, time);

  }

  function output(s) {
    var el = document.getElementById("code_out");
    var txt = el.value;
    el.value = txt + "\n" + s;
  }

  function outputArray(a) {
    var lineLength = 70;
    var txt = "";
    var len = a.length;

    a.forEach(function(item, idx, array) {

      if (idx !== len - 1) {
        item += ", ";
      }

      if ((txt.length + item.length) > lineLength) {
        output(txt);
        txt = item;
      } else {
        txt += item;
      }
    });
    output(txt);
  }

  return function() {

    var time = 0;

    for (var i = 0; i < music.length; i += 2) {
      var note = music[i];
      var notelen = music[i + 1];
      var duration = notelen * eighnotelen;

      console.log(time, note, notelen, duration);

      queueNote(time, note, duration);
      time += duration;
    }

    window.setTimeout(function() {
      stopTone();
    }, time);

    output("/* This file was generated automatically by */");
    output("/* build_tools/music_generator/music_to_c.html */");
    output("");
    output("");

    output("/* The frequencies to play, in order. */");
    output("static const unsigned int PROGMEM  stillAliveNotes[] = {");
    outputArray(outNote);
    output("};");

    output("");
    output("");
    output("/* The relative start time of each note, which is derived from the duration. */");
    output("static const unsigned long PROGMEM  stillAliveStartTime[] = {");
    outputArray(outStartTime);
    output("};");

    output("");
    output("");
    output("/* Makes looping easier. */");
    output("static const unsigned int stillAliveNoteCount = " + outNote.length + ";");
  };

}());