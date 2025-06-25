#include "audio_player.h"
#include <stdio.h>
#include <stdlib.h>

struct pistas{
    uint32_t samplerate: 4; //bitmap
    uint32_t samplecount: 28;
};

int main(void){
    FILE *f=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Ejercicio de parcial (TP6)\\audio_list.raw","rb");
    if(f==NULL){
        printf("Hubo un error al abrir el archivo");
        return -1;
    }

    fseek(f,0,SEEK_END);//muevo el puntero al final del archivo
    long tamañototal=ftell(f);//consigo el tamaño del archivo con tell
    printf("\nEl archivo con tiene un total de %ld bytes\n",tamañototal);
    fseek(f,0,SEEK_SET);
    int flag=0;
    int selec=0;
    int n=0;
    struct pistas audio;
    float *a;
    size_t leidos;
    uint32_t vel=0;

    while(ftell(f)<tamañototal){
        fread(&audio,sizeof(audio),1,f);
        fseek(f,audio.samplecount*sizeof(float),SEEK_CUR);
        n++;
    }
    fseek(f,0,SEEK_SET);
    printf("\nHay %d pistas de audio en el archivo\n",n);
    uint8_t velpist[n];
    uint32_t pistasguard[n];
    float **canciones=malloc(n*sizeof(float *));//creo puntero a puntero para n punteros y le asigno memoria para las pistas

for(int i=0;i<n;i++){
    fread(&audio,sizeof(audio),1,f);
    //printf("\nCantidad de bytes: %u\n",audio.samplecount);
    //printf("\nVelocidad de las muestras: %u=44100Hz\n",audio.samplerate);

    a=malloc(audio.samplecount*sizeof(float));
    if(a==NULL){
        printf("\nError al asignar memoria\n");
        fclose(f);
        return -1;
    }
    fread(a, sizeof(float), audio.samplecount, f);

    velpist[i]=audio.samplerate;
    pistasguard[i]=audio.samplecount;
    canciones[i]=malloc(audio.samplecount*sizeof(float));
    canciones[i]=a;
    }

    do{
    printf("\n||-------------------------MENU------------------------||\n");
    printf("Que cancion quieres seleccionar?\n1)\n2)\n3)\n4)\n5)\n9)Salir\n");
    scanf("%d",&selec);
    selec--; //le resto una para que el menu tenga coerencia con el numero ingresado ya que no existe como tal una cancion 0 pero en el codigo si hay
    if(selec<6&&selec!=9){
        if(velpist[selec]==0){ //cambia la primera variable de playaudio segun la pista seleccionada 
        vel=32000;
    }
        if(velpist[selec]==1){
        vel=44100;
    }
        if(velpist[selec]==2){
        vel=48000;
    }
        if(velpist[selec]==3){
        vel=88200;
    }

    play_audio(vel,pistasguard[selec], canciones[selec]);
    }
    if(selec==8){
        flag=1;
    }
    }while(flag==0);
    for(int i=0;i<n;i++){
        free(canciones[i]);
    }
    free(canciones);
    free(pistasguard);
    free(velpist);
    fclose(f);
    free(a);
    return 0;
}