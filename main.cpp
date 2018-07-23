#include "slib.h"
#include "pa.h"
#include "fm.h"
#include "windows.h"

    
KeyHandler keys;
TXVoice v[8];
double sig = 0;
   
void paFunc(const float* in, float* out, unsigned long frames, void* data){    
    
    for(unsigned long i = 0; i < frames; i++ ){
        sig = 0;
        for(int i = 0; i < 8; i++){
            sig += 0.125*v[i].out(getNoteStep(keys.keys[i].note), keys.keys[i].on );              
        }
            *out++ = sig;
    }
}

int main(int argc, char** argv){

    initLib();
    keys.setBaseNote(48);
    
    for (int i = 0; i < keys.polylimit; i++) {
        keys.keys[i].setEnv( &v[i].op2.env );
    }


    Pa a(paFunc, NULL);
    a.start();
    
    while(true){
    
        keys.scan();
            
        for(int i =0; i < keys.polylimit; i++){
           printf("%i ", !keys.keys[i].ready());
        }  printf("\r"); 
        
           Sleep(5);      
    } 
    
    a.stop(true);
    a.terminate();
    return 0;
}

