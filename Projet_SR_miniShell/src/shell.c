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
		int i, j;

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

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			if(!strcmp(cmd[0],"quit")){
				printf("exit\n"); 
				exit(0);	
			}
			if(fork()==0){
				//redirection a faire
				if(execv(seq[i][0],seq[i]) == -1){
					printf("inco\n"); 
				}
			}else{
				while(wait(NULL));
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
