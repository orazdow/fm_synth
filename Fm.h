
#ifndef FM_H
#define FM_H

#include "SLib.h"
#include <vector>
#include "windows.h"


class Osc : public Sig{
    
    double phase = 0, step = 0;
    float* table;
    unsigned int tsize;    

public:
    
    Osc(){
        table = sinetable;
        tsize = table_size;
    }
    Osc(double freq){
        table = sinetable;
        tsize = table_size;
        step = freq*tsize/(double)sampling_rate;
    }
   ~Osc(){}
 
    inline double out(double step){
        phase += step;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
    }
   
    inline double out(){
        phase += step;
        if(phase >= tsize){ phase -= tsize; }       
        return output = lerp(table[(int)phase], table[(int)phase+1], phase);       
   }
    
    
};

class FmOsc : public Sig{
    
    double phase = 0, step = 0;
    Sig* mod = nullptr;
    bool feedback = false;
    double fb = 0, fbamt = 0;
    float* table;
    unsigned int tsize;    

public:
    
    FmOsc(){
        table = sinetable;
        tsize = table_size;
    }
    FmOsc(double freq){
        table = sinetable;
        tsize = table_size;
        step = freq*tsize/(double)sampling_rate;
    }
   ~FmOsc(){}
   
   void setMod(Sig* in){
       mod = in;
   }
   
   void setFreq(double freq){
        step = freq*tsize/(double)sampling_rate;
   }
   
   void setFeedBack(double in){
        if(in == 0){
           feedback = false;  
        }else{ 
           feedback = true; 
        }
        fbamt = in;
    }
   
   inline double out(double step){

        if(mod == nullptr)
        {
            phase += step;
            if(phase >= tsize){ phase -= tsize; }
            if(feedback)
            {
            double fbphase = phase+fb*fbamt;
            wrap(fbphase);
         //   fb = table[(int)fbphase]; //leaving trunc noise intentionally for fb...
            fb = lerp(table[(int)fbphase], table[((int)fbphase)+1], fbphase);
            return output = fb;                  
            }
            return output = lerp(table[(int)phase], table[(int)phase+1], phase);
        }else
        {
        phase += step;
//        if(phase >= tsize){ phase -= tsize; }
        wrap(phase);
        double tablephase = phase + mod->output*tsize;
        wrap(tablephase);
        return output = lerp(table[(int)tablephase], table[((int)tablephase)+1], tablephase);        
        }      
       
   }

   inline double out(){

        if(mod == nullptr)
        {
            phase += step;
            if(phase >= tsize){ phase -= tsize; }
            if(feedback)
            {
            double fbphase = phase+fb*fbamt;
            wrap(fbphase);
         //   fb = table[(int)fbphase];
            fb = lerp(table[(int)fbphase], table[((int)fbphase)+1], fbphase);
            return output = fb;                  
            }
            return output = lerp(table[(int)phase], table[(int)phase+1], phase);
        }else
        {
        phase += step;
//        if(phase >= tsize){ phase -= tsize; }
        wrap(phase);
        double tablephase = phase + mod->output*tsize;
        wrap(tablephase);
        return output = lerp(table[(int)tablephase], table[((int)tablephase)+1], tablephase);        
        }      
       
   }
   private:
   inline void wrap(double &in)
    {
        while(in >= tsize)
            in -= tsize;
    
        while(in < 0)
            in += tsize;
    }    
    
};


class Env{
public: 
   unsigned int on = 0; 
   virtual double get(unsigned int trig){}
   virtual void reset(){}
};

class ADSR : public Env{
    bool attack = true;
    double s = 0.7;
    double astep = 0.0001;
    double dstep = 0.0001;
    double rstep = 0.00001;
  //  friend class Env;
public:
    double lev = 0;
    bool linear = true;
    
    ADSR(double _a, double _d, double _s, double _r, bool lin){
        setA((int)_a); setD((int)_d); setS(_s); setR((int)_r); setLinear(lin);
    }
    
    ADSR(ADSR* master){
        
    }
    
    ADSR(){}
    
    void setLinear(bool in){
        linear = in;
    }
    void setA(unsigned int ms){
      astep = 1/(double)(sampling_rate*ms*0.001);
    }
    void setD(unsigned int ms){
      dstep = 1/(double)(sampling_rate*ms*0.001);
    }
    void setS(double _lev){
        if(linear){
        s = _lev;}
        else{ sqrt(_lev); } 
        if(s > 1){s = 1;}
        if(s < 0){s = 0;}
    }
    void setR(unsigned int ms){
      rstep = 1/(double)(sampling_rate*ms*0.001);
    }
    
    void setADSR(double _a, double _d, double _s, double _r){
        setA((int)_a); setD((int)_d); setS(_s); setR((int)_r);
    }
    
    inline double get(unsigned int trig){       
        if(trig)
        {  on = 1;
           if(attack){
              if(lev < 1.0){lev += astep;}else{ attack = false; }         
           }
           else{
              if(lev > s){lev -= dstep;} 
           }

        }
        else{
            attack = true;
            if(lev > 0){lev -= rstep;}else{ on = 0; }
        }
        
        if(lev > 1){lev = 1;}
        if(lev < 0){lev = 0;}
        
        if(linear){
        return lev;
        }else{return lev*lev;}
    } 
    
    void reset(){
        lev = 0;
        on = 0;
        attack = true;
    }

};

class Op : public Sig{
        
public:
    
    FmOsc osc;
    ADSR env;   
    double amp = 1;
    double ratio = 1;
    
    Op(){
      //  env.setLinear(false);
    }
    
    void setMod(Op* mod){
        osc.setMod(mod);      
    }
    
    inline double out(double step, unsigned int trig){       
       return output = osc.out(step*ratio)*env.get(trig)*amp;       
    }
    
};

class TXVoice : public Sig{
        
public:
     
    Op op1; Op op2; Op op3; Op op4;
    TXVoice(){
        op1.ratio = 2;
        op1.osc.setFeedBack(20);
        op2.setMod(&op1);
        /*
        op1.ratio = 0.499;
        op1.amp = 3;
        op1.osc.setFeedBack(0);
        op2.setMod(&op1);
        op1.env.setLinear(false);
        op1.env.setA(3);
        op1.env.setD(700);
        op2.env.setD(400);
        op2.env.setS(0.5);
        op2.env.setR(800);
        op2.env.setA(3);
        */
        //op2.osc.setFeedBack(60);
    }
    
    inline double out(double step, unsigned int trig){
        op1.out(step, trig);
        return op2.out(step, trig);
    }
    
    
};


class Sum : public Sig{

   // Sig* source;   
    int index = 0;
    std::vector<Sig*>source;
    double div = 1;
    
    public:
    void addSource(Sig* s){
        source.push_back(s);
        index++;
        div = 1/(double)index;
    }
    
    inline double out(){      
        output = 0;
        for (int i = 0; i < index; i++) {
        output += source[i]->output;
        }       
        return output*div; 
    }
        
};


class KeyHandler{
    // add note stealing for mono
    unsigned int keymap[21];
    unsigned int charmap[21];
    typedef int(*keyCallback)(int);
    keyCallback keyCb;
    bool cbSet = false;
    bool stealing = false;
    public:   
    int polylimit = 8;
    int baseNote = 48;
    
    typedef struct{
        int note;
        bool on;  
        bool envset = 0;
        Env* env;

        bool ready(){
         //   return !(on || env->on);
       // return (!on && (!envset || !env->on));
        return !( on || (envset && env->on) );
        }
        void setEnv(Env* e){
           env = e; 
           envset = 1;
       }
    }key;
    
    key* keys;
    
    KeyHandler(){
        keys = new key[polylimit];
        init();
    }
    KeyHandler(unsigned int poly){
        polylimit = poly;
        keys = new key[polylimit];
        init();
    }  
   // key keys[polylimit];
    
    ~KeyHandler(){
        delete[] keys;
    }
    
    void setPolyLimit(unsigned int poly){
        polylimit = poly;
    }
    
    void setBaseNote(unsigned int note){
        baseNote = note;
    }
    
    void setKeyCb(keyCallback cb){
        keyCb = cb;
        cbSet = true;
    }
    void scan(){  
        
        
        for(int i = 0; i < 21; i++){
        if(cbSet){
    
            if(keyCb(keymap[i])){
                keyDown(i+baseNote);
            }else{
                keyUp(i+baseNote);
            }
           // setKey( i+baseNote,  keyCb(keymap[i]));
        } else{
         setKey(i+baseNote, (int)GetAsyncKeyState(keymap[i]));             
    
        }   
              
    }   
    
}    
    void keyDown(int note){
        bool found = false;
        for (int i = 0; i < polylimit; i++){
            if(keys[i].note == note){
                keys[i].on = 1;
                found = true;
                break;
            }
                
        }      
        if(!found){
            for(int i = 0; i < polylimit; i++){
                if(keys[i].ready()){
                    keys[i].note = note;
                    keys[i].on = 1;
                    break;
                }else if(stealing && i == polylimit-1){
                    
                    keys[0].note = note;
                    keys[0].env->reset();
                    
                 //  (oldest note stealing) push new voice num to queue (if num not in queue)
                 //   choose oldest (tail) element to replace        
                    
//                    keys[0].on = 0;
                }
            }
        }
    
    }

    void keyUp(int note){
        for(int i = 0; i < polylimit; i++){
            if(keys[i].note == note){
                keys[i].on = 0;
                break;
            }
        }
    }
        
    void setKey(int note, int on){// printf("\n%i\n", note);
        bool found = false;
        for (int i = 0; i < polylimit; i++){
            if(keys[i].note == note){
                keys[i].on = on;
                found = true;
                break;
            }
        }
        if(!found){
            for(int i = 0; i < polylimit; i++){
          //  for (int i = polylimit-1; i >= 0; i--) {
              //  if(!keys[i].state()){
                if(keys[i].ready()){    
                    keys[i].note = note;
                    keys[i].on = on;
                    break;
                }
            }

        }

    }
    private:
    void init(){
    for (int i = 0; i < polylimit; i++) {
        keys[i].note = -1;
        keys[i].on = false;
    }

    keymap[0] = 'A';
    keymap[1] = 'Z';
    keymap[2] = 'S';
    keymap[3] = 'X';
    keymap[4] = 'D';
    keymap[5] = 'C';
    keymap[6] = 'F';
    keymap[7] = 'V';
    keymap[8] = 'G';
    keymap[9] = 'B';
    keymap[10] = 'H';
    keymap[11] = 'N';
    keymap[12] = 'J';
    keymap[13] = 'M';
    keymap[14] = 'K';
    keymap[15] = 188;
    keymap[16] = 'L';
    keymap[17] = 190;
    keymap[18] = 186;
    keymap[19] = 191;
    keymap[20] = 222;
    
    }
};


#endif /* FM_H */

