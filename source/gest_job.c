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