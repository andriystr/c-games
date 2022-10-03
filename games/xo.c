#include <ncurses.h>
#include <string.h>

#define FIGURE_SZ 5

typedef enum {Non=0, X, O}figure;

typedef struct vec{
	int x; int y;
}vec;

struct attrs{
	int selected_figure;
	int message;
	int wall;
}Attrs;

void init_attrs();
figure choice_figure();
void draw_field(figure field[3][3], vec pos);
void make_step(figure (*field)[3][3], figure f);
int check_win(figure field[3][3], figure f);
int has_spot(figure field[3][3]);
void win();
void lost();
void game_over();

int main(){
	int c;
	figure player_f, field[3][3] = {};
	vec p = {0, 0};

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, 1);
	curs_set(0);

	init_attrs();
	player_f = choice_figure();

	draw_field(field, p);
	while((c = getch()) != 'q'){
		switch(c){
			case KEY_LEFT:
			case 'a':
			p.x = p.x > 0 ? p.x - 1 : p.x;
			break;
			case KEY_RIGHT:
			case 'd':
			p.x = p.x < 2 ? p.x + 1 : p.x;
			break;
			case KEY_UP:
			case 'w':
			p.y = p.y > 0 ? p.y - 1 : p.y;
			break;
			case KEY_DOWN:
			case 's':
			p.y = p.y < 2 ? p.y + 1 : p.y;
			break;
		}
		if(strchr(" \n", c) && field[p.y][p.x] == Non){
			field[p.y][p.x] = player_f;
			if(check_win(field, player_f)){
				draw_field(field, p);
				win();
				break;
			}
			make_step(&field, player_f == X ? O : X);
		}
		draw_field(field, p);
		if(check_win(field, player_f == X ? O : X)){
			lost();
			break;
		}
		if(!has_spot(field)){
			game_over();
			break;
		}
	}

	endwin();
	return 0;
}


void init_attrs(){
	if(has_colors()){
		start_color();

		init_pair(1, COLOR_BLACK, COLOR_GREEN);
		init_pair(2, COLOR_BLACK, COLOR_RED);
		init_pair(3, COLOR_RED, 0);

		Attrs.selected_figure = COLOR_PAIR(1);
		Attrs.message = COLOR_PAIR(2);
		Attrs.wall = COLOR_PAIR(3);
	}
	else{
		Attrs.selected_figure = A_BOLD;
		Attrs.message = 0;
		Attrs.wall = 0;
	}
}

void draw_x(vec offset){
	for(int y = 0; y < FIGURE_SZ; y++){
		for(int x = 0; x < FIGURE_SZ; x++){
			if(y == x || y+x == FIGURE_SZ-1)
				mvaddch(offset.y+y, offset.x+x, '0');
			else
				mvaddch(offset.y+y, offset.x+x, ' ');
		}
	}
}

void draw_o(vec offset){
	for(int y = 0; y < FIGURE_SZ; y++){
		for(int x = 0; x < FIGURE_SZ; x++){
			if(x == 0 || y == 0 || x == FIGURE_SZ-1 || y == FIGURE_SZ-1)
				mvaddch(offset.y+y, offset.x+x, '0');
			else
				mvaddch(offset.y+y, offset.x+x, ' ');
		}
	}
}

void draw_non(vec offset){
	for(int y = 0; y < FIGURE_SZ; y++){
		for(int x = 0; x < FIGURE_SZ; x++){
			mvaddch(offset.y+y, offset.x+x, ' ');
		}
	}
}

figure choice_figure(){
	char msg[] = "Choice figure";
	int c, row, col, msg_len = strlen(msg);
	figure item = Non;
	vec offset;
	getmaxyx(stdscr, row, col);
	while(1){
		clear();
		offset.x = col/2 - msg_len/2;
		offset.y = row/2 - FIGURE_SZ/2 - 1;
		attrset(Attrs.message);
		mvprintw(offset.y, offset.x, msg);

		if(item == X)
			attrset(Attrs.selected_figure);
		else
			attrset(0);
		offset.x = col/2 - FIGURE_SZ - 1;
		offset.y += 2;
		draw_x(offset);

		if(item == O)
			attrset(Attrs.selected_figure);
		else
			attrset(0);
		offset.x += FIGURE_SZ + 2;
		draw_o(offset);
		attrset(0);
		refresh();

		c = getch();
		switch(c){
			case KEY_RESIZE:
			getmaxyx(stdscr, row, col);
			break;
			case KEY_LEFT:
			case 'a':
			item = X;
			break;
			case KEY_RIGHT:
			case 'd':
			item = O;
			break;
			case '\n':
			case ' ':
			if(item != Non)return item;
			break;
		}
	}
}

void draw_field(figure field[3][3], vec pos){
	int row, col;
	vec offset, offset_f, field_sz;
	getmaxyx(stdscr, row, col);
	field_sz.x = FIGURE_SZ * 3 + 4;
	field_sz.y = FIGURE_SZ * 3 + 4;
	offset.x = col/2 - field_sz.x/2;
	offset.y = row/2 - field_sz.y/2;
	offset_f.x = offset.x + 1;
	offset_f.y = offset.y + 1;

	clear();
	for(int y = 0; y < 3; y++){
		offset_f.x = offset.x + 1;
		for(int x = 0; x < 3; x++){
			if(pos.x == x && pos.y == y)
				attrset(Attrs.selected_figure);
			else
				attrset(0);
			if(field[y][x] == X)
				draw_x(offset_f);
			if(field[y][x] == O)
				draw_o(offset_f);
			if(field[y][x] == Non)
				draw_non(offset_f);
			offset_f.x += FIGURE_SZ + 1;
		}
		offset_f.y += FIGURE_SZ + 1;
	}
	for(int y = 0; y <= field_sz.y; y += FIGURE_SZ + 1){
		for(int x = 0; x < field_sz.x; x++){
			mvaddch(offset.y+y, offset.x+x, '#' | Attrs.wall);
		}
	}
	for(int y = 0; y < field_sz.y; y++){
		for(int x = 0; x <= field_sz.x; x += FIGURE_SZ + 1){
			mvaddch(offset.y+y, offset.x+x, '#' | Attrs.wall);
		}
	}
	attrset(0);
	refresh();
}


int eval_step(figure field[3][3], figure f){
	figure o_f = f == X ? O : X;
	if(check_win(field, f))
		return 10;
	if(check_win(field, o_f))
		return -10;
	if(!has_spot(field))
		return 0;
	int score = -1000000;
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(field[y][x] == Non){
				field[y][x] = f;
				int tmp = -eval_step(field, o_f);
				if(tmp > score)score = tmp;
				field[y][x] = Non;
			}
		}
	}
	return score;
}


void make_step(figure (*field)[3][3], figure f){
	figure o_f = f == X ? O : X;
	figure fld[3][3];
	int score = -1000000;
	vec pos = {-1, -1};
	memcpy(&fld, field, sizeof(figure)*3*3);
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(fld[y][x] == Non){
				fld[y][x] = f;
				int tmp = -eval_step(fld, o_f);
				if(tmp > score){
					score = tmp;
					pos.x = x;
					pos.y = y;
				}
				fld[y][x] = Non;
			}
		}
	}
	(*field)[pos.y][pos.x] = f;
}


int check_win(figure field[3][3], figure f){
	for(int i = 0; i < 3; i++){
		if(field[i][0] == f && field[i][1] == f && field[i][2] == f)
			return 1;
		if(field[0][i] == f && field[1][i] == f && field[2][i] == f)
			return 1;
	}
	if(field[0][0] == f && field[1][1] == f && field[2][2] == f)
		return 1;
	if(field[0][2] == f && field[1][1] == f && field[2][0] == f)
		return 1;
	return 0;
}

int has_spot(figure field[3][3]){
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(field[y][x] == Non)
				return 1;
		}
	}
	return 0;
}

void win(){
	char win_msg[] = "You win", exit_msg[] = "Press 'q' for exit";
	int row, col, x, y;
	getmaxyx(stdscr, row, col);
	x = col/2 - strlen(win_msg)/2;
	y = row/2;
	attrset(Attrs.message);
	mvprintw(y, x, win_msg);
	x = col/2 - strlen(exit_msg)/2;
	y += 1;
	mvprintw(y, x, exit_msg);
	attrset(0);
	while(getch() != 'q');
}

void lost(){
	char lost_msg[] = "You lost", exit_msg[] = "Press 'q' for exit";
	int row, col, x, y;
	getmaxyx(stdscr, row, col);
	x = col/2 - strlen(lost_msg)/2;
	y = row/2;
	attrset(Attrs.message);
	mvprintw(y, x, lost_msg);
	x = col/2 - strlen(exit_msg)/2;
	y += 1;
	mvprintw(y, x, exit_msg);
	attrset(0);
	while(getch() != 'q');
}

void game_over(){
	char over_msg[] = "Game over", exit_msg[] = "Press 'q' for exit";
	int row, col, x, y;
	getmaxyx(stdscr, row, col);
	x = col/2 - strlen(over_msg)/2;
	y = row/2;
	attrset(Attrs.message);
	mvprintw(y, x, over_msg);
	x = col/2 - strlen(exit_msg)/2;
	y += 1;
	mvprintw(y, x, exit_msg);
	attrset(0);
	while(getch() != 'q');
}
