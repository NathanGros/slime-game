#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>



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
	r->tab=malloc(n * sizeof(Block));
	r->n = n;
	r->x = startx;
	r->y = starty;
	r->width = width;
	r->height = height;
	return r;
}



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



void main() {
	//Init
	Color bg_color = (Color){ 20, 20, 20, 255 };
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(RED);
	int current_room = 0;
	float zoom = 1.;
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int camx = w/2;
	int camy = h/2;
	int playerx = 200;
	int playery = 200;

	//Make the map
		//blocks
		Block *b0_r0 = InitBlock(w/2-50, h/2, 100, h/2-100);
		Block *b0_r1 = InitBlock(0, 400, 200, 1000);

		//rooms
		Room *r0 = InitRoom(1, 100, 100, w-200, h-200);
		r0->tab[0] = b0_r0;

		Room *r1 = InitRoom(1, 200, 100, w-300, h-100);
		r1->tab[0] = b0_r1;

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
		if (IsKeyPressed(KEY_I) && zoom < 2.) zoom += 0.1; 
		if (IsKeyPressed(KEY_O) && zoom > 0.5) zoom -= 0.1; 
		if (IsKeyDown(KEY_W)) playery -= 10; 
		if (IsKeyDown(KEY_A)) playerx -= 10; 
		if (IsKeyDown(KEY_S)) playery += 10; 
		if (IsKeyDown(KEY_D)) playerx += 10; 
		if (IsKeyDown(KEY_UP)) camy -= 10; 
		if (IsKeyDown(KEY_LEFT)) camx -= 10; 
		if (IsKeyDown(KEY_DOWN)) camy += 10; 
		if (IsKeyDown(KEY_RIGHT)) camx += 10; 
		
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
