#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>



typedef struct {
	int n;
	Vector2 *a; //a = pointeur sur une liste de Vector2
} Block;

typedef struct {
	int n;
	Block* *a; //a = pointeur sur une liste de pointeurs sur Block
} Room;

typedef struct {
	int n;
	Room* *a; //a = pointeur sur une liste de pointeurs sur Rooms
} Map;





void window_init(Color bg) {
	InitWindow(1500, 900, "Blob game");
	ClearBackground(bg);
	SetTargetFPS(60);
}

bool CheckCollisions(Vector2 pos, Room *r) {
	bool res = false;
	for (int i=0; i < r->n; i++) {
		res = res || CheckCollisionPointPoly(pos, r->a[i]->a, r->a[i]->n);
	}
	return res;
}





void draw_block(Block *b, int ppu) {
	for (int i=0; i < b->n - 1; i++) {
		DrawLine(ppu * b->a[i].x, ppu * b->a[i].y, ppu * b->a[i+1].x, ppu * b->a[i+1].y, RED);
	};
	DrawLine(ppu * b->a[b->n - 1].x, ppu * b->a[b->n - 1].y, ppu * b->a[0].x, ppu * b->a[0].y, RED);
}

void draw_room(int nb, Map *m, int ppu) {
	Room *r = m->a[nb - 1];
	for (int i=0; i<r->n; i++) {
		draw_block(r->a[i], ppu);
	}
}





void main() {
	//Make the map
		//blocks
		Block *b1_r1 = malloc(sizeof(Block));
		b1_r1->a = malloc(6 * sizeof(Vector2));
		b1_r1->n = 6;
		b1_r1->a = (Vector2 [])	{
			(Vector2) {1., 1.},
			(Vector2) {1., 5.},
			(Vector2) {2., 5.},
			(Vector2) {2., 3.},
			(Vector2) {7., 3.},
			(Vector2) {7., 1.}
		};
		Block *b2_r1 = malloc(sizeof(Block));
		b2_r1->a = malloc(5 * sizeof(Vector2));
		b2_r1->n = 5;
		b2_r1->a = (Vector2 [])	{
			(Vector2) {7., 8.},
			(Vector2) {4., 7.},
			(Vector2) {6., 6.},
			(Vector2) {8., 4.},
			(Vector2) {10., 4.}
		};
		Block *b1_r2 = malloc(sizeof(Block));
		b1_r2->a = malloc(6 * sizeof(Vector2));
		b1_r2->n = 6;
		b1_r2->a = (Vector2 [])	{
			(Vector2) {1., 1.},
			(Vector2) {1., 3.},
			(Vector2) {2., 3.},
			(Vector2) {2., 5.},
			(Vector2) {4., 5.},
			(Vector2) {4., 1.}
		};

		//rooms
		Room *r1 = malloc(sizeof(Room));
		r1->a = malloc(2 * sizeof(Block));
		r1->n = 2;
		r1->a[0] = b1_r1;
		r1->a[1] = b2_r1;

		Room *r2 = malloc(sizeof(Room));
		r2->a = malloc(sizeof(Block));
		r2->n = 1;
		r2->a[0] = b1_r2;

		//map
		Map *m = malloc(sizeof(Map));
		m->a = malloc(2 * sizeof(Room));
		m->n = 1;
		m->a[0] = r1;
		m->a[1] = r2;
	
	//Values
	Color bg_color = (Color){ 20, 20, 20, 255 };
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	window_init(bg_color);
	bool stick_mode = true;
	Vector2 player_pos = (Vector2) {.5, .5};
	int room_in = 1;
	int ppu = GetScreenHeight() / 10; //pixel per unit (pixels on screen per game unit of measure)

	while (!WindowShouldClose()) {
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

		BeginDrawing();
		ppu = GetScreenHeight() / 10;
		ClearBackground(bg_color);
		draw_room(room_in, m, ppu);
		DrawRectangle((int) (ppu * player_pos.x) - 5, (int) (ppu * player_pos.y) - 5, 10, 10, RAYWHITE);
		EndDrawing();
	}
}
