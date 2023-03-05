#include "csapp.h"
#include "gest_job.h"

L_process *tab_process;
int nb_prc;

void addjob(pid_t pid, int bg){
    process *new_prc = Malloc(sizeof(process));
    new_prc->pid = pid;
    if (bg) { 
        new_prc->etat = -2;
    }else{
        new_prc->etat = 2;
    }
    new_prc->next = tab_process->head;
    tab_process->head = new_prc;
    nb_prc++;
}

void removejob(pid_t pid){
    if (tab_process->head != NULL){
        process *current_prc = tab_process->head; 
        process *prev_prc = NULL; 
        while (current_prc != NULL) {
            if (current_prc->pid == pid) {
                if(prev_prc == NULL){
                    tab_process->head = NULL;
                }else{
                    prev_prc->next = current_prc->next;
                    current_prc->pid=0;
                    current_prc->etat=0;
                }
                free(current_prc);
                nb_prc--;
                break;
            }
            prev_prc = current_prc;
            current_prc = current_prc->next;
        }
    }
}

void initjob(){
    tab_process = malloc(100 * sizeof(tab_process));
    tab_process->head = NULL;
}

void endjob(){
    free(tab_process);
}