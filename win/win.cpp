#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>

#define LEFT 75		// Keyboard Input
#define RIGHT 77
#define UP 72
#define DOWN 80
#define p 112
#define P 80
#define ESC 27

#define false 0
#define true 1

#define ACTIVE_BLOCK -2 // Block State
#define CEILLING -1
#define EMPTY 0
#define WALL 1
#define INACTIVE_BLOCK 2

#define MAIN_X 11	// Game borad Size
#define MAIN_Y 23
#define MAIN_X_ADJ 3
#define MAIN_Y_ADJ 1

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 // Status info position

int STATUS_Y_GOAL; // Status info position

int STATUS_Y_SCORE;

int blocks[7][4][4][4] = {
	{ { 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },
	{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 } },
	{ { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 },
	{ 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 } },
	{ { 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0 } },
	{ { 0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0 },{ 0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0 },{ 0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0 } }
}; // Shape of block

int b_type; // block type
int b_rotation; // block rotation
int b_type_next; //new block

int main_org[MAIN_Y][MAIN_X];
int main_cpy[MAIN_Y][MAIN_X];
time_t timer;
int bx, by; // x,y position of moving block

int key; // keyboard input

int speed; // Game Speed

int p1_score; // Game Score
int p1_turn=0;
int p1_item=0;
int p1_used_item=0;
int new_block_on = 0;
int crush_on = 0;
int turn_item_used=0;


void title(void);
void reset(void);
void reset_main(void); //
void reset_main_cpy(void);
void draw_map(void);
void draw_main(void);
void new_block(void);
void check_key(void);

int check_crush(int bx, int by, int rotation);
void drop_block(void);
void move_block(int dir);
void check_line(void);

void use_item(int turn);
void use_item_pause(void);

void check_game_over(void);
void pause(void);

void gotoxy(int x, int y) {
	COORD pos = { 2 * x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // hide cursor
void setcursortype(CURSOR_TYPE c) { // hide cursor
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int main() {
	int i;
	srand((unsigned)time(NULL));
	setcursortype(NOCURSOR);
	title();

	reset();
	
	while (1) {
		for (i = 0; i<5; i++) {
			check_key();
			draw_main();

			Sleep(speed);
			if (crush_on&&check_crush(bx, by + 1, b_rotation) == false) Sleep(100);

		}
		drop_block();

		check_game_over();
		if (new_block_on == 1){
			gotoxy(STATUS_X_ADJ,STATUS_Y_SCORE+8);
			new_block();
		}
	}
}

void title(void) {
	int x = 5; //
	int y = 4; //


	gotoxy(x, y + 0); printf("┌──────────────────────────┐");
	gotoxy(x, y + 1); printf("│                          │");
	gotoxy(x, y + 2); printf("│                          │");
	gotoxy(x, y + 3); printf("│                          │");
	gotoxy(x, y + 4); printf("└──────────────────────────┘");

	gotoxy(x + 5, y + 2); printf("T E T R I S");
	gotoxy(x, y + 7); printf("Please Enter Any Key to Start..");

	gotoxy(x, y + 9); printf("<Instruction>              ");
	gotoxy(x, y + 10); printf("  W   : Shift             ");
	gotoxy(x, y + 11); printf("A   D : Left / Right Right");
	gotoxy(x, y + 12); printf("  S   : Soft Drop         ");
	gotoxy(x, y + 13); printf("Space : Hard Drop         ");
	gotoxy(x, y + 15); printf("  P  : Pause              ");
	gotoxy(x, y + 16); printf(" ESC : Quit               ");

	while (true) {
		if (_kbhit()) break;
	}

	while (_kbhit()) _getch();

}

void reset(void) {

	p1_score = 0;
	key = 0;
	crush_on = 0;
	p1_turn = 0;
	speed = 100;
	timer = time(NULL);
	system("cls");
	reset_main();
	draw_map();
	draw_main();

	b_type_next = rand() % 7;
	new_block();

}

void reset_main(void) {
	int i, j;

	for (i = 0; i<MAIN_Y; i++) { // Game board initialize
		for (j = 0; j<MAIN_X; j++) {
			main_org[i][j] = 0;
			main_cpy[i][j] = 100;
		}
	}
	for (j = 1; j<MAIN_X; j++) {
		main_org[3][j] = CEILLING;
	}
	for (i = 1; i<MAIN_Y - 1; i++) {
		main_org[i][0] = WALL;
		main_org[i][MAIN_X - 1] = WALL;
	}
	for (j = 0; j<MAIN_X; j++) {
		main_org[MAIN_Y - 1][j] = WALL;
	}
}

void reset_main_cpy(void) {
	int i, j;

	for (i = 0; i<MAIN_Y; i++) {
		for (j = 0; j<MAIN_X; j++) {
			main_cpy[i][j] = 100;
		}
	}
}

void draw_map(void) { // Game Status
	int y = 3;

	gotoxy(STATUS_X_ADJ, y + 2); printf("+-  N E X T  -+ ");
	gotoxy(STATUS_X_ADJ, y + 3); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 4); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 5); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 6); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 7); printf("+-- -  -  - --+ ");
	gotoxy(STATUS_X_ADJ, y + 8); printf("Player_score :");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("               %6d", p1_score);
	//gotoxy(STATUS_X_ADJ, y + 10); printf("         %6d items", p1_item-p1_used_item);
	gotoxy(STATUS_X_ADJ, y + 12);
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),6);
		printf("<Player Key>");
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
	}
	gotoxy(STATUS_X_ADJ, y + 13); printf("  W   : Shift       ");
	gotoxy(STATUS_X_ADJ, y + 14); printf("A   D : Left / Right");
	gotoxy(STATUS_X_ADJ, y + 15); printf("  S   : Soft Drop   ");
	//gotoxy(STATUS_X_ADJ, y + 16); printf("  F   : Use Item    ");
	gotoxy(STATUS_X_ADJ,STATUS_Y_SCORE+8);
	gotoxy(STATUS_X_ADJ, y + 19); printf("    P  : Pause");
	gotoxy(STATUS_X_ADJ, y + 20); printf("   ESC : Quit");
	//   P   : Pause
	//  ESC  : Quit

}

void draw_main(void) {
	int i, j;

	for (j = 1; j<MAIN_X - 1; j++) {
		if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
	}
	for (i = 0; i<MAIN_Y; i++) {
		for (j = 0; j<MAIN_X; j++) {
			if (main_cpy[i][j] != main_org[i][j]) {

				gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);
				switch (main_org[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case CEILLING:
					printf(". ");
					break;
				case WALL:
					printf("▦");
					break;
				case INACTIVE_BLOCK:
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),6);
					printf("□");
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
					break;
				case ACTIVE_BLOCK:
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),6);
					printf("■");
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
					break;
				}
			}
		}
	}
	for (i = 0; i<MAIN_Y; i++) {
		for (j = 0; j<MAIN_X; j++) {
			main_cpy[i][j] = main_org[i][j];
		}
	}
}

void new_block(void) {
	int i, j;

	bx = (MAIN_X / 2) - 1;
	by = 0;
	b_type = b_type_next;
	b_type_next = rand() % 7;
	b_rotation = 0;

	new_block_on = 0;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1){
				main_org[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
	}
	for (i = 1; i<3; i++) {
		for (j = 0; j<4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),6);
				printf("■");
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void check_key(void) {
	key = 0;

	if (_kbhit()) {
		key = _getch();
		
		switch (key) {

		case 'P':
		case 'p':
			pause();
			break;
		case ESC:
			system("cls");
			exit(0);
		//case 'F':
		//case 'f':
		//	use_item(1);
		//	break;
		case 'A':
		case 'a':
			if (check_crush(bx - 1, by, b_rotation) == true) move_block(LEFT);
			break;
		case 'D':
		case 'd':
			if (check_crush(bx + 1, by, b_rotation) == true) move_block(RIGHT);
			break;
		case 'S':
		case 's':
			if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
			break;
		case 'w':
		case 'W':
			if (check_crush(bx, by, (b_rotation + 1) % 4) == true) move_block(UP);

			else if (crush_on == 1 && check_crush(bx, by - 1, (b_rotation + 1) % 4) == true) move_block(100);
		}
	}
	while (_kbhit()) _getch();
}


int check_crush(int bx, int by, int b_rotation) {
	int i, j;

	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1 && main_org[by + i][bx + j]>0) return false;
		}
	}
	return true;
};

void drop_block(void) {
	int i, j;

	if (crush_on&&check_crush(bx, by + 1, b_rotation) == true) crush_on = 0;
	if (crush_on&&check_crush(bx, by + 1, b_rotation) == false) {
		for (i = 0; i<MAIN_Y; i++) {
			for (j = 0; j<MAIN_X; j++) {
				if (main_org[i][j] == ACTIVE_BLOCK){
					main_org[i][j] = INACTIVE_BLOCK;
				}
			}
		}
		crush_on = 0;
		check_line();
		new_block_on = 1;
		/*if(turn_item_used!=0) use_item_pause();
		return;*/
	}
	if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
	if (check_crush(bx, by + 1, b_rotation) == false) crush_on++;
}


void move_block(int dir) {
	int i, j;

	switch (dir) {
	case LEFT:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1){
					main_org[by + i][bx + j-1] = ACTIVE_BLOCK;
				}
			}
		}
		bx--;
		break;

	case RIGHT:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1){
					main_org[by + i][bx + j+1] = ACTIVE_BLOCK;
				}
			}
		}
		bx++;
		break;

	case DOWN:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1){
					main_org[by + i + 1][bx + j] = ACTIVE_BLOCK;
				}
			}
		}
		by++;
		break;

	case UP:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1){
					main_org[by + i][bx + j] = ACTIVE_BLOCK;
				}
			}
		}
		break;

	case 100:

		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1){
					main_org[by + i - 1][bx + j] = ACTIVE_BLOCK;
				}
			}
		}
		by--;
		break;
	}
}

void check_line(void) {
	int i, j, k, l;

	int block_amount; // block num of one line
	int count=0;

	for (i = MAIN_Y - 2; i>3;) {
		block_amount = 0;
		for (j = 1; j<MAIN_X - 1; j++) {
			if (main_org[i][j]>0) block_amount++;
		}
		if (block_amount == MAIN_X - 2) {
			count++;
			p1_score += 1 ;

			for (k = i; k>1; k--) {  // Block Down
				for (l = 1; l<MAIN_X - 1; l++) {
					if (main_org[k - 1][l] != CEILLING) main_org[k][l] = main_org[k - 1][l];
					if (main_org[k - 1][l] == CEILLING) main_org[k][l] = EMPTY;

				}
			}
		}
		else i--;
	}
	//if(count>=2){
	//	p1_item++;
	//}
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("               %6d", p1_score);
	//gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE+1); printf("         %6d items", p1_item-p1_used_item);
}



void check_game_over(void) {
	int i;
	FILE *fo;
	int x = 5;
	int y = 5;
	
   	struct tm *t;
   	t = localtime(&timer);
	for (i = 1; i<MAIN_X - 2; i++) {
		if (main_org[3][i]>0||p1_turn>=30) {  // end condition
			gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			gotoxy(x, y + 1); printf("▤                              ▤");
			gotoxy(x, y + 2); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 3); printf("▤  |  G A M E  O V E R..   |   ▤");
			gotoxy(x, y + 4); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 5); printf("▤   P1   SCORE: %6d         ▤", p1_score);
			gotoxy(x, y + 8); printf("▤  Press any key to restart..  ▤");
			gotoxy(x, y + 9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			Sleep(1000);
			while (_kbhit()) _getch();
			key = _getch();
			reset();
		}
	}
}

void pause(void) {
	int i, j;

	int x = 5;
	int y = 5;
	
	gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	gotoxy(x, y + 1); printf("▤                              ▤");
	gotoxy(x, y + 2); printf("▤  +-----------------------+   ▤");
	gotoxy(x, y + 3); printf("▤  |       P A U S E       |   ▤");
	gotoxy(x, y + 4); printf("▤  +-----------------------+   ▤");
	gotoxy(x, y + 5); printf("▤  Press any key to resume..   ▤");
	gotoxy(x, y + 6); printf("▤                              ▤");
	gotoxy(x, y + 7); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");

	_getch();

	system("cls");
	reset_main_cpy();
	draw_main();
	draw_map();

	for (i = 1; i<3; i++) {
		for (j = 0; j<4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),2);
				printf("■");
				SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
			}
			else {
				gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void use_item(int player){
	if(turn_item_used==0){
		if(player==1&&p1_item-p1_used_item>0){
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),6);
			gotoxy(STATUS_X_ADJ+9,STATUS_Y_SCORE+8);
			printf("Item!");
			p1_used_item++;
			turn_item_used=1;
		}
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
	}
}

void use_item_pause(void){
	int i, j;
	
	int x = 5;
	int y = 5;
	int t0=time(NULL);
	int t1;
	int input;
	{	
		gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		gotoxy(x, y + 1); printf("▤                                                  ▤");
		gotoxy(x, y + 2); printf("▤              <Item used!!>                       ▤");
		gotoxy(x, y + 3); printf("▤  Choose next block!                              ▤");
		gotoxy(x, y + 4); printf("▤  1. ■     2. ■■    3.  ■      4. ■          ▤");
		gotoxy(x, y + 5); printf("▤     ■        ■■      ■■■       ■■        ▤");
		gotoxy(x, y + 6); printf("▤     ■                                 ■        ▤");
		gotoxy(x, y + 7); printf("▤     ■                                           ▤");
		gotoxy(x, y + 8); printf("▤                                                  ▤");
		gotoxy(x, y + 9); printf("▤                                                  ▤");
		gotoxy(x, y + 10); printf("▤                                                  ▤");
		gotoxy(x, y + 11); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	}
	while(!_kbhit()&&((t1=time(NULL))-t0)<5){
		gotoxy(x+3,y+10); printf("%d seconds left.",5-(t1-t0));
	}
	input=_getch();
	while (_kbhit()) _getch();
	switch(input){
		case '1':
		
			b_type_next=1;
			break;
		case '2':
		
			b_type_next=0;
			break;
		case '3':
		
			b_type_next=6;
			break;
		case '4':
			b_type_next=3;
			break;
	}
	turn_item_used=0;
	//다시 원래 걸로
	{
		system("cls");
		reset_main_cpy();
		draw_main();
		draw_map();

		for (i = 1; i<3; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type_next][0][i][j] == 1) {
					gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),2);
					printf("■");
					SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),7);
				}
				else {
					gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
					printf("  ");
				}
			}
		}
	}
}