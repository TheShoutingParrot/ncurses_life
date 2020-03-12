#ifndef NCURSES_H
#define NCURSES_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define dead			0
#define live			1
#define potential_dead		2
#define potential_live		3

#define DEFAULT_SPEED 		100000

typedef uint8_t cell_t;

cell_t *cells;

size_t maxcell;
size_t maxrow, maxcol;

char dead_cell_char 	= ' ';
char live_cell_char 	= '*';

float speed 		= 1;

bool single_step 	= false;
bool standard_input 	= false;

void init_cells(size_t row, size_t col);
void print_cells(size_t row, size_t col);
void write_cell(size_t row, size_t col, cell_t c);
cell_t read_cell(size_t row, size_t col);
size_t cells_around_cell(size_t row, size_t col);
void enforce_rules(void);
void read_cell_file(char *fname, size_t start_row, size_t start_col, bool standard_input);

void usage(void);
void die(const bool win, const char *file, const char *func, const size_t l, const char *str, ...);

#endif // #ifndef NCURSES_H
