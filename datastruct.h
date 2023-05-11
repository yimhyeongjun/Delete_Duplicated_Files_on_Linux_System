#pragma once

#include <stdio.h>
#include <stdlib.h>

#define sha1HASH 65
#define md5HASH 33
#define PATHLEN 4096

typedef struct q{
        char dir_path[PATHLEN];
        struct q* next;
}q;

typedef struct dir_queue{
        q* front;
        q* rear;
}queue;


void queueInit(queue* pq); // 큐 초기화 함수
int QisEmpty(queue* pq); // 큐가 비어있는지 리턴하는 함수
void Enqueue(queue* pq,  char* pathname); // 큐에 데이터 삽입하는 함수
char* Dequeue(queue* pq, char * search_path); // 큐에서 데이터 추출하는 함수


// 파일 노드 구조체
typedef struct dupf{
        char pathname[PATHLEN];
        char mtime[100];
        char atime[100];

        int depth;
        struct dupf* next;

}dupf;

// 중복 파일 LinkedList 구조체
typedef struct dupf_list{
        dupf* head;
        dupf* cur;
        dupf* before;

        int numOfFile;

        int (*comp)(int l1, int l2, char *p1, char *p2);


}dupf_list;

void dupfInit(dupf_list* dupfList);
void dupfInsert(dupf_list* list, char* pathname, char* mtime, char * atime, int depth); // dup 파일 삽입
void dupfDelete(char* pathname, dupf_list* list, dupf* cursor, dupf* before); // dup 파일 삭제
void dpufSortRule(dupf_list* list, int (*comp)(int d1, int d2, char* p1, char *p2)); // 데이터 정렬 조건 설정 함수


// set 노드 구조체
typedef struct set {
        long size;
        char hash[sha1HASH];

        int exist_list; // 중복 파일이 존재하면 1 이상 아니면 0
        struct set* next;
        dupf_list* each_list;
}set;

// set LinkedList 구조체
typedef struct set_list {
        set* head;
        set* cur;
        set* before;

        int numOfSet;

        int (*comp)(long size1, long size2);
}set_list;

void setInit(set_list* setList, int(*dupfSort)(int l1, int l2,char* p1, char* p2)); // set_list 초기화 함수 
void setInsert(set_list* list, char* md5, long size, char* pathname, char *mtime, char * atime, int depth, int (*dupfSort)(int l1, int l2, char* p1, char* p2)); // set 삽입 함수
void setDelete(); // set 삭제 함수
void setTrash(); // set 
void setCount(); // set 갯수 리턴 함수
void setSortRule(set_list* list, int comp(long size1, long size2));
                                                                                                                                                                                    


