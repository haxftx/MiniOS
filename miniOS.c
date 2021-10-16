#include "functii_sistem.h"

void rezolvare(char *in, char *out)
{//deschid fisierele
    FILE *input = fopen(in, "r");
    if(!input)
    {//daca nu exista fisierul de intrare
        fprintf(stderr, "Nu s-a gasit fisierul de input\n");
        return;
    }
    FILE *output = fopen(out, "w");
    if(!output)
    {//daca nu reuseste sa gaseasca sau sa creeze fisierul de iesire
        fprintf(stderr, "Nu s-a putut deschide fisierul de output\n");
        fclose(input);
        return;
    }
    int T;//coanta de timp care o poate rula continuu un proces
    if(fscanf(input, "%d", &T));
    else return;
    char funtia[20];
    TGest MiniOS = Init();//initializez sistemul
    int pid = 0;//ultimul pid adaugat
    int adr = 0;//ultima adresa adaugata
    TLista lp = NULL, lm = NULL;
    while(fscanf(input, "%s", funtia) != EOF)
    {//citesc funtia si o verific
        if(strcmp(funtia, "add") == 0)
        {//daca trebuie sa adaug un proces
            add(&MiniOS, input, output, &pid, &adr, &lp, &lm);
        }
        if(strcmp(funtia, "get") == 0)
        {//daca trebuie sa gasesc un proces
            find_proces(MiniOS, input, output, funtia, 0);
        }
        if(strcmp(funtia, "print") == 0)
        {//daca trebuie sa printez
            print(MiniOS, input, output);
        }
        if(strcmp(funtia, "run") == 0)
        {//daca trebuie sa rulez
            run(&MiniOS, input, T, &lp, &lm, &adr);
        }
        if(strcmp(funtia, "push") == 0)
        {//dcaa trebuie sa adaug pe stiva unui proces
            find_proces(MiniOS, input, output, funtia, 1);
        }
        if(strcmp(funtia, "pop") == 0)
        {//daca trebuie sa scot de pe stiva unui proces
            find_proces(MiniOS, input, output, funtia, 1);
        }
        if(strcmp(funtia, "finish") == 0)
        {//daca trebuie sa termin totul
            finish(MiniOS, output);
        }
    }
    DistrugeL(&lm);//eliberarea memoriei libere
    DistrugeL(&lp);//eliberarea pidurilor libere
    Distruge_Guest(&MiniOS);//eliberarea sistemului
    fclose(input);//inchiderea fisierelor
    fclose(output);
}

int main(int argc, char **argv)
{
    if(argc != 3) 
    {//daca nu-i rulat programul corect 
        fprintf(stderr, "Usage: %s in_file out_file\n", argv[0]);
    }
    else
    {
        rezolvare(argv[1], argv[2]);
    }
    return 0;
}