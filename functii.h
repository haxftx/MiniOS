TGest Init(void)
{//initializeaza sistemul
    TGest a;
    a.Idle.PID = 0;
    a.Idle.inc_mem = 0;
    a.Idle.stack = NULL;
    a.running = (TProces *)malloc(sizeof(TProces));
    memcpy(a.running, &(a.Idle), sizeof(TProces));
    a.waiting = InitC();
    a.finished = InitC();
    return a;
}

int min(int a, int b)
{//calculeaza minimul dintre 2 numare
    if(a > b) return b;
    return a;
}

int cmp(void *a, void *b)
{//compara 2 piduri
    int x = *(int *)a;
    int y = *(int *)b;
    return x - y;
}

int comp_pr(TProces *a, TProces *b)
{//funtia de comparare a proceselor pentru adaugare
    int c = a->prioritate - b->prioritate;
    if(c == 0)c += b->timp - a->timp;
    if(c == 0)c += b->PID - a->PID;
    return c;
}

int comp_mem(void *a, void *b)
{//compara inceputl a doua blocuri
    Amem x = (Amem)a;
    Amem y = (Amem)b;
    return x->inceput - y->inceput;
}

void print_proces(TProces *p, FILE *out)
{//printeaza datele despre proces
    fprintf(out, "(%d: ", p->PID);
    fprintf(out, "priority = %d, ", p->prioritate);
    if(p->timp == 0)
    {
        fprintf(out, "executed_time = %d)", p->tot_timp);
    }
    else fprintf(out, "remaining_time = %d)", p->timp);
}

void AlocPid(TLista *l, void *info)
{//adauga in lista de piduri un nou pid
    TLista u, aux = (TLista)malloc(sizeof(Tcelula));
    if(!aux)return ;
    aux->info = (int *)malloc(sizeof(int));
    if(!aux->info)
    {
        free(aux);
        return;
    }
    memcpy(aux->info, info, sizeof(int));
    aux->urm = NULL;
    if(!*l || *(int *)info < *(int *)((*l)->info) )
    {//daca lista e vida sau pidul e mai mic ca primul din lista
        aux->urm = *l;
        *l = aux;
    }
    else
    {//insereaza crescator
        for(u = *l; u->urm && cmp(info, u->urm->info) > 0 ; u = u->urm);
        aux->urm = u->urm;
        u->urm = aux;        
    }
}

int new_pid(TLista *l)
{//intoarce primul pid din lista de piduri
    int pid = *(int *)((*l)->info);
    TLista aux = *l;
    *l = (*l)->urm;
    free(aux->info);
    free(aux); 
    return pid;
}

void AlocMem(TLista *l, int dim, int adr, int *last_adr)
{//adaug o celula in lista de memorie libera
    if(adr + dim == *last_adr)
    {//daca sa eliberat ultima adresa
        *last_adr -= dim;
        return;
    }
    Amem x = (Amem)malloc(sizeof(Tmem));
    if(!x)return;
    x->size = dim;
    x->inceput = adr;
    TLista u, aux = (TLista)malloc(sizeof(Tcelula));
    if(!aux)
    {
        free(x);
        return;
    }
    aux->info = x;
    aux->urm = NULL;
    if(!*l || comp_mem(aux->info, (*l)->info) < 0)
    {//daca lista e vida sau adresa e mai mica ca a primului element
        aux->urm = *l;
        *l = aux;
    }
    else
    {//adaug sortat crescator dupa inceputul adresei
        for(u =  *l; u->urm && comp_mem(x, u->urm->info) > 0; u = u->urm);
        aux->urm = u->urm;
        u->urm = aux;  
    }
}

int find_mem(TLista *l, int dim)
{//gaseste prima memorie disponibila din memoria de lista libera
    int x = -1;
    if(!*l) return x;
    TLista aux;
    Amem p;
    for(aux = *l; aux; aux = aux->urm)
    {
        p = (Amem)(aux->info);
        if(p->size >= dim)
        {//daca am gasit memorie disponibila
            x = p->inceput;
            p->inceput += dim;
            p->size -= dim;
            break;
        }
    }
    return x;
}

void DistrugeL(TLista *l)
{//Distruge lista
    TLista p;
    while (*l)
    {
        p = *l;
        *l = (*l)->urm;
        free(p->info);
        free(p);
    }
}

void Distruge_Guest(TGest *a)
{//distruge sistemul
    DistrQ(a->waiting);
    DistrQ(a->finished);
    free(a->waiting);
    free(a->finished);
    free(a->running);
}