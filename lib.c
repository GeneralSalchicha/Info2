#include "lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
/*float *resize(float *vector, size_t tamaño, size_t nuevotamaño){
    float *NuevoVector=realloc(vector,nuevotamaño*sizeof(float));
    if(NuevoVector==NULL){
        printf("No se pudo redimensionar el vector");
        return vector;
    }

    if(nuevotamaño>tamaño){

    }
}*/
int *resize(int *vectororiginal, size_t tamaño,size_t nuevotamaño){
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