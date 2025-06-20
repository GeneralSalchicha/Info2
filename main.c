#include "audio_player.h"
#include <stdio.h>
#include <stdlib.h>

struct pistas{
    uint32_t samplerate : 4; //bitmap
    uint32_t samplecount : 28;
};

int main(void){
    FILE *f=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Ejercicio de parcial (TP6)\\audio_list.raw","rb");
    if(f==NULL){
        printf("Hubo un error al abrir el archivo");
        return -1;
    }

    struct pistas audio;
    fread(&audio,sizeof(audio),1,f);

    printf("\nCantidad de elementos: %u\n",audio.samplecount);
    printf("\nVelocidad de las muestras: %u=44100Hz\n",audio.samplerate);

    float a=1;
    
    play_audio(audio.samplerate , audio.samplecount , a);
    return 0;
}