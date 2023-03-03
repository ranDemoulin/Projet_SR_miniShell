#ifndef PIPE_H_
#define PIPE_H_

#include <stdio.h>
#include <stdlib.h>
//struct process qui stocke les pid, les etats et les commandes avec foreground ou background
//les etats sont definis comme suit:
// 2: le processus est en cours d'execution
// 1: le processus est en pause
// 0: le processus est termine
// si le processus est en arriere plan on affiche l'etat en negatif
typedef struct process {
    pid_t pid;
    int etat;
} process;

// Fonction créant un fils lorsque il y a une seule commande 
void Aucun_pipe(char **cmd, int new_in, int new_out, process *tab_process);


// Fonction gérant les fils et les tubes lorsque il y a une suite de commandes
void Debut_Milieu(int i,char **cmd,int** MatPipe, int new_in, process *tab_process);


// Fonction gérant le dernier fils lorsque il y a une suite de commandes
void Fin(int i,char **cmd,int** MatPipe, int new_out, process *tab_process);

#endif