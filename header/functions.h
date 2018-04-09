#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "page.h"
char * getNextAdress(FILE *);
char * getNextOffset(FILE *);
char  getAction(FILE *);
int hash(int);
int checkMemory(page **,char *,char);
void insertToMemory(page **,char *,char);
void init(page**);
int removeFromMemory(page**,char*);
char * findVictim(double*,char**,double,char*,int);
void WsInit(char **,int);
void WsSetEmpty(page**,char**,int,int*);
void WsDestroy(char**,int);
int WsGetSize(char **,int);
int WsAdd(page **,char **,int,char*,char,char*,char**);
int WsInSet(char **,int,char*);
void WsReload(page**,char**,int,int*,int,char **,char*,int*);
void WsPrint(char**,char*,int);
void removeAll(page**,char**,int,int*);
void WsCopy(char**,char**,int);
void actionsInit(char*,int);
void WsSwapTables(char**,char**,char**,char**,char* ,char*,char** ,char** ,int );
void AddActualSet(page**,char*,char**,int);
#endif