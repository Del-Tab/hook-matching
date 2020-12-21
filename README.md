# hook-matching
playing with sound on an Arduino.

# this project is in _*CHAOTIC STATE*_ 
  - it's new and developped on the fly, learning by trials and errors, and "oh I remember this syntax"
  - plan may change several times a day :) (maybe even when i'm not working on it)
  - whole structure will change often, struct will disapear, Class will disapear and reapear
  - some functionalities will be deemed useless and shamelessly abandonned
  - some bugs will never be fixed because the whole structure would have changed

I'll create a branch for each video demo you may find on the web done by me. Branch will correspond to the code when the video was recorded.

## attained goals
 - allow to describe a scale
 - allow to calculate a note frequency given a scale, the degree of the note on the scale, the octave to play it 
 - allow to change the LA4 reference, in order to fit any instrument, even when they are not configured to LA4=440.0Hz
 - describe a hook as a list of offset on a scale with their durations
 - call a hook inside a hook with a starting point offset
 - allow to describe a sheet with rhythm, bpm, and scale
 - allow to set logical note duration (eg Quarter note/Crotchet = 24, Half note/Minim = 48) to handle triolets with ints
 - play a list of hook as a music :)
 - handling coordinates inside a playing process
 - handling a silence flag
 - handling a "force sharp", a "force flat" and a "force natural" flags
 - demo with a song using several times the same hook on different starting points
 - a hook can be a list of Playable_s
 - fetching hook data and playing them is asynchronous (you can handle several voices together)
 - play several notes / frequency at the same time (on the same buzzer) while still being time-coordinated
 
## future goals:
 - read a hook in advance to allow linking notes together
 - implement a synchroniation hook
 - implement state changing hook (changing the scale for example)
 - represent sheet in classes to simplify writing for non coders
 - handle max hook recursion level to avoid loops (there is a MAX macro, coordinates depend on it, but no control for now)
 - having a registry of already existing hook to avoid memory overuse and leaks
 - learn the English wordings for music stuff and translate it :)
 - clean up the code and make it easy to use
 - guess which chord to play alone with the played music
 - an engine to find better hook representation for a whole partition
 - implement a recycled memory pool to avoid malloc fragmentation
 - add scematics for a decent interface :)
 - Recognizing patterns in music sheets (aka processing music)
 - when processing music: use sd card as persistence and reset the card to avoid memory fragmentation
 - having 2 states: one playing state and one processing state (with leds and buttons)
 - store hooks on sd card after thinking state, play them from sd card when in playing state
 - detecting notes from audio microphone, then find some hooks inside
 - maybe improvising some day on what the program heard
 - maybe handle some day oriental scales with quart of tone granularity



following part is only updated on demo releases, there is not any reference play to compare size efficiency yet.
Compile output for this release (programmer = AVRISP MKII, board = Arduino Mega or Mega 2560): 

Sketch uses 15180 bytes (5%) of program storage space. Maximum is 253952 bytes.
Global variables use 481 bytes (5%) of dynamic memory, leaving 7711 bytes for local variables. Maximum is 8192 bytes.
