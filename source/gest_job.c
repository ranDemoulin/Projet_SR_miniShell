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
        while (current_prc != NULL) { //parcours de la liste
            if (current_prc->pid == pid) { //si le pid correspond
                if(prev_prc == NULL){ //si c'est le premier element
                    tab_process->head = current_prc->next; //on le supprime
                }else{ //sinon on supprime l'element
                    prev_prc->next = current_prc->next;
                }
                Free(current_prc); //on libere la memoire
                nb_prc--;
                break;
            }
            prev_prc = current_prc;
            current_prc = current_prc->next;
        }
    }
}


int  exist_prc_fg(){
    int is_done = 0;
    process *test_prc = tab_process->head;
    while (test_prc!=NULL){ 
        if (test_prc->etat > 0) {
            is_done = 1;
        }
        test_prc=test_prc->next;
    }
    return is_done;
}


void initjob(){
    tab_process = Malloc(sizeof(tab_process));
    tab_process->head = NULL;
}

void endjob(){
    if (tab_process->head != NULL){
        process *current_prc = tab_process->head; 
        process *tmp;
        while (current_prc != NULL) {
            tmp=current_prc;
            current_prc=current_prc->next;
            Free(tmp);
        }
        Free(tab_process);
    }
}