#include "readcmd.h"
#include "csapp.h"

//fonction pour 0 pipe
void Aucun_pipe(char **cmd, int new_in, int new_out){
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        if(new_in){
            dup2(new_in, 0);
        }
        if(new_out){
            dup2(new_out, 1);
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
}

void Debut_Milieu(int i,char **cmd,int** MatPipe, int new_in){
    pipe(MatPipe[i]);
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i][0],MatPipe[i][1]);
    if (Fork() == 0) { //on cree un fils qui va executer la commande
        Dup2(MatPipe[i][1], 1); //on redirige la sortie standard vers l'entré du pipe
        Close(MatPipe[i][0]); //on ferme la lecture du pipe
        if (i!=0) {
            Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers la sortie du pipe
        }else{
            if(new_in){
                dup2(new_in, 0);
            }
        }
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[i], strerror(errno));
            exit(1);
        }
    }
    Close(MatPipe[i][1]); //on ferme l'ecriture du pipe
}

void Fin(int i,char **cmd,int** MatPipe, int new_out){ //
    printf("indice %d, lecture %d, ecriture %d\n",i,MatPipe[i-1][0],MatPipe[i-1][1]);
    if (fork() == 0) { //on cree un fils qui va executer la commande
        if(new_out){
            Dup2(new_out, 1);
        }
        Dup2(MatPipe[i-1][0], 0); //on redirige l'entree standard vers la sortie du pipe
        if (execvp(cmd[0], cmd) == -1) {
            fprintf(stderr, "Error: %s: %s\n", cmd[0], strerror(errno));
            exit(1);
        }
    }
}