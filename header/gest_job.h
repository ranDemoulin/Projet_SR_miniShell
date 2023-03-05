#ifndef GEST_JOB_H_
#define GEST_JOB_H_

#include <stdio.h>
#include <stdlib.h>

//struct process qui stocke les pid, les etats et les commandes avec foreground ou background
//les etats sont definis comme suit:
// 2: le processus est en cours d'execution
// 1: le processus est en pause
// 0: le processus est termine
// si le processus est en arriere plan on affiche l'etat en negatif
typedef struct process_ {
    pid_t pid;
    int etat;
    struct process_ *next;
} process;

typedef struct L_process_ {
    process* head;
} L_process;


// void addjob(pid_t pid, process *tab_process , int bg);
void addjob(pid_t pid, int bg);

// void removejob(pid_t pid, process *tab_process);
void removejob(pid_t pid);

void initjob();

void endjob();

#endif