#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ON '#'
#define OFF ' '
#define WIDTH COLS
#define HEIGHT (LINES-1)
#define DELAY 75001

/*
 * The neighbors are the 8 cells around (x,y) that are on.
 * Remember the grid is toroidal, it wraps around on all sides.
 */
int num_neighbors(char grid[][HEIGHT], int x, int y){
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
			if(grid[i][j] == ON)
				count++;
		}
	}
	// Uncount the cell (x,y)
	if(grid[x][y] == ON)
		return count - 1;
	return count;
}

void update_grid(char grid[][HEIGHT]){
	int x, y;
	// Make a temporary grid to hold the new states
	char temp_grid[WIDTH][HEIGHT];
	for(x = 0; x < WIDTH; ++x){
		for(y = 0; y < HEIGHT; ++y){
			switch(num_neighbors(grid, x, y)){
				case 2:
					temp_grid[x][y] = grid[x][y];
					break;
				case 3:
					temp_grid[x][y] = ON;
					break;
				default:
					temp_grid[x][y] = OFF;
			}
		}
	}
	// Copy temp_grid into grid
	memcpy(grid, temp_grid, HEIGHT*WIDTH);
}

void display(char grid[][HEIGHT], const char *info){
	int x, y;
	clear();
	for(y = 0; y < HEIGHT; ++y){
		for(x = 0; x < WIDTH; ++x){
			mvaddch(y, x, grid[x][y]);
		}
	}
	mvprintw(LINES-1, 0, "%s", info);
	refresh();
}

/*
 * Randomly initialize the grid
 */
void init_grid_rand(char grid[][HEIGHT]){
	int count;
	// Set all the cells to OFF
	memset(grid, (int)OFF, HEIGHT*WIDTH);
	// Start with random cells on
	srand(time(NULL));
	for(count = rand() % (HEIGHT*WIDTH/2); count > 0; --count){
		grid[rand()%WIDTH][rand()%HEIGHT] = ON;
	}
}

/*
 * Initialize the grid from a file. Return 1 on success 0 on failure.
 */
int init_grid_file(char grid[][HEIGHT], char *filename){
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
			grid[x][y] = ON;
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

	char grid[WIDTH][HEIGHT];

	if(argc == 2){
		if(!init_grid_file(grid, argv[1])){
			endwin();
			return 1;
		}
	}else{
		init_grid_rand(grid);
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
			display(grid, "Press enter to start/stop or 'q' to exit");
			update_grid(grid);
		}
	}

	endwin();
	return 0;
}
