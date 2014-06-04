#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ON '#'
#define OFF ' '
#define WIDTH COLS
#define HEIGHT (LINES-1)
#define DELAY 75001

char *grid;

/*
 * The neighbors are the 8 cells around (x,y) that are on.
 * Remember the grid is toroidal, it wraps around on all sides.
 */
int num_neighbors(int x, int y){
	int count = 0, i, j;
	if(x == 0)
		i = WIDTH - 1;
	else
		i = x - 1;
	for(; i != (x+2)%WIDTH; i = (i+1)%WIDTH){
		if(y == 0)
			j = HEIGHT - 1;
		else
			j = y - 1;
		for(; j != (y+2)%HEIGHT; j = (j+1)%HEIGHT){
			if(grid[i + WIDTH*j] == ON)
				count++;
		}
	}
	// Uncount the cell (x,y)
	if(grid[x + WIDTH*y] == ON)
		return count - 1;
	return count;
}

void update_grid(){
	int x, y;
	// Make a temporary grid to hold the new states
	char *temp_grid = (char *)malloc(WIDTH*HEIGHT);
	for(x = 0; x < WIDTH; ++x){
		for(y = 0; y < HEIGHT; ++y){
			switch(num_neighbors(x, y)){
				case 2:
					temp_grid[x + WIDTH*y] = grid[x + WIDTH*y];
					break;
				case 3:
					temp_grid[x + WIDTH*y] = ON;
					break;
				default:
					temp_grid[x + WIDTH*y] = OFF;
			}
		}
	}
	// Copy temp_grid into grid
	strncpy(grid, temp_grid, HEIGHT*WIDTH);
	free(temp_grid);
}

void display(const char *info){
	int x, y;
	clear();
	for(y = 0; y < HEIGHT; ++y){
		for(x = 0; x < WIDTH; ++x){
			mvaddch(y, x, grid[x + WIDTH*y]);
		}
	}
	mvprintw(LINES-1, 0, "%s", info);
	refresh();
}

/*
 * Randomly initialize the grid
 */
void init_grid_rand(){
	int count;
	// Set all the cells to OFF
	memset(grid, (int)OFF, HEIGHT*WIDTH);
	// Start with random cells on
	srand(time(NULL));
	for(count = rand() % (HEIGHT*WIDTH/2); count > 0; --count){
		grid[(rand()%WIDTH) +WIDTH*(rand()%HEIGHT)] = ON;
	}
}

/*
 * Initialize the grid from a file. Return 1 on success 0 on failure.
 */
int init_grid_file(char *filename){
	FILE *fp;
	int c, x = 0, y = 0;
	if (!(fp = fopen(filename, "r"))){
		perror(filename);
		return 0;
	}
	// Set all the cells to OFF
	memset(grid, (int)OFF, HEIGHT*WIDTH);
	while((c = fgetc(fp)) != EOF){
		if(c == ON)
			grid[x + WIDTH*y] = ON;
		// increment x and possibly y
		x++;
		if(x == WIDTH || c == '\n'){
			x = 0;
			y++;
			if(y == HEIGHT)
				break;
		}
	}

	fclose(fp);
	return 1;
}

int main(int argc, char **argv){
	int c, paused = 0, delay;
	if(argc > 2){
		fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return 1;
	}
	initscr();
	keypad(stdscr, TRUE);
	timeout(10);
	noecho();

	grid = (char *)malloc(HEIGHT * WIDTH);
	if(grid == NULL){
		endwin();
		return 1;
	}

	if(argc == 2){
		if(!init_grid_file(argv[1])){
			endwin();
			free(grid);
			return 1;
		}
	}else{
		init_grid_rand();
	}
	delay = DELAY;
	while((c = getch()) != 'q'){
		usleep(delay);
		if(c == '\n')
			paused = !paused;
		else if(c == KEY_UP && delay > 5000)
			delay -= 5000;
		else if(c == KEY_DOWN)
			delay += 5000;

		if(!paused){
			display("Press enter to start/stop or 'q' to exit");
			update_grid();
		}
	}

	endwin();
	free(grid);
	return 0;
}
