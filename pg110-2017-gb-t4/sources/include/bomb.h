/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <player.h>
#include <map.h>
#include <map.h>

struct bomb;


struct bomb* set_bomb(struct player* player, struct map* map, struct bomb* game_bomb);
int bomb_get_x(struct bomb* bomb);
int bomb_get_y(struct bomb* bomb);
void set_bomb_position(struct bomb* bomb, int x, int y);
int bomb_get_birth(struct bomb* bomb);
void update_bomb(struct bomb* bomb, struct map* map, struct player* player);
void stop_fire(struct map* map, int x, int y, struct player* player, int bomb_range);
void start_fire(struct map* map, int x, int y, struct player* player, int bomb_range,int timer);
