###Changes in 17t:
- Added new gate modes: hold, half hold, rand33, rand50 and rand66
- Added MIDI channel select to global menu

###Changes in 17s:
- Fast chain mode added
    - In pattern select, press and hold a single pattern, then press more in sequence always holding one down to chain up to 16 patterns
- New Gate Types
    - Hold (holds gate open, meant for arpeggios)
    - Half (holds gate open for half of the gate length - meant for arpeggios)
    - Random (Will randomly not trigger a gate, works well with arpeggios)
    
###Changes in 17r:
- Song mode fixes:
    - fixed transition timing for odd time signatures
    - made song mode work with external and midi clock
    - updated to include channel mutes
    - updated to include key channel for each song event index
    - updated song menu to show selection properly in toggle mode
    - channel select fixed in song mode
- Fixed Multiselect selection indicator LED bug

###Changes in 17p:
- Song mode implemented
- Updated font (fixed width of 1s)
- Fixed quantized pattern loading
- Fixed a bug where using quantize mode customization may turn steps on

###Changes in 17n:
- Syses export fixed

###Changes in 17m:
- moved play mode to transport menu
- moved skip step to modulation 1 menu
- fixed random pitch input, removed gate mute for now (will replace with something better later)
- added quantized CV2 option (and restored the old unquantized voltage output option)
- overhauled pattern select, implemented quantized pattern change + bug fixes (first step to song mode :)
- CV2 changes will now update as they are changed
- added clock mode and tempo to EEPROM so they persist over reboots
- fixed some ‘steps turn on by mistake’ bugs
- multiselect velocity now shows in volts
- multiselect mode now correctly sets the offset voltage
- multiselect mode LEDs have been fixed
- envelopes are now unipolar, previously they were bipolar
- fixed a whole bunch of UI + LED issues

###Changes in 17k
- Implemented Random shortcut (ch button + rndm)
    - Allows you to set pitch+gate simultaneously, just pitch, or just gate (for now) 
    - Allows you to set low note for randomization, as well as randomize octave span
    - implemented true random number generator so randoms won’t repeat
- Implemented custom quantize scales
    - matrix buttons allow for input when quantize mode is selected in quantize channel menu
- “Voltage” CV2 output type now sends quantized voltages based on quantize scale
- Multi select has been overhauled and a number of UI and functional bugs have been fixed.
- Fixed bug where clock divisions higher than 1 resulted in arpeggios not playing properly
- Fixed a bug where arpeggios may play on the wrong step, or not play for as long as they should
- Updated LED code to reflect accurate playing status and channel selection status
- Updated Multi select mode LEDs
- made CV2 multi select offset work properly,
- updated multiselect velocity to show in volts
- fixed a bug where going from multi select to pattern select, or going from the random shortcut to pattern select after pressing a step would make pattern change happen instantly to the last button pressed

