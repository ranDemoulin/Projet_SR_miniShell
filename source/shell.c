#include <stdio.h>
#include <stdlib.h>
#include "gest_job.h"
#include "readcmd.h"
#include "csapp.h"
#include "pipe.h"

extern sigset_t mask_vide, mask_all, mask_INT_TSTP, mask_CHLD, mask_tmp;
extern int nb_prc;
extern L_process *tab_process;
// pid_t last_pid;

void free_l(struct cmdline *l){
    Free(l->in);
    Free(l->out);
    Free(l->err);
    for (int i=0; l->seq[i]!=NULL; i++){
        for (int j=0; l->seq[i][j]!=NULL; j++){
            Free(l->seq[i][j]);
        }
        Free(l->seq[i]);
    }
    Free(l->seq);
    Free(l);
}

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
            test_prc->etat = -1;
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
    int exit_status = -1;
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


    while (exit_status == -1) {
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
            exit_status = 0;
            continue;
        }
        if (l->err) {
            /* Syntax error, read another command */
            printf("error: %s\n", l->err);
            continue;
        }
        if (l->background) {
            bg = 1;
        }
        
//////////////////////// Début de la gastion des commande ////////////////////////
        //Vérifie qu'il y a bien une commande
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

//////////////////////////////// Redirection S/E ////////////////////////////////
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

/////////////////////////////// Fonctions interne ////////////////////////////////
            // Variable pour ne pas aussi faire la partie fonction externe si 
            // on n'a fait une fonction interne
            int test_est_interne = 0;

            // Fonction interne quit
            if (!strcmp(l->seq[0][0], "quit")) {
                printf("exit\n");
                exit_status = 0;
                free_l(l);
                continue;
            }

            ////// Fonction interne bg
            if (!strcmp(l->seq[0][0], "bg")) {
                int test_find = 0;
                process *test_prc = tab_process->head;
                if (l->seq[0][1]) {
                    while (test_prc!=NULL){
                        if (test_prc->pid == (pid_t)strtol(l->seq[0][1], NULL, 10)) {
                            Kill(test_prc->pid, SIGCONT);
                            test_prc->etat = -2;
                            test_find = 1;
                        }
                        test_prc=test_prc->next;
                    }
                    if(!test_find){
                        fprintf(stderr,"Error : there is no process of PID %d\n",(pid_t)strtol(l->seq[0][1], NULL, 10));
                    }
                }else{
                    while (test_prc!=NULL){
                        if (test_prc->etat == -1) {
                            Kill(test_prc->pid, SIGCONT);
                            test_prc->etat = -2;
                        }
                        test_prc=test_prc->next;
                    }
                    fprintf(stdout,"All stoped process are now in back-ground\n");
                }
                test_est_interne = 1;
            }

            ////// Fonction interne fg
            if (!strcmp(l->seq[0][0], "fg")) {
                int test_find = 0;
                process *test_prc = tab_process->head;
                if (l->seq[0][1]) {
                    Sigprocmask(SIG_BLOCK, &mask_all, &mask_tmp);
                    while (test_prc!=NULL){
                        if (test_prc->etat == (pid_t)strtol(l->seq[0][1], NULL, 10)) {
                            Kill(test_prc->pid, SIGCONT);
                            test_prc->etat = 2;
                            test_find = 1;
                        }
                        test_prc=test_prc->next;
                    }
                    if(!test_find){
                        fprintf(stderr,"Error : there is no process of PID %d\n",(pid_t)strtol(l->seq[0][1], NULL, 10));
                    }
                    Sigprocmask(SIG_SETMASK, &mask_tmp, NULL);
                }else{
                    while (test_prc!=NULL){
                        if (test_prc->etat == -1) {
                            Kill(test_prc->pid, SIGCONT);
                            test_prc->etat = 2;
                        }
                        test_prc=test_prc->next;
                    }
                    fprintf(stdout,"All stoped process are now in fore-ground\n");
                }
                test_est_interne = 1;
            }


////////////////////////// Gestion des fonctions externe et pipe ////////////////////////// 
            if (!test_est_interne){
                for (i = 0; l->seq[i] != NULL; i++) {
                    cmd = l->seq[i];
                    if (!strcmp(cmd[0], "quit")) {
                        printf("exit\n");
                        process *test_prc = tab_process->head;
                        while (test_prc!=NULL){
                            if (test_prc->etat != 0) {
                                Kill(test_prc->pid, SIGINT);
                            }
                            test_prc=test_prc->next;
                        }
                        endjob();
                        free_l(l);
                        exit_status = 0;
                        break;
                    }

                    if (i == 0 && l->seq[i + 1] == NULL) {             //si c'est le premier element de la sequence et le dernier
                        Aucun_pipe(cmd, new_in, new_out, bg);
                    } else if (l->seq[i + 1] != NULL) {               //si c'est pas le dernier
                        Debut_Milieu(i, cmd, MatPipe, new_in, bg);
                    } else {                                          //c'est une fin
                        Fin(i, cmd, MatPipe, new_out, bg);
                    }
                }
            }

////////////// Attente des processus en premier plan ///////////////////////////////////////
            int test_exist_fg = 1;
            while (test_exist_fg){
                // On bloque les signaux pour que le test dans "exist_prc_fg()"
                // se fasse sans problème (libération du process entre while et if)
                Sigprocmask(SIG_BLOCK, &mask_all, &mask_tmp);
                test_exist_fg = exist_prc_fg();
                Sigprocmask(SIG_SETMASK, &mask_tmp, NULL);
            }

////////////////////// Libération des tubes ////////////////////////////////////////////////
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
    exit(exit_status);
}