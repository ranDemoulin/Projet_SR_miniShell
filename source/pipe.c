#include "csapp.h"
#include "pipe.h"

sigset_t mask_vide, mask_all, mask_INT_TSTP, mask_CHLD, mask_tmp;
int nb_prc;

//fonction pour 0 pipe
void Aucun_pipe(char **cmd, int new_in, int new_out, process *tab_process, int background){
    pid_t pid;
    // On debloque les CTRL C et Z
    Sigprocmask(SIG_UNBLOCK,&mask_INT_TSTP,NULL);

    // On bloque SIDCHLD
    Sigprocmask(SIG_BLOCK, &mask_CHLD, &mask_tmp);
    if ((pid = Fork()) == 0) { // On cree un fils qui va executer la commande
        Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIDCHLD

        if(new_in){
            Dup2(new_in, 0);
        }
        if(new_out){
            Dup2(new_out, 1);
        }
        if (execvp(cmd[0], cmd) == -1) {
            if (errno == ENOENT){
                fprintf(stderr, "Error: %s: command not found\n", cmd[0]);
            }else{
                fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            }
            exit(1);
        }
    }
    // Mise en liste du job
    Sigprocmask(SIG_BLOCK, &mask_all, NULL);
    tab_process[nb_prc].pid = pid;
    tab_process[nb_prc].etat = 2;
    if (background) {
        tab_process[nb_prc].etat = -2;
    }
    nb_prc++;
    Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIGCHLD
}

void Debut_Milieu(int i,char **cmd,int** MatPipe, int new_in, process *tab_process, int background){
    pid_t pid;

    // On debloque les CTRL C et Z
    sigprocmask(SIG_UNBLOCK,&mask_INT_TSTP,NULL);

    // Création tube
    pipe(MatPipe[i]);
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i][0],MatPipe[i][1]);
     // On bloque SIDCHLD
    Sigprocmask(SIG_BLOCK, &mask_CHLD, &mask_tmp);
    if ((pid = Fork()) == 0) { // On cree un fils qui va executer la commande
        Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIDCHLD

        // Gestion des S/E des tubes
        Dup2(MatPipe[i][1], 1);        //on redirige la sortie standard vers l'entré du pipe
        Close(MatPipe[i][0]);          //on ferme la lecture du pipe
        if (i!=0) {
            Dup2(MatPipe[i-1][0], 0);  //on redirige l'entree standard vers la sortie du pipe
        }else{
            if(new_in){
                Dup2(new_in, 0);
            }
        }
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[i], strerror(errno));
            exit(1);
        }
    }
    // Mise en liste du job
    Sigprocmask(SIG_BLOCK, &mask_all, NULL);
    tab_process[nb_prc].pid = pid;
    tab_process[nb_prc].etat = 2;
    if (background) {
        tab_process[nb_prc].etat = -2;
    }
    nb_prc++;
    Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIGCHLD

    Close(MatPipe[i][1]); //on ferme l'ecriture du pipe
}

void Fin(int i,char **cmd,int** MatPipe, int new_out ,process *tab_process, int background){
    pid_t pid;
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i-1][0],MatPipe[i-1][1]);

    // On debloque les CTRL C et Z
    sigprocmask(SIG_UNBLOCK,&mask_INT_TSTP,NULL);

    // On bloque SIDCHLD
    Sigprocmask(SIG_BLOCK, &mask_CHLD, &mask_tmp);
    if ((pid = Fork()) == 0) { // On cree un fils qui va executer la commande
        Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIDCHLD

        if(new_out){
            Dup2(new_out, 1);
        }
        Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers la sortie du pipe
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    }
    // Mise en liste du job
    Sigprocmask(SIG_BLOCK, &mask_all, NULL);
    tab_process[nb_prc].pid = pid;
    tab_process[nb_prc].etat = 2;
    if (background) {
        tab_process[nb_prc].etat = -2;
    }
    nb_prc++;
    Sigprocmask(SIG_SETMASK, &mask_tmp, NULL); // On debloque SIDCHLD
}