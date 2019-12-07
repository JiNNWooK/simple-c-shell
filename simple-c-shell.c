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

#define LIMIT 256 // command에 대한 최대 토큰 수.
#define MAXLINE 1024 // user input의 최대 문자 수.


/**
 * shell을 초기화하는 데 사용되며, 설명 된 접근법을 사용했다.
 * http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
void init(){
		// interactive로 실행 중인지 확인
        GBSH_PID = getpid();
        // STDIN이 터미널인 경우 shell은 interactive다.
        GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);  

		if (GBSH_IS_INTERACTIVE) {
			// foreground까지 반복
			while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp()))
					kill(GBSH_PID, SIGTTIN);             
	              
	              
	        // SIGCHILD 와 SIGINT에 대한 시그널 핸들러 설정
			act_child.sa_handler = signalHandler_child;
			act_int.sa_handler = signalHandler_int;			
			
			/**sigaction의 구조는 다음과 같다.
			
			struct sigaction {
				void (*sa_handler)(int);
				void (*sa_sigaction)(int, siginfo_t *, void *);
				sigset_t sa_mask;
				int sa_flags;
				void (*sa_restorer)(void);
				
			}*/
			
			sigaction(SIGCHLD, &act_child, 0);
			sigaction(SIGINT, &act_int, 0);
			
			/* 자체 프로세스 그훕에 참여
			 * shell 프로세스를 새로운 프로세스 스룹 리더로 만든다
			 */
			setpgid(GBSH_PID, GBSH_PID);
			GBSH_PGID = getpgrp();
			if (GBSH_PID != GBSH_PGID) {
					printf("Error, the shell is not process group leader");
					exit(EXIT_FAILURE);
			}
			// 터미널 제어
			tcsetpgrp(STDIN_FILENO, GBSH_PGID);  
			
			// shell의 기본 터미널 속성 저장
			tcgetattr(STDIN_FILENO, &GBSH_TMODES);

			// 다른methods로 사용될 현재 디랙토리를 얻는다.
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
 * shell의 시작 회면을 인쇄하는 데 사용되는 method
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
	/* 모든 죽은 프로세스를 기다린다.
	 * 프로그램의 다른 부분에서 child를 cleaned up 할 결우 이 Signal handler가
	 * 차단되지 않는지 확인하기 위해 non-blocking call(WNOHANG)을 사용한다.
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
 *	shell prompt를 표시한다.
 */
void shellPrompt(){
	// prompt 를"<user>@<host> <cwd> >"형식으로 인쇄한다.
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("\n%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

/**
 * 디랙토리 변경 Method
 */
int changeDirectory(char* args[]){
	// 경로(path)를 쓰지 않으면('cd'만 해당) 홈 디랙토리로 이동
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	/* 아니면..
	 * 가능하다면 우리는 디랙토리를 argument에 의해 지정된 디렉토리로 바꾼다.
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
 * 다양한 옵션을 사용하여 환경 변수를 관리하는 데 사용되는  Method
 */ 
int manageEnviron(char * args[], int option){
	char **env_aux;
	switch(option){
		// Case 'environ': 환경변수와 그들의 values를 함께 print
		case 0: 
			for(env_aux = environ; *env_aux != 0; env_aux ++){
				printf("%s\n", *env_aux);
			}
			break;
		// Case 'setenv': 환경변수를 value로 설정
		case 1: 
			if((args[1] == NULL) && args[2] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			
			// 새로운 변수와 덮어 쓴 변수에 다른 output 사용한다.
			if(getenv(args[1]) != NULL){
				printf("%s", "The variable has been overwritten\n");
			}else{
				printf("%s", "The variable has been created\n");
			}
			
			// 변수에 value을 지정하지 않으 면""로 설정한다.
			if (args[2] == NULL){
				setenv(args[1], "", 1);
			// 주어진 value로 변수를 설정한다.
			}else{
				setenv(args[1], args[2], 1);
			}
			break;
		// Case 'unsetenv': 환경 변수를 삭제한다.
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
* 프로그램 시작  method로, background나 foreground에서 실행될 수 있다.
*/ 
void launchProg(char **args, int background){	 
	 int err = -1;
	 
	 if((pid=fork())==-1){
		 printf("Child process could not be created\n");
		 return;
	 }
	 // pid == 0 은 다음 코드가 child 프로세스와 관련이 있음을 나타낸다.
	if(pid==0){
		/*child가 'SIGINT' signals를 무시하도록 설정했다.
		 * (parent 프로세스가 그들을 signalHandler_int로 처리하기를 원한다.)
		 */
		signal(SIGINT, SIG_IGN);
		
		// child를 위해  parent=<pathname>/simple-c-shell를 환경변수로 설정.
		setenv("parent",getcwd(currentDirectory, 1024),1);	
		
		// 존재하지 않은 command 실행 시 프로세스가 종료됨.
		if (execvp(args[0],args)==err){
			printf("Command not found");
			kill(getpid(),SIGTERM);
		}
	 }
	 
	 // parent가 다음을 실행한다.
	 
	 // 프로세스가 background에서 요처청되지 않은 경우 child가 끝나기를 기다린다.
	 if (background == 0){
		 waitpid(pid,NULL,0);
	 }else{
		 /* background 프로세스를 생성하려면 현재 프로세스는 대기하기 위해
		  * 호출을 건너뛰어야 한다. SIGCHILD handler인 'signalHandler-child'는
		  * child의 return value를 처리한다.
		  */
		 printf("Process created with PID: %d\n",pid);
	 }	 
}
 
/**
* I/O redirection에 사용되는 method
*/ 
void fileIO(char * args[], char* inputFile, char* outputFile, int option){
	 
	int err = -1;
	
	int fileDescriptor; // 0~19 사이, output 또는 input파일 설명.
	
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){
		// Option 0: output redirection
		if (option == 0){
			// 파일을 0에서 truncating해 쓰기 전용으로 열기(create)
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			// 표준 output을 적절한 파일로 바꾼다.
			dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		// Option 1: input and output redirection
		}else if (option == 1){
			// 파일을 읽기 전용으로 연다 (it's STDIN).
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			// 표준 input을 적절한 파일로 바꾼다.
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			// output 파일과 동일함.
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
* 파이프 관리에 사용되는 method
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
	
	// 다른 루프에 사용되는 변수들
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	
	// 먼저 command 수를 계산한다( command들은'|' 로 구분되어있다).
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;
	
	/*이 method의 메인 루프.
	 * '|'사이의 각 command에 대해 파이프가 구성되고 표준 input, output이 교체된다.
	 * 그런 다음 이것은 실핼될 것이다.
	 */
	while (args[j] != NULL && end != 1){
		k = 0;
		// 각 반복(iteration) 에서 실행될 command를 저장하기 위해 보조 포인터 배열을 사용
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){
				// 더 이상의 arguments를 찾을 수 없을 떄, 프로그램이 루프에서
				// 다시 입력되지않도록 하는 'end'변수
				end = 1;
				k++;
				break;
			}
			k++;
		}
		// command의 마지막 위치는NULL이며, exec함수에 전달할 때 끝임을 나타낸다.
		command[k] = NULL;
		j++;		
		
		/*반복 중인지 아닌지에 따라 파이프 input, output에 대해 서로 다른 디스크립터를
		 * 설정할 것이다. 이렇게 하면 하이프가 각 두 반복(iteration) 사이에 공유되어
		 * 서로 다른 두 command의 input과 output을 연결 할 수 있게 된다.
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
			// 우리가 first command에 있다면..
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			/*만약 우리가 마지막 command에 있다면, 그것이 odd인지 even인지에 따라,
			 * 우리는 한 파이프 또는 다른 파이프에 대한 표준 input을 교체할 것이다.
			 * 터미널에서 output을 보고 싶기 때문에 표준 output은 손대지 않을 것이다.
			 */
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){ // for odd number of commands
					dup2(filedes[0],STDIN_FILENO);
				}else{ // for even number of commands
					dup2(filedes2[0],STDIN_FILENO);
				}
			/*중간에 있는 command라면 input용 파이프와 output용 파이프를 각각 2개씩
			 * 사용해야 할 것이다. 각 input/output에 해당하는 파일 디스크립터를 선택
			 * 하려면 위치도 중요하다.
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
				
		// PARENT의 설명자 닫기
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
* 표준 input 'via'로 입력(entered)된 command를 처리(handle)하는 데 사용되는 method
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

	// 우리는special characters 를 찾고 arguments를 위해 새로운 배열로 command 자체를 분리.
	while ( args[j] != NULL){
		if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
			break;
		}
		args_aux[j] = args[j];
		j++;
	}
	
	// 'exit' command는 shell을 종료한다.
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
	// 'pwd' command는 현재 디랙토리를 print.
 	else if (strcmp(args[0],"pwd") == 0){
		if (args[j] != NULL){
			// file output을 원한다면.
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// 표준 output을 적절한 파일로 바꾼다/
				standardOut = dup(STDOUT_FILENO); 	// 먼저, stdout의 사본을 만든다.
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
 	// 'clear' command 는 화면을 지운다.
	else if (strcmp(args[0],"clear") == 0) system("clear");

	// 'cd' command 는 디랙토리를 변경한다.
	else if (strcmp(args[0],"cd") == 0) changeDirectory(args);

	// 'environ' command 는 환경 변수를 나열한다.
	else if (strcmp(args[0],"environ") == 0){
		if (args[j] != NULL){
			// file output을 원한다면.
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// 표준 output을 적절한 파일로 바꾼다.
				standardOut = dup(STDOUT_FILENO); 	// 먼저 stdout의 사본을 만든다.
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
	// 'setenv' command 는 환경 변수를 설정한다.
	else if (strcmp(args[0],"setenv") == 0) manageEnviron(args,1);

	// 'unsetenv' command 는 환경 변수를 정의하지 않는다.
	else if (strcmp(args[0],"unsetenv") == 0) manageEnviron(args,2);
	else{
		// 앞의 command가 하나도 사용되지 않은 경우, 지정된 프로그램을 호출한다.
		// I/O 리디랙션, 파이프 실행 또는 background 실행이 요청되었는지를 
		// 감지해야 한다.
		while (args[i] != NULL && background == 0){
			// background 실행을 요청하면 (last argument'&') 루프를 종료한다.
			if (strcmp(args[i],"&") == 0){
				background = 1;
			// '|' 인 경우 pipe가 감지되어 다른 실행을 처리할 수 있는 적절한
			// 방법을 call한다.
			}else if (strcmp(args[i],"|") == 0){
				pipeHandler(args);
				return 1;
			// '<'가 감지되면 I/O 리디랙션이 있는데, 먼저 주어진 structure가
			// 올바른 structure인지 확인하고, 그렇다면 적절한 method를 call한다.
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
			// '<'가 감지되면 output 리디랙션이 있는데, 먼저 주어진 structure가
                        // 올바른 structure인지 확인하고, 그렇다면 적절한 method를 call한다.

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
		// method를 사용하여 background 실행 여부를 나타배는 프로그램 실행
		args_aux[i] = NULL;
		launchProg(args_aux,background);
		
		/**
		 * part 1.e의 경우, 'exit', 'pwd'또는 'clear'가 아닌 input만 print하면 되었다.
		 * 다음과 같은 방법으로 이를 구현하였다.
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
* shell의 메인 method
*/ 
int main(int argc, char *argv[], char ** envp) {
	char line[MAXLINE]; // user input을 위한 버퍼
	char * tokens[LIMIT]; // command에서 다른 토큰에 대한 배열
	int numTokens;
		
	no_reprint_prmpt = 0; 	// shell이 print되지 않도록
							// 특정 method 이후
	pid = -10; // 불가능한 pid로 pid를 초기화한다.
	
	// 초기화 method와 시작 화면 호출
	init();
	welcomeScreen();
    
    // extern char** environ을 environment에 설정하여
    // 나중에 다른 method로 처리 할 수 있다.
	environ = envp;
	
	// shell=<pathname>/simple-c-shell 을 child의 환경 변수로 설정했다.
	setenv("shell",getcwd(currentDirectory, 1024),1);
	
	// user의 input을 읽고 prompt가 print되는 메인 루프
	while(TRUE){
		// 필요한 경우 shell prompt를 print
		if (no_reprint_prmpt == 0) shellPrompt();
		no_reprint_prmpt = 0;
		
		// 라인 버퍼를 비운다.
		memset ( line, '\0', MAXLINE );

		// user input을 기다린다.
		fgets(line, MAXLINE, stdin);
	
		// 아무것도 쓰지 않으면, 루프가 다시 실
		if((tokens[0] = strtok(line," \n\t")) == NULL) continue;
		
		// input의 모든 토큰을 읽고 그것을 commandHandler에게 argument로 전달한다.
		numTokens = 1;
		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL){ 
			numTokens++;}
		
//		printf("%d\n",numTokens);
		commandHandler(numTokens,tokens);
		
	}          

	exit(0);
}

