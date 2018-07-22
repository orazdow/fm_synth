#ifndef SLIB_H
#define SLIB_H

#include "math.h"

static unsigned int sampling_rate = 44100;
static const unsigned int table_size = 512;
static const double tau = acos(0)*4;

static float sinetable[table_size+1];
double notes[128];


void initSineTable(float table[], int tsize, bool guard_point){
    for (int i = 0; i < tsize; i++) {
         table[i] =  sin(tau*(i/(double)tsize)); 
       //  table[i] = tanh(sin(tau*(i/(double)tsize))*2.0);
    }
    if(guard_point)
        table[tsize] = table[0];
}

static inline double numToFreq(int n, int basenote){
     return 130.813*pow(2, (n-basenote)/(double)12);
}


void fillNotes(double notes[], int len, int tablesize, double samplerate){
    for(int i = 0; i < len; i++){
     notes[i] = numToFreq(i, 48)*tablesize/samplerate;
    }
}
    
void initLib(){ 
    fillNotes(notes, 128, table_size, sampling_rate);
    initSineTable(sinetable, table_size, true);
 //   initWshapeTable(tanhTable, tanhTableSize, true);
}
    
static inline double getNoteStep(int note){
  if(note < 0){ note = 0;}
  else if(note > 127){ note = 127;}
  return notes[note];
}

static inline double lerp(double v0, double v1, double t) {
  double fracpart = t-(int)t;
  return v0 + fracpart * (v1 - v0);
}    

    
class Sig{
    
public:
    double output;
    virtual double out(){}
    virtual double out(int note){}
    virtual double out(double step){}
    virtual double out(double step, unsigned int trig){} //...
    
};


#endif /* SLIB_H */

