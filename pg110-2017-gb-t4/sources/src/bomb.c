/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h> // IMG_Load

#include <bomb.h>
#include <map.h>
#include <constant.h>
#include <player.h>
#include <assert.h>



struct bomb {
	int x;
	int y;
	int birth;
	int bomb_range;
	struct bomb *next;
	struct bomb *prev;

};
void set_bomb_position(struct bomb* bomb, int x, int y){
	 assert(bomb);
	 bomb->x = x;
	 bomb->y = y;
 }

void bomb_free(struct bomb* bomb){
	 assert(bomb);
	 free(bomb);
 }

int bomb_get_x(struct bomb* bomb){
	 assert(bomb);
	 return bomb->x;
 }

int bomb_get_y(struct bomb* bomb){
	 assert(bomb);
	 return bomb->x;
 }

int bomb_get_birth(struct bomb* bomb){
	assert(bomb);
	return bomb->birth;
}

struct bomb* set_bomb(struct player* player, struct map* map, struct bomb* game_bomb) {
	if(player_get_nb_bomb(player) >= 1){
		int x = player_get_x(player);
		int y = player_get_y(player);
		struct bomb* new_bomb = malloc(sizeof(*new_bomb));
		new_bomb->next=NULL;
		new_bomb->prev=NULL;
		new_bomb->x=x;
		new_bomb->y=y;
		new_bomb->birth=SDL_GetTicks();
		new_bomb->bomb_range = player_get_bomb_range(player);
		printf("%d",new_bomb->bomb_range);

		if (game_bomb!= NULL) {
			game_bomb->prev=new_bomb;
			new_bomb->next=game_bomb;
		}
		map_set_cell_type(map, x, y, CELL_BOMB);
		player_dec_nb_bomb(player);
		return new_bomb;
	}
	return NULL;

}



void start_fire(struct map* map, int x, int y, struct player* player, int bomb_range){
	map_set_cell_type(map, x, y, CELL_FIRE);
	for(int i=0; i<=bomb_range;i++) {
		if (map_get_cell_type(map, x + i, y) != CELL_SCENERY && map_get_cell_type(map, x + i, y) != CELL_DOOR) {
			if(map_get_cell_type(map, x + i, y) == CELL_BOX){
				map_set_cell_type(map, x+i, y, CELL_BONUS | map_get_cell_sub_type(map,x+i,y));
			}
			else if ((x + i == player_get_x(player)) && (y == player_get_y(player))){
				int currentTime = SDL_GetTicks();
				if (currentTime - player_get_last_attacked(player) > 2000) {
					player_dec_hp(player);
					player_set_last_attacked(player,SDL_GetTicks());
				}
			}
			else {
				map_set_cell_type(map, x+i, y, CELL_FIRE);
			}

		}


	}

	for(int i=0; i<=bomb_range;i++) {
		if (map_get_cell_type(map, x - i, y) != CELL_SCENERY && map_get_cell_type(map, x - i, y) != CELL_DOOR) {
			if(map_get_cell_type(map, x - i, y) == CELL_BOX){
				map_set_cell_type(map, x-i, y, CELL_BONUS | map_get_cell_sub_type(map,x-i,y));
			}
			else if ((x - i == player_get_x(player)) && (y == player_get_y(player))){
				int currentTime = SDL_GetTicks();
				if (currentTime - player_get_last_attacked(player) > 2000) {
					player_dec_hp(player);
					player_set_last_attacked(player,SDL_GetTicks());
				}
			}
			else {
				map_set_cell_type(map, x-i, y, CELL_FIRE);
			}
		}

	}

	for(int i=0; i<=bomb_range;i++) {
		if (map_get_cell_type(map, x , y+i) != CELL_SCENERY && map_get_cell_type(map, x , y+i) != CELL_DOOR) {
			if(map_get_cell_type(map, x , y+i) == CELL_BOX){
				map_set_cell_type(map, x, y+i, CELL_BONUS | map_get_cell_sub_type(map,x,y+i));
			}
			else if ((x == player_get_x(player)) && (y +i == player_get_y(player))){
				int currentTime = SDL_GetTicks();
				if (currentTime - player_get_last_attacked(player) > 2000) {
					player_dec_hp(player);
					player_set_last_attacked(player,SDL_GetTicks());
				}
			}
			else {
				map_set_cell_type(map, x, y+i, CELL_FIRE);
			}

		}


	}

	for(int i=0; i<=bomb_range;i++) {
		if (map_get_cell_type(map, x, y-i) != CELL_SCENERY && map_get_cell_type(map, x , y-i) != CELL_DOOR) {
			if(map_get_cell_type(map, x , y-i) == CELL_BOX){
				map_set_cell_type(map, x, y-i, CELL_BONUS | map_get_cell_sub_type(map,x,y-i));
			}
			else if ((x == player_get_x(player)) && (y - i == player_get_y(player))){
				int currentTime = SDL_GetTicks();
				if (currentTime - player_get_last_attacked(player) > 2000) {
					player_dec_hp(player);
					player_set_last_attacked(player,SDL_GetTicks());
				}
			}
			else {
				map_set_cell_type(map, x, y-i, CELL_FIRE);
			}

		}


	}
}

void stop_fire(struct map* map, int x, int y, struct player* player, int bomb_range) {
	for (int i=0; i<=bomb_range;i++) {

		if (map_get_cell_type(map,x+i,y) == CELL_FIRE) {
			map_set_cell_type(map,x+i,y,CELL_EMPTY);
			}

		if (map_get_cell_type(map,x-i,y) == CELL_FIRE) {
			map_set_cell_type(map,x-i,y,CELL_EMPTY);
			}

		if (map_get_cell_type(map,x,y+i) == CELL_FIRE) {
			map_set_cell_type(map,x,y+i,CELL_EMPTY);
			}

		if (map_get_cell_type(map,x,y-i) == CELL_FIRE) {
			map_set_cell_type(map,x,y-i,CELL_EMPTY);
			}
	}
}

void update_bomb(struct bomb* bomb, struct map* map, struct player* player) {
	if(bomb != NULL){
		struct bomb* currentbomb = bomb;
		struct bomb* tmpbomb = malloc(sizeof(*tmpbomb));
		int currentTime= SDL_GetTicks();
		int x = currentbomb->x;
		int y = currentbomb->y;
		int bomb_range = currentbomb->bomb_range;
		int timer = currentTime - bomb_get_birth(currentbomb);
		if (0<timer && timer<1000) {
			map_set_cell_type(map, x, y, BOMB_1);
		}
		else if (1000<timer && timer<2000) {
			map_set_cell_type(map, x, y, BOMB_2);
		}
		else if (2000<timer && timer<3000) {
			map_set_cell_type(map, x, y, BOMB_3);
		}
		else if (3000<timer && timer<4000) {
			map_set_cell_type(map, x, y, BOMB_4);
		}
		else if (4000<timer && timer<5000) {
			start_fire(map,x,y,player,bomb_range);
		}
		else if (timer>6000) {
			stop_fire(map,x,y,player,bomb_range);
			if (currentbomb->next == NULL && currentbomb->prev !=NULL) {
				tmpbomb = currentbomb;
				(currentbomb->prev)->next=NULL;
				free(tmpbomb);
				}
			}
		currentbomb = currentbomb->next;
		}

}

