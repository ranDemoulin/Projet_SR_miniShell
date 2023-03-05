#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

int nb_processus = 0;

//fonction pour 0 pipe
void Aucun_pipe(char **cmd, int new_in, int new_out){
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        if(new_in){
            dup2(new_in, 0);
        }
        if(new_out){
            dup2(new_out, 1);
        }
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    }
}

void Debut_Milieu(int i,char **cmd,int** MatPipe, int new_in){
    pipe(MatPipe[i]);
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i][0],MatPipe[i][1]);
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        Dup2(MatPipe[i][1], 1); //on redirige la sortie standard vers l'entré du pipe
        Close(MatPipe[i][0]); //on ferme la lecture du pipe
        if (i!=0) {
            Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers la sortie du pipe
        }else{
            if(new_in){
                dup2(new_in, 0);
            }
        }
        nb_processus++;
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[i], strerror(errno));
            exit(1);
        }
    }
    Close(MatPipe[i][1]); //on ferme l'ecriture du pipe
}

void Fin(int i,char **cmd,int** MatPipe, int new_out){ //
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i-1][0],MatPipe[i-1][1]);
    if (fork() == 0) { //on cree un fils qui va executer la commande
        if(new_out){
            Dup2(new_out, 1);
        }
        Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers la sortie du pipe
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    }
}

int main() {
    while (1) {
        struct cmdline *l;
        int i, j, new_in = 0, new_out = 0;
        char **cmd;
        int is_pipe = 0;
        int **MatPipe; //on va stocker les pipes dans un tableau
        printf("shell> ");
        l = readcmd();
        for (j = 0; l->seq[j] != NULL; j++) { //on compte le nombre de commandes
        }
        if (j > 1) {
            is_pipe = 1;
            // allocation de la memoire pour le tableau de pipes
            MatPipe = malloc(j*sizeof(int*));
            for (i = 0; i < j; i++) {
                MatPipe[i] = malloc(2*sizeof(int));
            }
        }
        /* If input stream closed, normal termination */
        if (!l) {
            printf("exit\n");
            exit(0);
        }
        if (l->err) {
            /* Syntax error, read another command */
            printf("error: %s\n", l->err);
            continue;
        }
        if (!strcmp(l->seq[0][0], "quit")) {
            printf("exit\n");
            exit(0);
        }
        if (l->in) {
            new_in = open(l->in, O_RDONLY);
            if (new_in == -1) {
                fprintf(stderr, "Error: %s: %s\n", l->in, strerror(errno));
                exit(1);
            }
        }
        if (l->out) {
            new_out = open(l->out, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            if (new_out == -1) {
                fprintf(stderr, "Error: %s: %s\n", l->out, strerror(errno));
                exit(1);
            }
        }
        for (i = 0; l->seq[i] != NULL; i++) {
            cmd=l->seq[i];
            if (!strcmp(cmd[0], "quit")) {
                printf("exit\n");
                while (wait(NULL) > 0);
                if (is_pipe != 0){
                    //on ferme tout les pipes
                    for (i = 0; i < nb_processus; i++) {
                        Close(MatPipe[i][0]);
                    }
                }
                //on libere la memoire
                if (is_pipe != 0){
                    for (i = 0; l->seq[i] != NULL; i++) {
                        free(MatPipe[i]);
                    }
                    free(MatPipe);
                }
                exit(0);
            }

            if (i==0 && l->seq[i+1]==NULL){                 //si c'est le premier element de la sequence et le dernier
                Aucun_pipe(cmd, new_in, new_out);
            }
            
            else if (l->seq[i+1]!=NULL){                   //si c'est pas le dernier
                Debut_Milieu(i,cmd,MatPipe,new_in);
            }
            
            else{                                          //c'est une fin
                Fin(i,cmd,MatPipe, new_out);
            }
        }
        while (wait(NULL) > 0);
        if (is_pipe != 0){
        //on ferme tout les pipes
            for (i = 0; l->seq[i+1] != NULL; i++) {
                Close(MatPipe[i][0]);
            }
        }
        //on libere la memoire
        if (is_pipe != 0){
            for (i = 0; l->seq[i] != NULL; i++) {
                free(MatPipe[i]);
            }
            free(MatPipe);
        }
    }
}