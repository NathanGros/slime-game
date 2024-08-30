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



void DrawBlock(Block *b, float zoom) {
	DrawRectangle(
		(int) (zoom * (float) b->posx),
		(int) (zoom * (float) b->posy),
		(int) (zoom * (float) b->width),
		(int) (zoom * (float) b->height),
		RED
	);
}

void DrawRoom(Room *r, float zoom) {
	for (int i = 0; i < r->n; i++) {
		DrawBlock(r->tab[i], zoom);
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
	/*
		//Controls
			Vector2 player_pos_temp = player_pos;
			if (IsKeyDown(KEY_W)) {
				player_pos_temp.y -= 0.1;
			};
			if (IsKeyDown(KEY_S)) {
				player_pos_temp.y += 0.1;
			};
			if (IsKeyDown(KEY_A)) {
				player_pos_temp.x -= 0.1;
			};
			if (IsKeyDown(KEY_D)) {
				player_pos_temp.x += 0.1;
			};
			if (!CheckCollisions(player_pos_temp, m->a[room_in - 1])) {
				player_pos = player_pos_temp;
			};
			if (IsKeyPressed(KEY_KP_1)) {
				room_in = 1;
			};
			if (IsKeyPressed(KEY_KP_2)) {
				room_in = 2;
			};
	*/

		BeginDrawing();
		ClearBackground(bg_color);
		DrawRoom(r0, zoom);
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
