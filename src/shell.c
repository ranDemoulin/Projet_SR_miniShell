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
		int i;

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

		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			if(!strcmp(cmd[0],"quit")){
				printf("exit\n"); 
				exit(0);	
			}
			if(fork()==0){
				if(execvp(cmd[0],cmd) == -1){
					printf("Commande Inconnue\n");
				}
			}else{
				Wait(NULL);
			}
		}

		/* Display each command of the pipe */
		/* mise en évidence du découpage */
		// for (i=0; l->seq[i]!=0; i++) {
		// 	char **cmd = l->seq[i];
		// 	printf("seq[%d]: ", i);
		// 	printf("\n");
		// 	for (j=0; cmd[j]!=0; j++) {	
		// 		printf("w%d: %s ",j , cmd[j]);
		// 	}
		// 	printf("\n");
		// }
	}
}
