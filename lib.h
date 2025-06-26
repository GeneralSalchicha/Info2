#include <stddef.h>
#define LIB_H

int *resize(int *vectororiginal, size_t tamaño, size_t nuevotamaño); //se declara como float porque devuelve un puntero mirando hacia ese punto de la memoria
                                                 // size_t es un tipo de dato relacionado al tamaño en memoria 
int *removeitem(int *vectororiginal, int tamañoveco, int remover);
int *insertitem(int *vectororiginal,int tamañoveco, int posicion, int nuevoitem);
int *concatenar(int *vectororiginal, int tamañoveco, int*nuevovector, int tamañonvec);