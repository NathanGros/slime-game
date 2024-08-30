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

Room* InitRoom(int n) {
	Room* r = malloc(sizeof(Room));
	r->tab=malloc(n * sizeof(Block));
	r->n = n;
	return r;
}



void DrawBlock(Block *b, int w, int h, int camx, int camy, float zoom) {
	DrawRectangle(
		(int) (w/2 - camx + (float) b->posx),
		(int) (h/2 - camy + (float) b->posy),
		(int) ((float) b->width),
		(int) ((float) b->height),
		RED
	);
}

void DrawRoom(Room *r, int w, int h, int camx, int camy, float zoom) {
	for (int i = 0; i < r->n; i++) {
		DrawBlock(r->tab[i], w, h, camx, camy, zoom);
	}
}



void main() {
	//Init
	Color bg_color = (Color){ 20, 20, 20, 255 };
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Init(bg_color);
	int current_room = 0;
	float zoom = 1.;
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int camx = w/2;
	int camy = h/2;

	//Make the map
		//blocks
		Block *b0_r0 = InitBlock(-500, 100, 600, h-300);
		Block *b1_r0 = InitBlock(-500, -500, w+1000, 600);
		Block *b2_r0 = InitBlock(w-100, 100, 600, h-300);
		Block *b3_r0 = InitBlock(-500, h-100, w+1000, 600);
		Block *b4_r0 = InitBlock(w/2-50, h/2, 100, h/2-100);
		Block *b0_r1 = InitBlock(0, 400, 200, 1000);

		//rooms
		Room *r0 = InitRoom(5);
		r0->tab[0] = b0_r0;
		r0->tab[1] = b1_r0;
		r0->tab[2] = b2_r0;
		r0->tab[3] = b3_r0;
		r0->tab[4] = b4_r0;

		Room *r1 = InitRoom(1);
		r1->tab[0] = b0_r1;

		//map
		Map *m = malloc(sizeof(Map));
		m->tab = malloc(2 * sizeof(Room));
		m->n = 2;
		m->tab[0] = r0;
		m->tab[1] = r1;
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(bg_color);
		DrawRoom(r0, w, h, camx, camy, zoom);
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
