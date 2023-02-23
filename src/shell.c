/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"


int main()
{
	while (1) {
		struct cmdline *l;

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

        char **cmd = l->seq[0]; //on recupere la commande et ses arguments vu que l'on a pas de pipe
        if(!strcmp(cmd[0],"quit")){ //si la commande est quit on quitte le shell
            exit(0);
        }
        if(fork()==0){ //on cree un fils pour executer la commande
            if(execvp(cmd[0],cmd) == -1){ //si la commande n'existe pas on affiche un message d'erreur
                printf("Commande Inconnue\n");
            }
        }else{
            Wait(NULL); //on attend la fin de l'execution de la commande
        }
	}
}
