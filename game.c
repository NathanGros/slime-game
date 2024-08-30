#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>



//Struct

typedef struct {
	int posx;
	int posy;
	int width;
	int height;
} Block;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int n;
	Block **tab; //tab = pointeur sur une liste de pointeurs sur Block
} Room;

typedef struct {
	int n;
	Room **tab; //tab = pointeur sur une liste de pointeurs sur Room
} Map;

typedef struct {
	int pos1x;
	int pos1y;
	int pos2x;
	int pos2y;
} Segment;

typedef struct {
	int n;
	Segment **tab;
} Rail;



//Init

void Init(Color bg) {
	InitWindow(1500, 900, "Blob game");
	ClearBackground(bg);
	SetTargetFPS(60);
}

Block* InitBlock(int posx, int posy, int width, int height) {
	Block *b = malloc(4 * sizeof(int));
	b->posx = posx;
	b->posy = posy;
	b->width = width;
	b->height = height;
	return b;
}

Room* InitRoom(int n, int startx, int starty, int width, int height) {
	Room* r = malloc(sizeof(Room));
	r->tab = malloc(n * sizeof(Block));
	r->n = n;
	r->x = startx;
	r->y = starty;
	r->width = width;
	r->height = height;
	return r;
}

Segment* InitSegment(int x1, int y1, int x2, int y2) {
	Segment *s = malloc(sizeof(Segment));
	s->pos1x = x1;
	s->pos1y = y1;
	s->pos2x = x2;
	s->pos2y = y2;
	return s;
}

Rail* InitRail(int n) {
	Rail *rail = malloc(sizeof(Rail));
	rail->n = n;
	rail->tab = malloc(n * sizeof(Segment));
	return rail;
}



//Draw

void DrawBlock(Block *b, int w, int h, int camx, int camy, float zoom) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - b->posx))),
		(h/2 - (int) (zoom * (float) (camy - b->posy))),
		(int) (zoom * (float) b->width),
		(int) (zoom * (float) b->height),
		RED
	);
}

void DrawRoom(Room *r, int w, int h, int camx, int camy, float zoom, Color bg_color) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - r->x))),
		(h/2 - (int) (zoom * (float) (camy - r->y))),
		(int) (zoom * (float) r->width),
		(int) (zoom * (float) r->height),
		bg_color
	);
	for (int i = 0; i < r->n; i++) {
		DrawBlock(r->tab[i], w, h, camx, camy, zoom);
	}
}

void DrawPlayer(int playerx, int playery, int w, int h, int camx, int camy, float zoom) {
	DrawRectangle(
		(w/2 - (int) (zoom * (float) (camx - (playerx-20)))),
		(h/2 - (int) (zoom * (float) (camy - (playery-20)))),
		(int) (zoom * (float) 40),
		(int) (zoom * (float) 40),
		WHITE
	);
}



//Functions

bool CheckCollisionRecLine(int x, int y, int x1, int y1, int x2, int y2) {
	Rectangle player = (Rectangle) {x-20, y-20, 40, 40};
	Rectangle line = (Rectangle) {x1, y1, x2-x1, y2-y1};
	return CheckCollisionRecs(player, line);
}

bool IsOnRail(int x, int y, Rail *r) {
	bool onrail = false;
	for (int i = 0; i < r->n; i++) {
		Segment *s = r->tab[i];
		if (CheckCollisionRecLine(x, y, s->pos1x, s->pos1y, s->pos2x, s->pos2y)) onrail = true;
	}
	return onrail;
}



//Main

void main() {
	//Init
	Color bg_color = (Color){ 20, 20, 20, 255 };
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(RED);
	int current_room = 0;
	float zoom = 1.;
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int camx = 0;
	int camy = 0;
	int playerx = 0;
	int playery = -50;

	//Make the map
		//blocks
		Block *b0_r0 = InitBlock(-50, 0, 100, 300);
		Block *b0_r1 = InitBlock(0, 400, 200, 1000);

		//rooms
		Room *r0 = InitRoom(1, -500, -300, 1000, 600);
		r0->tab[0] = b0_r0;

		Room *r1 = InitRoom(1, 200, 100, w-300, h-100);
		r1->tab[0] = b0_r1;

		//rails
		Rail *rail0 = InitRail(8);
		rail0->tab[0] = InitSegment(-450, -250, 450, -250);
		rail0->tab[1] = InitSegment(450, -250, 450, 250);
		rail0->tab[2] = InitSegment(-450, -250, -450, 250);
		rail0->tab[3] = InitSegment(-450, 250, -100, 250);
		rail0->tab[4] = InitSegment(100, 250, 450, 250);
		rail0->tab[5] = InitSegment(-100, -50, 100, -50);
		rail0->tab[6] = InitSegment(-100, -50, -100, 250);
		rail0->tab[7] = InitSegment(100, -50, 100, 250);

		//map
		Map *m = malloc(sizeof(Map));
		m->tab = malloc(2 * sizeof(Room));
		m->n = 2;
		m->tab[0] = r0;
		m->tab[1] = r1;
	
	while (!WindowShouldClose()) {
		//Resize
		if (IsWindowResized()) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
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
		if (IsKeyDown(KEY_UP)) camy -= 10; 
		if (IsKeyDown(KEY_LEFT)) camx -= 10; 
		if (IsKeyDown(KEY_DOWN)) camy += 10; 
		if (IsKeyDown(KEY_RIGHT)) camx += 10; 
		//Rail Collision
		if (IsOnRail(newplayerx, newplayery, rail0)) {
			playerx = newplayerx;
			playery = newplayery;
		}
		
		//Drawing
		BeginDrawing();
			ClearBackground(RED);
			DrawRoom(r0, w, h, camx, camy, zoom, bg_color);
			DrawPlayer(playerx, playery, w, h, camx, camy, zoom);
		EndDrawing();
	}
	CloseWindow();

	//De-init
	for(int i = 0; i < m->n; i++) {
		Room *r = m->tab[i];
		for(int j = 0; j < r->n; j++) {
			Block *b = r->tab[j];
			free(b);
		}
		free(r->tab);
		free(r);
	}
	free(m->tab);
	free(m);
}
