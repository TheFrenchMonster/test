/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <time.h>

struct player {
	int x, y;
	enum direction current_direction;
	int hp;
	int bomb_range;
	int nb_bombs;
	int key;
};

struct player* player_init(int bomb_number) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->key=1;

	return player;
}


void player_set_position(struct player *player, int x, int y) {
	assert(player);
	player->x = x;
	player->y = y;
}


void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

void player_dec_hp(struct player* player) {
	assert(player);
	currentHp = player->hp;
	player->hp = currentHp - 1;
}

void player_inc_hp(struct player* player) {
	assert(player);
	currentHp = player->hp;
	player->hp = currentHp + 1;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bombs;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs -= 1;
}

void player_set_bomb(struct player* player, struct map* map){
	assert(player);
	if (player_get_nb_bomb(player) >= 1){
		int bomb = 1;
		int x = player_get_x(player);
		int y = player_get_y(player);
		int range = player->bomb_range;
		player_dec_nb_bomb(player);
		int startingTime = SDL_GetTicks();
		while (bomb == 1) {
			int currentTime = SDL_GetTicks();
			int timer = currentTime - startingTime;
			if (0<timer && timer<1000) {
				map_set_type(map, x, y, BOMB_1);
			}
			else if (1000<timer && timer<2000) {
				map_set_type(map, x, y, BOMB_2);
			}
			else if (2000<timer && timer<3000) {
				map_set_type(map, x, y, BOMB_3);
			}
			else if (3000<timer && timer<4000) {
				map_set_type(map, x, y, BOMB_4);
			}
			else if (4000<timer && timer<5000) {
				map_set_type(map, x, y, CELL_FIRE);
				if (player_get_x(player) == x && player_get_y(player) == y) {
					player_dec_hp(player);
				}
				if (map_get_cell_type(map, x + 1, y) != CELL_SCENERY && map_get_cell_type(map, x + 1, y) != CELL_DOOR) {
					map_set_type(map, x + 1, y, CELL_FIRE);
				}
				if (map_get_cell_type(map, x - 1, y) != CELL_SCENERY && map_get_cell_type(map, x + 1, y) != CELL_DOOR) {
					map_set_type(map, x - 1, y, CELL_FIRE);
				}
				if (map_get_cell_type(map, x , y+1) != CELL_SCENERY && map_get_cell_type(map, x + 1, y) != CELL_DOOR) {
					map_set_type(map, x , y+1, CELL_FIRE);
				}
				if (map_get_cell_type(map, x , y-1) != CELL_SCENERY && map_get_cell_type(map, x + 1, y) != CELL_DOOR) {
					map_set_type(map, x , y-1, CELL_FIRE);
				}
			


		}
	}
}


static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_BOX:

		return 1;
		break;

	case CELL_BONUS:
		return 1;
		break;

	case CELL_MONSTER:
		return 0;
		break;
	case CELL_BOMB:
		return 0;
		break;
		
	case CELL_DOOR:
		if (door_is_open(x, y, map) == 1) {
			int currentlvl = map_get_level(map);
			int x = map_get_door_level(map, x, y);
			map_free(map);
			int* ptr;
			ptr = &x;
			load_map(ptr);
			if (x <= currentlvl) {
				map_dec_level(map, currentlvl);
			}
			else {
				map_inc_level(map, currentlvl)
			}

			//still things to do here
			return 1;
		}
		else if (door_is_open(x,y,map) == 0 && (player->key == 1)) {
			int currentlvl = map_get_level(map);
			int x = map_get_door_level(map, x, y);
			map_free(map);
			int* ptr;
			ptr = &x;
			load_map(ptr);
			if (x <= currentlvl) {
				map_dec_level(map, currentlvl);
			}
			else {
				map_inc_level(map, currentlvl)
			}
			player->key--;
			return 1;
		}

		else {
			return 0;

		}
		break;
	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->current_direction) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1)) {
			if(y-1>=0){
				if(map_get_cell_type(map, x, y-1) == CELL_BOX) {
					if(map_get_cell_type(map, x, y-2) == CELL_EMPTY){
						if(y-2>=0){
							map_set_cell_type(map, x, y-1, CELL_EMPTY);
							map_set_cell_type(map, x, y-2, CELL_BOX);
							player->y--;
							move = 1;
							}
						}
					}
				else {
					player->y--;
					move = 1;
					}
				}

		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1)) {
			if(y+1<=map_get_height(map)-1 ){
				if(map_get_cell_type(map, x, y+1) == CELL_BOX) {
					if(map_get_cell_type(map, x, y+2) == CELL_EMPTY){
						if(y+2<=11){
							map_set_cell_type(map, x, y+1, CELL_EMPTY);
							map_set_cell_type(map, x, y+2, CELL_BOX);
							player->y++;
							move = 1;
							}
						}
					}
				else {
					player->y++;
					move = 1;
					}
			}
		}
		break;

	case WEST:
		if (player_move_aux(player, map, x - 1, y)) {
			if(x-1>=0){
				if(map_get_cell_type(map, x-1, y) == CELL_BOX) {
					if(map_get_cell_type(map, x-2, y) == CELL_EMPTY){
						if(x-2>=0){
							map_set_cell_type(map, x-1, y, CELL_EMPTY);
							map_set_cell_type(map, x-2, y, CELL_BOX);
							player->x--;
							move = 1;
							}
						}
					}
				
				else {
					player->x--;
					move = 1;
					}
			}
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y)) {
			if(x+1<=map_get_width(map)-1) {
				if(map_get_cell_type(map, x+1, y) == CELL_BOX) {
					if(map_get_cell_type(map, x+2, y) == CELL_EMPTY){
						if(x+2<=map_get_width(map)){
							map_set_cell_type(map, x+1, y, CELL_EMPTY);
							map_set_cell_type(map, x+2, y, CELL_BOX);
							player->x++;
							move = 1;
							}
						}
					}
				else {
					player->x++;
					move = 1;
					}
			}
		}
		break;
	}

	//if (move) {
		//map_set_cell_type(map, x, y, CELL_EMPTY);
	//}
	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

