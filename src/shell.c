/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"


int main() {
    while (1) {
        struct cmdline *l;
        int i, new_in, new_out;
        char **cmd;
        int is_pipe = 0;
        int* mat[100]; //on va stocker les pipes dans un tableau de 100 pipes


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
            dup2(new_in, 0);
        }
        if (l->out) {
            new_out = open(l->out, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            if (new_out == -1) {
                fprintf(stderr, "Error: %s: %s\n", l->out, strerror(errno));
                exit(1);
            }
            dup2(new_out, 1);
        }




        for (i = 0; l->seq[i] != NULL; i++) {
            cmd=l->seq[i];
            if (!strcmp(cmd[0], "quit")) {
                printf("exit\n");
                is_pipe = 1;
            }

            if (i==0 && l->seq[i+1]==NULL)                                             //si c'est le premier element de la sequence et le dernier
            {
                if (fork() == 0) { //on cree un fils qui va executer la commande
                    if (execvp(cmd[0], cmd) == -1) {
                        fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
                        exit(1);
                    }
                } else {
                    while (wait(NULL) > 0);
                }
            }


            else if (l->seq[i+1]!=NULL)                                        //si c'est pas le dernier
            {
                is_pipe = 1;
                int fd[2];
                pipe(fd);
                mat[i] = fd;
                if (fork() == 0) { //on cree un fils qui va executer la commande
                    dup2(mat[i][1], 1); //on redirige la sortie standard vers le pipe
                    if (i!=0) {
                        dup2(mat[i-1][0], 0); //on redirige l'entree standard vers le pipe
                    }
                    if (execvp(cmd[i], cmd) == -1) {
                        fprintf(stderr, "Error: %s: %s\n", cmd[i], strerror(errno));
                        exit(1);
                    }
                } else {
                    while (wait(NULL) > 0);
                }
            }


            else
            {
                is_pipe = 1;
                int fd[2];
                pipe(fd);
                mat[i] = fd;
                if (fork() == 0) { //on cree un fils qui va executer la commande
                    dup2(mat[i-1][0], 0); //on redirige l'entree standard vers le pipe
                    if (execvp(cmd[0], cmd) == -1) {
                        fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
                        exit(1);
                    }
                } else {
                    while (wait(NULL) > 0);
                }
            }
        }
        if (is_pipe != 0){
        //on ferme tout les pipes
            for (i = 0; l->seq[i] != NULL; i++) {
                close(mat[i][0]);
                close(mat[i][1]);
            }
        }
    }
}