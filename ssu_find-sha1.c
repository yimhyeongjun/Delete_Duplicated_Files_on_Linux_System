#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <openssl/sha.h>
#include <errno.h>
#include "datastruct.h"


#define INF 100000000000000
#define BUFSIZE 1024*16

char * fileExtension(char *rawExtension); // 파일 확장자를 통일해주는 함수
long fileSize(char *rawSize, char* kind); // 파일 크기를 바이트로 환산해주는 함수
char *targetPath(char *pathname); // 파일 경로를 절대경로로 변환해주는 함수
char getType(struct stat statbuf); // 파일의 확장자를 검사하여 리턴해주는 함수
void bfs(char *dir_path, int depth); // bfs로 탐색하고 리스트 생성해주는 함수
char* get_time(time_t stime); // time_t 타입의 time을 문자열로 변환해주는 함수
int setSort(long p1, long p2); // set_list 정렬 조건 함수
int dupfSort(int l1, int l2, char* p1, char *p2); // dupf_list 정렬 조건 함수
void printList(char * target_path, struct timeval end_t, struct timeval start_t); // 중복 파일 리스트 출력 함수
const char* transtoSHA1(char* pathname, char* sha_buf); // pathname의 경로를 가진 파일의  MD5 파일을 추출해주는 함수
int split(char* string, char* seperator, char* argv[]); // split하는 함수
void option(); // 옵션 프롬프트 함수
void d_option(char* op_argv[]); // d 옵션 함수
void i_option(char* op_argv[]); // i 옵션 함수
void f_option(char* op_argv[]); // f 옵션 함수
void t_option(char* op_argv[]); // t 옵션 함수
void printOpSet();  // set 출력 함수
void printOnlySet(dupf_list *each_list, set* c, char* num); // 특정 set만 출력하는 함수

char file_extension[1000];
long min_size;
long max_size;
char* target_path;
set_list list;
queue dirQ;
int k=1;
int r=1;

int main(int argc, char* argv[]){

        struct timeval start_t, end_t;


        strcpy(file_extension, fileExtension(argv[1])); // 확장자 조건에 맞게 가공
        min_size = fileSize(argv[2], "min"); // min 조건에 맞게 변환
        max_size = fileSize(argv[3], "max"); // max 조건에 맞게 변환
        target_path = targetPath(argv[4]); // 절대경로 변환

        queueInit(&dirQ); // 큐 초기화
        setInit(&list, dupfSort); // set 초기화

        setSortRule(&list, setSort); // set_list 정렬 조건 설정

        gettimeofday(&start_t, NULL); // 탐색 시작 시간 측정
        bfs(target_path, 1); //bfs 탐색
        gettimeofday(&end_t, NULL); // 탐색 종료 시간 측정

        printList(target_path, end_t, start_t); // list 출력 

	// 옵션 
        while(1){
                option(); //옵션 프롬프트
        }

        exit(0);


}

// bfs 탐색 함수
void bfs(char *target_path, int depth){

        struct dirent **namelist; // 하위 디렉토리 검사 후 넣을 구조체
        struct stat statbuf; // 탐색할 디렉토리를 잠시 담을 statbuf
        int count; // 하위 디렉토리의 갯수
        char search_path[PATHLEN]; // 비교하고자 하는 디렉토리의 경로
        char search_sha1[sha1HASH]; // sha1 추출하여 저장할 배열
                                                                                                                                                                                         
	char queue_front[PATHLEN]; // queue의 가장 앞 부분 요소를 뽑아내어 담을 배열
        char sha_buf[sha1HASH]; 

	// 탐색할 경로에 접근해도 되는지 확인
        if(access(target_path, F_OK) != 0){
                fprintf(stderr, "access error for %s\n", target_path);
                exit(1);
        }

        // namlist에 target_path 하위 디렉토리 저장 및 에러처리
        if((count = scandir(target_path, &namelist, NULL, alphasort)) == -1){
                fprintf(stderr, "scandir error for %s\n", target_path);
                if(errno == 2){
			fprintf(stderr,"Not exist Directory\n");
			exit(1);
		}else if(errno == 12){
			fprintf(stderr, "out of memeory\n");
			exit(1);
		}else if(errno == 20){
			fprintf(stderr, "Not a Directory\n");
			exit(1);
		}else{
			printf("errno : %d\n",errno);
			exit(1);
		}
        }

        // 하위 디렉토리를 탐색
        for(int i = 0; i< count; i++){
                if(!strcmp(namelist[i]->d_name, ".")|| !strcmp(namelist[i]->d_name, "..") || !strcmp(namelist[i]->d_name, "proc")|| !strcmp(namelist[i]->d_name,"run")|| !strcmp(namelist[i]->d_name, "sys"))
                        continue;

                // targetPath가  /일 경우 다음 탐색할 경로 처리
                if(!strcmp(target_path, "/"))
                        sprintf(search_path, "%s%s", target_path,namelist[i]->d_name);
                else
                        sprintf(search_path, "%s/%s", target_path, namelist[i]->d_name);

                // 문제 없을 시 statbuf에 담음 -> 에러처리 
                if((lstat(search_path, &statbuf)<0) && (access(search_path, F_OK)!= 0)){
                        fprintf(stderr, "lstat error for %s\n", search_path);
                        exit(1);
                }

                // 디렉토리인지 검사 -> 디렉토리이고 크기가 min_size보다 크거나 같다면 queue에 삽입 
                if(getType(statbuf)=='d' && statbuf.st_size >= min_size){
                        Enqueue(&dirQ, search_path);
		//	printf("search / sub : %s, num : %d, r : %d\n", target_path, k,r);
			k++;	
			continue;
                }


                // 정규 파일인지 검사 -> 맞다면 file_extension 검사 -> size 검사 -> 같다면 sha 해쉬값 검사 
                if(getType(statbuf)=='r'){ // 정규 파일이라면 
			r++;
                        if(!strcmp(file_extension,"all")){ // 파일 확장자가 all이면 전부 탐색
                                if(statbuf.st_size >= min_size && statbuf.st_size <= max_size) // 파일의 크기가 주어진 범위 안에 속하면
                                {
                                        // sha1 추출하여 search_sha1에 저장
                                        strcpy(search_sha1,transtoSHA1(search_path,sha_buf));
					// 리스트에 삽입
		                        setInsert(&list, search_sha1, statbuf.st_size, search_path, get_time(statbuf.st_mtime), get_time(statbuf.st_atime), depth+1, dupfSort);

                                }
                        }else if(strrchr(search_path,'.')==NULL){
				;
			}else if(!strcmp(strrchr(search_path,'.'),file_extension)){ // 파일 확장자가 사용자가 입력한 파일 확장자와 같으면 
                                if(statbuf.st_size >= min_size && statbuf.st_size <= max_size){ // 파일의 크기가 주어진 범위 안에 속하면
                                        //sha1 추출하여 sha1_search에 저장        
                                        strcpy(search_sha1, transtoSHA1(search_path, sha_buf));
                                       // 리스트에 삽입
				       	setInsert(&list, search_sha1, statbuf.st_size,search_path, get_time(statbuf.st_mtime), get_time(statbuf.st_atime), depth+1, dupfSort);     // mtime, atime 문자열 버전
                                }
                        }else ;
			
                }

       	}


        // 하위 디렉토리 탐색 종료 후 queue에 담긴 것 하나씩 Dequeue하여 재귀
        while(!QisEmpty(&dirQ)) {
                strcpy(queue_front, Dequeue(&dirQ, search_path));

		depth = 0;
		// 경로의 '/' 개수를 파악하여 depth 계산
		for(int i=0; i<strlen(queue_front); i++)
			if(queue_front[i]=='/')
				depth++;
		// queue의 front를 Dequeue하여 재귀적을로 탐색
                bfs(queue_front, depth);
        }

}

// 리스트를 출력하는 함수 
void printList(char* pathname, struct timeval end_t, struct timeval start_t){

	int i=0;
	set* cursor = list.head->next;

	// 중복 파일이 존재하는 set 찾기
	while(cursor != NULL){
		if(cursor->exist_list == 1)
			i++;
		cursor = cursor->next;
	}

	// 중복 파일이 없다면 해당 문구와 탐색 시간 출력 후 명령 프롬프트 창으로 이동
        if(i==0){
                printf("No duplicates in %s\n",pathname);
                printf("Searching time: %ld:%06ld(sec:usec)\n\n", end_t.tv_sec - start_t.tv_sec, end_t.tv_usec - start_t.tv_usec);
                exit(0);
        }else{ // 중복 파일이 존재한다면 리스트 출력 함수 호출

       		 printOpSet();
       		 printf("Searching time: %ld:%06ld(sec:usec)\n\n", end_t.tv_sec - start_t.tv_sec, end_t.tv_usec - start_t.tv_usec);

	}

}


// 옵션 후 리스트를 출력해주는 함수
void printOpSet(){

        int n=1, i=1;
        set* c = list.head->next;
        dupf* f;
	
		// 중복 파일이 존재하는 set를 찾고
                while(c != NULL){
                        if(c->exist_list == 1){
                                printf(" ---- Identical files #%d (%ld bytes - %s) ----\n", n, c->size, c->hash);
                                f = c->each_list->head->next;
				i=1;
                                while(f!=NULL){ // 그 set의 중복 파일들을 출력
                                        printf(" [%d] %s (mtime : %s) (atime : %s)\n", i, f->pathname, f->mtime, f->atime);
                                        f = f->next;
					i++;
                       		 }
                                printf("\n");
                                n++;
                        }
                        c = c->next;
                }

		// 중복 리스트가 존재하지 않으면 시작 프롬프트로 이동
		if(n==1)
			exit(0);

		return ;
}

// 옵션 프롬프트 창 함수
void option(){

        int op_argc = 0; //option split 개수
        char * op_argv[4]; // option splot 저장
        char op_input[100]; // option 입력 명령어

        printf(">> "); // 옵션 프롬프트 출력
        fgets(op_input, sizeof(op_input), stdin); // 명령어 op_input에 저장
        op_input[strlen(op_input) - 1] = '\0';
        op_argc = split(op_input, " ", op_argv); // 입력받은 명령어를 공백 단위로 분리하여 op_argv 배열에 저장

        if(op_argc == 0)
                return;

	// exit 입력시 명령 프롬프트 창으로 이동
        if(!strcmp(op_input,"exit")){
                printf(">> Back to Prompt\n");
                exit(0);
        }// d 입력시 d_option 수행
        else if(!strcmp(op_argv[1],"d"))
                d_option(op_argv);
        else if(!strcmp(op_argv[1],"i")) // i 입력시 i 옵션 수행
                i_option(op_argv);
        else if(!strcmp(op_argv[1],"f")) // f 입력시 f 옵션 수행
                f_option(op_argv);
        else if(!strcmp(op_argv[1],"t")) // t 입력시 t 옵션 수행
                t_option(op_argv);
        else{ // 이외 입력시 예외 처리
                printf("Option input error\n");
                return ;
        }
}


// d 옵션 함수 
void d_option(char *op_argv[]){

        int i=0;
        set* cursor = list.head->next;
        dupf* files, * before;

        while(cursor != NULL){ // 입력 받은 set 번호에 커서 옮겨놓기
                if(cursor->exist_list == 1){ 
                        i++;
                        if(i == atoi(op_argv[0]))
                                break;
                }

                cursor = cursor->next;
        }

	// 해당하는 set 찾은 후 중복 파일 리스트를 순회할 find, before 초기화
        files = cursor->each_list->head->next;
        before = cursor->each_list->head;

	// 삭제할 파일 위치 찾기
        for(i=1; i<atoi(op_argv[2]); i++){
                before = files;
                files = files->next;
                                                                                                                                                                                         
        }

        printf("\"%s\" has been deleted in %d\n\n",files->pathname, atoi(op_argv[0]));
        dupfDelete(files->pathname, cursor->each_list, files, before); // 파일 삭제

	// 중복 파일 리스트에 파일이 하나 남는다면 exist_list를 0으로 수정
	if((cursor->each_list->numOfFile) == 1)
		cursor->exist_list = 0;

        // list 전체 출력
	printOpSet();
	printf("\n");
}

// i 옵션 함수
void i_option(char * op_argv[]){
	set* cursor = list.head->next; 
	dupf* files, *before;
	int i=0;
	char answer[1000];

	// 입력 받은 set 번호에 커서 옮겨두기
	while(cursor != NULL){
		if(cursor->exist_list == 1){
			i++;
			if(i == atoi(op_argv[0]))
				break;
		}

		cursor = cursor->next;
	}

	files = cursor->each_list->head->next;
	before = cursor->each_list->head;

	// 중복 파일 리스트들을 순차적으로 삭제 여부 검사
	while(files != NULL){
		printf("Delete \"%s\"? [y/n] ", files->pathname);
		fgets(answer, 1000, stdin);
		answer[strlen(answer)-1] = '\0';
		fflush(stdin);
		if(!strcmp(answer, "y")||!strcmp(answer, "Y")) // y 입력 시 삭제
			dupfDelete(files->pathname, cursor->each_list, files, before);
		else if(!strcmp(answer, "n")||!strcmp(answer, "N")) // n 입력 시 삭제하지 않음
			;
		else{ // 이외 입력시 에러 출력 후 옵션 프롬프트 창으로 이동
			printf("Please input character only y or n\n");
			return ;
		}

		before = files;
		files = files->next;
	}

	// 파일을 삭제하여 중복 파일 리스트의 파일 수가 1개가 되면 해당 set의 중복 파일 리스트 여부를 없음으로 바꿈
	if(cursor->each_list->numOfFile <= 1)
		cursor->exist_list = 0;

	// 리스트 출력
	printf("\n");
	printOpSet();

}


// f 옵션 함수
void f_option(char *op_argv[]){
        // 1 f -> 1 세트 내에서 mtime이 가장 최근인 값 빼고 삭제
        // 전부 같을 시 맨 위에꺼 빼고 다 삭제
        // set를 순회하면서 맨 위에꺼 보다 mtime이 작으면 삭제


        int i=0;
        set* cursor = list.head->next;
        dupf* files, *latest, *before;

	// 입력 받은 set 번호에 커서를 옮겨 놓음 
        while(cursor != NULL){
                if(cursor->exist_list == 1){
                        i++;
                        if(i == atoi(op_argv[0]))
                                break;
                }

                cursor = cursor->next;
        }

        latest = cursor->each_list->head->next; // mtime이 가장 최근인 파일 저장할 포인터
        before = cursor->each_list->head; // 삭제를 위해 files의 한 칸 뒤를 가리키는 커서
        files = latest->next; // 중복 파일 리스트를 순차적으로 읽을 파일 커서


	// 중복 파일 리스트의 끝까지 순차적으로 탐색
        while(files != NULL){
                if(strcmp(latest->mtime, files->mtime) < 0){ // latest의 mtime이 files의 mtime보다 오래됐을 경우
                        remove(latest->pathname); // latest가 가리키는 파일 삭제
                        before->next = files;
                        latest = files; // latest에 files가 가리키는 파일 저장
                        files = files->next;
                        cursor->each_list->numOfFile -=1;
                }else{ // latest mtime이  files 보다 최근일 경우 
                        remove(files->pathname); // files가 가리키는 파일 삭제
                        latest->next = files->next;
                        files = files->next;
                        cursor->each_list->numOfFile -=1;
                }

        }


	// 중복 파일 리스트에 파일이 하나 남는다면 exist_list를 0으로 수정
	if(cursor->each_list->numOfFile <= 1)
		cursor->exist_list = 0;
	

	// 리스트 출력
        printf("Left file in #%d : %s (%s)\n\n", atoi(op_argv[0]), latest->pathname, latest->mtime);
        printOpSet();

}


// t 옵션 함수
void t_option(char *op_argv[]){


        int i=0;
        set* cursor = list.head->next;
        dupf* files, *latest, *before;

	char trashPath[PATHLEN];
	char fname[1000];
	char num[10000];

	// 입력 받은 set 번호에 커서를 옮겨 놓음
        while(cursor != NULL){
                if(cursor->exist_list == 1){
                        i++;
                        if(i == atoi(op_argv[0]))
                                break;
                }

                cursor = cursor->next;
        }
        latest = cursor->each_list->head->next;
        before = cursor->each_list->head;
        files = latest->next;


	// 중복 파일 리스트의 끝까지 탐색
	i=0;
        while(files != NULL){
		i++;

		sprintf(num, "%d", i);
		// 현재까지의 최댓 값이 files가 가리키는 것보다 작다면 files로 최신화해줘야 함, latest를 trash로 이동
                if(strcmp(latest->mtime, files->mtime) < 0){
                        // move to trash
			realpath("./trash", trashPath); // trash 디렉터리의 절대경로 변환
			strcpy(fname, strrchr(latest->pathname, '/')); // pathname에서 파일명만 추출
			strcat(trashPath, fname); // trash로 이동할 파일명 생성
			strcat(trashPath, num); // 파일명 중복되지 않도록 num값 붙여주기

			//  trash 디렉터리에 latest가 가리키는 파일의 하드링크를 생성하고
			if(link(latest->pathname, trashPath) == -1){
				fprintf(stderr, "link error for %s\n",latest->pathname);
				exit(1);
			}else{ // 원래 위치의 파일을 삭제
				remove(latest->pathname);
			}

			// 결론적으로 원 위치에서  trash 디렉터리로 이동
                        before->next = files;
                        latest = files;
                        files = files->next;
                        cursor->each_list->numOfFile -=1;
                }else{ // latest가 더 크다면 files를 삭제
			
			realpath("./trash", trashPath);
			strcpy(fname, strrchr(files->pathname, '/'));
			strcat(trashPath, fname);
			strcat(trashPath, num);
			
			if(link(files->pathname, trashPath) == -1){
				fprintf(stderr, "link error for %s\n", files->pathname);
				exit(1);
			}else{
				remove(files->pathname);
			}

                        latest->next = files->next;
                        files = files->next;
                        cursor->each_list->numOfFile -=1;
                }

        }

        printf("All files in #%d have moved to Trash except \"%s\" (%s)\n\n", atoi(op_argv[0]), latest->pathname, latest->mtime);

	// 중복 파일이 한 개 이하로 남을 경우 set의 중복 파일 리스트 존재 여부를 없음으로 바꿈
	if(cursor->each_list->numOfFile <= 1)
		cursor->exist_list = 0;

	// 리스트 출력
        printOpSet();

}


// time_t 타입의 stime을 문자열 형식으로 변환하여 리턴
char* get_time(time_t stime){

        char * time = (char*)malloc(sizeof(char) * 30);
        struct tm* tm;

        tm = localtime(&stime);
        sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

        return time;
}

// 파일 확장자 통일
char *fileExtension(char *rawExtension){

        if(!strcmp("*",rawExtension)){ // * 입력시 모든 파일 확장자
                return "all"; // all 리턴시 모든 정규파일 탐색
        }

        if(!strncmp("*.",rawExtension,2)){ // 파일 확장자 부분만 분리하여 리턴

                return rawExtension+1;
        }
        else{
                printf("FileExtension input Error\n");
                exit(1);
        }
}

// 파일 사이즈 바이트로 변환
long fileSize(char *rawSize, char *kind){

        char *unit = "";
        double size;
        // KB, MB, GB가 있을시 

        if(!strcmp(rawSize,"~")){
                if(!strcmp(kind,"min"))
                        return 1;
                else
                        return INF;
        }
        size = strtod(rawSize,&unit);

	// 소문자도 함께 처리
        if(!(strcmp(unit,"KB")&&strcmp(unit,"kb")))
                size *= 1000;
        else if(!(strcmp(unit, "MB")&&strcmp(unit,"mb")))
                size *= 1000000;
        else if(!(strcmp(unit, "GB")&&strcmp(unit, "gb")))
                size *= 1000000000;
        else if(!strcmp(unit,"")){
                // 실수이면 에러 처리 후 exit(0)
                if((size - (long)size) != 0){
                        printf("FILE SIZE input Error\n");
                        exit(0);
                }else // 정수이면 long 타입으로 형변환 후 리턴
                        return (long)size;

        }else{
                printf("FILE SIZE input Error\n");
                exit(0);
        }


        return (long)size;

}


// 타겟 디렉토리 경로 절대경로로 변환
char *targetPath(char *pathname){
	char home[200];
	char temp[PATHLEN];
	struct passwd *user;

	// 만약 pathname이 ~로 시작한다면
	if(!strncmp(pathname,"~",1)){
		user = getpwnam(getlogin());
		strcpy(home, user->pw_dir);
		strcat(home, pathname+1);
		strcpy(pathname, home);
		return pathname;	
	}


	// 만약 pathname이 .으로 시작한다면
        if(realpath(pathname, temp)==NULL){
                printf("Target Directory input Error\n");
                exit(0);
        }

	strcpy(pathname, temp);
        return pathname;

}
	


// 데이터 타입 리턴 함수 
char getType(struct stat statbuf){
        char type = 'u';

        if(S_ISREG(statbuf.st_mode))
                type = 'r';
        else if(S_ISDIR(statbuf.st_mode))
                type = 'd';
        else if(S_ISCHR(statbuf.st_mode))
                type = 'c';
        else if(S_ISBLK(statbuf.st_mode))
                type = 'b';
        else if (S_ISFIFO(statbuf.st_mode))
                type = 'f';
        else if(S_ISSOCK(statbuf.st_mode))
                type = 's';

        else
                ;

        return type;
}


// set를 정렬하는 조건
int setSort(long size1, long size2){
        if(size1 >= size2){
                return 0;
        }
        else  // 삽입되는 것이 다음 것 보다 크다면 더 가야 됨
                return 1;

}

// dupf들을 정렬하는 조건
int dupfSort(int d1, int d2, char* p1, char *p2){
      if(d1 == d2){ // depth가 같으면
	      if(strlen(p1)==strlen(p2)){ // path 길이가 같으면
		      if(strcmp(p1,p2) > 0) // 아스크코드 비교
                	     return 1;
		      else  
                             return 0;
	      }else{
	        	if(strlen(p1) > strlen(p2))
				return 1;
			else
				return 0;
	      }
      }else{
                if(d1>=d2)
                        return 1;
                else
                        return 0;
      }
}


// sha1 추출 함수
const char* transtoSHA1(char* pathname, char* sha_buf){

        char ret[65];
        SHA_CTX c;
        unsigned char md[SHA_DIGEST_LENGTH];
        int fd;
        int i;
        static unsigned char buf[BUFSIZE];

	fd = open(pathname,O_RDONLY);
        SHA1_Init(&c);
        for(;;){
                i =read(fd, buf, BUFSIZE);
                if( i <= 0) break;
                SHA1_Update(&c, buf, (unsigned long)i);
        }
        SHA1_Final(&(md[0]), &c);

        for(i = 0; i < SHA_DIGEST_LENGTH; i++)
                sprintf(ret+2*i, "%02x", md[i]);


        if(strcpy(sha_buf, ret) <= 0){
                fprintf(stderr,"File %s Transfer to sha1 error\n", pathname);
                exit(-1);
        }

	
	close(fd);
        return sha_buf;
}

// 공백 단위로 문자열을 쪼개는 함수
int split(char* string, char* seperator, char* argv[]){
        int argc = 0;
        char* ptr = NULL;

        ptr = strtok(string, seperator);
        while(ptr != NULL){
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
        }
        return argc;
}


// -------------- datastruct.h 헤더파일에 존재하는 함수 정의부-------------------

// 중복 파일 LinkedList (한 set) 초기화 함수
void dupfInit(dupf_list* list){
        list->head = (dupf*)malloc(sizeof(dupf));
        list->head->next = NULL;
        list->head->depth = 0;
        strcpy(list->head->pathname, "");
        list->numOfFile = 0;

}


// 중복 파일 삽입 함수 -> 삽입시 path길이 비교, ASCII code 비교
void dupfInsert(dupf_list* list, char* pathname, char* mtime, char* atime, int depth){

        dupf* find = list->head->next;
        dupf* before = list->head;
        // 새로운 노드 생성
        dupf* temp =(dupf*)malloc(sizeof(dupf));
        temp->next == NULL;
        strcpy(temp->pathname, pathname);
        strcpy(temp->mtime, mtime);
        strcpy(temp->atime, atime);
        temp->depth = depth;

        if(list->numOfFile == 0){
                list->head->next = temp;
                (list->numOfFile)++;

                return ;
        }

	// 저장될 위치 찾기
	while(find != NULL && list->comp(depth, find->depth, pathname, find->pathname) > 0){
		before =find;
		find = find->next;
	}

	// 커서가 끝까지 도달했다면
	if(find == NULL){

		before->next = temp;
		temp->next = NULL;
		(list->numOfFile)++;

		return ;
	}

	// 커서가 큰 값에 멈춰있는 경우
	before->next = temp;
	temp->next = find;
	(list->numOfFile)++;

}


// 중복 파일 삭제 함수
void dupfDelete(char* pathname, dupf_list *list, dupf* cursor, dupf* before){

	// pathname의 파일 삭제
	if(remove(pathname)<0){
                fprintf(stderr,"remove error for %s\n",pathname);
                exit(1);
        }

        if(cursor == NULL){
                before->next = NULL;
                (list->numOfFile)--;
                return ;
        }

	// 삭제시 numOfFile 1감소
        before->next = cursor->next;
        (list->numOfFile)--;

}

// 중복 파일 한 세트 내에서 정렬 기준이 되는 함수 포인터를 list 요소에 넣어주는 함수
void dupfSortRule(dupf_list* list, int (*comp)(int l1, int l2, char * p1, char * p2)){
        list->comp = comp;
}



//------ set linked list------


// set LinkedList 초기화 함수
void setInit(set_list* list, int (*dSort)(int l1, int l2, char* p1, char * p2)){
        list->head = (set*)malloc(sizeof(set)); // head에 set 생성
        list->head->next = NULL;
        list->head->exist_list = 0;
        list->comp = NULL;
        list->head->size = -1;

        list->head->each_list = (dupf_list*)malloc(sizeof(dupf_list)); // set의 중복 파일 리스트를 가리키는 포인터
        dupfInit(list->head->each_list); 
        dupfSortRule(list->head->each_list, dSort);

        list->numOfSet = 0;

}



// set 삽입 함수
void setInsert(set_list* list, char* sha1, long size, char* search_path, char* mtime, char* atime, int depth, int (*dupfSort)(int l1, int l2, char * p1, char * p2)){
        set* cursor = list->head;
        set* before = list->head;
	
	// 존재하는 set중  정렬 조건에 부합하는 위치에 커서를 이동시킴
        while(cursor != NULL && list->comp(size, cursor->size) < 1){

                if(!strcmp(cursor->hash, sha1)){ // size가 같은데 hash 값까지 같다면 중복 파일 리스트에 저장
                        dupfInsert(cursor->each_list, search_path, mtime, atime, depth); // 중복 파일 리스트에 삽입
                        cursor->exist_list = 1;
                        return ;
                }

                before = cursor;
                if(cursor->next == NULL){
                        cursor->next = NULL;
                }
                cursor = cursor->next;
         }

	//  인자로 받은 정보를 토대로 set 생성
        set* temp = (set*)malloc(sizeof(set));
        temp->size = size;
        strcpy(temp->hash,sha1);
        temp->exist_list = 0;

        temp->each_list = (dupf_list*)malloc(sizeof(dupf_list));
        dupfInit(temp->each_list);
        dupfSortRule(temp->each_list, dupfSort);
        dupfInsert(temp->each_list, search_path, mtime, atime, depth);

        //커서가  끝에 있으면 그 자리에 set삽입
        if(cursor == NULL){
                temp->next = NULL;
                before->next = temp;
                (list->numOfSet)++;

                return;
        }

        // 커서가 삽입되어야 하는 다음 자리를 가리킬 시 그 전에 삽입
        temp->next = cursor;
        before->next = temp;
        (list->numOfSet)++;
        return ;

}

// set 정렬 조건 함수를 set linked list의 함수 포인터에 넘겨주는 함수
void setSortRule(set_list* list, int (*comp) (long size1, long size2)){
        list->comp =comp;
}



//---------------------- queue 함수--------------------------

// queue 초기화 함수
void queueInit(queue *pq){
        pq->front = NULL;
        pq->rear = NULL;
}

// queue가 비어 있는지 확인하는 함수
int QisEmpty(queue *pq){
        if(pq->front == NULL)
                return 1;
        else
                return 0;
}

// queue 삽입 함수
void Enqueue(queue *pq, char* pathname){
        q* newQ = (q*)malloc(sizeof(q)); // 삽입할 큐 생성
        strcpy(newQ->dir_path, pathname);
        newQ->next = NULL;

	// aueue의 뒷 부분에 삽입
        if(QisEmpty(pq)){
                pq->front = newQ;
                pq->rear = newQ;
        } else{
                pq->rear->next = newQ;
                pq->rear = newQ;
        }
}

// queue 추출 함수
char* Dequeue(queue *pq, char* ret){
        if(pq->front == NULL){
                fprintf(stderr,"Queue us Empty\n");
                exit(1);
        }
	
	//queue의 맨 앞 요소를 추출
        strcpy(ret, pq->front->dir_path);
        pq->front = pq->front->next;

        return ret;
}

