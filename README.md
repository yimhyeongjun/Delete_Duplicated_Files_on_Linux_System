# Delete_Duplicated_Files_on_Linux_System

## 개요
 ssu_sdup 프로그램은 사용자가 요구하는 조건에 맞는 중복되는 파일을 찾고 삭제하는 프로그램입니다. 이 프로그램은 ssu_sdup.c, ssu_find-fmd5.c, ssu_find-sha1.c, ssu_help.c 총 4개의 .c파일과 .c파일을 컴파일해주는 makefile로 구성되어 있으며 makefile의 결과물로 ssu_sdup 파일이 완성되고 이 과정에서 ssu_find-fmd5, ssu_find-sha1, ssu_help라는 이름의 실행 파일이 완성됩니다. <br>
 ssu_sdup는 명령 프롬프트 창으로 20182649> 라는 입력창이 출력되고 입력창에 명령어를 입력하면 명령어를 토큰화하여 char \*argv[] 배열에 순차적으로 저장합니다. <br>
argv[0]에 fmd5가 저장되어 있을 경우 ssu_find-fmd5가 fork, exec되게 하는 방식으로 프로그램이 작동합니다. 마찬가지로 argv[0]에 sha1이 저장되어 있다면 ssu_find –sha1이, help가 저장되어 있다면 ssu_help가 fork, exec 하여 수행됩니다. 만약 argv[0]에 exit가 들어올 경우 프로그램이 종료되며 이외의 명령어가 들어올 경우 ssu_help를 fork, exec하여 실행시킵니다.<br>
 ssu_find-fmd5는 ssu_sdup에 의해 exec될 때 총 5개의 인자를 받아 실행되게 됩니다.<br> 
 첫 번째 인자는 실행 파일 경로명인 ./ssu_find-fmd5 이고 나머지는 차례로 확장자, 탐색 범위의 최솟값, 탐색 범위의 최댓값, 탐색 시작 디렉토리 입니다. 입력 받은 인자를 가공하여 탐색 시작 디렉토리의 하위 디렉토리를 bfs로 탐색하며 인자로 받은 확장자와 동일하고 인자로 받은 범위에 속하는 정규파일들을 탐색합니다. 탐색을 하며 조건에 맞는 정규 파일들의 hash 값을 md5 형식으로 추출하고 여태까지 탐색했던 파일들과 중복되지 않으면 set를 저장하는 linked list에 삽입하고 중복된다면 중복되는 linked list의 dupf_list라는 linked list에 저장합니다. 이렇게 이중 linked list로 중복 파일 세트를 저장하며 탐색이 끝나면 중복되는 파일들의 set를 출력하고 옵션 프롬프트 창을 띄웁니다. 옵션 프롬프트 창인 >>이 출력되면 옵션을 입력하여 삭제 기능을 수행합니다. 옵션은 d, i, f, t 옵션이 존재하며 모두 삭제의 기능이지만 조금씩 다른 방법으로 수행됩니다.<br>
  ssu_find-sha1은 ssu_find-fmd5와 동일한 방식의 프로그램이지만 hash 값을 추출할 때 md5가 아닌 sha1의 형식으로 추출합니다. 이외의 모든 기능이 ssu_find-fmd5와 동일합니다.<br>
 ssu_help는 ssu_sdup 프로그램의 기능 도움말을 출력해주는 실행 파일입니다. ssu_sdup에 의해 실행되면 화면에 도움말을 출력해주고 다시 ssu_sdup의 프롬프트 창으로 돌아옵니다.<br><br>
 
## 구현 플랫폼
 이 프로그램은 6.1버전의 Virtual box에서 20.04.4 버전의 우분투를 설치하여 Linux 운영체제에서 구현하였습니다.<br><br>
 
## 프로그램 설계
1. ssu_sdup.c<br>
  ssu_sdup.c는 프롬프트 창에서 명령어를 입력받아 입력 값에 따라 실행파일을 fork, exec해주는 파일이다.<br>
 따라서 입력 받은 명령어를 처리해주는 함수가 필요한데 문자열을 공백 기준으로 분리하여 저장하는 int split(char \*string, char \*seperator, char \*argv[])라는 함수로 처리하여 argv[0] 값에 따라 void fork_exec(char \*command[])에 split() 함수로 저장한 char \*argv를 인자로 넣어 알맞은 실행파일을 fork하고 exec하여 실행시키도록 설계하였습니다. 만약 argv[0]에 exit 명령어가 들어온다면 void command_exit() 함수를 실행하도록 하였습니다.<br><br>
 
 2. ssu_find-fmd5.c<br>
 ssu_find-fmd5.c는 확장자, 탐색 범위의 최솟값, 탐색 범위의 최댓값, 타겟 디렉토리를 인자로 받아 인자로 받은 조건에 맞는 정규파일을 탐색하여 중복 파일들을 linked list로 구현하고 옵션에 따라 여러 형태로 삭제하는 프로그램입니다.<br>
 따라서 main 함수에서 확장자 형태를 추출하는 char \*fileExtension(char \*rawExtension)과 입력받은 size 문자열을 바이트로 변환해주는 long fileSize(char \*rawSize, char \*kind), 타겟 디렉토리를 절대경로로 변환해주는 char \*targetPath (char \*pathname)라는 함수를 설계하여 탐색 조건을 먼저 정리하도록 설계하였습니다. 그리고 bfs 탐색에 앞서 사용할 큐와 중복 리스트를 저장할 linked list를 초기화하도록 void queueInit(queue \*q), setInit(set_list \*list, int\*(comp)(int l1, int l2, char \*p1, char \*p2)) 함수를 설계하여 사용하였습니다. 그리고 중복 파일 set의 정렬 조건을 설정하는 setSortRule이라는 함수도 구현에 포함하였습니다.<br>
 void bfs(char \*dir_path)는 타겟 디렉터리 하위 파일들을 dfs로 탐색하기 위해 설계한 함수입니다. bfs() 내부에서 탐색한 디렉터리가 정규파일이 아닌 디렉터리인 경우 Queue에 삽입하기 위해 파일의 type을 검사하여 리턴하는 getType() 함수와 queue에 디렉터리를 삽입하는 Enqueue() 함수를 설계하였습니다. 그렇다면 디렉터리가 정규파일인 경우 인자로 받았던 조건에 맞을 시 그 파일의 hash값을 추출하여 linked list에 존재하는 파일들의 hash값과 비교하여 linked list의 적절한 위치에 삽입하는데 이 때 md5 형식으로 hash 값을 추출하는 transtoMD5()라는 함수와 linked list에 삽입하는 setInsert() 함수를 설계하였습니다. bfs()에서 인자로 받은 dir_path의 자식 디렉터리의 탐색이 끝나면 queue의 가장 앞에 있는 요소를 dequeue하여 bfs에 재귀적으로 넣도록 Dequeue() 함수와 queue가 빌 때까지 Dequeue 할 수 있도록 큐가 비어있으면 1을 리턴하는 QisEmpty() 함수 구현을 설계하였습니다.<br>
  linked list에 새로운 파일 데이터를 삽입할 때 두 가지 경우를 고려하여 설계하였습니다. linked list에 hash 값이 동일한 파일이 없는 경우는 set 리스트에 삽입을 해야 하고 hash 값이 동일한 파일이 존재할 경우 해당 set의 멤버 변수인 dupf_list에 삽입을 하도록 설계하였습니다. 따라서 중복 파일 리스트는 탐색한 모든 정규 파일들의 hash값과 size를 set 구조체에 저장하여 보관하는 linked list가 있고 각 set 별로 hash 값이 같은 중복 파일을 저장하는 linked list를 멤버 변수로 포함시켜 이중 linked list로 데이터를 저장하도록 설계하였습니다.<br><br>
  
3. ssu_find-sha1.c<br>
 ssu_find-sha1.c는 ssu_find-fmd5.c와 함수의 흐름이 동일하며 hash값을 추출할 때 md5의 형식이 아니라 sha1의 형식을 사용한다는 점만 차이가 있도록 설계하였습니다.<br><br>

4. ssu_help.c<br>
 ssu_help.c는 ssu_sdup 프로그램의 명령어 도움말을 출력해주는 프로그램입니다. 따라서 출력하는 기능만 수행하면 되는데 void help()라는 함수에 출력부를 넣어 main 함수에서 help()를 호출하도록 설계하였습니다.<br><br>

5. datastruct.h<br>
 프로그램을 설계할 때 이중 linked list와 queue를 직접 구현해야 한다는 것을 인지하여 자료구조의 구조체와 함수 선언부를 담는 datastruct.h 헤더파일을 설계하였습니다.<br><br>
 
 #### 함수 콜 그래프
 
 ![IMG_0175](https://github.com/yimhyeongjun/Delete_Duplicated_Files_on_Linux_System/assets/64363668/37fe3da5-8a0a-4be3-a507-c524bfe74107)
<br><br>
 
 
