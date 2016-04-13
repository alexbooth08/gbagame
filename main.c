#include "main.h"
#include <stdlib.h>
#include <stdio.h>
static int framecounter = 0;
static int score = 0;
static u16 colors[] = {RED, GREEN, BLUE};


static enum GBAState state = START_SCENE;
static unsigned short LAST_A = 0;
static unsigned short LAST_B = 0;
static unsigned short LAST_SELECT = 0;
static unsigned short LAST_START = 0;


int main() {
	char* welcome;
	char* start;
	//char* scoreString;
	char welcomeletter[25];
	char startletter[17];
	int goalcounter = 0;

	REG_DISPCTL = MODE3 | BG2_ENABLE;


	MAINCHAR mainChar = {40, 40, 0, 0, 10, BLACK};
	MAINCHAR oldChar = mainChar;

	int nextcolor = 0;

	GOAL goal1 = {0, 0, 0, 0, 0, 0, 0, WHITE};
	GOAL goal2 = {0, 0, 0, 0, 0, 0, 0, WHITE};
	GOAL goal3 = {0, 0, 0, 0, 0, 0, 0, WHITE};
	GOAL goal4 = {0, 0, 0, 0, 0, 0, 0, WHITE};
	GOAL goal5 = {0, 0, 0, 0, 0, 0, 0, WHITE};

	GOAL goals[GOALSSIZE] = {goal1, goal2, goal3, goal4, goal5};
	GOAL oldgoals[GOALSSIZE];
	GOAL *goalpointer = goals;
	int tutorialdoneA = 0;
	int tutorialdoneB = 0;

	while(1) {
		waitForVblank();
		switch(state) {
			case START_SCENE:
				drawImage3(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, startscreen);
				welcome = "Welcome to my geemboigeeme";
				for (int i = 0; welcome[i]; i++){
					welcomeletter[i] = welcome[i];
					drawString(8, 0, welcomeletter, BLACK);
					delay(1);
				}


				start = "Press ENTER to start!";
				for (int i = 0; start[i]; i++){
					startletter[i] = start[i];
					drawString(140, 0, startletter, BLACK);
					delay(1);
				}
				state = START_SCENE_NODRAW;
				break;

			case START_SCENE_NODRAW:
				if (KEY_DOWN_NOW(BUTTON_START) && !LAST_START) {
					state = MAIN_SCENE_SETUP;
				}
				break;

			case MAIN_SCENE_SETUP:
				// drawImage3(0, 0, GAMESCREEN_WIDTH, GAMESCREEN_HEIGHT, gamescreen);
				fillScreen(WHITE);
				drawImage3(HEIGHTMAX + 1, 0, GROUND_WIDTH, GROUND_HEIGHT, ground);
				//trackers reset
				nextcolor = 0;
				goalpointer = goals;
				framecounter = 0;
				goalcounter = 0;
				score = 0;
				tutorialdoneA = 0;
				tutorialdoneB = 0;
				drawString(0, 0, "0", BLACK);
				drawString(8, 0, "PRESS LEFT/RIGHT TO MOVE", BLACK);
				drawString(16, 0, "PRESS B TO CHANGE COLORS", BLACK);
				//mainChar reset
				mainChar.row = 40;
				mainChar.col = 40;
				mainChar.color = BLACK;
				mainChar.size = 10;
				mainChar.deltar = 0;
				mainChar.deltac = 0;
				//oldChar reset
				oldChar = mainChar;
				oldChar.color = WHITE;
				//goal reset
				goals[0].set = 0;
				goals[1].set = 0;
				goals[2].set = 0;
				goals[3].set = 0;
				goals[4].set = 0;
				goals[0].row = 0;
				goals[1].row = 0;
				goals[2].row = 0;
				goals[3].row = 0;
				goals[4].row = 0;
				goals[0].col = 0;
				goals[1].col = 0;
				goals[2].col = 0;
				goals[3].col = 0;
				goals[4].col = 0;

				state = MAIN_SCENE;
				break;

			case MAIN_SCENE:
				waitForVblank();
				if (KEY_DOWN_NOW(BUTTON_SELECT && !LAST_SELECT)) {
					state = START_SCENE;
				}

				//update counters
				framecounter++;
				goalcounter++;

				//set oldChar to previous version of mainChar before mainChar gets updated. Updates old goals too.
				oldChar = mainChar;
				for (int i = 0; i < GOALSSIZE; i++) {
					oldgoals[i] = goals[i];
				}

				//check goals
				if (goalcounter >= 45 && tutorialdoneA && tutorialdoneB) {
					if (!goalpointer->set) {
						goalpointer->set = 1;
						goalpointer->height = 15 + rand()%16;
						//goalpointer->width = 10 + rand()%16;
						goalpointer->width = 5;
						goalpointer->color = colors[rand()%3];
						goalpointer->row = HEIGHTMAX - 4;
						goalpointer->col = (240 - goalpointer->height);
						goalpointer->deltac = -1*(rand()%2 + 1);
						goalpointer->deltar = 0;
						goalpointer++;
						if (goalpointer == &goals[GOALSSIZE]) {
							goalpointer = goals;
						}
					}
					goalcounter = 0;
				}

				//control mechanism
				if (KEY_DOWN_NOW(BUTTON_LEFT) && !KEY_DOWN_NOW(BUTTON_RIGHT)) {
					mainChar.deltac = -2;
					if (!tutorialdoneA) {
						drawString(8,0, "PRESS LEFT/RIGHT TO MOVE", WHITE);
						tutorialdoneA = 1;
					}
				} else if (KEY_DOWN_NOW(BUTTON_RIGHT) && !KEY_DOWN_NOW(BUTTON_LEFT)) {
					mainChar.deltac = 2;
					if (!tutorialdoneA) {
						drawString(8,0, "PRESS LEFT/RIGHT TO MOVE", WHITE);
						tutorialdoneA = 1;
					}
				} else {
					mainChar.deltac = 0;
				}

				if (KEY_DOWN_NOW(BUTTON_B) && !LAST_B) {
					mainChar.color = colors[nextcolor];
					nextcolor++;
					if (nextcolor == 3) {
						nextcolor = 0;
					}
					if (!tutorialdoneB) {
						drawString(16,0, "PRESS B TO CHANGE COLORS", WHITE);
						tutorialdoneB = 1;
					}
				}

				//gravity
				if (framecounter >= 8) {
					mainChar.deltar += GRAVITY;
					// if (mainChar.deltar >= 10) {
					// 	mainChar.deltar = 10;
					// }
					framecounter = 0;
				}

				//update objects
				for (int i = 0; i < GOALSSIZE; i++) {
					// if (goals[i].set) {
					updateGoal(&goals[i]);
					// }
				}
				updateChar(&mainChar, goals);
				//drawing
				for (int i = 0; i < GOALSSIZE; i++) {
					GOAL* oldcur = oldgoals + i;
					if (oldcur->set) {
						drawRect(oldcur->row,
								oldcur->col,
								oldcur->width,
								oldcur->height,
								WHITE);
					}
				}
				for (int i = 0; i < GOALSSIZE; i++) {
					GOAL* cur = goals + i;
					if (cur->set && cur->color != BLACK) {
						drawRect(cur->row,
								cur->col,
								cur->width,
								cur->height,
								cur->color);
					}
				}

				//drawing
				drawRect(oldChar.row, oldChar.col, oldChar.size, oldChar.size, WHITE);
				drawRect(mainChar.row, mainChar.col, mainChar.size, mainChar.size, mainChar.color);
				// drawHollowRect(mainChar.row, mainChar.col, mainChar.size, mainChar.size, WHITE);
				// if (KEY_DOWN_NOW(BUTTON_A) && !pressed) {
				// 	state = END_SCENE;
				// }

				break;

			case END_SCENE:
				drawImage3(0, 0, ENDSCREEN_WIDTH, ENDSCREEN_HEIGHT, endscreen);
				char scoreString[50];
				sprintf(scoreString,"It's game over fam. Your score was %d.", score);
				drawString(8, 8, scoreString, GREEN);
				drawString(110, 50, "Press START to RESTART", WHITE);
				drawString(130, 50, "Press SELECT for MAIN MENU", WHITE);
				state = END_SCENE_NODRAW;
				break;

			case END_SCENE_NODRAW:
				if (KEY_DOWN_NOW(BUTTON_SELECT) && !LAST_SELECT)
					state = START_SCENE;
				if (KEY_DOWN_NOW(BUTTON_START) && !LAST_START)
					state = MAIN_SCENE_SETUP;
				break;
		}
		LAST_A = KEY_DOWN_NOW(BUTTON_A);
		LAST_B = KEY_DOWN_NOW(BUTTON_B);
		LAST_SELECT = KEY_DOWN_NOW(BUTTON_SELECT);
		LAST_START = KEY_DOWN_NOW(BUTTON_START);
	}
	// for (int i = 0; i < STARTSCREEN_SIZE; i++){
	// 	videoBuffer[i] = startscreen[i];
	// }
	//drawImage3(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_WIDTH, startscreen);
	//drawImage3(0, 0, GAMESCREEN_WIDTH, GAMESCREEN_HEIGHT, gamescreen);
	//drawString(8, 0, "You are now playing Arihan's Game!", BLACK);
	//drawString(8, 0, "Welcome to Arihan's Game!", BLACK);
	return 0;
}

void updateChar(MAINCHAR* mainChar, GOAL* goals) {
	//update mainChar
	mainChar->row = mainChar->row + mainChar->deltar;
	mainChar->col = mainChar->col + mainChar->deltac;
	if (mainChar->row + mainChar->size > HEIGHTMAX + 1) {
		mainChar->row = HEIGHTMAX - mainChar->size + 1;
		mainChar->deltar = -mainChar->deltar;
		framecounter = 0;
		//state = END_SCENE;
	}

	for (int i = 0; i < GOALSSIZE; i++) {
		GOAL* cur = goals + i;
		if (checkCollisions(mainChar, cur)) {
			if (cur->color == mainChar->color && cur->color != WHITE && cur->set) {
				cur->color = WHITE;
				cur->set = 0;
				cur->col = 0;
				cur->row = 0;
				score++;
				char buffer[10];
				sprintf(buffer, "%i", score - 1);
				drawString(0, 0, buffer, WHITE);
				sprintf(buffer, "%i", score);
				drawString(0, 0, buffer, BLACK);
			} else {
				state = END_SCENE;
			}
		}

	}

	if (mainChar->col <= 0) {
		mainChar->col = 0;
		mainChar->deltac = 0;
	} if (mainChar->col + mainChar->size >= 240) {
		mainChar->col = 240 - mainChar->size;
		mainChar->deltac = 0;
	}
}

int checkCollisions(MAINCHAR* mainChar, GOAL* cur) {
	return (cur->set && ((((mainChar->row + mainChar->size) >= cur->row) && (mainChar->col < cur->col) && ((mainChar->col + mainChar->size) > cur->col))
	|| (((mainChar->row + mainChar->size) >= cur->row) && (mainChar->col < (cur->col + cur->height)) && ((mainChar->col + mainChar->size) > (cur->col + cur->height)))
	|| (((mainChar->row + mainChar->size) >= cur->row) && (mainChar->col >= cur->col) && ((mainChar->col + mainChar->size) < (cur->col + cur->height)))));
}

void updateGoal(GOAL *goal) {
	goal->row = goal->row + goal->deltar;
	goal->col = goal->col + goal->deltac;
	if (goal->col <= 0) {
		goal->set = 0;
	}
}
