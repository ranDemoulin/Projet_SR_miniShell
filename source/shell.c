#include <stdio.h>
#include <stdlib.h>
#include "gest_job.h"
#include "readcmd.h"
#include "csapp.h"
#include "pipe.h"

sigset_t mask_vide, mask_all, mask_INT_TSTP, mask_CHLD, mask_tmp;
int nb_prc;
L_process *tab_process;

void CTRL_C_handler(int sig){
    process *test_prc = tab_process->head;
    while (test_prc!=NULL){
        if (test_prc->etat == 2) {
            Kill(test_prc->pid, SIGINT);
            test_prc->etat = 0;
        }
        test_prc=test_prc->next;
    }
}

void CTRL_Z_handler(int sig){
    process *test_prc = tab_process->head;
    while (test_prc!=NULL){
        if (test_prc->etat == 2) {
            Kill(test_prc->pid, SIGTSTP);
            test_prc->etat = 0;
        }
        test_prc=test_prc->next;
    }
}

void child_handler(int sig){
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status,WNOHANG)) > 0) {
        //actualiser le tableau de processus
        Sigprocmask(SIG_BLOCK, &mask_all, &mask_tmp);
        removejob(pid);
        Sigprocmask(SIG_SETMASK, &mask_tmp, NULL);
    }
    if (errno != ECHILD && errno != EXIT_SUCCESS)
        unix_error("waitpid error");
}


int main() {

    nb_prc=0;

    Sigemptyset(&mask_vide);
    Sigfillset(&mask_all);

    Sigemptyset(&mask_INT_TSTP);
    Sigaddset(&mask_INT_TSTP,SIGINT);
    Sigaddset(&mask_INT_TSTP,SIGTSTP);

    Sigemptyset(&mask_CHLD);
    Sigaddset(&mask_CHLD, SIGCHLD);

    Signal(SIGINT, CTRL_C_handler);
    Signal(SIGTSTP, CTRL_Z_handler);
    Signal(SIGCHLD, child_handler);

    initjob();


    while (1) {
        struct cmdline *l;
        int i, j, new_in = 0, new_out = 0, bg = 0;
        char **cmd;
        int is_pipe = 0;
        int **MatPipe; //on va stocker les pipes dans un tableau

        Sigprocmask(SIG_BLOCK, &mask_INT_TSTP, &mask_tmp);

        printf("shell> ");
        l = readcmd();

        /* If input stream closed, normal termination */
        if (!l) {
            printf("exit\n");
            endjob();
            exit(0);
        }
        if (l->err) {
            /* Syntax error, read another command */
            printf("error: %s\n", l->err);
            continue;
        }
        if (l->background) {
            bg = 1;
        }
        
        // Vérifie qu'il y a bien une commande
        if (l->seq[0]) {

            //on compte le nombre de commandes
            for (j = 0; l->seq[j] != NULL; j++);

            // allocation de la memoire pour le tableau de pipes
            if (j > 1) {
                is_pipe = 1;
                MatPipe = Malloc((j-1) * sizeof(int *));
                for (i = 0; i < j-1; i++) {
                    MatPipe[i] = Malloc(2 * sizeof(int));
                }
            }

            if (!strcmp(l->seq[0][0], "quit")) {
                printf("exit\n");
                endjob();
                exit(0);
            }

            if (l->in) {
                new_in = Open(l->in, O_RDONLY, 0);
                if (new_in == -1) {
                    fprintf(stderr, "Error: %s: %s\n", l->in, strerror(errno));
                    continue;
                }
            }
            if (l->out) {
                new_out = Open(l->out, O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
                if (new_out == -1) {
                    fprintf(stderr, "Error: %s: %s\n", l->out, strerror(errno));
                    continue;
                }
            }


            for (i = 0; l->seq[i] != NULL; i++) {
                cmd = l->seq[i];
                if (!strcmp(cmd[0], "quit")) {
                    printf("exit\n");
                    process *test_prc = tab_process->head;
                    while (test_prc!=NULL){
                        if (test_prc->etat != 0) {
                            Kill(test_prc->pid, SIGKILL);
                        }
                        test_prc=test_prc->next;
                    }
                    endjob();
                    exit(0);
                }

                if (i == 0 && l->seq[i + 1] == NULL) {             //si c'est le premier element de la sequence et le dernier
                    Aucun_pipe(cmd, new_in, new_out, bg);
                } else if (l->seq[i + 1] != NULL) {               //si c'est pas le dernier
                    Debut_Milieu(i, cmd, MatPipe, new_in, bg);
                } else {                                          //c'est une fin
                    Fin(i, cmd, MatPipe, new_out, bg);
                }
            }

            //tant que tous les processus au premier plan ne sont pas termines
            while (1) {
                int is_done = 1;
                process *test_prc = tab_process->head;
                while (test_prc!=NULL){
                    if (test_prc->etat > 0) {
                        is_done = 0;
                    }
                    test_prc=test_prc->next;
                }
                if (is_done == 1) {
                    break;
                }
            }

            //on libere la memoire
            if (is_pipe != 0) {
                //on ferme tout les pipes
                for (i = 0; l->seq[i + 1] != NULL; i++) {
                    // ici on ne ferme la sortie du tube car l'entré est fermée avant
                    Close(MatPipe[i][0]);
                }
                for (i = 0; l->seq[i+1] != NULL; i++) {
                    Free(MatPipe[i]);
                }
                Free(MatPipe);
            }
        }
    }
    endjob();
}