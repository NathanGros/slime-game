#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



//Struct

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int type;
} Block;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	char direction;
} Wall;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int destRoom;
	int destX;
	int destY;
} Gate;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int blockNb;
	int wallNb;
	int gateNb;
	Block **blocks; //pointer on a list of pointers
	Wall **walls;
	Gate **gates;
} Room;

typedef struct {
	int roomNb; //number of rooms
	Room **rooms;
} Map;

typedef struct {
	float forceX;
	float forceY;
} Force;

typedef struct {
	Color color;
	int radius;
	float mass;
	float airResistance;
	float posX;
	float posY;
	float velocityX;
	float velocityY;
	int forceNb;
	Force *forces;
} Body;



//Init

void Init(Color backgroundColor) {
	InitWindow(1500, 900, "Blob game");
	ClearBackground(backgroundColor);
	SetTargetFPS(60);
}

Block* InitBlock(int x1, int y1, int x2, int y2, int type) {
	Block *block = malloc(sizeof(Block));
	block->x1 = x1;
	block->y1 = y1;
	block->x2 = x2;
	block->y2 = y2;
	block->type = type;
	return block;
}

Wall* InitWall(int x1, int y1, int x2, int y2, char direction) {
	Wall *wall = malloc(sizeof(Wall));
	wall->x1 = x1;
	wall->y1 = y1;
	wall->x2 = x2;
	wall->y2 = y2;
	wall->direction = direction;
	return wall;
}

Gate* InitGate(int x1, int y1, int x2, int y2, int destRoom, int destX, int destY) {
	Gate *gate = malloc(sizeof(Gate));
	gate->x1 = x1;
	gate->y1 = y1;
	gate->x2 = x2;
	gate->y2 = y2;
	gate->destRoom = destRoom;
	gate->destX = destX;
	gate->destY = destY;
	return gate;
}

Room* InitRoom(int blockNb, int wallNb, int gateNb, int x1, int y1, int x2, int y2) {
	Room* room = malloc(sizeof(Room));
	room->x1 = x1;
	room->y1 = y1;
	room->x2 = x2;
	room->y2 = y2;
	room->blocks = malloc(blockNb * sizeof(Block));
	room->blockNb = blockNb;
	room->walls = malloc(wallNb * sizeof(Wall));
	room->wallNb = wallNb;
	room->gates = malloc(gateNb * sizeof(Gate));
	room->gateNb = gateNb;
	return room;
}

Body* InitBody(Color color, int radius, float mass, float airResistance, float posX, float posY, float velocityX, float velocityY, int forceNb) {
	Body *body = malloc(sizeof(Body));
	body->color = color;
	body->radius = radius;
	body->mass = mass;
	body->airResistance = airResistance;
	body->posX = posX;
	body->posY = posY;
	body->velocityX = velocityX;
	body->velocityY = velocityY;
	body->forceNb = forceNb;
	body->forces = malloc(forceNb * sizeof(Force));
	return body;
}



//Draw

void DrawBlock(Block *block, int screenWidth, int screenHeight, int cameraX, int cameraY, float zoom, Color blockColor) {
	DrawRectangle(
		(screenWidth / 2 - (int) (zoom * (float) (cameraX - block->x1))),
		(screenHeight / 2 - (int) (zoom * (float) (cameraY - block->y1))),
		(int) (zoom * (float) (block->x2 - block->x1)),
		(int) (zoom * (float) (block->y2 - block->y1)),
		blockColor
	);
}

void DrawRoom(Room *room, int screenWidth, int screenHeight, int cameraX, int cameraY, float zoom, Color backgroundColor, Color blockColor) {
	DrawRectangle(
		(screenWidth / 2 - (int) (zoom * (float) (cameraX - room->x1))),
		(screenHeight / 2 - (int) (zoom * (float) (cameraY - room->y1))),
		(int) (zoom * (float) (room->x2 - room->x1)),
		(int) (zoom * (float) (room->y2 - room->y1)),
		backgroundColor
	);
	for (int i = 0; i < room->blockNb; i++) {
		Block *block = room->blocks[i];
		DrawBlock(block, screenWidth, screenHeight, cameraX, cameraY, zoom, blockColor);
	}
}

void DrawWalls(Room *room, int screenWidth, int screenHeight, int cameraX, int cameraY, float zoom) {
	for (int i = 0; i < room->wallNb; i++) {
		Wall *wall = room->walls[i];
		DrawLine(
			(screenWidth / 2 - (int) (zoom * (float) (cameraX - wall->x1))),
			(screenHeight / 2 - (int) (zoom * (float) (cameraY - wall->y1))),
			(screenWidth / 2 - (int) (zoom * (float) (cameraX - wall->x2))),
			(screenHeight / 2 - (int) (zoom * (float) (cameraY - wall->y2))),
			RED
		);
	}
}

void DrawPlayer(Body *player, int screenWidth, int screenHeight, int cameraX, int cameraY, float zoom) {
	DrawRectangle(
		(screenWidth / 2 - (int) (zoom * (float) (cameraX - ((int) player->posX - player->radius)))),
		(screenHeight / 2 - (int) (zoom * (float) (cameraY - ((int) player->posY - player->radius)))),
		(int) (zoom * (float) (2 * player->radius)),
		(int) (zoom * (float) (2 * player->radius)),
		player->color
	);
}



//Functions

void UpdateForces(Body *player) {
	//Weight
	float weightForce = player->mass * 500.;
	player->forces[0].forceX = 0.;
	player->forces[0].forceY = weightForce;
	//Air resistance
	float airResistanceForceX = -1. * player->airResistance * player->velocityX;
	float airResistanceForceY = -1. * player->airResistance * player->velocityY;
	player->forces[1].forceX = airResistanceForceX;
	player->forces[1].forceY = airResistanceForceY;
}

float ForceSumX(Body *player) {
	float result = 0;
	for (int i = 0; i < player->forceNb; i++) result += player->forces[i].forceX;
	return result;
}

float ForceSumY(Body *player) {
	float result = 0;
	for (int i = 0; i < player->forceNb; i++) result += player->forces[i].forceY;
	return result;
}

void UpdateBodyPosition(float *newPosX, float *newPosY, Body *player) { //Euler algorythm
	float dt = GetFrameTime();
	float accelerationX = ForceSumX(player) / player->mass;
	float accelerationY = ForceSumY(player) / player->mass;
	player->velocityX += accelerationX * dt;
	player->velocityY += accelerationY * dt;
	*newPosX += player->velocityX * dt;
	*newPosY += player->velocityY * dt;
}

bool IsAboveLine(float slope, int pointOnLineX, int pointOnLineY, float posX, float posY) {
	int ordinateAtOrigin = (float) pointOnLineY - slope * (float) pointOnLineX; //Linear function : t |--> slope * t + ordinateAtOrigin
	return (posY > (slope * posX + ordinateAtOrigin));
}

bool CheckCollisionPlayerWall(float newPosX, float newPosY, Body *player, int x1, int y1, int x2, int y2, char direction) {
	bool isInWall = false;
	if (direction == 'U'
		&& (int) newPosY + player->radius > y1 && (int) newPosY - player->radius < y1
		&& (int) newPosX + player->radius > x1 && (int) newPosX - player->radius < x2
		&& !IsAboveLine(1., x1, y1, player->posX, player->posY) && !IsAboveLine(-1., x2, y2, player->posX, player->posY)) isInWall = true;
	if (direction == 'D'
		&& (int) newPosY + player->radius > y1 && (int) newPosY - player->radius < y1
		&& (int) newPosX + player->radius > x1 && (int) newPosX - player->radius < x2
		&& IsAboveLine(-1., x1, y1, player->posX, player->posY) && IsAboveLine(1., x2, y2, player->posX, player->posY)) isInWall = true;
	if (direction == 'L'
		&& (int) newPosX + player->radius > x1 && (int) newPosX - player->radius < x1
		&& (int) newPosY + player->radius > y1 && (int) newPosY - player->radius < y2
		&& IsAboveLine(1., x1, y1, player->posX, player->posY) && !IsAboveLine(-1., x2, y2, player->posX, player->posY)) isInWall = true;
	if (direction == 'R'
		&& (int) newPosX + player->radius > x1 && (int) newPosX - player->radius < x1
		&& (int) newPosY + player->radius > y1 && (int) newPosY - player->radius < y2
		&& IsAboveLine(-1., x1, y1, player->posX, player->posY) && !IsAboveLine(1., x2, y2, player->posX, player->posY)) isInWall = true;
	return isInWall;
}

void ExecuteCollisions(Room *room, float *newPosX, float *newPosY, Body *player) {
	for (int i = 0; i < room->wallNb; i++) {
		Wall *wall = room->walls[i];
		if (CheckCollisionPlayerWall(*newPosX, *newPosY, player, wall->x1, wall->y1, wall->x2, wall->y2, wall->direction)) {
			if (wall->direction == 'U') {
				*newPosY = (float) (wall->y1 - player->radius);
				player->velocityY = 0.;
			}
			if (wall->direction == 'D') {
				*newPosY = (float) (wall->y1 + player->radius);
				player->velocityY = 0.;
			}
			if (wall->direction == 'L') {
				*newPosX = (float) (wall->x1 - player->radius);
				player->velocityX = 0.;
			}
			if (wall->direction == 'R') {
				*newPosX = (float) (wall->x1 + player->radius);
				player->velocityX = 0.;
			}
		}
	}
}

float Distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool ClosestPointToWall(Body *player, Wall *wall, int *wallPointX, int *wallPointY) {
	bool onWall = false;
	if (wall->direction == 'U') {
		if (player->posY < wall->y1) {
			onWall = true;
			*wallPointY = wall->y1;
			if (player->posX < wall->x1) *wallPointX = wall->x1;
			else if (player->posX > wall->x2) *wallPointX = wall->x2;
			else *wallPointX = player->posX;
		}
	}
	else if (wall->direction == 'D') {
		if (player->posY > wall->y1) {
			onWall = true;
			*wallPointY = wall->y1;
			if (player->posX < wall->x1) *wallPointX = wall->x1;
			else if (player->posX > wall->x2) *wallPointX = wall->x2;
			else *wallPointX = player->posX;
		}
	}
	else if (wall->direction == 'L') {
		if (player->posX < wall->x1) {
			onWall = true;
			*wallPointX = wall->x1;
			if (player->posY < wall->y1) *wallPointY = wall->y1;
			else if (player->posY > wall->y2) *wallPointY = wall->y2;
			else *wallPointY = player->posY;
		}
	}
	else {
		if (player->posX > wall->x1) {
			onWall = true;
			*wallPointX = wall->x1;
			if (player->posY < wall->y1) *wallPointY = wall->y1;
			else if (player->posY > wall->y2) *wallPointY = wall->y2;
			else *wallPointY = player->posY;
		}
	}
	return onWall;
}

void ClosestPointToAllWalls(Body *player, Room *room, int *wallPointX, int *wallPointY) {
	int k = 0;
	int thisWallPointX;
	int thisWallPointY;
	while (!ClosestPointToWall(player, room->walls[k], &thisWallPointX, &thisWallPointY)) {
		k++;
	}
	for (int i = k; i < room->wallNb; i++) {
		if (ClosestPointToWall(player, room->walls[i], &thisWallPointX, &thisWallPointY)) {
			if (Distance(player->posX, player->posY, (float) *wallPointX, (float) *wallPointY) > Distance(player->posX, player->posY, (float) thisWallPointX, (float) thisWallPointY)) {
				*wallPointX = thisWallPointX;
				*wallPointY = thisWallPointY;
			}
		}
	}
}



//Main

void main() {
	//Init
	Color backgroundColor = (Color) {200, 200, 200, 255};
	Color blockColor = (Color) {20, 20, 20, 255};
	Color playerColor = (Color) {115, 0, 255, 255};
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(blockColor);
	float zoom = 1.;
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int cameraX = 0;
	int cameraY = 0;
	bool displayWalls = false;

	//Make player
	Body *player = InitBody(playerColor, 20, 1., 0.5, 0., -100., 0., 0., 3);
	player->forces[0].forceX = 0.;
	player->forces[0].forceY = 0.;
	player->forces[1].forceX = 0.;
	player->forces[1].forceY = 0.;
	player->forces[2].forceX = 0.;
	player->forces[2].forceY = 0.;

	//Make the map
		//blocks
		Block *block0_room0 = InitBlock(-50, 0, 50, 300, 0);
		Block *block0_room1 = InitBlock(0, -500, 400, -400, 0);
		
		//gates
		Gate *gate0_room0 = InitGate(-600, 200, -500, 300, 1, 350, -550);
		Gate *gate0_room1 = InitGate(400, -600, 500, -500, 0, -450, 250);

		//rooms
		Room *room0 = InitRoom(1, 8, 1, -500, -300, 500, 300);
		room0->blocks[0] = block0_room0;
		room0->gates[0] = gate0_room0;
		room0->walls[0] = InitWall(-500, -300, 500, -300, 'D');
		room0->walls[1] = InitWall(500, -300, 500, 300, 'L');
		room0->walls[2] = InitWall(-500, -300, -500, 200, 'R');
		room0->walls[3] = InitWall(-500, 300, -50, 300, 'U');
		room0->walls[4] = InitWall(50, 300, 500, 300, 'U');
		room0->walls[5] = InitWall(-50, 0, 50, 0, 'U');
		room0->walls[6] = InitWall(-50, 0, -50, 300, 'L');
		room0->walls[7] = InitWall(50, 0, 50, 300, 'R');

		Room *room1 = InitRoom(1, 7, 1, -400, -600, 400, 600);
		room1->blocks[0] = block0_room1;
		room1->gates[0] = gate0_room1;
		room1->walls[0] = InitWall(-400, -600, 400, -600, 'D');
		room1->walls[1] = InitWall(-400, -600, -400, 600, 'R');
		room1->walls[2] = InitWall(-400, 600, 400, 600, 'U');
		room1->walls[3] = InitWall(0, -500, 0, -400, 'L');
		room1->walls[4] = InitWall(0, -400, 400, -400, 'D');
		room1->walls[5] = InitWall(0, -500, 400, -500, 'U');
		room1->walls[6] = InitWall(400, -400, 400, 600, 'L');

		//map
		Map *map = malloc(sizeof(Map));
		map->rooms = malloc(2 * sizeof(Room));
		map->roomNb = 2;
		map->rooms[0] = room0;
		map->rooms[1] = room1;
	
	Room *currentRoom = room0;

	while (!WindowShouldClose()) {
	//Resize
		if (IsWindowResized()) {
			screenWidth = GetScreenWidth();
			screenHeight = GetScreenHeight();
		}

	//Controls
		float newPosX = player->posX;
		float newPosY = player->posY;
		float controlForceX = 0.;
		float controlForceY = 0.;
		if (IsKeyPressed(KEY_I) && zoom < 2.) zoom += 0.1;
		if (IsKeyPressed(KEY_O) && zoom > 0.5) zoom -= 0.1;
		if (IsKeyDown(KEY_W)) controlForceY -= 1500.;
		if (IsKeyDown(KEY_A)) controlForceX -= 500.;
		if (IsKeyDown(KEY_S)) controlForceY += 500.;
		if (IsKeyDown(KEY_D)) controlForceX += 500.;
		if (IsKeyPressed(KEY_R)) displayWalls = !displayWalls;
		player->forces[2].forceX = controlForceX;
		player->forces[2].forceY = controlForceY;

	//Physics
		UpdateForces(player);
		UpdateBodyPosition(&newPosX, &newPosY, player);

	//Wall collisions
		ExecuteCollisions(currentRoom, &newPosX, &newPosY, player);
		player->posX = newPosX;
		player->posY = newPosY;

	//Find closest point to wall
		int closestToWallX;
		int closestToWallY;
		ClosestPointToAllWalls(player, currentRoom, &closestToWallX, &closestToWallY); //potential crash when out of bounds ?

	//Camera update
		cameraX = (int) player->posX;
		cameraY = (int) player->posY;
	
	//Changing room TODO
		Room *newRoom = currentRoom;
		for (int i = 0; i < currentRoom->gateNb; i++) {
			Gate *gate = currentRoom->gates[i];
			Rectangle gateRect = (Rectangle) {gate->x1, gate->y1, gate->x2 - gate->x1, gate->y2 - gate->y1};
			Rectangle playerRect = (Rectangle) {(int) player->posX - player->radius, (int) player->posY - player->radius, 2 * player->radius, 2 * player->radius};
			if (CheckCollisionRecs(gateRect, playerRect)) {
				newRoom = map->rooms[gate->destRoom];
				player->posX = (float) gate->destX;
				player->posY = (float) gate->destY;
			}
		}
		currentRoom = newRoom;
		
	//Drawing
		BeginDrawing();
			ClearBackground(blockColor);
			DrawRoom(currentRoom, screenWidth, screenHeight, cameraX, cameraY, zoom, backgroundColor, blockColor);
			if (displayWalls) DrawWalls(currentRoom, screenWidth, screenHeight, cameraX, cameraY, zoom);
			DrawPlayer(player, screenWidth, screenHeight, cameraX, cameraY, zoom);
			DrawLine(
				(screenWidth / 2 - (int) (zoom * (float) (cameraX - player->posX))),
				(screenHeight / 2 - (int) (zoom * (float) (cameraY - player->posY))),
				(screenWidth / 2 - (int) (zoom * (float) (cameraX - closestToWallX))),
				(screenHeight / 2 - (int) (zoom * (float) (cameraY - closestToWallY))),
				playerColor
			);
		EndDrawing();
	}
	CloseWindow();

	//De-init
	for(int i = 0; i < map->roomNb; i++) {
		Room *room = map->rooms[i];
		for(int j = 0; j < room->blockNb; j++) {
			free(room->blocks[j]);
		}
		free(room->blocks);
		for(int j = 0; j < room->wallNb; j++) {
			free(room->walls[j]);
		}
		free(room->walls);
		for(int j = 0; j < room->gateNb; j++) {
			free(room->gates[j]);
		}
		free(room->gates);
		free(room);
	}
	free(map->rooms);
	free(map);
	free(player->forces);
	free(player);
}
