#include "gest_job.h"

int nb_prc;

void addjob(pid_t pid, process *tab_process , int bg){
    tab_process[nb_prc].pid = pid;
        tab_process[nb_prc].etat = 2;
        if (bg) {
            tab_process[nb_prc].etat = -2;
        }
        nb_prc++;
}

void removejob(pid_t pid, process *tab_process){
    for (int i = 0; i < nb_prc; i++) {
                if (tab_process[i].pid == pid) {
                    nb_prc--;
                    tab_process[i].etat = 0;
                }
            }
}

void initjob(process *tab_process){
    tab_process = malloc(100 * sizeof(process));
    for (int i = 0; i < 100; i++) {
        tab_process[i].pid = 0;
        tab_process[i].etat = 0;
    }
}