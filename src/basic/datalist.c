#include <stdlib.h>
#include <string.h>
#include "datalist.h"

DataList *datalist_new()
{
	return calloc(1, sizeof(DataList));
}
void datalist_push_back(DataList *datalist, void *item)
{
	DataList *last_node = datalist;
	while (last_node->next) {
		last_node = last_node->next;
	}
	if (last_node->value) {
		last_node->next = datalist_new();
		last_node->next->value = item;
	} else {
		last_node->value = item;
	}
}
void *datalist_pop_back(DataList *datalist)
{
	DataList *prelast_node = 0;
	DataList *last_node = datalist;
	while (last_node->next) {
		prelast_node = last_node;
		last_node = last_node->next;
	}
	void *ret_value = last_node->value;
	if (prelast_node) {
		prelast_node->next = 0;
	} else {
		last_node->value = 0;
	}
	return ret_value;
}
void *datalist_peek(DataList *datalist)
{
	DataList *last_node = datalist;
	while (last_node->next) {
		last_node = last_node->next;
	}
	return last_node->value;
}
void datalist_free(DataList *datalist)
{
	DataList *last_node = datalist;
	while (last_node->next) {
		DataList *prelast_node = last_node;
		last_node = last_node->next;
		free(prelast_node);
	}
	free(last_node);
}
int datalist_size(DataList *datalist)
{
	int size = 0;
	if (datalist->value) {
		size++;
	}
	DataList *last_node = datalist;
	while (last_node->next) {
		last_node = last_node->next;
		size++;
	}
	return size;
}