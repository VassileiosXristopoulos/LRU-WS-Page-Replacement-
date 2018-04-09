#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/lru.h"

void LRU(page **PT,FILE* readStream1,FILE* readStream2,int q,int frameNum,int max){
	int pagefaults=0;
	int memoryWrites=0;
	int reads=0;
	int writes=0;
	char * pageNum=malloc(5*sizeof(char));
	char * offset=malloc(3*sizeof(char));
	char  action;
	FILE *reader=readStream1;
	double *time=malloc(frameNum*sizeof(double));
	char *stack[frameNum];
	for(int i=0;i<5;i++){
		stack[i]=malloc(5*sizeof(char));
	}
	int counter=0,flag=0,flag1=0,flag2=0; //flag=1 if one file=eof-------flag1=1 if file1=eof ------ flag2=1 if file2=eof
	int num=1;
	double i=0;
	int j=0;
	int total_requests=0;
	if(max!=0) num=max;
	while(num>0){
		if(flag==0){ //if both files!= eof then do changes
			if(q==j){ //SWITCH EVERY Q READS
				if(reader==readStream1) reader=readStream2;
				else reader=readStream1;
				j=0;
			}
		}
		pageNum=getNextAdress(reader);
		offset=getNextOffset(reader);
		action=getAction(reader);
		if(action=='R') reads++;
		if(action=='W') writes++;
		if((pageNum==NULL)||(offset==NULL)||(action=='A')){
			flag=1;
			if(!((flag1==1)&&(flag2==1))){
				if(reader==readStream1){
					flag1=1;
					reader=readStream2;
					continue;
				}
				else{
					flag2=1;
					reader=readStream1;
					continue;
				}
			}
			else{
				break;
			}
		}
		int in=checkMemory(PT,pageNum,action); 
		if(in==0){
			pagefaults++;
			if(counter<frameNum){
				time[counter]=i;
				stack[counter]=pageNum;
				insertToMemory(PT,pageNum,action);
				counter++;
			}
			else{
				char * victim=findVictim(time,stack,i,pageNum,frameNum);
				memoryWrites+=removeFromMemory(PT,victim);
				insertToMemory(PT,pageNum,action);
			}
		}
		i+=1.0e-6;
		j++;
		total_requests++;
		if(max!=0) num--; //if input is given then we will do "max" loops, otherwise num will always be >0 and we will stop when both files=eof		
	}
		

	printf("reads:%d writes:%d available frames:%d modified:%d page faults:%d requests readen:%d\n",reads,writes,frameNum,memoryWrites,pagefaults,total_requests );
	free(pageNum);
	free(offset);
	for(int i=0;i<5;i++){
		free(stack[i]);
	}
	free(time);
}
