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

    int remover=0;
    printf("\nSelecciona la posicion del elemento a eliminar:\n");
    scanf("%d",&remover);
    vectordin=removeitem(vectordin,nuevotamaño,remover);
    nuevotamaño--;
    for(int i=0;i<nuevotamaño;i++){
        printf("Vectordin[%d]=%d\n",i,vectordin[i]);
    }

    int nuevoitem=0, posicion=0;
    printf("\nIngresa la posicion y que elemento te gustaria añadir al vector:\n");
    scanf("%d",&posicion);
    scanf("%d",&nuevoitem);
    vectordin=insertitem(vectordin,nuevotamaño,posicion,nuevoitem);
    for(int i=0;i<nuevotamaño;i++){
        printf("Vectordin[%d]=%d\n",i,vectordin[i]);
    }

    int tamaño=0;
    int *nuevovec;
    printf("\nA tu vector se le concatenara el siguiente, elige su tamaño:\n");
    scanf("%d",&tamaño);
    nuevovec=malloc((tamaño)*sizeof(int));
    for(int i=0;i<tamaño;i++){
        nuevovec[i]=rand()%3;
        printf("NuevoVec[%d]=%d\n",i,nuevovec[i]);
    }
    int *vectorunido;
    vectorunido=malloc((nuevotamaño+tamaño)*sizeof(int));
    vectorunido=concatenar(vectordin,nuevotamaño,nuevovec,tamaño);
    for(int i=0;i<(nuevotamaño+tamaño);i++){
        printf("VectorConcatenado[%d]=%d \n",i,vectorunido[i]);
    }
    free(vectordin);
    free(vectorunido);
    return 0;
}   