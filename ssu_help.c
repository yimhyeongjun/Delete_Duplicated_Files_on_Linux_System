#include <stdio.h>
#include <stdlib.h>

void help();

int main(int argc, char *argv[]){
        help();
        exit(0);

}
void help(){
printf("Usage:\n");
printf(" > fmd/fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
printf("    >> [SET_INDEX] [OPTION ... ]\n");
printf("       [OPTION ... ]\n");
printf("       d [LIST_IDX] : delete [LIST_IDX] file\n");
printf("       i : ask for confirmation before delete\n");
printf("       f : force delete except the recently modified file\n");
printf("       t : force move to Trash except the recently modified file\n");
printf(" > help\n");
printf(" > exit\n\n");
}
