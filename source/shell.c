#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "pipe.h"

sigset_t vide, masque_INT_TSTP;

// void CTRL_C_handler(int sig){

// }

int main() {
    // Signal(SIGINT,CTRL_C_handler);
    // Signal(SIGTSTP,CTRL_C_handler);

    sigemptyset(&vide);
    sigemptyset(&masque_INT_TSTP);
    sigaddset(&masque_INT_TSTP,SIGINT);
    sigaddset(&masque_INT_TSTP,SIGTSTP);

    while (1) {
        struct cmdline *l;
        int i, j, new_in = 0, new_out = 0;
        char **cmd;
        int is_pipe = 0;
        int **MatPipe; //on va stocker les pipes dans un tableau

        sigprocmask(SIG_BLOCK,&masque_INT_TSTP,&vide);

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

        if (l->seq[0]){

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
                    is_pipe = 1;
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


            if (is_pipe != 0){
            //on ferme tout les pipes
                for (i = 0; l->seq[i+1] != NULL; i++) {
                    // ici il y a un probleme de fermeture des pipes (pour les redirections probablement)
                    Close(MatPipe[i][0]);
                }
            }


            while (wait(NULL) > 0);


            //on libere la memoire
            if (is_pipe != 0){
                for (i = 0; l->seq[i] != NULL; i++) {
                    free(MatPipe[i]);
                }
                free(MatPipe);
            }
        }
    }
}