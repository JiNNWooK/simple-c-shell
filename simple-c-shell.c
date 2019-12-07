/*
  * simple-c-shell.c
  * 
  * Copyright (c) 2013 Juan Manuel Reyes
  * 
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
This space is where we write down what we modified, removed or added.
ex)
2019.11.05 add Easter Egg show Team Member  -  By JW.CHOI
2019.12.07 modify comment Eng to Kor -  By SH.CHOI 
2019.12.07 add command rm and show Warnings  -  By JW.CHOI & SH.SHOI
2019.12.07 add command rmdir and show Warnings  -  By SH.CHOI & JW.CHOI
2019.12.07 add command mv  - By JW.CHOI
2019.12.08 add command mkdir  -  By SH.CHOI
2019.12.08 add commanf ls  -  By JW.CHOI
2019.12.08 add command cp

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include "util.h"
#include <sys/stat.h>
#include <dirent.h>

#define LIMIT 256 // command�� ���� �ִ� ��ū ��.
#define MAXLINE 1024 // user input�� �ִ� ���� ��.


/**
 * shell�� �ʱ�ȭ�ϴ� �� ���Ǹ�, ���� �� ���ٹ��� ����ߴ�.
 * http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
void init(){
		// interactive�� ���� ������ Ȯ��
        GBSH_PID = getpid();
        // STDIN�� �͹̳��� ��� shell�� interactive��.
        GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);  

		if (GBSH_IS_INTERACTIVE) {
			// foreground���� �ݺ�
			while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp()))
					kill(GBSH_PID, SIGTTIN);             
	              
	              
	        // SIGCHILD �� SIGINT�� ���� �ñ׳� �ڵ鷯 ����
			act_child.sa_handler = signalHandler_child;
			act_int.sa_handler = signalHandler_int;			
			
			/**sigaction�� ������ ������ ����.
			
			struct sigaction {
				void (*sa_handler)(int);
				void (*sa_sigaction)(int, siginfo_t *, void *);
				sigset_t sa_mask;
				int sa_flags;
				void (*sa_restorer)(void);
				
			}*/
			
			sigaction(SIGCHLD, &act_child, 0);
			sigaction(SIGINT, &act_int, 0);
			
			/* ��ü ���μ��� ���g�� ����
			 * shell ���μ����� ���ο� ���μ��� ���� ������ �����
			 */
			setpgid(GBSH_PID, GBSH_PID);
			GBSH_PGID = getpgrp();
			if (GBSH_PID != GBSH_PGID) {
					printf("Error, the shell is not process group leader");
					exit(EXIT_FAILURE);
			}
			// �͹̳� ����
			tcsetpgrp(STDIN_FILENO, GBSH_PGID);  
			
			// shell�� �⺻ �͹̳� �Ӽ� ����
			tcgetattr(STDIN_FILENO, &GBSH_TMODES);

			// �ٸ�methods�� ���� ���� ���丮�� ��´�.
			currentDirectory = (char*) calloc(1024, sizeof(char));
        } else {
                printf("Could not make the shell interactive.\n");
                exit(EXIT_FAILURE);
        }
}
void ls(char Opt[],char Pos[]){
	char* ls_args[3]={"ls",Opt,Pos};
	int status;
	pid_t c_pid,pid;
	//printf("Ok");
	//printf("\n%s,%s\n",ls_args[1],ls_args[2]);
	c_pid=fork();

	if(c_pid==0){//child
		execvp(ls_args[0],ls_args);
		exit(0);
	}else if(c_pid>0) {   /* PARENT */

    		if( (pid = wait(&status)) < 0){
      			perror("wait");
    			_exit(1);
		}


       }else{
    	perror("fork failed");
       }

}

void move(char f1[], char f2[]){
    int fd1,fd2;
    int r_size,w_size;

    int status;
    pid_t c_pid,pid;
    c_pid=fork();
    if(c_pid==0){
    	 char buf[100];
	 fd1 = open(f1, O_RDONLY);
    	 fd2 = open(f2, O_RDWR|O_CREAT|O_EXCL, 0664);

    	 r_size = read(fd1,buf,100);
    	 w_size = write(fd2,buf,r_size);
    	 while(r_size == 100)
    	 {
         	r_size=read(fd1,buf,100);
         	w_size=write(fd2,buf,r_size);
    	 }

    	 unlink(f1);
	 exit(0);
    }else if(c_pid>0){
    	if((pid=wait(&status))<0){
		perror("wait");
		_exit(1);
	}
    }else{
    	perror("fork failed\n");
    }

}

void copy(char f1[], char f2[]){
    int fd1,fd2;
    int r_size,w_size;
    int status;
    pid_t c_pid,pid;
    c_pid=fork();

    if(c_pid==0){
         char buf[100];
         fd1 = open(f1, O_RDONLY);
         fd2 = open(f2, O_RDWR|O_CREAT|O_EXCL, 0664);

         r_size = read(fd1,buf,100);
         w_size = write(f:d2,buf,r_size);
         while(r_size == 100)
         {
                r_size=read(fd1,buf,100);
                w_size=write(fd2,buf,r_size);
         }

	 exit(0);
    }else if(c_pid>0){
        if((pid=wait(&status))<0){
                perror("wait");
		_exit(1);
        }
    }else{
        perror("fork failed\n");
    }

}


/**
 * shell�� ���� ȸ���� �μ��ϴ� �� ���Ǵ� method
 */
void welcomeScreen(){
        printf("\n\t============================================\n");
        printf("\t               Simple C Shell\n");
        printf("\t--------------------------------------------\n");
        printf("\t             Licensed under GPLv3:\n");
        printf("\t============================================\n");
	printf("\t             OpenSource Project\n");
        printf("\n\n");
}

/**
 * SIGNAL HANDLERS
 */

/**
 * signal handler for SIGCHLD
 */
void signalHandler_child(int p){
	/* ��� ���� ���μ����� ��ٸ���.
	 * ���α׷��� �ٸ� �κп��� child�� cleaned up �� ��� �� Signal handler��
	 * ���ܵ��� �ʴ��� Ȯ���ϱ� ���� non-blocking call(WNOHANG)�� ����Ѵ�.
	 */
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
	printf("\n");
}

/**
 * Signal handler for SIGINT
 */
void signalHandler_int(int p){
	// We send a SIGTERM signal to the child process
	if (kill(pid,SIGTERM) == 0){
		printf("\nProcess %d received a SIGINT signal\n",pid);
		no_reprint_prmpt = 1;			
	}else{
		printf("\n");
	}
}

/**
 *	shell prompt�� ǥ���Ѵ�.
 */
void shellPrompt(){
	// prompt ��"<user>@<host> <cwd> >"�������� �μ��Ѵ�.
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("\n%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

/**
 * ���丮 ���� Method
 */
int changeDirectory(char* args[]){
	// ���(path)�� ���� ������('cd'�� �ش�) Ȩ ���丮�� �̵�
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	/* �ƴϸ�..
	 * �����ϴٸ� �츮�� ���丮�� argument�� ���� ������ ���丮�� �ٲ۴�.
	 */
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
            return -1;
		}
	}
	return 0;
}

/**
 * �پ��� �ɼ��� ����Ͽ� ȯ�� ������ �����ϴ� �� ���Ǵ�  Method
 */ 
int manageEnviron(char * args[], int option){
	char **env_aux;
	switch(option){
		// Case 'environ': ȯ�溯���� �׵��� values�� �Բ� print
		case 0: 
			for(env_aux = environ; *env_aux != 0; env_aux ++){
				printf("%s\n", *env_aux);
			}
			break;
		// Case 'setenv': ȯ�溯���� value�� ����
		case 1: 
			if((args[1] == NULL) && args[2] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			
			// ���ο� ������ ���� �� ������ �ٸ� output ����Ѵ�.
			if(getenv(args[1]) != NULL){
				printf("%s", "The variable has been overwritten\n");
			}else{
				printf("%s", "The variable has been created\n");
			}
			
			// ������ value�� �������� ���� ��""�� �����Ѵ�.
			if (args[2] == NULL){
				setenv(args[1], "", 1);
			// �־��� value�� ������ �����Ѵ�.
			}else{
				setenv(args[1], args[2], 1);
			}
			break;
		// Case 'unsetenv': ȯ�� ������ �����Ѵ�.
		case 2:
			if(args[1] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			if(getenv(args[1]) != NULL){
				unsetenv(args[1]);
				printf("%s", "The variable has been erased\n");
			}else{
				printf("%s", "The variable does not exist\n");
			}
		break;
			
			
	}
	return 0;
}
 
/**
* ���α׷� ����  method��, background�� foreground���� ����� �� �ִ�.
*/ 
void launchProg(char **args, int background){	 
	 int err = -1;
	 
	 if((pid=fork())==-1){
		 printf("Child process could not be created\n");
		 return;
	 }
	 // pid == 0 �� ���� �ڵ尡 child ���μ����� ������ ������ ��Ÿ����.
	if(pid==0){
		/*child�� 'SIGINT' signals�� �����ϵ��� �����ߴ�.
		 * (parent ���μ����� �׵��� signalHandler_int�� ó���ϱ⸦ ���Ѵ�.)
		 */
		signal(SIGINT, SIG_IGN);
		
		// child�� ����  parent=<pathname>/simple-c-shell�� ȯ�溯���� ����.
		setenv("parent",getcwd(currentDirectory, 1024),1);	
		
		// �������� ���� command ���� �� ���μ����� �����.
		if (execvp(args[0],args)==err){
			printf("Command not found");
			kill(getpid(),SIGTERM);
		}
	 }
	 
	 // parent�� ������ �����Ѵ�.
	 
	 // ���μ����� background���� ��óû���� ���� ��� child�� �����⸦ ��ٸ���.
	 if (background == 0){
		 waitpid(pid,NULL,0);
	 }else{
		 /* background ���μ����� �����Ϸ��� ���� ���μ����� ����ϱ� ����
		  * ȣ���� �ǳʶپ�� �Ѵ�. SIGCHILD handler�� 'signalHandler-child'��
		  * child�� return value�� ó���Ѵ�.
		  */
		 printf("Process created with PID: %d\n",pid);
	 }	 
}
 
/**
* I/O redirection�� ���Ǵ� method
*/ 
void fileIO(char * args[], char* inputFile, char* outputFile, int option){
	 
	int err = -1;
	
	int fileDescriptor; // 0~19 ����, output �Ǵ� input���� ����.
	
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){
		// Option 0: output redirection
		if (option == 0){
			// ������ 0���� truncating�� ���� �������� ����(create)
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			// ǥ�� output�� ������ ���Ϸ� �ٲ۴�.
			dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		// Option 1: input and output redirection
		}else if (option == 1){
			// ������ �б� �������� ���� (it's STDIN).
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			// ǥ�� input�� ������ ���Ϸ� �ٲ۴�.
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			// output ���ϰ� ������.
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);		 
		}
		 
		setenv("parent",getcwd(currentDirectory, 1024),1);
		
		if (execvp(args[0],args)==err){
			printf("err");
			kill(getpid(),SIGTERM);
		}		 
	}
	waitpid(pid,NULL,0);
}

/**
* ������ ������ ���Ǵ� method
*/ 
void pipeHandler(char * args[]){
	// File descriptors
	int filedes[2]; // pos. 0 output, pos. 1 input of the pipe
	int filedes2[2];
	
	int num_cmds = 0;
	
	char *command[256];
	
	pid_t pid;
	
	int err = -1;
	int end = 0;
	
	// �ٸ� ������ ���Ǵ� ������
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	
	// ���� command ���� ����Ѵ�( command����'|' �� ���еǾ��ִ�).
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;
	
	/*�� method�� ���� ����.
	 * '|'������ �� command�� ���� �������� �����ǰ� ǥ�� input, output�� ��ü�ȴ�.
	 * �׷� ���� �̰��� ���۵� ���̴�.
	 */
	while (args[j] != NULL && end != 1){
		k = 0;
		// �� �ݺ�(iteration) ���� ����� command�� �����ϱ� ���� ���� ������ �迭�� ���
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){
				// �� �̻��� arguments�� ã�� �� ���� ��, ���α׷��� ��������
				// �ٽ� �Էµ����ʵ��� �ϴ� 'end'����
				end = 1;
				k++;
				break;
			}
			k++;
		}
		// command�� ������ ��ġ��NULL�̸�, exec�Լ��� ������ �� ������ ��Ÿ����.
		command[k] = NULL;
		j++;		
		
		/*�ݺ� ������ �ƴ����� ���� ������ input, output�� ���� ���� �ٸ� ��ũ���͸�
		 * ������ ���̴�. �̷��� �ϸ� �������� �� �� �ݺ�(iteration) ���̿� �����Ǿ�
		 * ���� �ٸ� �� command�� input�� output�� ���� �� �� �ְ� �ȴ�.
		 */
		if (i % 2 != 0){
			pipe(filedes); // for odd i
		}else{
			pipe(filedes2); // for even i
		}
		
		pid=fork();
		
		if(pid==-1){			
			if (i != num_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); // for odd i
				}else{
					close(filedes2[1]); // for even i
				} 
			}			
			printf("Child process could not be created\n");
			return;
		}
		if(pid==0){
			// �츮�� first command�� �ִٸ�..
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			/*���� �츮�� ������ command�� �ִٸ�, �װ��� odd���� even������ ����,
			 * �츮�� �� ������ �Ǵ� �ٸ� �������� ���� ǥ�� input�� ��ü�� ���̴�.
			 * �͹̳ο��� output�� ���� �ͱ� ������ ǥ�� output�� �մ��� ���� ���̴�.
			 */
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){ // for odd number of commands
					dup2(filedes[0],STDIN_FILENO);
				}else{ // for even number of commands
					dup2(filedes2[0],STDIN_FILENO);
				}
			/*�߰��� �ִ� command��� input�� �������� output�� �������� ���� 2����
			 * ����ؾ� �� ���̴�. �� input/output�� �ش��ϴ� ���� ��ũ���͸� ����
			 * �Ϸ��� ��ġ�� �߿��ϴ�.
			 */
			}else{ // for odd i
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO); 
					dup2(filedes[1],STDOUT_FILENO);
				}else{ // for even i
					dup2(filedes[0],STDIN_FILENO); 
					dup2(filedes2[1],STDOUT_FILENO);					
				} 
			}
			
			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}		
		}
				
		// PARENT�� ������ �ݱ�
		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1){
			if (num_cmds % 2 != 0){					
				close(filedes[0]);
			}else{					
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){					
				close(filedes2[0]);
				close(filedes[1]);
			}else{					
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}
			
/**
* ǥ�� input 'via'�� �Է�(entered)�� command�� ó��(handle)�ϴ� �� ���Ǵ� method
*/ 
int commandHandler(int argc,char * args[]){
	int i = 0;
	int j = 0;
	
	int fileDescriptor;
	int standardOut;
	
	int aux;
	int background = 0;
	
	char *args_aux[256];
	char ans='\0';

	// �츮��special characters �� ã�� arguments�� ���� ���ο� �迭�� command ��ü�� �и�.
	while ( args[j] != NULL){
		if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
			break;
		}
		args_aux[j] = args[j];
		j++;
	}
	
	// 'exit' command�� shell�� �����Ѵ�.
	if(strcmp(args[0],"exit") == 0) exit(0);

	//Easter Egg : input team or Team show Open Source Team Project Member
	//2019.11.05
	else if (strcmp(args[0],"Team")==0||strcmp(args[0],"team")==0){
		printf("\t====================\n");
		printf("\tB589077 Jin-Wook CHOI\n");
		printf("\t----------------------\n");
		printf("\tB589075 Soo-Hyuck CHOI\n");
		printf("\t=======================\n");
	}
	//2019.12.06
	//'rm' command
	else if(strcmp(args[0],"rm")==0){
		if(argc==1){
			printf(" \x1b[31m Not Select file");
			printf(" \x1b[0m \n");
		}else{
			printf("\x1b[31m Are U sure delete File? [y/n] : ");
			printf("\x1b[0m");
			scanf("%c",&ans);
			if(ans=='y'){
				int result=unlink(args[1]);
				if(result==0){
					printf("\x1b[31m \nSucces File Remove");
					printf("\x1b[0m \n");
					ls(NULL,NULL);
				}else printf("\x1b[31m Can't remove Dir");
				      printf("\x1b[0m \n");
			}
		}
	}

	//2019.12.07 
	//'rmdir' command
	else if(strcmp(args[0],"rmdir")==0){
		if(argc==1){
			printf("Not Select Directory\n");
		}else{ 
			
			printf("Are U sure delete Dir? [y/n] : ");
			scanf("%c",&ans);
			if(ans=='y'){
				remove(args[1]);
				printf("Succes Dir Remove\n");
				ls(NULL,NULL);
			}
		}
	}
	//2019.12.07
	//'mv' command
	else if(strcmp(args[0],"mv")==0){
		char *token[2];
		if (argc<3){
			printf("Not Move File\n");
		}
		else{
			move(args[1],args[2]);
		}	
	}
	//2019.12.08
	//'ls'command
	
	else if(strcmp(args[0],"ls")==0){
		if(argc==1){
			ls(NULL,NULL);
		}else if (argc==2){
			ls(args[1],NULL);
		}else{
			ls(args[1],args[2]);
		}
	}

	//2019.12.08
        //'mkdir' command
        else if(strcmp(args[0],"mkdir")==0){
                if(argc<2){
                        printf("\x1b[31m Enter a directory name");
                        printf("\x1b[0m \n");
                }else{

                        printf("\x1b[31m Do you want to change the directory?[Y/N] \x1b[0m");
                        scanf("%c",&ans);

                        if(ans=='n'){
                                mkdir(args[1],0755);
                                printf("Success mkdir\n");
                        }else
                        {
                                mkdir(args[1],0755);
                                changeDirectory(args);
                                printf("Success mkdir & cd");
                        }
                }
        }
	//2019.12.08
	//'cp'command
	else if(strcmp(args[0],"cp")==0){
		copy(args[1],args[2]);
	}
	// 'pwd' command�� ���� ���丮�� print.
 	else if (strcmp(args[0],"pwd") == 0){
		if (args[j] != NULL){
			// file output�� ���Ѵٸ�.
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// ǥ�� output�� ������ ���Ϸ� �ٲ۴�/
				standardOut = dup(STDOUT_FILENO); 	// ����, stdout�� �纻�� �����.
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				printf("%s\n", getcwd(currentDirectory, 1024));
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			printf("%s\n", getcwd(currentDirectory, 1024));
		}
	} 
 	// 'clear' command �� ȭ���� �����.
	else if (strcmp(args[0],"clear") == 0) system("clear");

	// 'cd' command �� ���丮�� �����Ѵ�.
	else if (strcmp(args[0],"cd") == 0) changeDirectory(args);

	// 'environ' command �� ȯ�� ������ �����Ѵ�.
	else if (strcmp(args[0],"environ") == 0){
		if (args[j] != NULL){
			// file output�� ���Ѵٸ�.
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// ǥ�� output�� ������ ���Ϸ� �ٲ۴�.
				standardOut = dup(STDOUT_FILENO); 	// ���� stdout�� �纻�� �����.
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				manageEnviron(args,0);
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			manageEnviron(args,0);
		}
	}
	// 'setenv' command �� ȯ�� ������ �����Ѵ�.
	else if (strcmp(args[0],"setenv") == 0) manageEnviron(args,1);

	// 'unsetenv' command �� ȯ�� ������ �������� �ʴ´�.
	else if (strcmp(args[0],"unsetenv") == 0) manageEnviron(args,2);
	else{
		// ���� command�� �ϳ��� ������ ���� ���, ������ ���α׷��� ȣ���Ѵ�.
		// I/O ���𷢼�, ������ ���� �Ǵ� background ������ ��û�Ǿ������� 
		// �����ؾ� �Ѵ�.
		while (args[i] != NULL && background == 0){
			// background ������ ��û�ϸ� (last argument'&') ������ �����Ѵ�.
			if (strcmp(args[i],"&") == 0){
				background = 1;
			// '|' �� ��� pipe�� �����Ǿ� �ٸ� ������ ó���� �� �ִ� ������
			// ����� call�Ѵ�.
			}else if (strcmp(args[i],"|") == 0){
				pipeHandler(args);
				return 1;
			// '<'�� �����Ǹ� I/O ���𷢼��� �ִµ�, ���� �־��� structure��
			// �ùٸ� structure���� Ȯ���ϰ�, �׷��ٸ� ������ method�� call�Ѵ�.
			}else if (strcmp(args[i],"<") == 0){
				aux = i+1;
				if (args[aux] == NULL || args[aux+1] == NULL || args[aux+2] == NULL ){
					printf("Not enough input arguments\n");
					return -1;
				}else{
					if (strcmp(args[aux+1],">") != 0){
						printf("Usage: Expected '>' and found %s\n",args[aux+1]);
						return -2;
					}
				}
				fileIO(args_aux,args[i+1],args[i+3],1);
				return 1;
			}
			// '<'�� �����Ǹ� output ���𷢼��� �ִµ�, ���� �־��� structure��
                        // �ùٸ� structure���� Ȯ���ϰ�, �׷��ٸ� ������ method�� call�Ѵ�.

			else if (strcmp(args[i],">") == 0){
				if (args[i+1] == NULL){
					printf("Not enough input arguments\n");
					return -1;
				}
				fileIO(args_aux,NULL,args[i+1],0);
				return 1;
			}
			i++;
		}
		// method�� ����Ͽ� background ���� ���θ� ��Ÿ��� ���α׷� ����
		args_aux[i] = NULL;
		launchProg(args_aux,background);
		
		/**
		 * part 1.e�� ���, 'exit', 'pwd'�Ǵ� 'clear'�� �ƴ� input�� print�ϸ� �Ǿ���.
		 * ������ ���� ������� �̸� �����Ͽ���.
		 */
		//	i = 0;
		//	while(args[i]!=NULL){
		//		printf("%s\n", args[i]);
		//		i++;
		//	}
	}
return 1;
}


/**
* shell�� ���� method
*/ 
int main(int argc, char *argv[], char ** envp) {
	char line[MAXLINE]; // user input�� ���� ����
	char * tokens[LIMIT]; // command���� �ٸ� ��ū�� ���� �迭
	int numTokens;
		
	no_reprint_prmpt = 0; 	// shell�� print���� �ʵ���
							// Ư�� method ����
	pid = -10; // �Ұ����� pid�� pid�� �ʱ�ȭ�Ѵ�.
	
	// �ʱ�ȭ method�� ���� ȭ�� ȣ��
	init();
	welcomeScreen();
    
    // extern char** environ�� environment�� �����Ͽ�
    // ���߿� �ٸ� method�� ó�� �� �� �ִ�.
	environ = envp;
	
	// shell=<pathname>/simple-c-shell �� child�� ȯ�� ������ �����ߴ�.
	setenv("shell",getcwd(currentDirectory, 1024),1);
	
	// user�� input�� �а� prompt�� print�Ǵ� ���� ����
	while(TRUE){
		// �ʿ��� ��� shell prompt�� print
		if (no_reprint_prmpt == 0) shellPrompt();
		no_reprint_prmpt = 0;
		
		// ���� ���۸� ����.
		memset ( line, '\0', MAXLINE );

		// user input�� ��ٸ���.
		fgets(line, MAXLINE, stdin);
	
		// �ƹ��͵� ���� ������, ������ �ٽ� ��
		if((tokens[0] = strtok(line," \n\t")) == NULL) continue;
		
		// input�� ��� ��ū�� �а� �װ��� commandHandler���� argument�� �����Ѵ�.
		numTokens = 1;
		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL){ 
			numTokens++;}
		
//		printf("%d\n",numTokens);
		commandHandler(numTokens,tokens);
		
	}          

	exit(0);
}

