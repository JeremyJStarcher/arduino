# Portal Name Badge - Animated and Music
A Portal-based interactive name badge for the Arduino.


The Portal Badge was both the crowning piece to my Halloween costume a few years back as well as a study in multi-tasking on the `Arduino` system.

Using nested state machines to emulate co-operative multi-tasking, the `Portal Name Badge` plays three different screens with a repeating background music loop.

## Screens

### Aperture Science Logo

This logo isn't animated, but provides a bold visual presentation regarding the name badge and the information to follow.

### The Badge Owner Status Screen

The Status Screen provides a quick view of the user's name, department, employee number.  At the top of the
screen a flashing banner announces the wearer's status:  `(Still) Alive`.


### The Tweet Screen

This screen selects one of over a dozen quotes from the game and scrolls it up the screen slowly.

To simply the C coding, all of the quotes are word-wrapped at 21 characters in order to fit the OLED screen 
used on this project.  The JavaScript code to format the strings and convert them to an Arduino `.h` file are included in the project source.


## The Music

The basic melody for `Still Alive` plays in the background.

Rather than coding the music sequence on the Arduino itself, the sheet music was manually translated into
`JavaScript` array and played through the Chrome browser.  This allowed a rapid development.  The `JavaScript`
code created the appropriate Adruino `.h` file for playback.

This JavaScript translation code is provided in the source.

## Known issues

### Music rushes to get catch up after lags.

Sometimes when updating the OLED, the music may fall behind.  The playback system will rush through the next several
notes until it has caught up.

### Sometimes the sound simply goes -- weird.

I believe this may be related to one of the timers being re-used during the OLED update code.  Sometimes the 
pitch will simply go ... wrong for a note or two.  It quickly fixes itself.

Given the rather quirky nature of the name and the various minor malfunctions within GladOS herself, 
I felt both of these flaws to fit quite nicely within the mood I was attempting to create.

## The Code

I wrote the code to be as straight-forward as I could.


The `setup` function set up number of housekeeping values then calls this functions:

    setDisplayState(TWEET);
    music(INITIALIZE);

`setDisplayState` changes which of the three screens is currently being displayed and calls the appropriate
function with the `INITIALIZE` parameter.

The `music` call initializes the music playback routines.

Within the `loop` function, both `music` and the current screen's function are called with the `RESUME`
parameter, allowing them one execution.

It is up to the screen itself to decide when it has finished and call `setDisplayState` with the next value.

  


