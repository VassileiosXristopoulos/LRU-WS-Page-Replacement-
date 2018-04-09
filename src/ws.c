#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/ws.h"

void WS(page **PT,FILE* readStream1,FILE* readStream2,int q,int frameNum,int max,int setSize){
	int pagefaults=0;
	int memoryWrites=0;
	int reads=0;
	int writes=0;
	int swaps=0;
	char * pageNum=malloc(5*sizeof(char));
	char * offset=malloc(3*sizeof(char));
	char * temp[setSize];
	char * workingset2[setSize];
	char *workingset[setSize],*outsideSet[setSize],*thrashedWorkingSet[setSize];
	char workingset_actions[setSize];
	char outside_actions[setSize];
	char *actualSet[setSize];
	char *outActualSet[setSize];
	actionsInit(workingset_actions,setSize);
	actionsInit(outside_actions,setSize);
	/*------------------------------------------------*/
	WsInit(temp,setSize);
	WsSetEmpty(PT,temp,setSize,&memoryWrites);

	WsInit(workingset,setSize);
	WsSetEmpty(PT,workingset,setSize,&memoryWrites);

	WsInit(outsideSet,setSize);
	WsSetEmpty(PT,outsideSet,setSize,&memoryWrites);

	WsInit(thrashedWorkingSet,setSize);
	WsSetEmpty(PT,thrashedWorkingSet,setSize,&memoryWrites);

	WsInit(actualSet,setSize);
	WsSetEmpty(PT,actualSet,setSize,&memoryWrites);

	WsInit(outActualSet,setSize);
	WsSetEmpty(PT,outActualSet,setSize,&memoryWrites);
	/*------------------------------------------------*/
	char  action;
	FILE *reader=readStream1;
	int counter=0,flag=0,flag1=0,flag2=0; //flag=1 if one file=eof-------flag1=1 if file1=eof ------ flag2=1 if file2=eof
	int num=1;
	int j=0;
	int total_requests=0;
	if(max!=0) num=max;
	while(num>0){
		if(flag==0){ //if both files!= eof then do changes
			if(q==j){ //SWITCH EVERY Q READS
				if(reader==readStream1){
					reader=readStream2;
				}
				else{
					reader=readStream1;
				}

					WsSwapTables(workingset,outsideSet,temp,thrashedWorkingSet,
						workingset_actions,outside_actions,actualSet, outActualSet, setSize);
					/*-----------swap out policy------------------*/
					int temp1=WsGetSize(workingset,setSize);
					int temp2=WsGetSize(outsideSet,setSize);
					if(temp1+temp2>=frameNum) { //swap out policy
						swaps++;
						WsCopy(thrashedWorkingSet,outsideSet,setSize); //when reloading, able to see each old element
						removeAll(PT,outsideSet,setSize,&memoryWrites);
						WsSetEmpty(PT,outsideSet,setSize,&memoryWrites); 
					}
					/*------------------------------------------------*/				
					counter=WsGetSize(workingset,setSize);
					WsReload(PT,workingset,setSize,&pagefaults,frameNum,outsideSet,workingset_actions,&memoryWrites);
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
					WsSwapTables(workingset,outsideSet,temp,thrashedWorkingSet,
						workingset_actions,outside_actions,actualSet, outActualSet, setSize);
					continue;
				}
				else{
					flag2=1;
					reader=readStream1;
					WsSwapTables(workingset,outsideSet,temp,thrashedWorkingSet,
						workingset_actions,outside_actions,actualSet, outActualSet, setSize);					
					continue;
				}
			}
			else{
				break;
			}
		}

		int currSize=WsGetSize(workingset,setSize);
		int outsideSize=WsGetSize(outsideSet,setSize);
		if(currSize+outsideSize>=frameNum) { //swap out policy
			swaps++;
			WsCopy(thrashedWorkingSet,outsideSet,setSize); //when reloading, able to see each old element
			removeAll(PT,outsideSet,setSize,&memoryWrites);
			WsSetEmpty(PT,outsideSet,setSize,&memoryWrites); 
		}
		int in=checkMemory(PT,pageNum,action);
		if(in==0){
			pagefaults++;
			if(currSize<setSize){
				insertToMemory(PT,pageNum,action);
			}
			else{
				memoryWrites+=removeFromMemory(PT,workingset[0]);;
				insertToMemory(PT,pageNum,action);
			}
		}
		WsAdd(PT,workingset,setSize,pageNum,action,workingset_actions,actualSet); //only does 1 remove if necesary, returns 1 if it did
		AddActualSet(PT,pageNum,actualSet,setSize);
		j++;
		total_requests++;
		if(max!=0) num--; //if input is given then we will do "max" loops, otherwise num will always be >0 and we will stop when both files=eof		
	}
		

	printf("reads:%d writes:%d available frames:%d modified:%d page faults:%d requests readen:%d swap out occured %d times\n",reads,writes,frameNum,memoryWrites,pagefaults,total_requests , swaps);
	free(pageNum);
	free(offset);
	free(pageNum);
	WsDestroy(workingset,setSize);
	WsDestroy(outsideSet,setSize);
	WsDestroy(temp,setSize);
	WsDestroy(actualSet,setSize);
	WsDestroy(outActualSet,setSize);
	WsDestroy(thrashedWorkingSet,setSize);
}
