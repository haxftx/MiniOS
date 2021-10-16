#include "stiva_coada.h"
#include "functii.h"

void run(TGest *mOS, FILE *in, int all_time,
            TLista *lpid, TLista *lmem, int *adr)
{//funtia run
    int timp, schimb;
    if(fscanf(in, "%d", &timp));
    else return;
    //aloc variabile auxiliare
    TCoada *aux = InitC();
    if(!aux) return;
    TProces *p = (TProces *)malloc(sizeof(TProces));
    if(!p)
    {
        free(aux);
        return;
    }
    TProces *el = (TProces *)malloc(sizeof(TProces));
    if(!el)
    {
        free(aux);
        free(el);
        return;
    }
    while(timp > 0 && mOS->running->PID != 0)
    {//cat exista timp de rulat sau n-au rulat toate procesele
        schimb = 0;
        if(mOS->running->timp_run > 0)
        {//daca procesul mai are de rulat din coanta maxima
            mOS->running->timp -= mOS->running->timp_run;
            timp -= mOS->running->timp_run;
            mOS->running->timp_run = 0;
            schimb = 1;   
        }
        else
        {
            if(mOS->running->timp >= min(all_time, timp))
            {//cat are de rulat procesul mai mult ca timpul sau cuanta
                //rulez cuanta sau timpul de rulare
                mOS->running->timp -= min(all_time, timp);
                mOS->running->timp_run = all_time - min(all_time, timp);
                timp -= min(all_time, timp);//actualizez timpul
                if(mOS->running->timp_run == 0)
                    schimb = 1;//daca porcesul nu mai are de rulat il schimb
            }
            else
            {//daca procesul are de rulat mai putin timp decat timpul de rulare
                timp -= mOS->running->timp;//actualizez timpul
                mOS->running->timp = 0;//se finiseaza procesul
            }            
        }
        if(mOS->running->timp == 0)
        {//daca procesul a rulat complet
            AlocPid(lpid, &(mOS->running->PID));
            AlocMem(lmem, mOS->running->dim_mem, mOS->running->inc_mem, adr);
            IntrQ(mOS->finished, mOS->running);
            if(ExtrQ(mOS->waiting, mOS->running));
            else memcpy(mOS->running, &mOS->Idle, sizeof(TProces));
            schimb = 0;
        }
        if(schimb == 1)
        {//daca trebuie schimbat porcesul din running
            IntrQ(aux, mOS->running);
            if(ExtrQ(mOS->waiting, mOS->running))           
            {
                ExtrQ(aux, p);
                int v = 1;
                while (ExtrQ(mOS->waiting, el) )
                {
                    IntrQ(aux, el);
                }
                while (ExtrQ(aux, el))
                {//inserez porcesul sortat
                    if(comp_pr(p, el) > 0  && v)
                    {
                        IntrQ(mOS->waiting, p);
                        v = 0;//daca l-am inserat
                    }
                    IntrQ(mOS->waiting, el);
                }
                if(v) IntrQ(mOS->waiting, p);
                
            }
            else
            {
                ExtrQ(aux, mOS->running);
            }          
        }
    }
    //eliberez variabilele;
    free(el);
    free(aux);
    free(p);    
}

int defragmentare(TGest *mOS)
{//returneaza noua adresa de inceput
    int size = 0;//noua adresa de inceput
    if(mOS->running->PID != 0)
    {//daca exista proces care ruleaza
        size += mOS->running->dim_mem;
    }
    else return 0;
    TCoada *aux = InitC();
    if(!aux) return -1;
    TProces *el = (TProces *)malloc(sizeof(TProces));
    if(!el)
    {
        free(aux);
        return -1;
    }
    while(ExtrQ(mOS->waiting, el))
    {//adun memoria proceselor din coada de aseptare
        size += el->dim_mem;
        IntrQ(aux, el);
    }
    while(ExtrQ(aux, el))
    {
        IntrQ(mOS->waiting, el);
    }
    free(aux);
    free(el);
    return size;  
}

void add(TGest *mOS, FILE *in , FILE *out, int *pid, 
            int *start_mem, TLista *lpid, TLista *lmem)
{//adauga un nou proces daca exista memorie in sistem
    int mem_size, exec_time, prioritate;
    if(fscanf(in, "%d %d %d",&mem_size, &exec_time, &prioritate));
    else return;
    TProces *p = (TProces *)malloc(sizeof(TProces));
    if(!p)return;
    //atribui pidul procesului
    if(!(*lpid))
    {//daca nu exista diponibile, umatorul cel mai mare
        (*pid)++;
        p->PID = *pid;
    }
    else
    {//cel mai mic disponibil
        p->PID = new_pid(lpid);
    }
    if(mOS->running->PID == 0)
    {//daca nu exista procese resetez memoria
        *start_mem = 0;
        if(*lmem)
        {
            DistrugeL(lmem);
            *lmem = NULL;
        }
    }
    p->inc_mem = find_mem(lmem, mem_size);//caut memorie libera
    if(p->inc_mem == -1 && *start_mem + mem_size >= 3*1024*1024) 
    {//daca nu exista memeorie disponibila
        *start_mem = defragmentare(mOS);//defragmentez
        if(*lmem)
        {//eliberez lista de memorie libera
            DistrugeL(lmem);
            *lmem = NULL;
        }
        if(*start_mem + mem_size > 3*1024*1024)
        {//daca nu exista memorie nici dupa defragmentare afisez mesajul
            fprintf(out, "Cannot reserve memory for PID %d.\n", p->PID);
            //rentorc pidul pentru refolosire
            if(p->PID == *pid) (*pid)--;
            else AlocPid(lpid, &p->PID);
            free(p);
            return;
        }   
    }
    if(p->inc_mem == -1)
    {//daca nu sa gasit memorie pana la maximul ocupat
        p->inc_mem = *start_mem;
        *start_mem += mem_size;
    }//atribui componentele procesului
    p->prioritate = prioritate;
    p->stack = InitS();
    p->stack->size = mem_size;
    p->timp = exec_time;
    p->tot_timp = exec_time;
    p->timp_run = 0;
    p->dim_mem = mem_size;
    //afisez mesajul respectiv unui proces creat
    fprintf(out, "Process created successfully: PID: %d, ",p->PID); 
    fprintf(out, "Memory starts at 0x%x.\n", p->inc_mem);
    if(mOS->running->PID == 0)
    {//daca nu exista procese
        free(mOS->running);
        mOS->running = p;
        return;
    }
    else 
    {
        if(mOS->waiting->inc == NULL)
        {//daca coada de ateptare e goala
            IntrQ(mOS->waiting, p);
            free(p);
            return;
        }
        //introduc in coada de asteptare sortat 
        TCoada *aux = InitC();
        if(!aux)return;
        TProces *el = (TProces *)malloc(sizeof(TProces));
        if(!el)
        {
            free(aux);
            return;
        }
        int inserat = 0;
        while (ExtrQ(mOS->waiting, el))
        {
            IntrQ(aux, el);
        }
        while (ExtrQ(aux, el))
        {
            if(comp_pr(p, el) > 0  && inserat == 0)
            {//adaug sortat procesul
                IntrQ(mOS->waiting, p);
                inserat = 1;
            }
            IntrQ(mOS->waiting, el);
        }
        if(inserat == 0) IntrQ(mOS->waiting, p);
        free(el);
        free(aux);
        free(p);
    }
}

void print_stack(TStiva *s, int pid, FILE *out)
{//afiseaza o stiva
    if(s->vf == NULL)
    {//daca stiva e goala
        fprintf(out, "Empty stack PID %d.\n", pid);
        return;
    }
    TStiva *aux = InitS();
    if(!aux) return;
    int *ae = (int *)malloc(sizeof(int));
    if(!ae)
    {
        free(aux);
        return;
    }
    fprintf(out, "Stack of PID %d:", pid);
    while (Pop(s, (void *)ae))
    {//cat exista elemente din stiva
        Push(aux, (void *)ae);
    }
    while (Pop(aux, (void *)ae))
    {//le afisez de la baza la varf
        fprintf(out, " %d", *ae );
        Push(s, (void *)ae);
    }
    fprintf(out, ".\n");
    free(ae);
    free(aux);
}
void printQ(TCoada *c, FILE *out)
{//printeaza o coada in fisier
    TCoada *aux = InitC();
    TProces *el = (TProces *)malloc(sizeof(TProces));
    while (ExtrQ(c, el))
    {//afisez datele unui proces
        print_proces(el, out);
        if(c->inc != NULL)fprintf(out, ",\n");
        IntrQ(aux, el);
    }
    while(ExtrQ(aux, el))
    {
        IntrQ(c, el);
    }
    free(el);
    free(aux);   
}

void push_pop(TStiva **s, int pid, char *f, FILE *in, FILE *out)
{//face pop sau push pe stiva
    int data;
    if(strcmp(f, "push") == 0)
    {//daca trebuie sa adaug in stack
        if(fscanf(in, "%d", &data));//citesc infomatia
        else return;
        if((*s)->size == 0)
        {//daca nu mai exsista memorie
            fprintf(out, "Stack overflow PID %d.\n", pid);
        }
        else
        {
            Push(*s, &data);
            (*s)->size -= 4;
        }  
    }
    if(strcmp(f, "pop") == 0)
    {//daca trebuie sa scot din stack
        if(Pop(*s, &data))
        {//daca exista elemete
            (*s)->size += 4;
        }
        else
        {//in caz ca pe stiva nu exista elemente
            fprintf(out, "Empty stack PID %d.\n", pid);
        }  
    }
}

void prelucrare_proces(TProces *p, char *f, FILE *out, int stare)
{//prelucreaza pocesul pentru funtia get sau print stack
    if(strcmp(f, "get") == 0)
    {//daca e funtia get
        if(stare == 1)
        {//daca ruleaza
            fprintf(out, "Process %d is running ", p->PID);
        }
        if(stare == 2)
        {//daca e in coada de asteptare
            fprintf(out, "Process %d is waiting ", p->PID);
        }
        if(stare == 3)
        {//daca procesul e terminat
            fprintf(out, "Process %d is finished.\n", p->PID);
        }
        else
        {//afisez timpul ramas de rulare
            fprintf(out, "(remaining_time: %d).\n", p->timp);
        }
        
    }
    if(strcmp(f, "stack") == 0)
    {//daca trebuie sa afise stack-l unui proces
        if(stare == 3)
        {//in caz de procesul si-a finisat rularea
            fprintf(out, "PID %d not found.\n", p->PID);
            return;
        }
        print_stack(p->stack, p->PID, out);
    }
}

void find_proces(TGest mOS, FILE *in, FILE *out, char *funtia, int l)
{//funtia ce gaseste un proces in sistemul meu
    int pid;//citesc pidul
    if(fscanf(in, "%d", &pid));
    else return;
    int  T_F = 0;
    if(mOS.running->PID == pid)
    {//daca procesul cautat est in stare running
        if(l == 1)
        {//daca trebuie sa aduag sau sa scot de pe stiva unui proces
            push_pop(&(mOS.running->stack),mOS.running->PID, funtia, in, out);
        }
        else prelucrare_proces(mOS.running, funtia, out, 1);
        T_F = 1;
    }
    TCoada *aux = InitC();//coada auxiliara
    TProces *el = (TProces *)malloc(sizeof(TProces));
    while (ExtrQ(mOS.waiting, (void *)el))
    {//verific procesele din coada de asteptare
        if(el->PID == pid)
        {//daca procesul cautat este in stare waiting
            if(l == 1)
            {//daca trebuie sa aduag sau sa scot de pe stiva unui proces
                push_pop(&(el->stack), el->PID, funtia, in, out);
            }
            else prelucrare_proces(el, funtia, out, 2);
            T_F = 1;
        }
        IntrQ(aux, (void *)el);
    }
    while(ExtrQ(aux, (void *)el))
    {
        IntrQ(mOS.waiting, (void *)el);
    }
    if(T_F && l != 0)
    {//daca l-am gasit si nu e funtia get
        free(aux);
        free(el);
        return;
    }
    if(l == 5 || l == 1)
    {//cand printez stack-l sau lucrez cu stiva procesului
        fprintf(out, "PID %d not found.\n", pid);
        free(el);
        free(aux);
        return;
    }
    while(ExtrQ(mOS.finished, (void *)el))
    {//verific procesele din coada procese terminate
        if(el->PID == pid )
        {//daca procesul cautat este in stare finished
            prelucrare_proces(el, funtia, out, 3);
            T_F = 1;
        }
        IntrQ(aux, (void *)el);
    }
    while(ExtrQ(aux, (void *)el))
    {
        IntrQ(mOS.finished, (void *)el);
    }
    //daca trebuie sa afisez starea procesului si nu a fost gasit
    if(T_F == 0 && strcmp(funtia, "get") == 0 )
        fprintf(out, "Process %d not found.\n", pid);
    else if(T_F == 0)//daca trebuie sa lucrez cu un proces si nu a fost gasit
        fprintf(out, "PID %d not found.\n", pid);
    free(el);
    free(aux);
}

void print(TGest mOS, FILE *in, FILE *out)
{//funtia print
    char print[10];
    if(fscanf(in, "%s", print));
    else return;
    if(strcmp(print, "waiting") == 0)
    {//daca printez waiting
        fprintf(out, "Waiting queue:\n[");
        printQ(mOS.waiting, out);
        fprintf(out,"]\n");
    }
    if(strcmp(print, "finished") == 0)
    {//daca printez finishde
        fprintf(out, "Finished queue:\n[");
        printQ(mOS.finished, out);
        fprintf(out,"]\n");
    }
    if(strcmp(print, "stack") == 0)
    {//daca printrez un stack a unui poces
        find_proces(mOS, in, out, print, 5);
    }
}

void finish(TGest mOS, FILE *out)
{//funtia finish
    if(mOS.running->PID == 0)
    {//daca ruleaza Idle
        fprintf(out, "Total time: 0\n");
        return;
    }
    int timp = mOS.running->timp;
    TProces *p =  mOS.running;
    IntrQ(mOS.finished, (void *)p);
    while(ExtrQ(mOS.waiting, (void *)p))
    {
        timp += p->timp;//calculez timpul ramas
        IntrQ(mOS.finished, (void *)p);//introduc procesel in finished
    }
    memcpy(mOS.running, &(mOS.Idle), sizeof(TProces));
    fprintf(out, "Total time: %d\n", timp);//printez timpul
}