#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../header/page.h"
char * getNextAdress(FILE * readStream){
	int currChar = 0;            
    int i = 0;
    char *adress=malloc(10*sizeof(char));
    char toReturn[5];
    while (i  < 5 && ((currChar = fgetc (readStream)) != ' ' && 
                              currChar != EOF)) {
        adress[i++] = currChar;
    }
    if(currChar==EOF) return NULL;
    return adress;
}

char * getNextOffset(FILE* readStream){
	char* offset=malloc(3*sizeof(char));
	int currChar = 0;            
    int i = 0;
     while (i < 3 &&  currChar != EOF) {
     	currChar = fgetc (readStream);
        offset[i++] = currChar;
    }
    if(currChar==EOF) return NULL;
    return offset;
}
char  getAction(FILE * readStream){
	int currChar = 0;
	int i=0;
	char action[1];

    currChar = fgetc (readStream);
    action[0]=fgetc(readStream);
    currChar=fgetc(readStream);
    if(currChar==EOF) return 'A';// A represents the NULL case
    return action[0];
}
 int hash( int key) {
  return (int)(sqrt(key))%10;
}
void init(page **PT){
	for(int i=0;i<10;i++) PT[i]=NULL;
}
int checkMemory(page **PT,char * p,char action){
	int position=hash(atoi(p));
	if(PT[position]==NULL) return 0;
	page *ptr=PT[position];
	page *temp=ptr;
	while(1){
		char * name=temp->key;
		if(strcmp(name,p)==0){
			if(action=='W') temp->modified=1;
			return 1;
		}

		if(temp->next==NULL) return 0;
		else temp=temp->next;
	}
}
void insertToMemory(page **PT,char *p,char action){
	int position=hash(atoi(p));
	page *ptr=PT[position];
	page *temp;
	if(ptr==NULL){
		page* myPage;
		myPage=malloc(sizeof(page));
		myPage->key=malloc(sizeof(strlen(p)-1));
		strcpy(myPage->key,p);		
		myPage->next=NULL;
		if(action=='W') myPage->modified=1;
		else myPage->modified=0;
		PT[position]=myPage;
	}
	else{
		temp=ptr;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		page* myPage;
		myPage=malloc(sizeof(page));
		myPage->key=malloc(sizeof(strlen(p)));
		strcpy(myPage->key,p);
		myPage->next=NULL;
		if(action=='W') myPage->modified=1;
		else myPage->modified=0;
		temp->next=myPage;
	}
}
int removeFromMemory(page **PT,char *p){
	int ret=0;
	int position=hash(atoi(p));
	page *previous=PT[position];
	page *ptr=previous->next;
	if(strcmp(previous->key,p)==0){
		if(previous->next==NULL){
			free(previous);
			PT[position]=NULL;
		}
		else{
			page *temp=previous->next;
			free(previous);
			PT[position]=temp;
		}
		if(previous->modified==1) ret=1;
	}
	else{
		page *temp=ptr;
		page *previous_temp=previous;
		while(strcmp(temp->key,p)!=0){
			temp=temp->next;
			previous_temp=previous_temp->next;
		}
		if(temp->modified==1) ret=1;
		page *temp1;
		if(temp->next!=NULL)temp1=temp->next;
		else temp1=NULL;
		free(temp);
		previous_temp->next=temp1;
	}
	char ac;
	if(ret==1) ac='W';
	else ac='R';
	return ret;
}
char * findVictim(double*time,char**stack,double k,char* pageNum,int frameNum){
	int position;
	double min;
	char *victim;
	for(int i=0;i<frameNum;i++){
		if(i==0){
			min=time[i];
			victim=stack[i];
			position=i;
		}
		else{
			if(time[i]<=min){
				min=time[i];
				victim=stack[i];
				position=i;
			}
		}
	}
	stack[position]=pageNum;
	time[position]=k;
	return victim;
}
void WsInit(char **workingSet,int setSize){
	for(int i=0;i<setSize;i++){
		workingSet[i]=malloc(5*sizeof(char));
	}
}
void actionsInit(char*array,int setSize){
	for(int i=0;i<setSize;i++){
		array[i]=0;
	}
}
void WsSetEmpty(page **PT,char** workingSet,int setSize,int* memoryWrites){
	for(int i=0;i<setSize;i++){
		if(strcmp(workingSet[i],"")!=0){
			strcpy(workingSet[i],"");
			if(checkMemory(PT,workingSet[i],'R')!=0){
				int temp=removeFromMemory(PT,workingSet[i]);
				*memoryWrites+=temp;
			}
		}
	}
}
void WsDestroy(char** workingSet,int setSize){
	for(int i=0;i<setSize;i++){
		free(workingSet[i]);
	}
}
int WsGetSize(char** workingSet,int setSize){
	int i=0;
	while((strcmp(workingSet[i],"")!=0)&&(i<setSize)){
		i++;
		if(i==setSize) break;
	}
	return i;
}
int WsInSet(char ** workingSet,int setSize,char* pageNum){
	for(int i=0;i<setSize;i++){
		if(strcmp(workingSet[i],"")!=0){
			if(strcmp(workingSet[i],pageNum)==0){
				return i;
			}
		}
	}
	return -1;
}
void WsPrint(char **workingSet,char *name,int setSize){
	printf("%s: ",name );
	for(int i=0;i<setSize;i++){
		printf("%s ",workingSet[i] );
	}
	printf("\n");
}

void WsAdd(page** PT,char** workingSet,int setSize,char *pageNum,char action,char*actions,char**actualSet){
	int WsSize=WsGetSize(workingSet,setSize);
	int ret=0;
	if(WsSize==setSize){ //the working set if full
		int position=WsInSet(workingSet,setSize,pageNum);
		if(position==-1){ //the page does not exist in working set
			position=0;
			strcpy(workingSet[0],pageNum);
			char *temp[setSize];
			WsInit(temp,setSize);
			int noth; //memotyWrites are not real here, i set empty a temporary table
			/*------------wokingset handling -------------------*/
			WsSetEmpty(PT,temp,setSize,&noth); 
			for(int k=0;k<position;k++) strcpy(temp[k],workingSet[k]);
			for(int k=position+1;k<WsSize;k++) strcpy(temp[k-1],workingSet[k]);
			strcpy(temp[WsSize-1],pageNum);
			for(int k=0;k<WsSize;k++) strcpy(workingSet[k],temp[k]);
			WsDestroy(temp,setSize);
			/*------------actions handling -------------------*/
			actions[0]=action;
			char temporary[setSize];
			actionsInit(temporary,setSize);
			for(int k=0;k<position;k++) temporary[k],actions[k];
			for(int k=position+1;k<WsSize;k++) temporary[k-1]=actions[k];
			temporary[WsSize-1]=action;
			for(int k=0;k<WsSize;k++) actions[k]=temporary[k];
		}
		else{ //the page exists in working set, rearrange
			char *temp[setSize];
			WsInit(temp,setSize);
			int noth; //memotyWrites are not real here, i set empty a temporary table
			/*---------------------workingset handling ------------------------ */
			WsSetEmpty(PT,temp,setSize,&noth); 
			for(int k=0;k<position;k++) strcpy(temp[k],workingSet[k]);
			for(int k=position+1;k<WsSize;k++) strcpy(temp[k-1],workingSet[k]);
			strcpy(temp[WsSize-1],pageNum);
			for(int k=1;k<WsSize;k++) strcpy(temp[k-1],temp[k]);
			strcpy(temp[WsSize-1],"");
			for(int k=0;k<WsSize;k++) strcpy(workingSet[k],temp[k]);
			WsDestroy(temp,setSize);
		/*---------------------actions handling ------------------------ */
			char temporary[setSize];
			actionsInit(temporary,setSize);
			for(int k=0;k<position;k++) temporary[k]=actions[k];
			for(int k=position+1;k<WsSize;k++)temporary[k-1]=actions[k];
			temporary[WsSize-1]=action;
			for(int k=1;k<WsSize;k++) temporary[k-1]=temporary[k];
			temporary[WsSize-1]=0;
			for(int k=0;k<WsSize;k++) actions[k]=temporary[k];
		}
	}
	else{ //we have room, no victim
		int position=WsInSet(workingSet,setSize,pageNum);
		if(position==-1){ //the page does not exist in working set, pagefault
			strcpy(workingSet[WsSize],pageNum);
			actions[WsSize]=action; 
		}
		else{ //the page exists, rearrange the working set, pagenum as last element
			char *temp[setSize];
			WsInit(temp,setSize);
			int noth; //memotyWrites are not real here, i set empty a temporary table
			WsSetEmpty(PT,temp,setSize,&noth); 
			
			for(int k=0;k<position;k++) strcpy(temp[k],workingSet[k]);
			for(int k=position+1;k<WsSize;k++) strcpy(temp[k-1],workingSet[k]);
			strcpy(temp[WsSize-1],pageNum);
			if(WsGetSize(actualSet,setSize)==setSize){ //we throw out the first element because we can no longer see it
				int flag=0;
				for(int k=0;k<setSize;k++){
					if((k>0)&&(strcmp(actualSet[k],workingSet[0])==0) )flag=1;
				}
				if(flag==0){
					for(int k=1;k<WsSize;k++){
						strcpy(temp[k-1],temp[k]);
						actions[k-1]=actions[k];
					}
					strcpy(temp[WsSize-1],"");
					actions[WsSize-1]=0;
				}
			}
			for(int k=0;k<WsSize;k++) strcpy(workingSet[k],temp[k]);
			for(int k=WsSize;k<setSize;k++) strcpy(workingSet[k],"");
			WsDestroy(temp,setSize);

		/*---------------------actions handling ------------------------ */
			char temporary[setSize];
			actionsInit(temporary,setSize);
			for(int k=0;k<position;k++)temporary[k]=actions[k];
			for(int k=position+1;k<WsSize;k++) temporary[k-1]=actions[k];
			temporary[WsSize-1]=action;
			for(int k=0;k<WsSize;k++) actions[k]=temporary[k];
			for(int k=WsSize;k<setSize;k++) actions[k]=0;
		}
	}
	
}
void AddActualSet(page**PT,char*pageNum,char**actualSet,int setSize){
	int WsSize=WsGetSize(actualSet,setSize);
	int ret=0;
	if(WsSize==setSize){ //the actual set if full
		for(int k=1;k<WsSize;k++) strcpy(actualSet[k-1],actualSet[k]);
		strcpy(actualSet[WsSize-1],pageNum);
		
	}
	else{ //we have room, no victim
		strcpy(actualSet[WsSize],pageNum);
	}
}
void WsReload(page**PT,char **workingSet,int setSize,int*pagefaults,int frameNum,char **workingSet2,char*actions,int*memoryWrites){
	int size=WsGetSize(workingSet,setSize);
	int count=0;
	for(int i=0;i<setSize;i++){
		if(strcmp(workingSet[i],"")!=0){
			if(checkMemory(PT,workingSet[i],actions[i])==0){ //handle actions needed!!
				*pagefaults+=1;
				if(size<=frameNum){
					insertToMemory(PT,workingSet[i],actions[i]);
				}
				else{//not enough space in memory--workingSet2 won't be empty because that would mean setSize>frameNum
					int k=removeFromMemory(PT,workingSet2[count++]);
					*memoryWrites+=k;
					insertToMemory(PT,workingSet[i],actions[i]);
				}
			}
		}
	}
}
void removeAll(page**PT,char **workingSet,int setSize,int *memoryWrites){
	int temp;
	for(int i=0;i<setSize;i++){
		if(strcmp(workingSet[i],"")!=0){
			int j=removeFromMemory(PT,workingSet[i]);
			*memoryWrites+=j;
		}
	}
}
void WsCopy(char **dest,char**src,int setSize){
	for(int i=0;i<setSize;i++){
		strcpy(dest[i],src[i]);
	}
}
void WsSwapTables(char**workingset,char**outsideSet,char**temp,char**thrashedWorkingSet,
	char* workingset_actions,char*outside_actions,char** actualSet,char** outActualSet,int setSize){

WsCopy(temp,actualSet,setSize);
WsCopy(actualSet,outActualSet,setSize);
WsCopy(outActualSet,temp,setSize);
WsCopy(temp,workingset,setSize);
if(WsGetSize(outsideSet,setSize)==0){ //it has been thrashed
	WsCopy(workingset,thrashedWorkingSet,setSize);
}
else{
	WsCopy(workingset,outsideSet,setSize);
}
WsCopy(outsideSet,temp,setSize);
char temporary_actions[setSize];
actionsInit(temporary_actions,setSize);
for(int k=0;k<setSize;k++) temporary_actions[k]=workingset_actions[k];
for(int k=0;k<setSize;k++) workingset_actions[k]=outside_actions[k];
for(int k=0;k<setSize;k++) outside_actions[k]=temporary_actions[k];
}

	

