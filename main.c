#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct complejos{
    double real;
    double complejo;
};  

int main(){
    FILE *f=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Archivos (Tp2)\\Parcial\\integers.dat","rb");
    if(f==NULL){
        printf("Error al abrir el archivo");
        return -1;
    }
    fseek(f,0,SEEK_END);
    int n=ftell(f);
    fseek(f,0,SEEK_SET);
    int cantidad;
    cantidad=n/sizeof(int);

    int *vector;
    vector=malloc(cantidad*sizeof(int));
    fread(vector,sizeof(int),cantidad,f);
    int suma=0;
    for(int i=0;i<cantidad;i++){
        printf("vector[%d]=%d\n",i,vector[i]);
        suma=suma+vector[i];
    }
    int media=0;
    media=suma/cantidad;
    printf("\nEsta es la media de los numeros alojados en el archivo: %d\n\n",media);
    free(vector);
    fclose(f);


    FILE *fc=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Archivos (Tp2)\\Parcial\\complex.dat","rb");
    fseek(fc,0,SEEK_END);
    n=ftell(fc);
    fseek(fc,0,SEEK_SET);
    cantidad=n/(2*sizeof(double));
    struct complejos *Complejos=malloc(cantidad*sizeof(Complejos));
    double modulo[cantidad];

    for(int i=0;i<cantidad;i++){
        fread(&Complejos[i].real,sizeof(double),1,fc); //lee el primer numero
        fread(&Complejos[i].complejo,sizeof(double),1,fc); //lee el segundo numero 
        modulo[i]=sqrt((Complejos[i].real)*(Complejos[i].real)+(Complejos[i].complejo)*(Complejos[i].complejo));   
    }
    for(int i=0;i<cantidad;i++){
        printf("Z[%d]=%.1f+%.1f y su modulo es %.1f\n",i,Complejos[i].real,Complejos[i].complejo,modulo[i]);
    }
    free(Complejos); 
    fclose(fc);


    FILE *fn=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Archivos (Tp2)\\Parcial\\numbers_1.dat","rb");
    if(fn==NULL){
        printf("Error al abrir el archivo de numeros con tipo de dato");
        return -1;
    }
    fseek(fn,0,SEEK_END);
    n=ftell(fn);
    rewind(fn);
    char tipodedato=fgetc(fn);
    int ntotal=n-1;//le resto un byte para ignorar la letra del primer byte


    printf("El tipo de dato del primer archivo es %c\n",tipodedato);
    printf("Tamaño total del archivo: %ld bytes\n", n);

    if(tipodedato=='i'){
        int *vectorint;
        cantidad=ntotal/sizeof(int);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorint=malloc(cantidad*sizeof(int));
        fread(vectorint,sizeof(int),cantidad,fn);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%d\n",i,vectorint[i]);
        }
    }

    if(tipodedato=='f'){
        float *vectorfloat;
        cantidad=ntotal/sizeof(float);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorfloat=malloc(cantidad*sizeof(float));
        fread(vectorfloat,sizeof(float),1,fn);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectorfloat[i]);
        }
    }
    if(tipodedato=='d'){
        double *vectord;
        cantidad=ntotal/sizeof(double);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectord=malloc(cantidad*sizeof(double));
         fread(vectord,sizeof(double),1,fn);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectord[i]);
        }
    }
    fclose(fn);
    
        FILE *fn2=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Archivos (Tp2)\\Parcial\\numbers_2.dat","rb");
    if(fn==NULL){
        printf("Error al abrir el archivo de numeros con tipo de dato");
        return -1;
    }
    fseek(fn2,0,SEEK_END);
    n=ftell(fn2);
    rewind(fn2);
    tipodedato=fgetc(fn2);
    ntotal=n-1;//le resto un byte para ignorar la letra del primer byte


    printf("El tipo de dato del primer archivo es %c\n",tipodedato);
    printf("Tamaño total del archivo: %ld bytes\n", n);

    if(tipodedato=='i'){
        int *vectorint;
        cantidad=ntotal/sizeof(int);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorint=malloc(cantidad*sizeof(int));
        fread(vectorint,sizeof(int),cantidad,fn2);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%d\n",i,vectorint[i]);
        }
    }

    if(tipodedato=='f'){
        float *vectorfloat;
        cantidad=ntotal/sizeof(float);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorfloat=malloc(cantidad*sizeof(float));
        fread(vectorfloat,sizeof(float),1,fn2);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectorfloat[i]);
        }
    }
    if(tipodedato=='d'){
        double *vectord;
        cantidad=ntotal/sizeof(double);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectord=malloc(cantidad*sizeof(double));
         fread(vectord,sizeof(double),1,fn2);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectord[i]);
        }
    }
    fclose(fn2);

    FILE *fn3=fopen("C:\\Users\\ezequ\\OneDrive\\Escritorio\\Informatica 2\\Archivos (Tp2)\\Parcial\\numbers_3.dat","rb");
    if(fn3==NULL){
        printf("Error al abrir el archivo de numeros con tipo de dato");
        return -1;
    }
    fseek(fn3,0,SEEK_END);
    n=ftell(fn3);
    rewind(fn3);
    tipodedato=fgetc(fn3);
    ntotal=n-1;//le resto un byte para ignorar la letra del primer byte


    printf("El tipo de dato del primer archivo es %c\n",tipodedato);
    printf("Tamaño total del archivo: %ld bytes\n", n);

    if(tipodedato=='i'){
        int *vectorint;
        cantidad=ntotal/sizeof(int);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorint=malloc(cantidad*sizeof(int));
        fread(vectorint,sizeof(int),cantidad,fn3);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%d\n",i,vectorint[i]);
        }
    }

    if(tipodedato=='f'){
        float *vectorfloat;
        cantidad=ntotal/sizeof(float);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectorfloat=malloc(cantidad*sizeof(float));
        fread(vectorfloat,sizeof(float),1,fn3);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectorfloat[i]);
        }
    }
    if(tipodedato=='d'){
        double *vectord;
        cantidad=ntotal/sizeof(double);
        printf("\nEl archivo contiene %d elementos\n",cantidad);
        vectord=malloc(cantidad*sizeof(double));
         fread(vectord,sizeof(double),1,fn3);
        for(int i=0;i<cantidad;i++){
            printf("Vector[%d]=%.1f\n",i,vectord[i]);
        }
    }
    fclose(fn3);
    return 0;
}