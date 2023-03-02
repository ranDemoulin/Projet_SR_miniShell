/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

//fonction pour 0 pipe
void gere0pipe(char **cmd){
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    } else {
        while (wait(NULL) > 0);
    }
}

void geremidlecommande(int i,char **cmd,int** MatPipe){
    int fd[2];
    pipe(fd);
    MatPipe[i] = fd;
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        Dup2(MatPipe[i][1], 1); //on redirige la sortie standard vers le pipe
        if (i!=0) {
            Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers le pipe
        }
        if (execvp(cmd[i], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[i], strerror(errno));
            exit(1);
        }
    } else {
        while (wait(NULL) > 0);
    }
}

void gerefin(int i,char **cmd,int** MatPipe){ //
    int fd[2];
    pipe(fd);
    MatPipe[i] = fd;
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers le pipe
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    } else {
        while (wait(NULL) > 0);
    }

}

int main() {
    while (1) {
        struct cmdline *l;
        int i, new_in, new_out;
        char **cmd;
        int is_pipe = 0;
        int* MatPipe[100]; //on va stocker les pipes dans un tableau de 100 pipes


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
            Dup2(new_in, 0);
        }
        if (l->out) {
            new_out = Open(l->out, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            if (new_out == -1) {
                fprintf(stderr, "Error: %s: %s\n", l->out, strerror(errno));
                exit(1);
            }
            Dup2(new_out, 1);
        }


        for (i = 0; l->seq[i] != 0; i++) {
            cmd = l->seq[i];
            if (!strcmp(cmd[0], "quit")) {
                printf("exit\n");
                is_pipe = 1;
            }
            if (i==0 && l->seq[i+1]==NULL){                 //si c'est le premier element de la sequence et le dernier
                fprintf(stderr,"no pipe");
                gere0pipe(cmd);
            }else if (l->seq[i+1]!=NULL){                   //si c'est pas le dernier
                is_pipe = 1;
                geremidlecommande(i,cmd,MatPipe);
            }else{                                          //c'est une fin
                is_pipe = 1;
                gerefin(i,cmd,MatPipe);
            }
        }
        if (is_pipe != 0){
        //on ferme tout les pipes
            for (i = 0; l->seq[i] != 0; i++) {
                Close(MatPipe[i][0]);
                Close(MatPipe[i][1]);
            }
        }
    }
}