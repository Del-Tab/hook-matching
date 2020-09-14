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
 - allow to change the LA4 reference, in order to fit any instrument (with the semitone granularity)
 - describe a hook as a list of deplacements on a scale and durations
 - describe a hook along with a starting point
 - allow to describe a sheet with rhythm, bpm
 - allow to set logical note duration (eg Quarter note/Crotchet = 24, Half note/Minim = 48) to handle triolets with ints
 - play a list of hook to play a sound :) 
 - demo with a song reusing several times the same hook on different starting points
 - a hook can be a list of Playable_s
 - data fetching is asynchronous
 
## future goals:
 - read a hook in advanc to allow linking notes together
 - implement a synchroniation hook
 - represent sheet in classes to simplify writing for non coders
 - handle max hook recursion level to avoid loops
 - learn the English wordings for music stuff and translate it :)
 - clean up the code and make it easy to use
 - guess which chord to play alone with the played music
 - an engine to find better hook representation for a whole partition
 - implement a recycled memory pool to avoid malloc fragmentation
 - add scematics for a decent interface :)
 - play several notes / frequency at the same time (maybe same buzzer, maybe different buzzers)
 - Recognizing patterns in music sheets
 - having 2 states: one playing state and one thinking state (with leds and buttons)
 - store hooks on sd card after thinking state, play them from sd card when in playing state
 - detecting notes from audio microphone, then find some hooks inside
 - maybe improvising some day on what the program heard
 - maybe handle some day oriental scales with quart of tone granularity



following part is only updated on demo releases, there is not any reference play to compare size efficiency yet.
Compile output for this release (programmer = AVRISP MKII, board = Arduino Mega or Mega 2560): 

Sketch uses 7484 bytes (2%) of program storage space. Maximum is 253952 bytes.
Global variables use 318 bytes (3%) of dynamic memory, leaving 7874 bytes for local variables. Maximum is 8192 bytes.
