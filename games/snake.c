#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

typedef struct pos{
	int x;
	int y;
}pos;

#define SNAKE_MAXLEN 30

int main(){
	int c, row, col, snake_len = 0;
	pos snake[SNAKE_MAXLEN], dir = {1, 0}, food, p;

	srand(time(NULL));

	initscr();
	noecho();
	cbreak();
	keypad(stdscr, 1);
	curs_set(0);
	nodelay(stdscr, 1);
	getmaxyx(stdscr, row, col);
	start_color();
	init_pair(1, COLOR_RED, 0);
	init_pair(2, COLOR_GREEN, 0);

	p.x = col/2;
	p.y = row/2;
	snake[snake_len++] = p;

	food.x = rand() % col;
	food.y = rand() % row;

	while((c = getch()) != 'q'){
		clear();
		for(int i = 0; i < snake_len; i++){
			mvaddch(snake[i].y, snake[i].x, '#' | COLOR_PAIR(1));
		}
		mvaddch(food.y, food.x, '@' | COLOR_PAIR(2));
		refresh();
		if(dir.x)
			timeout(70);
		else
			timeout(100);

		switch(c){
			case KEY_RESIZE:
			getmaxyx(stdscr, row, col);
			break;
			case KEY_LEFT:
			case 'a':
			if(dir.x != 1){
				dir.x = -1; dir.y = 0;
			}
			break;
			case KEY_RIGHT:
			case 'd':
			if(dir.x != -1){
				dir.x = 1; dir.y = 0;
			}
			break;
			case KEY_UP:
			case 'w':
			if(dir.y != 1){
				dir.x = 0; dir.y = -1;
			}
			break;
			case KEY_DOWN:
			case 's':
			if(dir.y != -1){
				dir.x = 0; dir.y = 1;
			}
			break;
		}

		for(int i = snake_len-1; i > 0; i--){
			snake[i] = snake[i-1];
		}
		snake[0].x = snake[0].x < 0 ? col-1 : snake[0].x;
		snake[0].y = snake[0].y < 0 ? row-1 : snake[0].y;
		snake[0].x = (snake[0].x+dir.x) % col;
		snake[0].y = (snake[0].y+dir.y) % row;

		for(int i = 1; i < snake_len; i++){
			if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
				nodelay(stdscr, 0);
				mvprintw(row/2, col/2-4, "Game ower");
				mvprintw(row/2+1, col/2-9, "Press 'q' for exit");
				while(getch() != 'q');
				goto exit;
			}
		}

		if(snake[0].x == food.x && snake[0].y == food.y){
			if(snake_len >= SNAKE_MAXLEN-1){
				nodelay(stdscr, 0);
				mvprintw(row/2, col/2-3, "You win");
				mvprintw(row/2+1, col/2-9, "Press 'q' for exit");
				while(getch() != 'q');
				break;
			}
			p.x = food.x;
			p.y = food.y;
			snake[snake_len++] = p;
			food.x = rand() % col;
			food.y = rand() % row;
		}

	}

	exit:

	endwin();
	return 0;
}
