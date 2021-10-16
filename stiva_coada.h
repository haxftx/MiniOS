#include "struct.h"

TStiva *InitS(void)
{//initializeaza stiva
    TStiva *s = (TStiva *)malloc(sizeof(TStiva));
    if(!s)return NULL;
    s->vf =  NULL;
    s->size = 0;
    return s;
}

TCoada *InitC(void)
{//initializeaza coada
    TCoada *c = (TCoada *)malloc(sizeof(TCoada));
    if(!c)return NULL;
    c->inc = c->sf = NULL;
    return c;
}

int Push(TStiva *s, void *ae)
{//introduce un element in stiva
    TLista l = (TLista)malloc(sizeof(Tcelula));
    if(!l)return 0;
    l->info = (int *)malloc(sizeof(int));
    if(!l->info)
    {
        free(l);
        return 0;
    }
    memcpy(l->info, ae, sizeof(int));
    l->urm = s->vf;
    s->vf = l;
    return 1;
}

int Pop(TStiva *s, void *ae)
{//extrage un element din stiva
    if(s->vf == NULL)return 0;
    TLista l = s->vf;
    memcpy(ae, l->info, sizeof(int));
    s->vf = s->vf->urm;
    free(l->info);
    free(l);
    return 1;
}


int IntrQ(TCoada *c, void *ae)
{//introduce un element in coada
    TLista l = (TLista)malloc(sizeof(Tcelula));
    if(!l) return 0;
    l->info = (TProces *)malloc(sizeof(TProces));
    if(!l->info)
    {
        free(l);
        return 0;
    }
    memcpy(l->info, ae, sizeof(TProces));
    l->urm = NULL;
    if(c->inc == NULL)
    {
        c->inc = l;
    }
    else
    {
        c->sf->urm = l;
    }
    c->sf = l;
    return 1;
}

int ExtrQ(TCoada *c, void *ae)
{//extrage un element din coada
    if(c->inc == NULL) return 0;
    TLista l = c->inc;
    c->inc = c->inc->urm;
    memcpy(ae, l->info, sizeof(TProces));
    free(l->info);
    free(l);
    return 1;
}

void DistrugeS(TStiva *s)
{//distruge stiva
    int ae;
    while (Pop(s, (void *)&ae));
    
}

void DistrQ(TCoada *c)
{//distruge coada
    TProces el;
    while (ExtrQ(c, (void *)&el))
    {
        DistrugeS( el.stack );
        free( el.stack);
    }
}