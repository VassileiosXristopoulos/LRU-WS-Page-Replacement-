#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/lru.h"
#include "../header/ws.h"

int main(int argv, char *argc[]){
FILE* readStream2 = NULL; 
FILE* readStream1 = NULL;
 if ((readStream1 = fopen("bzip.trace", "r")) == NULL) {
        fprintf (stderr, "error: file open failed 'bzip.trace'.\n");
        return 1;
 }
 if ((readStream2 = fopen("gcc.trace", "r")) == NULL) {
    fprintf (stderr, "error: file open failed 'gcc.trace'.\n");
    return 1;
 }
int max;
printf("Do you want to set a max value for the requests to be readen? (yes/no)\n");
char answer[3];
printf("here\n");
scanf("%s",answer);
if(strcmp(answer,"yes")==0){
	printf("please give the number:");
	scanf("%d",&max);
}
else if(strcmp(answer,"no")==0) max=0;
else{
	printf("wrong entry\n");
	return 0;
}

char *algorithm=argc[1];
int frameNumber=atoi(argc[2]);
int q = atoi(argc[3]);

int setSize=0;
if(strcmp(algorithm,"WS")==0) setSize=atoi(argc[4]);
if(setSize>frameNumber) {
	printf("invalid setSize\n");
	return 0;
}
page * PT[10];  // page table
init(PT);
if(setSize!=0) {
	WS(PT,readStream1,readStream2,q,frameNumber,max,setSize);
}
else {
	LRU(PT,readStream1,readStream2,q,frameNumber,max);
}
fclose(readStream1);
fclose(readStream2);


return 0;
}
