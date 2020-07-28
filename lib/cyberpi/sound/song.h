
uint8_t voicePrefs[6][4][7]= //the voice parameters for each of the six songs and four channels (voices, instruments). The 5 parameters are wave[0-11], pitch[0-127], envelope[0-3], length[0-127], mod[0-127:64=no mod], MIDI channel, MIDI instrument[0-127]

//if you turn on the printSongs() call in setup you can print your own song to the serial moitor and then paste it in below to change the demo song to your own.

 {
//song 0 is the first half of the demo song, so the first four voices never get changed. 
{{0,87,1,38,64,1,1},
{4,63,1,22,64,1,26},
{1,63,1,60,64,10,0},
{2,75,2,80,64,10,0}},


//song 1 is the second half of the demo song, so the first four voices never get changed.
{{10,62,1,60,64,1,1},
{0,43,1,35,64,10,0},
{4,80,1,35,64,10,0},
{2,75,2,60,64,10,0}},

 {{10,60,1,60,64,1,0},    //voices for song2     song 2 is the one that we write to when we're sequencing. It's initially set up to play the reference notes for each voice when we start the device in Program Mode.
{10,64,1,60,64,1,0},         
{10,67,1,60,64,1,0},            
{10,72,1,60,64,1,0}}, 


 {{0,0,0,0,0,0,0},  //voices for song3     songs 3-5 are used to store additional songs that are loaded from memory and appended.
{0,0,0,0,0,0,0},         
{0,0,0,0,0,0,0},            
{0,0,0,0,0,0,0}}, 

 {{0,0,0,0,0,0,0},     //voices for song4        
{0,0,0,0,0,0,0},         
{0,0,0,0,0,0,0},            
{0,0,0,0,0,0}}, 

 {{0,0,0,0,0,0,0},     //voices for song5   
{0,0,0,0,0,0,0},         
{0,0,0,0,0,0,0},            
{0,0,0,0,0,0,0}},

};


uint8_t song[6][4][16]= //dimension 1 is the song, dimension 2 is the voice (channel), dimension 3 is the MIDI note. A zero value is a rest (no note played).
{
  
//first half of demo song
{{39,67,31,75,87,72,0,79,0,72,43,75,99,63,55,72},
{65,67,0,72,63,67,0,72,63,0,63,0,0,60,0,72},
{0,0,0,0,0,0,0,0,63,48,55,46,67,0,0,0},
{87,0,75,54,87,27,39,87,87,48,55,0,99,0,60,75}},


//second half of demo song  
{{62,62,0,67,67,51,36,24,0,72,27,39,0,48,39,51},
{43,0,43,0,43,0,0,43,43,0,43,0,43,0,0,43},
{0,43,0,43,0,80,0,80,80,80,0,80,80,80,0,80},
{60,79,48,75,0,67,0,96,0,60,0,75,60,0,43,91}},
 

{{60,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},        //song2 (this is the one that we edit with the sequencer).
{0,0,0,0,   64,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  67,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   72,0,0,0}},


{{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},      //song3 (we only use 3-5 when we have appended additional songs from Eeprom)
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0}},


{{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},      //song4
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0}},


{{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},      //song5
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0},
{0,0,0,0,   0,0,0,0,  0,0,0,0,   0,0,0,0}},};


