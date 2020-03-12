//TODO: And multithreading, so that one pthread is handling
//	The main "game" loop and the other is reading input
//	so there's no delay between a user pressing 'q' and
//	the program finishing.
//	PS: I tried doing this but it the there was still a
//	delay between the user's input and the programs
//	reaction.

#include "ncurses_life.h"

int main(int argv, char *argc[]) {
	int ch, thread_ret;
	size_t gen, i, j, start_row, start_col;
	char *filename;
	pthread_t thread;

	filename = (char *)malloc(100 * sizeof(char));

	start_row = 2;
	start_col = 2;

	for(i = 1; i < argv; i++) {
		if(*argc[i] == '-') {
			if(!strcmp(argc[i], "-speed"))
				speed = atof(argc[++i]);
			else if(!strcmp(argc[i], "-single-step"))
				single_step = true;
			else if(!strcmp(argc[i], "-no-single-step"))
				single_step = false;
			else if(!strcmp(argc[i], "-dead-cell-char"))
				dead_cell_char = *argc[++i];
			else if(!strcmp(argc[i], "-live-cell-char"))
				live_cell_char = *argc[++i];
			else if(!strcmp(argc[i], "-start-row"))
				start_row = atoi(argc[++i]);
			else if(!strcmp(argc[i], "-start-col"))
				start_col = atoi(argc[++i]);
			else if(!strcmp(argc[i], "-"))
				standard_input = true;
			else
				usage();
		}
		else
			strcpy(filename, argc[i]);
	}

	if(standard_input)
		free(filename);
	else if(strlen(filename) <= 0)
		usage();

	speed = DEFAULT_SPEED / speed;
	
	initscr();
	noecho();
	curs_set(0);
	getmaxyx(stdscr, maxrow, maxcol);

	gen = 0;

	maxrow -= 1;

	init_cells(maxrow, maxcol);

	read_cell_file(filename, start_row, start_col, standard_input); // Single stepping doesn't work if program reads input from stdin

	if(single_step) {
		nodelay(stdscr, false);
		speed = 0;
	}
	else
		nodelay(stdscr, true);

	while(true) {
		mvprintw(0, 0, "Generation: %d", gen++);

		print_cells(1, 0);

		refresh();

		enforce_rules();
			
		usleep(speed);

		if(getch() == 'q')
			break;
	}

	endwin();

	return EXIT_SUCCESS;	
}

void init_cells(size_t row, size_t col) {
	maxcell = row * col;
	cells = calloc(maxcell, sizeof(cell_t));

	if(cells == NULL)
		die(true, __FILE__, __func__, __LINE__, "memory allocation failed");
}

void print_cells(size_t row, size_t col) {
	size_t i;
	
	move(row, col);

	for(i = 0; i < ((maxrow - 1) * maxcol) - (col + (row*maxcol)); i++) {
		if(*(cells+i) == live)
			addch(live_cell_char);
		else {
			addch(dead_cell_char);
		}
	}
}

void write_cell(size_t row, size_t col, cell_t c) {
	*(cells+(col+(row * maxcol))) = c;
}

cell_t read_cell(size_t row, size_t col) {
	return *(cells+(col + (row * maxcol)));
}

size_t cells_around_cell(size_t row, size_t col) {
	size_t r, i, j, c;

	size_t l[8][2] = {	
				{row-1, col-1}, {row-1, col}, {row-1, col+1},
				{row, col-1}, {row, col+1},
				{row+1, col-1}, {row+1, col}, {row+1, col+1}
		};

	r = 0;

	for(i = 0; i < 8; i++) {
		c = read_cell(l[i][0], l[i][1]);
		if(c == live || c == potential_dead)
			r++;
	}

	return r;
}

void enforce_rules(void) {
	size_t row, col, cells_around;

	for(row = 1; row < maxrow; row++) {
		for(col = 0; col < maxcol; col++) {
			cells_around = cells_around_cell(row, col);
			if(*(cells+(col + (row * maxcol))) == live) {
				if(cells_around < 2)
					write_cell(row, col, potential_dead);
				else if(cells_around >= 4)
					write_cell(row, col, potential_dead);
				else if(cells_around == 2 || cells_around == 3);
			}

			else if(*(cells+(col + (row * maxcol))) == dead) {
				if(cells_around == 3)
					write_cell(row, col, potential_live);
			}
		}
	}

	for(row = 1; row < maxrow; row++) {
		for(col = 0; col < maxcol; col++) {
			if(*(cells+(col + (row * maxcol))) == potential_dead)
				write_cell(row, col, dead);
			else if(*(cells+(col + (row * maxcol))) == potential_live) {
				write_cell(row, col, live);
			}
		}
	}
}

void read_cell_file(char *fname, size_t start_row, size_t start_col, bool standard_input) {
	FILE *fp;
	char ch, c;

	size_t row, col;

	row = start_row;
	col = start_col;

	if(standard_input)
		fp = stdin;
	else
		fp = fopen(fname, "r");

	if(fp == NULL)
		die(true, __FILE__, __func__, __LINE__, "Couldn't open file %s", fname);

	while((ch = fgetc(fp)) != EOF) {
		if(ch == '!') 
			while((ch = fgetc(fp)) != EOF)
				if(ch == '\n')
					break;
		if(ch == '.') {
			col++;
		}
		if(ch == 'O') {
			write_cell(row, col++, live);
		}
		if(ch == '\n') {
			row++;
			col = start_col;
		}
	}
}

void usage(void) {
	 fputs("usage: ncurses_life [-speed speed] [-single-step] [-no-single-step]\n"
		"\t[-dead-cell-char dead_cell_char] [-live-cell-char live_cell_char]\n"
		"\t[-start-row start_row] [-start-col start_col] cell_file\n", stderr);
	 exit(EXIT_FAILURE);
}

void die(const bool win, const char *file, const char *func, const size_t l, const char *str, ...) {
	va_list vargs;

	if(win)
		endwin();

	va_start(vargs, str);	
	fprintf(stderr, "%s/%s/%ld: ", file, func, l);
	vfprintf(stderr, str, vargs);
	va_end(vargs);

	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}
