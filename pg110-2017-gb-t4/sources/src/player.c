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
	int last_attacked;
	int next_level;
};


struct player* player_init(int bomb_number, int key_number, int hp_number, int bomb_range) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->key = 0;
	player->hp = hp_number;
	player->bomb_range = bomb_range;
	player->last_attacked = 0;
	player->next_level = 0;

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

int player_get_last_attacked(struct player* player){
	assert(player != NULL);
	return player->last_attacked;
}

void player_set_last_attacked(struct player* player,int time){
	assert(player);
	player->last_attacked = time;
}
void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

void player_dec_hp(struct player* player) {
	assert(player);
	int currentHp = player->hp;
	if (currentHp>=1){
	player->hp = currentHp - 1;
	}
	if (currentHp == 0){
		player_game_over();
	}
}

int player_get_hp(struct player* player) {
	assert(player);
	return player->hp;
}

void player_inc_hp(struct player* player) {
	assert(player);
	int currentHp = player->hp;
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

void player_dec_bomb_range(struct player* player) {
	assert(player);
	player->bomb_range -= 1;
}

void player_inc_bomb_range(struct player* player) {
	assert(player);
	player->bomb_range += 1;
}

int player_get_bomb_range(struct player* player){
	assert(player);
	return player->bomb_range;
}

int player_get_next_level(struct player* player){
	assert(player);
	return player->next_level;
}
void player_set_next_level(struct player* player){
	assert(player);
	player->next_level=0;
}
int player_get_key(struct player* player){
	assert(player);
	return player->key;
}
static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;
	int currentTime = SDL_GetTicks();

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		switch(map_get_scenery_type(map,x,y)){
		case SCENERY_PRINCESS:
			player_win_game();
			return 1;
			break;
		default:
			return 0;
			break;
		}

		break;

	case CELL_BOX:
		return 1;
		break;

	case CELL_BONUS:
			switch(map_get_bonus_type(map,x,y)){
			case BONUS_BOMB_RANGE_INC:
				player_inc_bomb_range(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_BOMB_RANGE_DEC:
				player_dec_bomb_range(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_BOMB_NB_DEC:
				player_dec_nb_bomb(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_BOMB_NB_INC:
				player_inc_nb_bomb(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_LIFE:
				player_inc_hp(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_MONSTER:
				break;
			}
		return 1;
		break;

	case CELL_BOMB:
		return 0;
		break;
		
	case CELL_DOOR:
		if (door_is_open(x, y, map) == 1) {
			int currentlvl = map_get_level(map);
			int lvl = map_get_door_level(map,x,y);
			map_free(map);
			int* ptr;
			ptr = &lvl;
			load_map(ptr);
			if (lvl <= currentlvl) {
				map_dec_level(map, currentlvl);
			}
			else {
				map_inc_level(map, currentlvl);
			}
			player->next_level=1;
			return 1;
		}
		else if (door_is_open(x,y,map) == 0 && (player->key >= 1)) {
			int currentlvl = map_get_level(map);
			int lvl = map_get_door_level(map,x,y);
			map_free(map);
			int* ptr;
			ptr = &lvl;
			load_map(ptr);
			if (lvl <= currentlvl) {
				map_dec_level(map, currentlvl);
			}
			else {
				map_inc_level(map, currentlvl);
			}
			player->key--;
			player->next_level=1;
			return 1;
		}

		else {
			return 0;

		}
		break;
	case CELL_FIRE:
		if (currentTime - player_get_last_attacked(player) > 2000) {
			player_dec_hp(player);
			player_set_last_attacked(player,SDL_GetTicks());
		}
		return 1;
		break;
	case CELL_MONSTER:
		if (currentTime - player_get_last_attacked(player) > 2000) {
			player_dec_hp(player);
			player_set_last_attacked(player,SDL_GetTicks());
		}
		return 1;
		break;
	case CELL_KEY:
		player->key++;
		map_set_cell_type(map,x,y,CELL_EMPTY);
		return 1;
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
							map_set_cell_type(map, x, y-2, CELL_BOX | map_get_cell_sub_type(map,x,y-1));
							map_set_cell_type(map, x, y-1, CELL_EMPTY);
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
						if(y+2<=map_get_height(map)-1){
							map_set_cell_type(map, x, y+2, CELL_BOX | map_get_cell_sub_type(map,x,y+1));
							map_set_cell_type(map, x, y+1, CELL_EMPTY);
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
							map_set_cell_type(map, x-2, y, CELL_BOX | map_get_cell_sub_type(map,x-1,y));
							map_set_cell_type(map, x-1, y, CELL_EMPTY);
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
						if(x+2<=map_get_width(map)-1){
							map_set_cell_type(map, x+2, y, CELL_BOX | map_get_cell_sub_type(map,x+1,y));
							map_set_cell_type(map, x+1, y, CELL_EMPTY);
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

	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

void player_game_over(){
	window_refresh();
	SDL_Delay(5000);
	exit(EXIT_SUCCESS);
}

void player_win_game(){
	window_refresh();
	SDL_Delay(5000);
	exit(EXIT_SUCCESS);
}
