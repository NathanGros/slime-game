#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>



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
	int destroom;
	int destx;
	int desty;
} Gate;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int blocknb;
	int wallnb;
	int gatenb;
	Block **blocks; //pointer on a list of pointers
	Wall **walls;
	Gate **gates;
} Room;

typedef struct {
	int roomnb; //number of rooms
	Room **rooms;
} Map;



//Init

void Init(Color bg) {
	InitWindow(1500, 900, "Blob game");
	ClearBackground(bg);
	SetTargetFPS(60);
}

Block* InitBlock(int x1, int y1, int x2, int y2, int type) {
	Block *b = malloc(sizeof(Block));
	b->x1 = x1;
	b->y1 = y1;
	b->x2 = x2;
	b->y2 = y2;
	b->type = type;
	return b;
}

Wall* InitWall(int x1, int y1, int x2, int y2, char direction) {
	Wall *w = malloc(sizeof(Wall));
	w->x1 = x1;
	w->y1 = y1;
	w->x2 = x2;
	w->y2 = y2;
	w->direction = direction;
	return w;
}

Gate* InitGate(int x1, int y1, int x2, int y2, int destroom, int destx, int desty) {
	Gate *g = malloc(sizeof(Gate));
	g->x1 = x1;
	g->y1 = y1;
	g->x2 = x2;
	g->y2 = y2;
	g->destroom = destroom;
	g->destx = destx;
	g->desty = desty;
	return g;
}

Room* InitRoom(int blocknb, int wallnb, int gatenb, int x1, int y1, int x2, int y2) {
	Room* r = malloc(sizeof(Room));
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	r->blocks = malloc(blocknb * sizeof(Block));
	r->blocknb = blocknb;
	r->walls = malloc(wallnb * sizeof(Wall));
	r->wallnb = wallnb;
	r->gates = malloc(gatenb * sizeof(Gate));
	r->gatenb = gatenb;
	return r;
}



//Draw

void DrawBlock(Block *b, int w, int h, int camx, int camy, float zoom, Color col) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - b->x1))),
		(h/2 - (int) (zoom * (float) (camy - b->y1))),
		(int) (zoom * (float) (b->x2 - b->x1)),
		(int) (zoom * (float) (b->y2 - b->y1)),
		col
	);
}

void DrawRoom(Room *r, int w, int h, int camx, int camy, float zoom, Color bg_col, Color block_col) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - r->x1))),
		(h/2 - (int) (zoom * (float) (camy - r->y1))),
		(int) (zoom * (float) (r->x2 - r->x1)),
		(int) (zoom * (float) (r->y2 - r->y1)),
		bg_col
	);
	for (int i = 0; i < r->blocknb; i++) {
		Block *b = r->blocks[i];
		Color draw_col;
		if (b->type == 0)
			draw_col = block_col;
		else
			draw_col = bg_col;
		DrawBlock(b, w, h, camx, camy, zoom, draw_col);
	}
}

void DrawWalls(Room *r, int w, int h, int camx, int camy, float zoom) {
	for (int i = 0; i < r->wallnb; i++) {
		Wall *wall = r->walls[i];
		DrawLine(
			(w/2 - (int) (zoom * (float) (camx - wall->x1))),
			(h/2 - (int) (zoom * (float) (camy - wall->y1))),
			(w/2 - (int) (zoom * (float) (camx - wall->x2))),
			(h/2 - (int) (zoom * (float) (camy - wall->y2))),
			RED
		);
	}
}

void DrawPlayer(int playerx, int playery, int playerradius, int w, int h, int camx, int camy, float zoom) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - (playerx-playerradius)))),
		(h/2 - (int) (zoom * (float) (camy - (playery-playerradius)))),
		(int) (zoom * (float) (2 * playerradius)),
		(int) (zoom * (float) (2 * playerradius)),
		(Color) {115, 0, 255, 255}
	);
}



//Functions

bool IsAboveLine(int coeff, int pointx, int pointy, int x, int y) {
	int ordinateAtOrigin = pointy - coeff * pointx; //Line : t |--> coeff * t + ordinateAtOrigin
	return y > (coeff * x + ordinateAtOrigin);
}

bool CheckCollisionPlayerWall(int newx, int newy, int x, int y, int playerradius, int x1, int y1, int x2, int y2, char direction) {
	bool inwall = false;
	if (direction == 'U' && newy + playerradius > y1 && newy - playerradius < y1 && newx + playerradius > x1 && newx - playerradius < x2
		&& !IsAboveLine(1, x1, y1, x, y) && !IsAboveLine(-1, x2, y2, x, y)) inwall = true;
	if (direction == 'D' && newy + playerradius > y1 && newy - playerradius < y1 && newx + playerradius > x1 && newx - playerradius < x2
		&& IsAboveLine(-1, x1, y1, x, y) && IsAboveLine(1, x1, y1, x, y)) inwall = true;
	if (direction == 'L' && newx + playerradius > x1 && newx - playerradius < x1 && newy + playerradius > y1 && newy - playerradius < y2
		&& IsAboveLine(1, x1, y1, x, y) && !IsAboveLine(-1, x2, y2, x, y)) inwall = true;
	if (direction == 'R' && newx + playerradius > x1 && newx - playerradius < x1 && newy + playerradius > y1 && newy - playerradius < y2
		&& IsAboveLine(-1, x1, y1, x, y) && !IsAboveLine(1, x2, y2, x, y)) inwall = true;
	return inwall;
}

void ExecuteCollisions(Room *r, int *newx, int *newy, int x, int y, int playerradius) {
	for (int i = 0; i < r->wallnb; i++) {
		Wall *wall = r->walls[i];
		if (CheckCollisionPlayerWall(*newx, *newy, x, y, playerradius, wall->x1, wall->y1, wall->x2, wall->y2, wall->direction)) {
			if (wall->direction == 'U') *newy = wall->y1 - playerradius;
			if (wall->direction == 'D') *newy = wall->y1 + playerradius;
			if (wall->direction == 'L') *newx = wall->x1 - playerradius;
			if (wall->direction == 'R') *newx = wall->x1 + playerradius;
		}
	}
}



//Main

void main() {
	//Init
	Color bg_col = (Color) {200, 200, 200, 255};
	Color block_col = (Color) {20, 20, 20, 255};
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(block_col);
	float zoom = 1.;
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int camx = 0;
	int camy = 0;
	int playerx = 0;
	int playery = -50;
	int playerradius = 20;
	bool showwalls = false;

	//Make the map
		//blocks
		Block *b0_r0 = InitBlock(-50, 0, 50, 300, 0);
		Block *b0_r1 = InitBlock(0, -500, 400, -400, 0);
		
		//gates
		Gate *g0_r0 = InitGate(-600, 200, -500, 300, 1, 350, -550);
		Gate *g0_r1 = InitGate(400, -600, 500, -500, 0, -450, 250);

		//rooms
		Room *r0 = InitRoom(1, 8, 1, -500, -300, 500, 300);
		r0->blocks[0] = b0_r0;
		r0->gates[0] = g0_r0;
		r0->walls[0] = InitWall(-500, -300, 500, -300, 'D');
		r0->walls[1] = InitWall(500, -300, 500, 300, 'L');
		r0->walls[2] = InitWall(-500, -300, -500, 200, 'R');
		r0->walls[3] = InitWall(-500, 300, -50, 300, 'U');
		r0->walls[4] = InitWall(50, 300, 500, 300, 'U');
		r0->walls[5] = InitWall(-50, 0, 50, 0, 'U');
		r0->walls[6] = InitWall(-50, 0, -50, 300, 'L');
		r0->walls[7] = InitWall(50, 0, 50, 300, 'R');

		Room *r1 = InitRoom(1, 7, 1, -400, -600, 400, 600);
		r1->blocks[0] = b0_r1;
		r1->gates[0] = g0_r1;
		r1->walls[0] = InitWall(-400, -600, 400, -600, 'D');
		r1->walls[1] = InitWall(-400, -600, -400, 600, 'R');
		r1->walls[2] = InitWall(-400, 600, 400, 600, 'U');
		r1->walls[3] = InitWall(0, -500, 0, -400, 'L');
		r1->walls[4] = InitWall(0, -400, 400, -400, 'D');
		r1->walls[5] = InitWall(0, -500, 400, -500, 'U');
		r1->walls[6] = InitWall(400, -400, 400, 600, 'L');

		//map
		Map *m = malloc(sizeof(Map));
		m->rooms = malloc(2 * sizeof(Room));
		m->roomnb = 2;
		m->rooms[0] = r0;
		m->rooms[1] = r1;
	
	Room *current_room = r0;

	while (!WindowShouldClose()) {
	//Resize
		if (IsWindowResized()) {
			w = GetScreenWidth();
			h = GetScreenHeight();
		}

	//Controls
		int newplayerx = playerx;
		int newplayery = playery;
		if (IsKeyPressed(KEY_I) && zoom < 2.) zoom += 0.1; 
		if (IsKeyPressed(KEY_O) && zoom > 0.5) zoom -= 0.1; 
		if (IsKeyDown(KEY_W)) newplayery -= 10; 
		if (IsKeyDown(KEY_A)) newplayerx -= 10; 
		if (IsKeyDown(KEY_S)) newplayery += 10; 
		if (IsKeyDown(KEY_D)) newplayerx += 10; 
		if (IsKeyPressed(KEY_R)) showwalls = !showwalls; 

	//Wall collisions
		ExecuteCollisions(current_room, &newplayerx, &newplayery, playerx, playery, playerradius);
		playerx = newplayerx;
		playery = newplayery;

	//Camera update
		camx = playerx;
		camy = playery;
	
	//Changing room
		Room *new_room = current_room;
		for (int i = 0; i < current_room->gatenb; i++) {
			Gate *g = current_room->gates[i];
			Rectangle gate = (Rectangle) {g->x1, g->y1, g->x2 - g->x1, g->y2 - g->y1};
			Rectangle player = (Rectangle) {playerx-20, playery-20, 40, 40};
			if (CheckCollisionRecs(gate, player)) {
				new_room = m->rooms[g->destroom];
				playerx = g->destx;
				playery = g->desty;
			}
		}
		current_room = new_room;
		
	//Drawing
		BeginDrawing();
			ClearBackground(block_col);
			DrawRoom(current_room, w, h, camx, camy, zoom, bg_col, block_col);
			if (showwalls) DrawWalls(current_room, w, h, camx, camy, zoom);
			DrawPlayer(playerx, playery, playerradius, w, h, camx, camy, zoom);
		EndDrawing();
	}
	CloseWindow();

	//De-init
	for(int i = 0; i < m->roomnb; i++) {
		Room *r = m->rooms[i];
		for(int j = 0; j < r->blocknb; j++) {
			free(r->blocks[j]);
		}
		free(r->blocks);
		for(int j = 0; j < r->wallnb; j++) {
			free(r->walls[j]);
		}
		free(r->walls);
		for(int j = 0; j < r->gatenb; j++) {
			free(r->gates[j]);
		}
		free(r->gates);
		free(r);
	}
	free(m->rooms);
	free(m);
}
