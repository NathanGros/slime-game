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
} Segment;

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
	int segmentnb;
	int gatenb;
	Block **blocks; //pointer on a list of pointers
	Segment **rail;
	Gate **gates;
} Room;

typedef struct {
	int n; //number of rooms
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

Segment* InitSegment(int x1, int y1, int x2, int y2) {
	Segment *s = malloc(sizeof(Segment));
	s->x1 = x1;
	s->y1 = y1;
	s->x2 = x2;
	s->y2 = y2;
	return s;
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

Room* InitRoom(int blocknb, int segmentnb, int gatenb, int x1, int y1, int x2, int y2) {
	Room* r = malloc(sizeof(Room));
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	r->blocks = malloc(blocknb * sizeof(Block));
	r->blocknb = blocknb;
	r->rail = malloc(segmentnb * sizeof(Segment));
	r->segmentnb = segmentnb;
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

void DrawRoom(Room *r, int w, int h, int camx, int camy, float zoom, Color bg_col, Color wall_col) {
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
			draw_col = wall_col;
		else
			draw_col = bg_col;
		DrawBlock(b, w, h, camx, camy, zoom, draw_col);
	}
}

void DrawRail(Room *r, int w, int h, int camx, int camy, float zoom) {
	for (int i = 0; i < r->segmentnb; i++) {
		Segment *s = r->rail[i];
		DrawLine(
			(w/2 - (int) (zoom * (float) (camx - s->x1))),
			(h/2 - (int) (zoom * (float) (camy - s->y1))),
			(w/2 - (int) (zoom * (float) (camx - s->x2))),
			(h/2 - (int) (zoom * (float) (camy - s->y2))),
			RED
		);
	}
}

void DrawPlayer(int playerx, int playery, int w, int h, int camx, int camy, float zoom) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - (playerx-20)))),
		(h/2 - (int) (zoom * (float) (camy - (playery-20)))),
		(int) (zoom * (float) 40),
		(int) (zoom * (float) 40),
		(Color) {115, 0, 255, 255}
	);
}



//Functions

bool CheckCollisionRecLine(int x, int y, int x1, int y1, int x2, int y2) {
	Rectangle player = (Rectangle) {x-20, y-20, 40, 40};
	Rectangle line = (Rectangle) {x1, y1, x2-x1, y2-y1};
	return CheckCollisionRecs(player, line);
}

bool IsOnRail(int x, int y, Room *r) {
	bool onrail = false;
	for (int i = 0; i < r->segmentnb; i++) {
		Segment *s = r->rail[i];
		if (CheckCollisionRecLine(x, y, s->x1, s->y1, s->x2, s->y2)) onrail = true;
	}
	return onrail;
}



//Main

void main() {
	//Init
	Color bg_col = (Color) {200, 200, 200, 255};
	Color wall_col = (Color) {20, 20, 20, 255};
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(wall_col);
	float zoom = 1.;
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int camx = 0;
	int camy = 0;
	int playerx = 0;
	int playery = -50;
	bool showrail = false;

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
		r0->rail[0] = InitSegment(-450, -250, 450, -250);
		r0->rail[1] = InitSegment(450, -250, 450, 250);
		r0->rail[2] = InitSegment(-450, -250, -450, 250);
		r0->rail[3] = InitSegment(-500, 250, -100, 250);
		r0->rail[4] = InitSegment(100, 250, 450, 250);
		r0->rail[5] = InitSegment(-100, -50, 100, -50);
		r0->rail[6] = InitSegment(-100, -50, -100, 250);
		r0->rail[7] = InitSegment(100, -50, 100, 250);

		Room *r1 = InitRoom(1, 6, 1, -400, -600, 400, 600);
		r1->blocks[0] = b0_r1;
		r1->gates[0] = g0_r1;
		r1->rail[0] = InitSegment(-350, -550, 400, -550);
		r1->rail[1] = InitSegment(-350, -550, -350, 550);
		r1->rail[2] = InitSegment(-350, 550, 350, 550);
		r1->rail[3] = InitSegment(-50, -550, -50, -350);
		r1->rail[4] = InitSegment(-50, -350, 350, -350);
		r1->rail[5] = InitSegment(350, -350, 350, 550);

		//map
		Map *m = malloc(sizeof(Map));
		m->rooms = malloc(2 * sizeof(Room));
		m->n = 2;
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
		if (IsKeyPressed(KEY_R)) showrail = !showrail; 

	//Rail Collision
		if (IsOnRail(newplayerx, newplayery, current_room)) {
			playerx = newplayerx;
			playery = newplayery;
		}
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
			ClearBackground(wall_col);
			DrawRoom(current_room, w, h, camx, camy, zoom, bg_col, wall_col);
			if (showrail) DrawRail(current_room, w, h, camx, camy, zoom);
			DrawPlayer(playerx, playery, w, h, camx, camy, zoom);
		EndDrawing();
	}
	CloseWindow();

	//De-init
	for(int i = 0; i < m->n; i++) {
		Room *r = m->rooms[i];
		for(int j = 0; j < r->blocknb; j++) {
			free(r->blocks[j]);
		}
		free(r->blocks);
		for(int j = 0; j < r->segmentnb; j++) {
			free(r->rail[j]);
		}
		free(r->rail);
		for(int j = 0; j < r->gatenb; j++) {
			free(r->gates[j]);
		}
		free(r->gates);
		free(r);
	}
	free(m->rooms);
	free(m);
}
