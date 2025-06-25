#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    int *vectordin;
    int tamañooriginal;
    printf("\nDe que tamaño quieres que sea el vector?");
    scanf("%d",&tamañooriginal);

    vectordin=malloc(tamañooriginal*sizeof(int));
    for(int i=0;i<tamañooriginal;i++){
        vectordin[i]=rand()%100;
        printf("vectordin[%d]=%d\n",i,vectordin[i]);
    }
    int nuevotamaño;
    printf("\nIngresa en nuevotamaño de tu vector:\n");
    scanf("%d",&nuevotamaño);

    vectordin=resize(vectordin,tamañooriginal,nuevotamaño);

    for(int i=0;i<nuevotamaño;i++){
        printf("vectordin[%d]=%d \n",i,vectordin[i]);
    }

    free(vectordin);
    return 0;
}