#ifndef INTERFACE_H
#define INTERFACE_H

int mi_printinterface(int select_x, int select_y);
int mi_printmemory(int x, int y, int select_x, int select_y);
int mi_printflags(int x, int y);
int mi_printkeytooltip(int x, int y);
int mi_printselectedmemory(int x, int y, int address);
int mi_printinstrutioncounter(int x, int y, int insp_reg);
int mi_printaccumulator(int x, int y, int acc_reg);
#endif
