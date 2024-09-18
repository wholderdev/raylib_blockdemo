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
	enum TeamID owner;
} Square;

typedef struct Ball {
	Vector2 position;
	Vector2 speed;
	int radius;
	enum TeamID owner;
} Ball;

typedef struct BallList {
	Ball* ball;
	struct BallList* next;
	struct BallList* prev;
} BallList;

static const Color ballOutline = DARKGRAY;//BLACK;

static const int screenWidth = 1600;
static const int screenHeight = 800;

static const int gridMinX = (screenWidth/2) - ((SQUARE_SIZE * SQUARE_ROWS)/2);
static const int gridMaxX = gridMinX + SQUARE_SIZE * SQUARE_COLUMNS;

static const int gridMinY = (screenHeight/2) - ((SQUARE_SIZE * SQUARE_COLUMNS)/2);
static const int gridMaxY = gridMinY + SQUARE_SIZE * SQUARE_COLUMNS;

static const int defaultRadius = 2;
static const int defaultOutline = 3;

static Square squares[SQUARE_ROWS][SQUARE_COLUMNS] = { 0 };
static BallList* bl = { 0 };

static BallList* CreateBallList(void);
static Ball* CreateBall(void);
static void RemoveBallListNode(BallList* curBL);
static void DestroyBallList(void);
static void DestroyBall(Ball* ball);

static void InitGame(void);
static int UpdateBall(Ball* ball, float delta);
static void UpdateGame(void);

static Color GetTeamColor(enum TeamID owner);
static void DrawBalls(void);
static void DrawGame(void);

int main(void)
{
	InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
	SetTargetFPS(60);
	
	InitGame();
	DrawGame();
	while (!WindowShouldClose())
	{
		UpdateGame();
		DrawGame();
	}
	CloseWindow();
	DestroyBallList();

	return 0;
}

//	Game Items
BallList* CreateBallList(void)
{
	BallList* toRet = (struct BallList*)malloc(sizeof(struct BallList));
	toRet->ball = NULL;
	toRet->next = NULL;
	toRet->prev = NULL;
	return toRet;
}

//	Not sure why I don't want to send params for struct vals, but I think it might be better separate
//Ball CreateBall(Vector2 position, Vector2 speed, int radius, enum TeamID owner)
Ball* CreateBall(void)
{
	return (struct Ball*)malloc(sizeof(struct Ball));
}

void RemoveBallListNode(BallList* curBL)
{
	if (bl = curBL) {
		bl = curBL->next;
	}

	if (curBL->prev && curBL->next)
	{
		BallList* temp = curBL->prev;
		temp->next = curBL->next;
		temp->next->prev = temp;
	}
	else if (curBL->prev)
	{
		curBL->prev->next = curBL->next;
	}
	else if (curBL->next)
	{
		curBL->next->prev = curBL->prev;
	}
	//curBL->prev = NULL;
	//curBL->next = NULL;
	free(curBL);
}

//	TODO: Do some refactoring to make it obvious it deletes the entire list and not just a node
//		Tecnically you can figure it out because it doesn't say Node, but should probably be clear
void DestroyBallList(void)
{
	BallList* preBL = bl;
	BallList* curBL = preBL;
	while (curBL)
	{
		DestroyBall(curBL->ball);
		curBL = curBL->next;
		free(preBL);
		preBL = curBL;
	}
}

void DestroyBall(Ball* ball)
{
	free(ball);
}

//	Game Logic
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

	//bl = CreateBallList();
	BallList* curBL = CreateBallList();
	BallList* newBL = NULL;
	bl = curBL;
	Ball* newBall = CreateBall();
	
	int startBalls = 5;
	for (int i = 1; i < startBalls; i++)
	{
		newBall = CreateBall();
		newBall->position = (Vector2){ gridMinX + (10 * i), gridMinY + (10 * i) };
		newBall->speed = (Vector2){ 100 + (i * SQUARE_SIZE), 120 };
		newBall->radius = defaultRadius;
		newBall->owner = (enum TeamID)(i % 5);
		curBL->ball = newBall;

		newBL = CreateBallList();
		newBL->prev = curBL;

		curBL->next = newBL;
		curBL = curBL->next;
	}

	newBall = CreateBall();
	newBall->position = (Vector2){ gridMinX + (startBalls * SQUARE_SIZE), gridMinY + (10 * startBalls) };
	newBall->speed = (Vector2){ 100, 120 };
	newBall->radius = defaultRadius;
	newBall->owner = (enum TeamID)(startBalls % 5);
	curBL->ball = newBall;
}

int UpdateBall(Ball* ball, float delta)
{
	ball->position.x += ball->speed.x * delta;
	ball->position.y += ball->speed.y * delta;

	if ( (ball->position.x > gridMaxX) && (ball->speed.x > 0) )
	{
			ball->speed.x *= -1;
			ball->position.x -= (ball->position.x - (float)gridMaxX) * 2;
	}
	else if ( (ball->position.x < gridMinX) && (ball->speed.x < 0) )
	{
			ball->speed.x *= -1;
			ball->position.x += ((float)gridMinX - ball->position.x) * 2;
	}

	if ( (ball->position.y > gridMaxY) && (ball->speed.y > 0) )
	{
			ball->speed.y *= -1;
			ball->position.y -= (ball->position.y - (float)gridMaxY) * 2;
	}
	else if ( (ball->position.y < gridMinY) && (ball->speed.y < 0) )
	{
			ball->speed.y *= -1;
			ball->position.y += ((float)gridMinY - ball->position.y) * 2;
	}

	//	Check square underneath. If on differnt size, change to side and consume ball
	
	int sx = ((int)((ball->position.x - gridMinX) / SQUARE_SIZE));
	int sy = ((int)((ball->position.y - gridMinY) / SQUARE_SIZE));
	Square* curSquare = &squares[sx][sy];
	if (ball->owner != curSquare->owner)
	{
		printf("ball team %i != square team %i\n", ball->owner, curSquare->owner);
		curSquare->owner = ball->owner;
		printf("c ball team %i != square team %i\n", ball->owner, curSquare->owner);
		return 1;
	}	
	printf("ball team %i == square team %i\n", ball->owner, curSquare->owner);
	return 0;
}

void UpdateGame(void)
{
	float delta = GetFrameTime();

	BallList* curBL = bl;
	while (curBL)
	{
		printf("There's still a ball");
		switch(UpdateBall(curBL->ball, delta))
		{
			case 0:
				curBL = curBL->next;
				break;
			case 1:
				if (curBL != NULL && curBL->next == NULL) {
					RemoveBallListNode(curBL);
					curBL = NULL;
				}
				else
				{
					curBL = curBL->next;
					RemoveBallListNode(curBL->prev);
				}
				break;

		}
	}
}

//	Game Renders
Color GetTeamColor(enum TeamID owner)
{
	switch(owner)
	{
		case TEAM1:
			return RED;
		case TEAM2:
			return GOLD;
		case TEAM3:
			return GREEN;
		case TEAM4:
			return BLUE;
		default:
			//return BLACK;
			//return DARKGRAY;
			return RAYWHITE;
	}
}

void DrawBalls(void) {
	BallList* curBL = bl;
	while (curBL) {
		DrawCircleV(curBL->ball->position, curBL->ball->radius + defaultOutline, ballOutline);
		DrawCircleV(curBL->ball->position, curBL->ball->radius, GetTeamColor(curBL->ball->owner));
		curBL = curBL->next;
	}
}

void DrawGame(void)
{
	BeginDrawing();
	ClearBackground(BLACK);
	for (int i = 0; i < SQUARE_ROWS; i++) {
		for (int j = 0; j < SQUARE_COLUMNS; j++)
		{
			DrawRectangle(	(SQUARE_SIZE * i) + gridMinX + 1,
					(SQUARE_SIZE * j) + gridMinY + 1,
					SQUARE_SIZE - 1,
					SQUARE_SIZE - 1,
					GetTeamColor(squares[i][j].owner));
		}
	}
	DrawBalls();
	EndDrawing();
}
