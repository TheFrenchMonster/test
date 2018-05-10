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
#include <monster.h>

#define min(a,b) (a<=b?a:b)

struct bomb {
	int x;
	int y;
	int birth;
	int bomb_range;
	struct bomb *next;
	struct bomb *prev;
	int around[4];

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
		new_bomb->around[0] = player_get_bomb_range(player);
		new_bomb->around[1] = player_get_bomb_range(player);
		new_bomb->around[2] = player_get_bomb_range(player);
		new_bomb->around[3] = player_get_bomb_range(player);

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



void start_fire(struct map* map, int x, int y, struct player* player, int bomb_range, struct bomb* bomb,struct monster** monster){
	map_set_cell_type(map, x, y, CELL_FIRE);
	for(int i=0; i<=min(bomb_range,bomb->around[0]);i++) {
		if(x+i<=map_get_width(map)-1){
			if (map_get_cell_type(map, x +i, y) != CELL_SCENERY && map_get_cell_type(map, x+i , y) != CELL_DOOR && map_get_cell_type(map, x+i , y) != CELL_BOX ) {
				if ((x+i == player_get_x(player)) && (y  == player_get_y(player))){
					int currentTime = SDL_GetTicks();
					if (currentTime - player_get_last_attacked(player) > 2000) {
						player_dec_hp(player);
						player_set_last_attacked(player,SDL_GetTicks());
						map_set_cell_type(map, x+i, y, CELL_FIRE);
						break;
					}
				}
				else if ((map_get_cell_type(map, x+i , y) == CELL_BONUS)){
					break;
				}
				else if (map_get_cell_type(map, x+i , y) == CELL_MONSTER){
					decrease_hp_monster(monster, x+i,y);
				}


				else 	{
					map_set_cell_type(map, x+i, y, CELL_FIRE);
				}
			}

			else if (map_get_cell_type(map, x+i , y) == CELL_BOX){
				if (map_get_cell_sub_type(map,x+i,y) == 0) {
					map_set_cell_type(map, x+i, y, CELL_EMPTY);
					break;
				}
				else {
					map_set_cell_type(map, x+i, y, CELL_BONUS | map_get_cell_sub_type(map,x+i,y));
					break;
				}
			}
			else {
				break;
			}

		}
	}





	for(int i=0; i<=min(bomb_range,bomb->around[1]);i++) {
		if(x-i>=0){
			if (map_get_cell_type(map, x -i, y) != CELL_SCENERY && map_get_cell_type(map, x-i , y) != CELL_DOOR ) {
				if ((x-i == player_get_x(player)) && (y  == player_get_y(player))){
					int currentTime = SDL_GetTicks();
					if (currentTime - player_get_last_attacked(player) > 2000) {
						player_dec_hp(player);
						player_set_last_attacked(player,SDL_GetTicks());
						map_set_cell_type(map, x-i, y, CELL_FIRE);
						break;
					}
				}
				else if ((map_get_cell_type(map, x-i , y) == CELL_BONUS)){
					break;
				}
				else if (map_get_cell_type(map, x-i , y) == CELL_MONSTER){
					decrease_hp_monster(monster, x-i,y);
				}

				else {
					map_set_cell_type(map, x-i, y, CELL_FIRE);
				}
			}
			else if (map_get_cell_type(map, x-i , y) == CELL_BOX){
				if (map_get_cell_sub_type(map,x-i,y) == 0) {
					map_set_cell_type(map, x-i, y, CELL_EMPTY);
					break;
				}
				else {
					map_set_cell_type(map, x-i, y, CELL_BONUS | map_get_cell_sub_type(map,x-i,y));
					break;
				}
			}
			else 	{
				break;
			}
		}
	}

	for(int i=0; i<=min(bomb_range,bomb->around[2]);i++) {
		if(y+i<=map_get_height(map)-1){
			if (map_get_cell_type(map, x , y+i) != CELL_SCENERY && map_get_cell_type(map, x , y+i) != CELL_DOOR && map_get_cell_type(map, x , y+i) != CELL_BOX) {
				if ((x == player_get_x(player)) && (y +i == player_get_y(player))){
					int currentTime = SDL_GetTicks();
					if (currentTime - player_get_last_attacked(player) > 2000) {
						player_dec_hp(player);
						player_set_last_attacked(player,SDL_GetTicks());
						map_set_cell_type(map, x, y+i, CELL_FIRE);
						break;
					}
				}
				else if ((map_get_cell_type(map, x , y+i) == CELL_BONUS)){
					break;
				}
				else if (map_get_cell_type(map, x , y+i) == CELL_MONSTER){
					decrease_hp_monster(monster, x,y+i);
				}

				else {
					map_set_cell_type(map, x, y+i, CELL_FIRE);
				}

			}

			else if (map_get_cell_type(map, x , y+i) == CELL_BOX){
				if (map_get_cell_sub_type(map,x,y+i) == 0) {
					map_set_cell_type(map, x, y+i, CELL_EMPTY);
					printf("%d\n",0);
					break;
				}
				else {
					map_set_cell_type(map, x, y+i, CELL_BONUS | map_get_cell_sub_type(map,x,y+i));
					printf("%d\n",1);
					break;
				}
			}
			else {
				break;
			}

		}
	}

	for(int i=0; i<=min(bomb_range,bomb->around[3]);i++) {
		if(y-i>=0){
			if (map_get_cell_type(map, x , y-i) != CELL_SCENERY && map_get_cell_type(map, x , y-i) != CELL_DOOR && map_get_cell_type(map, x , y-i) != CELL_BOX) {
				if ((x == player_get_x(player)) && (y -i == player_get_y(player))){
					int currentTime = SDL_GetTicks();
					if (currentTime - player_get_last_attacked(player) > 2000) {
						player_dec_hp(player);
						player_set_last_attacked(player,SDL_GetTicks());
						map_set_cell_type(map, x, y-i, CELL_FIRE);
						break;
					}
				}
				else if ((map_get_cell_type(map, x, y-i) == CELL_BONUS)){
					break;
				}
				else if (map_get_cell_type(map, x , y-i) == CELL_MONSTER){
					decrease_hp_monster(monster, x,y-i);
				}

				else {
					map_set_cell_type(map, x, y-i, CELL_FIRE);
				}

			}
			else if (map_get_cell_type(map, x , y-i) == CELL_BOX){
				if (map_get_cell_sub_type(map,x,y-i) == 0) {
					map_set_cell_type(map, x, y-i, CELL_EMPTY);
					printf("%d\n",0);
					break;
				}
				else {
					map_set_cell_type(map, x, y-i, CELL_BONUS | map_get_cell_sub_type(map,x,y-i));
					printf("%d\n",1);
					break;
				}
			}


			else {
				break;
			}

		}
	}
}

void stop_fire(struct map* map, int x, int y, struct player* player, int bomb_range) {
	for (int i=0; i<=bomb_range;i++) {

		if (map_get_cell_type(map,x+i,y) == CELL_FIRE) {
			map_set_cell_type(map,x+i,y,CELL_EMPTY);
			if(i==0){
				player_inc_nb_bomb(player);
			}

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

void destroy_bonus(struct map* map, int x, int y,int bomb_range){
	for (int i=0; i<=bomb_range;i++){
		if(map_get_cell_type(map,x+i,y)== CELL_BONUS ){
			map_set_cell_type(map,x+i,y,CELL_EMPTY);
		}
	}
	for (int i=0; i<=bomb_range;i++){
			if(map_get_cell_type(map,x-i,y)== CELL_BONUS ){
				map_set_cell_type(map,x-i,y,CELL_EMPTY);
			}
		}
	for (int i=0; i<=bomb_range;i++){
			if(map_get_cell_type(map,x,y+i)== CELL_BONUS ){
				map_set_cell_type(map,x,y+i,CELL_EMPTY);
			}
		}
	for (int i=0; i<=bomb_range;i++){
			if(map_get_cell_type(map,x,y-i)== CELL_BONUS ){
				map_set_cell_type(map,x,y-i,CELL_EMPTY);
			}
		}
}

void set_bomb_time(struct bomb* bomb, int pause){
	int currentTime= SDL_GetTicks();
	struct bomb* current_bomb = bomb;
	while (current_bomb !=NULL){
		current_bomb->birth = current_bomb->birth + currentTime - pause;
		current_bomb = current_bomb->next;
	}
}

void check_fire(struct bomb* bomb, struct map* map, int x, int y, int bomb_range,struct monster** monster){
	for(int i=0;i<=bomb_range;i++){
		if (map_get_cell_type(map,x+i,y) == CELL_BOX){
			bomb->around[0]=i-1;
			if (map_get_cell_sub_type(map,x+i,y) == 0) {
				map_set_cell_type(map, x+i, y, CELL_EMPTY);
				printf("%d\n",0);
				break;
			}
			else if (map_get_cell_sub_type(map,x+i,y) == 5){
				spawn_new_monster(*monster,map,x+i,y);
				map_set_cell_type(map, x+i, y, CELL_EMPTY);
			}
			else {
				map_set_cell_type(map, x+i, y, CELL_BONUS | map_get_cell_sub_type(map,x+i,y));
				printf("%d\n",1);
				break;
			}
			break;
		}
	}
	for(int i=0;i<=bomb_range;i++){
		if (map_get_cell_type(map,x-i,y) == CELL_BOX){
			bomb->around[1]=i-1;
			if (map_get_cell_sub_type(map,x-i,y) == 0) {
				map_set_cell_type(map, x-i, y, CELL_EMPTY);
				printf("%d\n",0);
				break;
			}
			else if (map_get_cell_sub_type(map,x-i,y) == 5){
				spawn_new_monster(*monster,map,x-i,y);
				map_set_cell_type(map, x-i, y, CELL_EMPTY);
			}
			else {
				map_set_cell_type(map, x-i, y, CELL_BONUS | map_get_cell_sub_type(map,x-i,y));
				printf("%d\n",1);
				break;
			}
			break;
			}
		}
	for(int i=0;i<=bomb_range;i++){
		if (map_get_cell_type(map,x,y+i) == CELL_BOX){
			bomb->around[2]=i-1;
			if (map_get_cell_sub_type(map,x,y+i) == 0) {
				map_set_cell_type(map, x, y+i, CELL_EMPTY);
				printf("%d\n",0);
				break;
			}
			else if (map_get_cell_sub_type(map,x,y+i) == 5){
					spawn_new_monster(*monster,map,x,y+i);
					map_set_cell_type(map, x, y+i, CELL_EMPTY);
			}
			else {
				map_set_cell_type(map, x, y+i, CELL_BONUS | map_get_cell_sub_type(map,x,y+i));
				printf("%d\n",1);
				break;
							}
			break;
		}
	}
	for(int i=0;i<=bomb_range;i++){
		if (map_get_cell_type(map,x,y-i) == CELL_BOX){
			bomb->around[3]=i-1;
			if (map_get_cell_sub_type(map,x,y-i) == 0) {
				map_set_cell_type(map, x, y-i, CELL_EMPTY);
				printf("%d\n",0);
				break;
			}
			else if (map_get_cell_sub_type(map,x,y-i) == 5){
				spawn_new_monster(*monster,map,x,y-i);
				map_set_cell_type(map, x, y-i, CELL_EMPTY);
			}
			else {
				map_set_cell_type(map, x, y-i, CELL_BONUS | map_get_cell_sub_type(map,x,y-i));
				printf("%d\n",1);
				break;
			}
			break;
		}
	}
}
void update_bomb(struct bomb* bomb, struct map* map, struct player* player,struct monster** monster) {
	if(bomb != NULL){
		struct bomb* currentbomb = bomb;
		struct bomb* tmpbomb = malloc(sizeof(*tmpbomb));
		while(currentbomb!=NULL){
			int currentTime= SDL_GetTicks();
			int x = currentbomb->x;
			int y = currentbomb->y;

			int bomb_range = currentbomb->bomb_range;
			int timer = currentTime - bomb_get_birth(currentbomb);
			if (0<timer && timer<1000) {
			map_set_cell_type(map, x, y, 112);
			}
			else if (1000<timer && timer<2000) {
				map_set_cell_type(map, x, y, 113);
			}
			else if (2000<timer && timer<3000) {
				map_set_cell_type(map, x, y, 114);
			}
			else if (3000<timer && timer<4000) {
				map_set_cell_type(map, x, y, 115);
				if (timer>3950){
					destroy_bonus(map,x,y,bomb_range);
					check_fire(currentbomb,map,x,y,bomb_range,monster);
				}
			}
			else if (4000<timer && timer<5000) {
				start_fire(map,x,y,player,bomb_range,currentbomb,monster);
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


	}


