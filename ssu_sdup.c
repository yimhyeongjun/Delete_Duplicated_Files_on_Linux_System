#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFMAX 5000
#define ARGMAX 6

void fork_exec(char * command[]);
void command_exit();
int split(char* string, char* seperator, char * argv[]);


int main(){
	int argc=0;
	char input[BUFMAX];
	char * argv[ARGMAX];
	char* argv_fmd5[ARGMAX];
	char* argv_sha1[ARGMAX];
	char* argv_help[2];

	// 프롬프트 
	while(1){
		printf("20182649> "); // 프롬프트 시작
		fgets(input, sizeof(input), stdin); // 명령어 입력
		input[strlen(input)-1] = '\0'; // 문자열 처리
		argc = split(input, " ", argv); // 공백 기준으로 문자열을 argv에 저장하고 토큰 개수를 argc에 저장
		
		if(argc == 0) // 아무것도 입력되지 않았을 때 프롬프트 창 재출력
			continue;

		// 프롬프트 창에 fmd5 로 시작하는 명령어가 입력될 시 ./ssu_find-fmd5를 fork execv하여 실행
		if(!strcmp(argv[0], "fmd5")){
			argv_fmd5[0] = "./ssu_find-fmd5";
			for(int i=1; i<argc; i++){
				argv_fmd5[i] = argv[i];
			}

			argv_fmd5[argc] = (char *)0;

			fork_exec(argv_fmd5); // fork와 exec을 해주는 함수
		} 

		// 프롬프트 창에 sha1 로 시작하는 명령어가 입력될 시 ./ssu_find-sha1를 fork execv하여 실행
		else if(!strcmp(argv[0] , "sha1")){
			argv_sha1[0] = "./ssu_find-sha1";
			for(int i=1; i<argc; i++){
				argv_sha1[i] = argv[i];
			}

			argv_sha1[argc] = (char *)0;

			fork_exec(argv_sha1);
		}

		else if(!strcmp(argv[0], "exit"))
			command_exit();

		// 프롬프트 창에 위 명령어 외의 문자열 로 시작하는 명령어가 입력될 시 ./ssu_help를 fork execv하여 실행
		else{
			argv_help[0] = "./ssu_help";
			argv_help[1] = (char *)0;
			fork_exec(argv_help);
		}


	}

}

// command 배열을 인자로 받아 command[0]에 입력된 파일을 fork하여 execv시켜주는 함수
void fork_exec(char* command[]){
	int status;
	pid_t pid;
	pid = fork(); // fork

	wait(&status); // 부모 프로세스가 자식 프로세스의 종료를 기다림
	if(pid == 0){ // 자식 프로세스 실행 시
		if(execv(command[0], command) < 0){ // command[0]에 담긴 파일명을 실행
			fprintf(stderr, "execv error\n");
			exit(1);
		}
	}else if(pid < 0){
		printf("프로세스 생성 실패\n");
		exit(1);
	}else ;
}


// exit 명령어
void command_exit(){
	printf("Prompt End\n");
	exit(0);
}

// string을 seperator기준으로 토큰화하여 argv[]에 토큰 별로 저장
int split(char *string, char *seperator, char *argv[]){
	int argc = 0;
	char *ptr = NULL;

	ptr = strtok(string, seperator);
	while(ptr != NULL){
		argv[argc++] = ptr;
		ptr = strtok(NULL, " ");
	}

	return argc;
}
