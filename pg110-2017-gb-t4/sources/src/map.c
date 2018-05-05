/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <player.h>




struct map {
	int width;
	int height;
	unsigned char* grid;
	int map_level;
};

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	assert(map);
	return 1;
}

void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map);
}

int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}


int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
	
}

int map_get_level(struct map* map) {
	assert(map);
	return map->map_level;
	
}

void map_dec_level(struct map* map,int lvlnum) {
	assert(map);
	map->map_level=lvlnum -1;
}

void map_inc_level(struct map* map, int lvlnum) {
	assert(map);
	map->map_level = lvlnum + 1;
}

void map_set_level(struct map* map) {
	assert(map);
	map->map_level = map->map_level + 1;
		
}

enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0xf0;
}

enum cell_type map_get_cell_sub_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

char map_get_door_level(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	unsigned char type = map->grid[CELL(x, y)];
	return  ((type & 0x0e) >> 1);
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}

void display_bomb(struct map* map, int x, int y, unsigned char type){
	switch (type & 0x0f) {
		case 0:
			window_display_image(sprite_get_bomb4(), x, y);
			break;
		case 1:
			window_display_image(sprite_get_bomb3(), x, y);
			break;
		case 2:
			window_display_image(sprite_get_bomb2(), x, y);
			break;
		case 3:
			window_display_image(sprite_get_bomb1(), x, y);
			break;

	}

}





void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
		
	case BONUS_LIFE:
		window_display_image(sprite_get_bonus(BONUS_LIFE), x, y);
		break;
	}
}

void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	}
}

void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);
	char map_next_level(struct map* map);
	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];
	    
	    switch (type & 0xf0) {
		case CELL_SCENERY:
		  display_scenery(map, x, y, type);
		  break;
	    case CELL_BOX:
	      window_display_image(sprite_get_box(), x, y);
	      break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
	    case CELL_DOOR:
	      // pas de gestion du type de porte
	    	switch (type & 0x01){
	    		case 0:
	    			window_display_image(sprite_get_door_closed(), x, y);
	    			break;
	    		case 1:
	    			window_display_image(sprite_get_door_opened(), x, y);
	    			break;
	    	break;
	    	
	    	}
	    
	      break;
	    case CELL_BOMB:
	    	display_bomb(map,x,y,type);
	    	break;
		case CELL_FIRE:
			window_display_image(sprite_get_fire(), x, y);
			break;
	    }
	  }
	}
}

struct map* map_get_static(void){
	struct map* map = map_new(STATIC_MAP_WIDTH, STATIC_MAP_HEIGHT);

	unsigned char themap[STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT] = {
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
		  CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
		  CELL_BOX, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY , CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_TREE, CELL_BOX, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_TREE, CELL_TREE, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
		  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
		  CELL_BOX, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,  CELL_BOX_LIFE, CELL_EMPTY,
		  CELL_BOX,  CELL_EMPTY, CELL_DOOR, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			};

	FILE* fichier2 =NULL;
	fichier2 = fopen("map/currentlevel.txt","w");
	fputc('0',fichier2);
	fputs("\n",fichier2);
	fputc('0',fichier2);
	fputs(":",fichier2);
	fputs("0,0",fichier2);
	fclose(fichier2);

	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
			map->grid[i] = themap[i];
	return map;
}

struct map* load_map(int* N)
{
	
	char name[]="map/map_";
	char levelnum[2];
	sprintf(levelnum, "%d", *N);
	strcat(name,levelnum);
	int i=0;
	int j=0;
	int k=0;
	int width;
	int height;
	FILE* fichier =NULL;
	fichier = fopen(name,"r+");

	
	
	fscanf(fichier, "%dx%d" , &width , &height);
	struct map* map = map_new(width,height);
	
	
	for (i = 0; i < width; i++){
			 for (j = 0; j < height; j++){
				 fscanf(fichier, "%d" , &k);
				 map->grid[CELL(j,i)] = k;
			 }
	}

	fclose(fichier);
	return map;

		
}

int door_is_open(int x, int y, struct map* map){
	unsigned char type = map->grid[CELL(x,y)];
	return (type & 0x01);
}



