#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SQUARE_ROWS	50
#define SQUARE_COLUMNS	50
#define SQUARE_SIZE	14

enum TeamID {
	TEAM1,
	TEAM2,
	TEAM3,
	TEAM4,
	NONE
};

typedef struct Square {
	int owner;
} Square;

typedef struct Ball {
	Vector2 position;
	Vector2 speed;
	int radius;
	enum TeamID owner;
} Ball;

static const int screenWidth = 1600;
static const int screenHeight = 800;
static const int bufferWidth = (screenWidth/2) - ((SQUARE_SIZE * SQUARE_ROWS)/2);
static const int bufferHeight = (screenHeight/2) - ((SQUARE_SIZE * SQUARE_COLUMNS)/2);
static const int gridRightBound = bufferWidth + SQUARE_SIZE * SQUARE_COLUMNS;
static const int gridBottomBound = bufferHeight + SQUARE_SIZE * SQUARE_COLUMNS;

static Square squares[SQUARE_ROWS][SQUARE_COLUMNS] = { 0 };
static Ball tball = { 0 };

Color GetTeamColor(enum TeamID owner);

static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);

int main(void)
{
	InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
	InitGame();
	DrawGame();

	while (!WindowShouldClose())
	{
		UpdateGame();
		DrawGame();
	}

	CloseWindow();

	return 0;
}

Color GetTeamColor(enum TeamID owner)
{
	switch(owner)
	{
		case TEAM1:
			return RED;
		case TEAM2:
			return YELLOW;
		case TEAM3:
			return GREEN;
		case TEAM4:
			return BLUE;
		default:
			return DARKGRAY;
	}
}

void InitGame(void)
{
	for (int i = 0; i < SQUARE_ROWS/2; i++) {
		for (int j = 0; j < SQUARE_COLUMNS/2; j++) {
			squares[i][j].owner = TEAM1;
		}
	}
	for (int i = SQUARE_ROWS/2; i < SQUARE_ROWS; i++) {
		for (int j = 0; j < SQUARE_COLUMNS/2; j++) {
			squares[i][j].owner = TEAM2;
		}
	}
	for (int i = 0; i < SQUARE_ROWS/2; i++) {
		for (int j = SQUARE_COLUMNS/2; j < SQUARE_COLUMNS; j++) {
			squares[i][j].owner = TEAM3;
		}
	}
	for (int i = SQUARE_ROWS/2; i < SQUARE_ROWS; i++) {
		for (int j = SQUARE_COLUMNS/2; j < SQUARE_COLUMNS; j++) {
			squares[i][j].owner = TEAM4;
		}
	}

	tball.position = (Vector2){ bufferWidth + 10, bufferHeight + 10 };
	tball.speed = (Vector2){ 1, 3 };
	tball.radius = 5;
	tball.owner = NONE;
}

void UpdateGame(void)
{
	tball.position.x += tball.speed.x;
	tball.position.y += tball.speed.y;

	if (tball.position.x > gridRightBound || tball.position.x < bufferWidth) {
		tball.speed.x *= -1;
		tball.position.x += tball.speed.x * 2;
	}
	if (tball.position.y > gridBottomBound || tball.position.y < bufferHeight) {
		tball.speed.y *= -1;
		tball.position.y += tball.speed.y * 2;
	}
}

void DrawGame(void)
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	for (int i = 0; i < SQUARE_ROWS; i++) {
		for (int j = 0; j < SQUARE_COLUMNS; j++)
		{
			DrawRectangle(	(SQUARE_SIZE * i) + bufferWidth + 1,
					(SQUARE_SIZE * j) + bufferHeight + 1,
					SQUARE_SIZE - 1,
					SQUARE_SIZE - 1,
					GetTeamColor(squares[i][j].owner));
		}
	}
	DrawCircleV(tball.position, tball.radius, GetTeamColor(tball.owner));

	EndDrawing();
}
