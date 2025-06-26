#include "lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int *resize(int *vectororiginal, size_t tamaño,size_t nuevotamaño){ //redimensionar tamaño
int n=tamaño;
/*for(int i=0;i<n;i++){
    vector[i]=rand()%100;
    printf("%.1f ",vector[i]);*/
int nn=nuevotamaño;
int *nuevovector=malloc(nuevotamaño*sizeof(int)); //nuevo vector 
if(nuevovector==NULL){
    printf("Error al asginar memoria al vector");
}
for(int i=0;i<n;i++){ //copio el primer vector en el segundo
    nuevovector[i]=vectororiginal[i];
}
for(int i=n;i<nn;i++){ //relleno los espacios del nuevo vector
    nuevovector[i]=1;
}

return nuevovector;
}

int *removeitem(int *vectororiginal, int tamañoveco, int remover){ //remover un item elegido por posicion 
    int *nuevovector;
    nuevovector=malloc((tamañoveco-1)*sizeof(int));
    for(int i=0,x=0;i<tamañoveco;i++){
        if(i!=remover){
            nuevovector[x++]=vectororiginal[i];
            
        }
    }
    free(vectororiginal);
    return nuevovector;
}

int *insertitem(int *vectororiginal, int tamañoveco, int posicion, int nuevoitem){  //inserto un valor en una posicion
    int *nuevovector;
    nuevovector=malloc((tamañoveco+1)*sizeof(int));

    for(int i=0,x=0;i<tamañoveco;i++){
        if(i!=posicion){
            nuevovector[x++]=vectororiginal[i];
        }
        else if(i==posicion){
            nuevovector[x++]=nuevoitem;
        }
    }
    free(vectororiginal);
    return nuevovector;
}

int *concatenar(int *vectororiginal, int tamañoveco, int *nuevovector, int tamañonvec){ //concateno 2 vectores convirtiendolo en 1 solo
    int *vectorunido;
    vectorunido=malloc((tamañoveco+tamañonvec)*sizeof(int));

    for(int i=0;i<tamañoveco;i++){
        vectorunido[i]=vectororiginal[i];
    }
    for(int i=0;i<tamañonvec;i++){
        vectorunido[tamañoveco+i]=nuevovector[i];
    }
    free(vectororiginal);
    free(nuevovector);
    return vectorunido;
}