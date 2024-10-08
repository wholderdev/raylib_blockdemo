#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DEBUG		false
#define SQUARE_ROWS	50
#define SQUARE_COLUMNS	50
#define SQUARE_SIZE	14

enum TeamID {
	TEAM1,
	TEAM2,
	TEAM3,
	TEAM4,
	NOTEAM
};

typedef struct Square {
	enum TeamID owner;
} Square;

typedef struct Ball {
	Vector2 position;
	Vector2 velocity;
	int health;
	int radius;
	enum TeamID owner;
} Ball;

typedef struct BallList {
	Ball* ball;
	struct BallList* next;
	struct BallList* prev;
} BallList;

typedef struct Launcher {
	Vector2 position;
	//Vector2 velocity;
	float spawnSpeed;
	float spawnAngle;
	float lastRelease;
	enum TeamID owner;
} Launcher;

typedef struct LauncherList {
	Launcher* launcher;
	struct LauncherList* next;
	struct LauncherList* prev;
} LauncherList;

typedef struct Plinko {
	Vector2 minLoc;
	Vector2 maxLoc;
	enum TeamID owner;
} Plinko;

typedef struct PlinkoList {
	Plinko* plinko;
	struct PlinkoList* next;
	struct PlinkoList* prev;
} PlinkoList;

static const Color ballOutline = DARKGRAY;//BLACK;

static const int screenWidth = 1600;
static const int screenHeight = 800;

static const int gridMinX = (screenWidth/2) - ((SQUARE_SIZE * SQUARE_ROWS)/2);
static const int gridMaxX = gridMinX + SQUARE_SIZE * SQUARE_COLUMNS;

static const int gridMinY = (screenHeight/2) - ((SQUARE_SIZE * SQUARE_COLUMNS)/2);
static const int gridMaxY = gridMinY + SQUARE_SIZE * SQUARE_COLUMNS;

static const int defaultRadius = 2;
static const int defaultOutline = 3;
static const int defaultHealth = 4;

static Square squares[SQUARE_ROWS][SQUARE_COLUMNS] = { 0 };
static BallList* startBL = NULL;
static LauncherList* startLL = NULL;
static PlinkoList* startPL = NULL;

//	Temporary ball spawner test
static float spawnInterval = 1.0f;
//	---------------------------

static void PrintBalls(void);

static BallList* CreateBallList(void);
static Ball* CreateBall(float px, float py, float sx, float sy, int health, int radius, enum TeamID owner);
static LauncherList* CreateLauncherList(void);
static Launcher* CreateLauncher(float px, float py, float speed, float angle, float lastRelease, enum TeamID owner);
static PlinkoList* CreatePlinkoList(void);
static Plinko* CreatePlinko(Vector2 minLoc, Vector2 maxLoc, enum TeamID owner);

static void AddBall(Ball* newBall);
static void AddLauncher(Launcher* newLauncher);
static void AddPlinko(Plinko* newPlinko);

static void RemoveBallListNode(BallList* curBL);
static void RemoveLauncherListNode(LauncherList* curLL);
static void RemovePlinkoListNode(PlinkoList* curPL);

static void DestroyBallList(void);
static void DestroyBall(Ball* ball);

static void DestroyLauncherList(void);
static void DestroyLauncher(Launcher* launcher);

static void DestroyPlinkoList(void);
static void DestroyPlinko(Plinko* plinko);

static void InitGame(float curTime, float delta);

static int UpdateBall(Ball* ball, float delta);
static void UpdateBallList(float delta);

static int UpdateLauncher(Launcher* launcher, float curTime, float delta);
static void UpdateLauncherList(float curTime, float delta);

static int UpdatePlinko(Plinko* plinko, float curTime, float delta);
static void UpdatePlinkoList(float curTime, float delta);

static void UpdateGame(float curTime, float delta);

static Color GetTeamColor(enum TeamID owner);

static void DrawBalls(void);
static void DrawLaunchers(float curTime, float delta);
static void DrawPlinkos(float curTime, float delta);

static void DrawGame(float curTime, float delta);

int main(void)
{
	InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
	SetTargetFPS(60);
	
	float curTime = GetTime();
	float delta = GetFrameTime();
	InitGame(curTime, delta);
	DrawGame(curTime, delta);
	while (!WindowShouldClose())
	{
		curTime = GetTime();
		delta = GetFrameTime();

		UpdateGame(curTime, delta);
		DrawGame(curTime, delta);
	}
	CloseWindow();
	DestroyBallList();

	return 0;
}

void PrintBalls(void)
{
	if (!DEBUG)
	{
		return;
	}
	printf("+++++++++++++\n");
	if (!startBL)
	{
		printf("No Balls\n");
		usleep(1000000);
	}
	BallList* curBL = startBL;
	Ball* ball = NULL;
	int i = 0;
	while (curBL)
	{
		ball = curBL->ball;
		printf("Ball %i\n", i);
		printf("\tball.position(%f, %f)\n", ball->position.x, ball->position.y);
		printf("\tball.velocity(%f, %f)\n", ball->velocity.x, ball->velocity.y);
		printf("\tball.radius(%i)\n", ball->radius);
		printf("\tball.owner(%i)\n", ball->owner);
		if (!curBL->next) {
			printf("\tNo curBL->next\n");
		}
		if (!curBL->prev) {
			printf("\tNo curBL->prev\n");
	}
		curBL = curBL->next;
		i++;
	}
	printf("-------------\n");
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

Ball* CreateBall(float px, float py, float sx, float sy, int health, int radius, enum TeamID owner)
{
	Ball* toRet = (struct Ball*)malloc(sizeof(struct Ball));
	toRet->position = (Vector2){ px, py };
	toRet->velocity = (Vector2){ sx, sy };
	toRet->health = health;
	toRet->radius = radius;
	toRet->owner = owner;
	return toRet;
}

LauncherList* CreateLauncherList(void)
{
	LauncherList* toRet = (struct LauncherList*)malloc(sizeof(struct LauncherList));
	toRet->launcher = NULL;
	toRet->next = NULL;
	toRet->prev = NULL;
	return toRet;
}

Launcher* CreateLauncher(float px, float py, float speed, float angle, float lastRelease, enum TeamID owner)
{
	Launcher* toRet = (struct Launcher*)malloc(sizeof(struct Launcher));
	toRet->position = (Vector2){ px, py };
	//toRet->velocity = (Vector2){ sx, sy };
	toRet->spawnSpeed = speed;
	toRet->spawnAngle = angle;
	toRet->lastRelease = lastRelease;
	toRet->owner = owner;
	return toRet;
}

PlinkoList* CreatePlinkoList(void)
{
	PlinkoList* toRet = (struct PlinkoList*)malloc(sizeof(struct PlinkoList));
	toRet->plinko = NULL;
	toRet->next = NULL;
	toRet->prev = NULL;
	return toRet;
}

Plinko* CreatePlinko(Vector2 minLoc, Vector2 maxLoc, enum TeamID owner)
{
	Plinko* toRet = (struct Plinko*)malloc(sizeof(struct Plinko));
	toRet->minLoc = minLoc;
	toRet->maxLoc = maxLoc;
	toRet->owner = owner;
	return toRet;
}

void AddBall(Ball* newBall)
{
	BallList* newBallList = CreateBallList();
	newBallList->ball = newBall;

	if (startBL != NULL)
	{
		newBallList->next = startBL;
		startBL->prev = newBallList;
	}
	startBL = newBallList;
}

void AddLauncher(Launcher* newLauncher)
{
	LauncherList* newLauncherList = CreateLauncherList();
	newLauncherList->launcher = newLauncher;

	if (startLL != NULL)
	{
		newLauncherList->next = startLL;
		startLL->prev = newLauncherList;
	}
	startLL = newLauncherList;
}

void AddPlinko(Plinko* newPlinko)
{
	PlinkoList* newPlinkoList = CreatePlinkoList();
	newPlinkoList->plinko = newPlinko;

	if (startLL != NULL)
	{
		newPlinkoList->next = startLL;
		startLL->prev = newPlinkoList;
	}
	startLL = newPlinkoList;
}

void RemoveBallListNode(BallList* curBL)
{
	if (startBL == curBL) {
		startBL = curBL->next;
	}

	if (curBL->prev && curBL->next)
	{
		BallList* temp = curBL->prev;
		temp->next = curBL->next;
		curBL->next->prev = temp;
	}
	else if (curBL->prev)
	{
		curBL->prev->next = curBL->next;
	}
	else if (curBL->next)
	{
		curBL->next->prev = curBL->prev;
	}

	free(curBL);
}

void RemoveLauncherListNode(LauncherList* curLL)
{
	if (startLL == curLL) {
		startLL = curLL->next;
	}

	if (curLL->prev && curLL->next)
	{
		LauncherList* temp = curLL->prev;
		temp->next = curLL->next;
		curLL->next->prev = temp;
	}
	else if (curLL->prev)
	{
		curLL->prev->next = curLL->next;
	}
	else if (curLL->next)
	{
		curLL->next->prev = curLL->prev;
	}

	free(curLL);
}

void RemovePlinkoListNode(PlinkoList* curPL)
{
	if (startPL == curPL) {
		startPL = curPL->next;
	}

	if (curPL->prev && curPL->next)
	{
		PlinkoList* temp = curPL->prev;
		temp->next = curPL->next;
		curPL->next->prev = temp;
	}
	else if (curPL->prev)
	{
		curPL->prev->next = curPL->next;
	}
	else if (curPL->next)
	{
		curPL->next->prev = curPL->prev;
	}

	free(curPL);
}

//	TODO: Do some refactoring to make it obvious it deletes the entire list and not just a node
//		Tecnically you can figure it out because it doesn't say Node, but should probably be clear
void DestroyBallList(void)
{
	BallList* preBL = startBL;
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

void DestroyLauncherList(void)
{
	LauncherList* preLL = startLL;
	LauncherList* curLL = preLL;
	while (curLL)
	{
		DestroyLauncher(curLL->launcher);
		curLL = curLL->next;
		free(preLL);
		preLL = curLL;
	}
}

void DestroyLauncher(Launcher* launcher)
{
	free(launcher);
}

void DestroyPlinkoList(void)
{
	PlinkoList* prePL = startPL;
	PlinkoList* curPL = prePL;
	while (curPL)
	{
		DestroyPlinko(curPL->plinko);
		curPL = curPL->next;
		free(prePL);
		prePL = curPL;
	}
}

void DestroyPlinko(Plinko* plinko)
{
	free(plinko);
}

//	Game Logic
void InitGame(float curTime, float delta)
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

	/*
	int startBalls = 10;
	for (int i = 0; i < startBalls; i++)
	{
		Ball* newBall = CreateBall(
					gridMinX + (SQUARE_SIZE * 4 * i), gridMinY + (SQUARE_SIZE * 3 * i), 
					20 + (i * SQUARE_SIZE), 120,
					defaultHealth,
					defaultRadius,
					(enum TeamID)(i % (NOTEAM + 1))
				);
		
		AddBall(newBall);
	}
	*/

	AddLauncher(CreateLauncher(gridMinX + (SQUARE_SIZE * 3), gridMinY + (SQUARE_SIZE * 3),
			//200.0, 50.0,
			200.0, 3.1415/2.0,
			curTime, TEAM1));
	AddLauncher(CreateLauncher(gridMaxX - (SQUARE_SIZE * 3), gridMinY + (SQUARE_SIZE * 3),
			//-50.0, 200.0,
			200.0, (3.1415 * 3.0)/2.0,
			curTime, TEAM2));
	AddLauncher(CreateLauncher(gridMinX + (SQUARE_SIZE * 3), gridMaxY - (SQUARE_SIZE * 3),
			//50.0, -200.0,
			200.0, (3.1415 * 7.0)/2.0,
			curTime, TEAM3));
	AddLauncher(CreateLauncher(gridMaxX - (SQUARE_SIZE * 3), gridMaxY - (SQUARE_SIZE * 3),
			//-200.0, -50.0,
			200.0, (3.1415 * 5.0)/2.0,
			curTime, TEAM4));
}

int UpdateBall(Ball* ball, float delta)
{
	ball->position.x += ball->velocity.x * delta;
	ball->position.y += ball->velocity.y * delta;

	if ( (ball->position.x > gridMaxX) && (ball->velocity.x > 0) )
	{
			ball->velocity.x *= -1;
			ball->position.x -= (ball->position.x - (float)gridMaxX) * 2;
	}
	else if ( (ball->position.x < gridMinX) && (ball->velocity.x < 0) )
	{
			ball->velocity.x *= -1;
			ball->position.x += ((float)gridMinX - ball->position.x) * 2;
	}

	if ( (ball->position.y > gridMaxY) && (ball->velocity.y > 0) )
	{
			ball->velocity.y *= -1;
			ball->position.y -= (ball->position.y - (float)gridMaxY) * 2;
	}
	else if ( (ball->position.y < gridMinY) && (ball->velocity.y < 0) )
	{
			ball->velocity.y *= -1;
			ball->position.y += ((float)gridMinY - ball->position.y) * 2;
	}

	//	Check square underneath. If on differnt size, change to side and consume ball
	
	int sx = ((int)((ball->position.x - gridMinX) / SQUARE_SIZE));
	int sy = ((int)((ball->position.y - gridMinY) / SQUARE_SIZE));
	Square* curSquare = &squares[sx][sy];
	if (ball->owner != curSquare->owner)
	{
		curSquare->owner = ball->owner;
		ball->health--;
		//printf("%i\n", ball->health);
		if (ball->health <= 0)
		{
			//printf("%i\n", ball->health);
			return 1;
		}
	}	
	return 0;
}

void UpdateBallList(float delta)
{
	BallList* curBL = startBL;
	while (curBL)
	{
		PrintBalls();
		switch(UpdateBall(curBL->ball, delta))
		{
			case 0:
				curBL = curBL->next;
				break;
			case 1:
				if (curBL->next == NULL) {
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

int UpdateLauncher(Launcher* launcher, float curTime, float delta)
{
	float timePassed = curTime - launcher->lastRelease;
	//launcher->velocity.x = 100 * cos(curTime/2);
	//launcher->velocity.y = 100 * sin(curTime/2);
	if ( timePassed > spawnInterval )
	{
		Ball* newBall = CreateBall(
					launcher->position.x, launcher->position.y, 
					100 * cos((curTime + launcher->spawnAngle)/2),
					100 * sin((curTime + launcher->spawnAngle)/2),
					//100 * cos((launcher->spawnAngle)/2),
					//100 * sin((launcher->spawnAngle)/2),
					defaultHealth,
					defaultRadius,
					launcher->owner
				);
		
		AddBall(newBall);
		launcher->lastRelease = curTime;
	}
	return 0;
}

void UpdateLauncherList(float curTime, float delta)
{
	LauncherList* curLL = startLL;
	while (curLL)
	{
		switch(UpdateLauncher(curLL->launcher, curTime, delta))
		{
			case 0:
				curLL = curLL->next;
				break;
			case 1:
				if (curLL->next == NULL) {
					RemoveLauncherListNode(curLL);
					curLL = NULL;
				}
				else
				{
					curLL = curLL->next;
					RemoveLauncherListNode(curLL->prev);
				}
				break;
		}
	}
}

int UpdatePlinko(Plinko* plinko, float curTime, float delta)
{
	return 0;
}

void UpdatePlinkoList(float curTime, float delta)
{
	//	Delete
	return;
}

void UpdateGame(float curTime, float delta)
{
	PrintBalls();
	UpdateBallList(delta);
	UpdateLauncherList(curTime, delta);
	UpdatePlinkoList(curTime, delta);
	
	//	TODO:
	//		UpdateBall should check for and update launcher states
	//		If isDead is true or some similar boolean, remove on UpdateLauncher
	//
	//		Give balls hitpoints, subtract until 0 then remove
	//		Hitpoints should be per update, not per square removed
	//			At least at the moment I guess.
	//
	//		Also maybe make a UpdateBalls function?
	//
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

void DrawBalls(void)
{
	BallList* curBL = startBL;
	while (curBL) {
		DrawCircleV(curBL->ball->position, curBL->ball->radius + defaultOutline, ballOutline);
		DrawCircleV(curBL->ball->position, curBL->ball->radius, GetTeamColor(curBL->ball->owner));
		curBL = curBL->next;
	}
}

void DrawLaunchers(float curTime, float delta)
{
	LauncherList* curLL = startLL;
	while (curLL) {
		float lineLen = curLL->launcher->spawnSpeed / 10;
		Vector2 endPos = (Vector2){
				curLL->launcher->position.x + (lineLen * cos((curTime + curLL->launcher->spawnAngle)/2)),
				curLL->launcher->position.y + (lineLen * sin((curTime + curLL->launcher->spawnAngle)/2))
			};
		Vector2 startPosIn = (Vector2){
				curLL->launcher->position.x + ((lineLen / 8) * cos((curTime + curLL->launcher->spawnAngle)/2)),
				curLL->launcher->position.y + ((lineLen / 8) * sin((curTime + curLL->launcher->spawnAngle)/2))
			};
		Vector2 endPosIn = (Vector2){
				curLL->launcher->position.x + (((lineLen * 7) / 8) * cos((curTime + curLL->launcher->spawnAngle)/2)),
				curLL->launcher->position.y + (((lineLen * 7) / 8) * sin((curTime + curLL->launcher->spawnAngle)/2))
			};
		DrawLineEx(curLL->launcher->position, endPos, 5.0f, BLACK);
		DrawLineEx(startPosIn, endPosIn, 2.0f, GetTeamColor(curLL->launcher->owner));
		curLL = curLL->next;
	}
}

void DrawPlinkos(float curTime, float delta)
{
	//	Delete
	return;	
}

void DrawGame(float curTime, float delta)
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
	DrawLaunchers(curTime, delta);
	DrawPlinkos(curTime, delta);
	EndDrawing();
}
