#ifndef PIPE_H_
#define PIPE_H_

#include <stdio.h>
#include <stdlib.h>

// Fonction créant un fils lorsque il y a une seule commande 
void Aucun_pipe(char **cmd, int new_in, int new_out);


// Fonction gérant les fils et les tubes lorsque il y a une suite de commandes
void Debut_Milieu(int i,char **cmd,int** MatPipe, int new_in);


// Fonction gérant le dernier fils lorsque il y a une suite de commandes
void Fin(int i,char **cmd,int** MatPipe, int new_out);

#endif