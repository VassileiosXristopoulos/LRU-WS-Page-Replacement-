#ifndef PAGE_H
#define PAGE_H
typedef struct page
{
	char *key;
	int modified;
	double time;
	struct page * next;
}page;
#endif