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

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
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

enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0xf0;
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
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
	      window_display_image(sprite_get_door_opened(), x, y);
	      break;
	    case CELL_BOMB:
	    	window_display_image(sprite_get_bomb(), x, y);
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

		for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
			map->grid[i] = themap[i];
	return map;
}

struct map* map_load_map(void)
{
	unsigned char themap[200];
	int i=0;
	int k=0;
	int l=0;
	char width[5]="";
	char height[5]="";
	char *chaine[100];

		FILE* fichier =NULL;
		fichier = fopen("pg110-2017-gb-t4/sources/map/map_1","r");

		while (fgets(*chaine, strlen(*chaine), fichier) != NULL) {
			if(i==0) {
				while(*chaine[k] != 'x') {
					strcat(width,chaine[k]);
					k++;
					}
				k++;
				while(*chaine[k] != '\0') {
					strcat(height,chaine[k]);
					k++;
					}
				k=0;
				i++;
				}
			else {
				while(*chaine[k] != '\0') {
					char cell[3];
					while(*chaine[k] != ' ') {
						strcat(cell,chaine[k]);
						k++;
					}

					switch(atoi(cell)) {
						case 0:
							themap[l]=CELL_EMPTY;
							l++;
							break;
						case 16:
							themap[l]=CELL_STONE;
							l++;
							break;
						case 17:
							themap[l]=CELL_TREE;
							l++;
							break;
						case 18:
							themap[l]=CELL_PRINCESS;
							l++;
							break;
						case 32:
							themap[l]=CELL_BOX;
							l++;
							break;
						case 33:
							themap[l]=CELL_BOX_RANGEINC;
							l++;
							break;
						case 34:
							themap[l]=CELL_BOX_RANGEDEC;
							l++;
							break;
						case 35:
							themap[l]=CELL_BOX_BOMBINC;
							l++;
							break;
						case 36:
							themap[l]=CELL_BOX_BOMBDEC;
							l++;
							break;
						case 37:
							themap[l]=CELL_BOX_LIFE;
							l++;
							break;
						case 38:
							themap[l]=CELL_BOX_MONSTER;
							l++;
							break;
						case 48:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 49:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 50:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 51:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 52:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 53:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 54:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 55:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 56:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 57:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 58:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 59:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 60:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 61:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 62:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 63:
							themap[l]=CELL_DOOR;
							l++;
							break;
						case 64:
							themap[l]=CELL_KEY;
							l++;
							break;

						case 80:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 81:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 82:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 83:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 84:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 85:
							themap[l]=CELL_BONUS;
							l++;
							break;
						case 96:
							themap[l]=CELL_MONSTER;
							l++;
							break;
						case 112:
							themap[l]=CELL_BOMB;
							l++;
							break;
						default:
							l++;
							break;
					}

				}
			}

	}
		struct map* map = map_new(atoi(width), atoi(height));
		for (int j = 0; j < atoi(width)*atoi(height); j++){
				map->grid[j] = themap[j];
		}
			return map;
}

