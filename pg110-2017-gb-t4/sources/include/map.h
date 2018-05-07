/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MAP_H_
#define MAP_H_

enum cell_type {
	CELL_EMPTY=0x00,   		//  0000 0000
	CELL_SCENERY=0x10, 		//  0001 0000
	CELL_BOX=0x20,   		//  0010 0000
	CELL_DOOR=0x30,      	//  0011 0000
	CELL_KEY=0x40,       	//  0100 0000
	CELL_BONUS=0x50, 		// 	0101 0000
	CELL_MONSTER=0x60, 		// 	0110 0000
	CELL_BOMB=0x70, 		// 	0111 0000
	CELL_FIRE=0x80,			//  1000 0000

};

enum bonus_type {
	BONUS_BOMB_RANGE_DEC=1,
	BONUS_BOMB_RANGE_INC,
	BONUS_BOMB_NB_DEC,
	BONUS_BOMB_NB_INC,
	BONUS_MONSTER,
	BONUS_LIFE
};

enum scenery_type {
	SCENERY_STONE,     // 0000 
	SCENERY_TREE,      // 0001
	SCENERY_PRINCESS   // 0010 
};

enum compose_type {
	CELL_TREE     = CELL_SCENERY | SCENERY_TREE,
	CELL_STONE    = CELL_SCENERY | SCENERY_STONE,
	CELL_PRINCESS = CELL_SCENERY | SCENERY_PRINCESS,

    CELL_BOX_RANGEINC = CELL_BOX | BONUS_BOMB_RANGE_DEC,
    CELL_BOX_RANGEDEC = CELL_BOX | BONUS_BOMB_RANGE_INC,
	CELL_BOX_BOMBINC  = CELL_BOX | BONUS_BOMB_NB_DEC,
    CELL_BOX_BOMBDEC  = CELL_BOX | BONUS_BOMB_NB_INC,
    CELL_BOX_LIFE     = CELL_BOX | BONUS_MONSTER,
    CELL_BOX_MONSTER  = CELL_BOX | BONUS_LIFE,
};

enum bomb_type {
	BOMB_1 = CELL_BOMB,     // 0000 
	BOMB_2 = CELL_BOMB,      // 0001
	BOMB_3 = CELL_BOMB,		// 0010 
	BOMB_4 = CELL_BOMB,		// 0011 
};

struct map;

// Create a new empty map
struct map* map_new(int width, int height);
void map_free(struct map* map);

struct map* map_load_map(char level[]);

// Return the height and width of a map
int map_get_width(struct map* map);
int map_get_height(struct map* map);

// Return the type of a cell
enum cell_type map_get_cell_type(struct map* map, int x, int y);
enum cell_type map_get_cell_sub_type(struct map* map, int x, int y);
enum bonus_type map_get_bonus_type(struct map* map, int x, int y);
enum scenery_type map_get_scenery_type(struct map* map, int x, int y);

// Set the type of a cell
void  map_set_cell_type(struct map* map, int x, int y, enum cell_type type);

// Test if (x,y) is within the map
int map_is_inside(struct map* map, int x, int y);

// Return a default static map
struct map* map_get_static();

// Display the map on the screen
void map_display(struct map* map);
struct map* load_map(int* N);
int map_get_level(struct map* map);
int door_is_open(int x, int y, struct map* map);
void map_dec_level(struct map* map, int lvlnum);
void map_inc_level(struct map* map, int lvlnum);
char map_get_door_level(struct map* map, int x, int y);
char map_next_level(struct map* map);

#endif /* MAP_H_ */
