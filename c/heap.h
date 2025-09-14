#ifndef HEAP_H
#define HEAP_H

struct arbreCompression {int frequency ; int value ; struct arbreCompression *fg ; struct arbreCompression *fd;};
typedef struct arbreCompression arbreCompression;
struct couple {arbreCompression *arbre ; int frequency;};
typedef struct couple couple;
struct tas { struct tas *fg ; arbreCompression *noeud ; struct tas *fd;};
typedef struct tas tas;

void ajout(tas **t, arbreCompression *a);
void ajoutListe(tas **t, arbreCompression **liste, unsigned int taille);
void popRacine(tas **t);
arbreCompression *popMinElement(tas **t);

#endif