#ifndef DATALIST_H
#define DATALIST_H

typedef struct DataList {
	void *value;
	struct DataList *next;
} DataList;

DataList *datalist_new();
void datalist_push_back(DataList *datalist, void *item);
void *datalist_pop_back(DataList *datalist);
void *datalist_peek(DataList *datalist);
void datalist_free(DataList *datalist);
int datalist_size(DataList *datalist);

#endif