//Axel Tomecek 
#include <stdlib.h>
#include <stdio.h>
#include "audio_player.h"
#include <string.h>

struct pistas{
    uint32_t samplerate; //velocidad
    uint32_t samplecount;//cant muestra de audio
    char  cadena[64];
};

int main(){
    FILE *f=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Parcial\\audio_list.raw","rb");
    if(f==NULL){
        printf("\nNo se pudo abrir el archivo\n");
        return -1;
    }

    uint32_t cantidadpistas;
    fread(&cantidadpistas,sizeof(cantidadpistas),1,f); //Leo la cantidad de pistas colocadas en count_of_sounds
    printf("\nEl archivo contiene %u pistas\n",cantidadpistas);
    
    float *audiodata;                                              //encargado de extraer la info de la cancion
    float **cancion=malloc(cantidadpistas*sizeof(float *));        //vector que guarda las canciones   
    uint32_t velocidad[cantidadpistas];                            //guarda la velocidad de las pistas 
    uint32_t cantmues[cantidadpistas];                             //guarda la cantidad de muestras
    char nombre[64],nombre2[64],nombre3[64],nombre4[64];                                    //nombres
    struct pistas muestras;                                         //Esctructura para leer la vel y las muestras 
    
    for(int i=0;i<cantidadpistas;i++){                //Leo los headers para obtener la velocidad, cant de muestra y el nombre de la cancion
        fread(&muestras,sizeof(muestras),1,f);

        velocidad[i]=muestras.samplerate;
        cantmues[i]=muestras.samplecount;
        if(i==0){
            strcpy(nombre,muestras.cadena);
        }
        if(i==1){
            strcpy(nombre2,muestras.cadena);
        }
        if(i==2){
            strcpy(nombre3,muestras.cadena);
        }
        if(i==3){
            strcpy(nombre4,muestras.cadena);
        }
    }
    
    for(int i=0;i<cantidadpistas;i++){                    //Leo los sound data y extraigo la informacion en cancion[]
        audiodata=malloc(cantmues[i]*sizeof(float));
        fread(audiodata,sizeof(float),cantmues[i],f);
        cancion[i]=malloc(cantmues[i]*sizeof(float));
        cancion[i]=audiodata;
    }

    FILE *c=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Parcial\\sound.raw","wb"); //creo un archivo sound para guardar la pista
                                                                                                     //que seleccione el usuario
    int flag=0;
    int select=0;
    do{
        printf("\n---------------------------------------------||MENU||-----------------------------------------\n");
        printf("\nSelecciona que cancion quieres reproducir:\n1)%s\n2)%s\n3)%s\n4)%s    \n\n9)Salir\n\n",nombre,nombre2,nombre3,nombre4);
        scanf("%d",&select);
        select--;   //corrijo el numero ya que en el menu no hay 0 pero en el codigo si 
        if(select>=4&&select<8){
            printf("\nEl numero que ingresaste no es valido, prueba de nuevo con uno de la lista\n");
        }
        if(select>7){
            flag=1;
        }
        else if(select<4){
        play_audio(velocidad[select],cantmues[select],cancion[select]);
        fwrite(cancion[select],sizeof(cancion),cantmues[select],c);
        }


    }while(flag==0);
    
    free(audiodata);    //cierro ambos archivos, y libero la mmemoria del vector dinamico
    fclose(f);
    fclose (c);
    return 0;
}