
/* This file was generated automatically by */
/* build_tools/music_generator/music_to_c.html */


/* The frequencies to play, in order. */
static const unsigned int PROGMEM  stillAliveNotes[] = {
440, 415, 370, 370, 415, 0, 0, 0, 0, 0, 0, 247, 440, 415, 330, 330, 
0, 0, 415, 330, 0, 0, 370, 247, 0, 370, 415, 440, 370, 311, 330, 370, 
247, 247, 415, 0, 0, 0, 0, 0, 247, 440, 415, 370, 370, 415, 0, 0, 0, 
0, 0, 0, 247, 440, 415, 370, 370, 0, 0, 415, 330, 0, 0, 370, 247, 0, 
370, 415, 440, 370, 311, 330, 370, 247, 262, 294, 415, 370, 330, 294, 
0, 247, 262, 294, 392, 370, 330, 330, 294, 330, 294, 294, 247, 262, 
294, 392, 440, 392, 370, 330, 330, 370, 392, 392, 440, 494, 523, 523, 
494, 440, 392, 440, 494, 494, 440, 392, 392, 330, 294, 330, 392, 392, 
370, 415, 392, 0
};


/* The relative start time of each note, which is derived from the duration. */
static const unsigned long PROGMEM  stillAliveStartTime[] = {
0, 375, 750, 1125, 1500, 1875, 2250, 3000, 4500, 4875, 5250, 5625, 
6000, 6375, 6750, 7125, 7500, 7875, 8250, 8625, 9000, 9375, 9750, 
10125, 10500, 13500, 13875, 14250, 15375, 15750, 16500, 16875, 18000, 
18375, 19125, 19875, 20250, 21750, 22125, 22500, 22875, 23250, 23625, 
24000, 24375, 24750, 25125, 25500, 26250, 27750, 28125, 28500, 28875, 
29250, 29625, 30000, 30375, 30750, 31125, 31500, 31875, 32250, 32625, 
33000, 33375, 33750, 36750, 37500, 37875, 39000, 39750, 40500, 40875, 
41625, 42000, 42375, 42750, 43125, 43875, 44250, 44625, 45375, 45750, 
46125, 46875, 47625, 48000, 48375, 48750, 49125, 49500, 49875, 50625, 
51000, 51375, 52125, 52875, 53250, 53625, 54000, 54375, 54750, 55125, 
55875, 56625, 57000, 57375, 57750, 58125, 58875, 59625, 60000, 60375, 
60750, 61125, 61500, 61875, 62625, 63000, 63375, 63750, 64125, 64500, 
65250, 65625, 66750
};


/* Makes looping easier. */
static const unsigned int stillAliveNoteCount = 126;
