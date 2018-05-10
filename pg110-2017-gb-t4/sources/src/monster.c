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
#include <time.h>




struct monster {
	int x;
	int y;
	enum direction current_direction;
	struct monster* next_monster;
	int speed;
	int last_move;
	int hp;
	int birth;
};

int monster_get_x(struct monster* monster){
	assert(monster);
	return monster->x;
}

int monster_get_y(struct monster* monster){
	assert(monster);
	return monster->y;
}

void monster_set_direction(struct monster* monster, enum direction direction){
	assert(monster);
	monster->current_direction=direction;
}

int monster_get_direction(struct monster* monster){
	assert(monster);
	return monster->current_direction;
}

void monster_set_position(struct monster* monster, int x, int y) {
	assert(monster);
	monster->x=x;
	monster->y=y;
}

struct monster* monster_get_next_monster(struct monster* monster){
	assert(monster);
	return monster->next_monster;
}

struct monster* spawn_new_monster(struct monster* monster, struct map* map, int x, int y){
	assert(map);
	struct monster* new_monster = malloc(sizeof(*monster));
	new_monster->x=x;
	new_monster->y=y;
	new_monster->current_direction= EAST;
	new_monster->last_move=SDL_GetTicks();
	new_monster->speed=1+.2*map_get_level(map);
	new_monster->next_monster=NULL;
	new_monster->hp=1;
	new_monster->birth = SDL_GetTicks();
	if(monster==NULL){
		return new_monster;
	}
	else {
		struct monster* current_monster = monster;
		while( current_monster->next_monster != NULL){
			current_monster=current_monster->next_monster;
		}
		current_monster->next_monster = new_monster;

		return monster;
	}

}

struct monster* monster_spawn_map(struct monster* monster, struct map* map){
	assert(map);
	for(int i=0;i<map_get_width(map);i++){
		for(int j=0;j<map_get_height(map);j++){
			if(map_get_cell_type(map,i,j)==CELL_MONSTER){
				monster=spawn_new_monster(monster,map,i,j);
			}
		}
	}
	return monster;
}

void set_new_monster_time(struct monster* monster, int pause){
	assert(monster);
	int currentTime = SDL_GetTicks();
	struct monster* current_monster = monster;
	while(current_monster != NULL){
		current_monster->last_move = current_monster->last_move + currentTime - pause;
		current_monster = current_monster->next_monster;
	}
}

struct monster* kill_monster(struct monster* monster,struct map* map){
	if(monster != NULL){
		if(monster->hp ==0){
			int x= monster->x;
			int y= monster->y;
			struct monster* tmpmonster = monster->next_monster;
			free(monster);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			tmpmonster = kill_monster(tmpmonster,map);
			return tmpmonster;
		}
		else {
			monster->next_monster = kill_monster(monster->next_monster,map);
			return monster;
		}

	}
	else {
		return NULL;
	}
}

void decrease_hp_monster(struct monster** monster, int x,int y){
	struct monster* current_monster = *monster;
	int currentTime = SDL_GetTicks();
	printf("%d",5);
	while(current_monster != NULL){
		int timer = currentTime - current_monster->birth;
		if(current_monster->x == x && current_monster->y == y && timer > 2000){
			current_monster->hp=0;
		}
		current_monster = current_monster->next_monster;
	}

}
int monster_move_aux(struct monster* monster,struct map* map, int x, int y,struct player* player){
	if (!map_is_inside(map, x, y))
		return 0;
	if(player_get_x(player)==x && player_get_y(player)==y){
		int currentTime=SDL_GetTicks();
		if (currentTime - player_get_last_attacked(player) > 2000) {
			player_dec_hp(player);
			player_set_last_attacked(player,SDL_GetTicks());
		}
	}
	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;
	case CELL_BOX:
		return 0;
		break;
	case CELL_BONUS:
		return 0;
		break;
	case CELL_DOOR:
		return 0;
		break;
	case CELL_KEY:
		return 0;
		break;
	case CELL_MONSTER:
		return 0;
		break;
	default:
		return 1;
		break;

	}
	return 1;
}

int random_number(int x, int y){

	return rand()%(y-x) +x;
}

int monster_move(struct monster* monster, struct map* map, struct player* player){
	int x = monster->x;
	int y = monster->y;
	int move=0;
	int currentTime = SDL_GetTicks();
	int timer = currentTime - (monster->last_move)*(1/(monster->speed));
	if(timer > 2500){
		monster ->current_direction = random_number(0,4);
		switch(monster->current_direction){
		case NORTH:
			if(monster_move_aux(monster,map,x,y-1,player) && y-1>=0){
				map_set_cell_type(map, x, y, CELL_EMPTY);
				monster->y--;
				move=1;
				monster->last_move=SDL_GetTicks();
				break;
			}
			break;

		case SOUTH:
			if(monster_move_aux(monster,map,x,y+1,player) && y+1< map_get_height(map)){
				map_set_cell_type(map, x, y, CELL_EMPTY);
				monster->y++;
				move=1;
				monster->last_move=SDL_GetTicks();
				break;
					}
			break;

		case EAST:
			if(monster_move_aux(monster,map,x+1,y,player) && x+1<map_get_width(map)){
				map_set_cell_type(map, x, y, CELL_EMPTY);
				monster->x++;
				move=1;
				monster->last_move=SDL_GetTicks();
				break;
			}
			break;

		case WEST:
			if(monster_move_aux(monster,map,x-1,y,player) && x-1>=0){
				map_set_cell_type(map, x, y, CELL_EMPTY);
				monster->x--;
				move=1;
				monster->last_move=SDL_GetTicks();
				break;
			}
			break;

		}

	}
	if (move) {
		map_set_cell_type(map, monster->x, monster->y, CELL_MONSTER);
	}
	return move;
}


