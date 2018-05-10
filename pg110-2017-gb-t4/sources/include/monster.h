/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <player.h>
#include <map.h>

struct monster;

void monster_set_direction(struct monster* monster, enum direction direction);
int monster_get_direction(struct monster* monster);
int monster_get_y(struct monster* monster);
int monster_get_x(struct monster* monster);

struct monster* monster_get_next_monster(struct monster* monster);
void monster_set_position(struct monster* monster, int x, int y);

struct monster* spawn_new_monster(struct monster* monster, struct map* map, int x, int y);
struct monster* monster_spawn_map(struct monster* monster, struct map* map);

int monster_move_aux(struct monster* monster,struct map* map, int x, int y,struct player* player);
int monster_move(struct monster* monster, struct map* map, struct player* player);

struct monster* kill_monster(struct monster* monster,struct map* map);
void decrease_hp_monster(struct monster** monster, int x,int y);

void set_new_monster_time(struct monster* monster, int pause);
