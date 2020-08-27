# hook-matching
playing with sound on an Arduino.

# this project is in _CHAOTIC STATE_ 
  - it's new and developped on the fly 
  - plan change several times a day :) 
  - whole structure will change often
  - some bugs will never be fixed because the whole structure would have changed

I'll create a branch for each video demo you may find on the web done by me. Branch will correspond to the code of the video

## attained goals
 - allow to describe a scale
 - allow to calculate a note frequency given a scale, the degree on the note on the scale, the octave to play it 
 - allow to change the LA4 reference, in order to fit any instrument (with the semitone granularity)
 - describe a hook as a list of deplacements on a scale and durations
 - describe a hook along with a starting point
 - allow to describe a sheet with rhythm, bpm
 - allow to set logical note duration (eg black = 24, white = 48) to handle triolets with ints
 - play a list of hook to play a sound :) 
 - demo with a song reusing several times the same hook on different starting points
 
 
## future goals:
 - learn the English wordings for music stuff and translate it :)
 - clean up the code and make it easy to use
 - add scematics for a decent interface :)
 - play several notes / frequency at the same time (maybe same buzzer, maybe different buzzers)
 - Recognizing patterns in music sheets
 - detecting notes from audio microphone
 - maybe improvising some day on what the program heard
 - maybe handle some day oriental scales with quart of tone granularity
