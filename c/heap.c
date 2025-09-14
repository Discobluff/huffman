#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

void ajout(tas **t, arbreCompression *a) {            
    if (*t == NULL) {
        *t = (tas*)malloc(sizeof(tas));        
        (*t)->fg = NULL;
        (*t)->noeud = a;
        (*t)->fd = NULL;        
    } else {
        if ((*a).frequency < (*t)->noeud->frequency) {
            tas *temp1 = (*t)->fg;
            arbreCompression *temp2 = malloc(sizeof(arbreCompression));
            temp2 = (*t)->noeud;
            (*t)->fg = (*t)->fd;
            (*t)->noeud = a;
            (*t)->fd = temp1;
            ajout(&((*t)->fd), temp2);
        }
        else{
            tas *temp = (*t)->fg;
            (*t)->fg = (*t)->fd;
            (*t)->fd = temp;
            ajout(&((*t)->fd), a);
        }
    }
}

void ajoutListe(tas **t, arbreCompression **liste, unsigned int taille){
    for(unsigned int i=0;i<taille;i++){
        ajout(t,liste[i]);
    }
}

void popRacine(tas **t){
    if((*t)->fg == NULL && (*t)->fd == NULL){
        *t = NULL;
    }
    else if((*t)->fd == NULL){
        *t = (*t)->fg;
    }
    else if((*t)->fg == NULL){
        *t = (*t)->fd;
    }
    else{
        arbreCompression *ag = (*t)->fg->noeud;
        arbreCompression *ad = (*t)->fd->noeud;
        if((*ag).frequency < (*ad).frequency){
            (*t)->noeud = ag;
            popRacine(&((*t)->fg));
        }
        else{
            (*t)->noeud = ad;
            popRacine(&((*t)->fd));
        }
    } 
}

arbreCompression *popMinElement(tas **t){
    arbreCompression *res = (*t)->noeud;
    popRacine(t);
    return res;
}
