#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

int nbpipe = 0;

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

void Debut_Milieu_Fin( struct cmdline *l,int** MatPipe, int new_in, int new_out){
    pipe(MatPipe[0]);
    if (Fork() == 0) {
        Dup2(MatPipe[0][1], 1);
        Close(MatPipe[0][0]);
        if(new_in){
            dup2(new_in, 0);
        }
        if (execvp(l->seq[0][0], l->seq[0]) == -1) {
            fprintf(stderr, "Error: %s: %s\n", l->seq[0][0], strerror(errno));
            exit(1);
        }
    }
    Close(MatPipe[0][1]);

    if (!strcmp(l->seq[1][0], "quit")) {
        printf("exit\n");
        while (wait(NULL) > 0);
        //on ferme tout les pipes
        for (int i = 0; l->seq[i + 1] != NULL; i++) {
            Close(MatPipe[i][0]);
        }
        //on libere la memoire
        for (int i = 0; i < nbpipe; i++) {
            free(MatPipe[i]);
        }
        free(MatPipe);
        exit(0);
    }
    if (fork() == 0) { //on cree un fils qui va executer la commande
        if(new_out){
            Dup2(new_out, 1);
        }
        Dup2(MatPipe[0][0], 0); //on redirige l'entree standard vers la sortie du pipe
        if (execvp(l->seq[1][0], l->seq[1]) == -1) {
            fprintf(stderr, "Error: %s: %s\n", l->seq[1][0], strerror(errno));
            exit(1);
        }
    }
}

int main() {
    while (1) {
        struct cmdline *l;
        int j, new_in = 0, new_out = 0;
        int is_pipe = 0;
        int **MatPipe; //on va stocker les pipes dans un tableau
        printf("shell> ");
        l = readcmd();
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
        if (l->seq[0]) {
            for (j = 0; l->seq[j] != NULL; j++) { //on compte le nombre de commandes
            }
            if (j == 2) {
                is_pipe = 1;
                // allocation de la memoire pour le tableau de pipes
                MatPipe = malloc(sizeof(int*));
                MatPipe[0] = malloc(2*sizeof(int));
            } else if (j > 2) {
                fprintf(stderr,"il y a plus qu'un seul pipe\n");
                exit(1);
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
            if (j == 1) {
                Aucun_pipe(l->seq[0], new_in, new_out);
            } else {
                Debut_Milieu_Fin(l, MatPipe, new_in, new_out);
            }
            while (wait(NULL) > 0);
            if (is_pipe != 0) {
                //on ferme tout les pipes
                for (int i = 0; l->seq[i + 1] != NULL; i++) {
                    Close(MatPipe[i][0]);
                }
            }
            //on libere la memoire
            if (is_pipe != 0) {
                for (int i = 0; l->seq[i] != NULL; i++) {
                    free(MatPipe[i]);
                }
                free(MatPipe);
            }
        }
    }
}
