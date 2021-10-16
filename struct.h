#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct celula
{//lista simplu inlatuita generica
    void *info;
    struct celula *urm;
    
}Tcelula, *TLista;

typedef struct  coada
{//coada
    TLista inc;
    TLista sf;
}TCoada, *ACoada;

typedef struct stiva
{//stiva
    int size;
    TLista vf;
}TStiva, *AStiva;

typedef struct proces
{//procesul
    int PID;
    int prioritate;
    int timp;//timpul de rulare
    int tot_timp;//timpul procesului
    int timp_run;//timpul ramas de rulare
    int inc_mem;//inceputul memoriei procesului
    int dim_mem;//dimenensiune memorie procesului
    TStiva *stack;
}TProces;

typedef struct gest_proces
{//gestionarea proceselor
    TProces Idle;//procesul Idle
    TProces *running;//procesul aflat in rulare
    TCoada *waiting;//coada de asteptare
    TCoada *finished;//coada cu procese terminate
}TGest;

typedef struct memorie
{//informatia memoriei libere 
    int inceput;//Inceputul memoriei
    int size;//dimensiune memoriei
}Tmem, *Amem;
