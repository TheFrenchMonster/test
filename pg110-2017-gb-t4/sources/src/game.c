/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <player.h>
#include <map.h>
#include <bomb.h>
#include <monster.h>


struct game {
	struct map** maps;       // the game's map
	short max_levels;        // nb maps of the game
	short current_level;
	struct player* player;
	struct bomb* bomb;
	struct monster* monster;
};

struct game*
game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->maps = malloc(sizeof(struct game));
	int x=0;
	int* ptr;
	ptr=&x;
	game->maps[0] = load_map(ptr);
	ini_map_level(game->maps[0]);
	game->max_levels = 1;
	game->current_level = 0;
	game->bomb = NULL;
	game->monster = NULL;
	game->player = player_init(5,1,2,2);
	// Set default location of the player
	player_set_position(game->player, 1, 0);
	game->monster = monster_spawn_map(game->monster, game->maps[0]);


	return game;
}

void game_next_map(struct game* game){
	if(player_get_next_level(game->player)==1){
		free(game->monster);
		free(game->bomb);
		game->monster=NULL;
		game->bomb=NULL;
		game->monster = monster_spawn_map(game->monster, game->maps[0]);
		player_set_next_level(game->player);
	}
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->max_levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->current_level];
}

struct bomb* game_get_bomb(struct game* game){
	assert(game);
	return game->bomb;
}

struct monster* game_get_monster(struct game* game){
	assert(game);
	return game->monster;
}

struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_hp(game_get_player(game))), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_bomb_range(game_get_player(game))), x, y);



	if(player_get_key(game_get_player(game)) >=1 ){
		x = 4 * white_bloc + 5 * SIZE_BLOC;
		window_display_image(sprite_get_key(), x, y);
	}
}

void game_display(struct game* game) {
	assert(game);
	printf("%d",map_get_level(game_get_current_map(game)));
	window_clear();
	game_next_map(game);
	update_bomb(game->bomb,game->maps[0],game->player, &(game->monster));
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);
	game_update_monsters(game->monster, game,game_get_current_map(game));


	window_refresh();
}

static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	struct bomb* bomb = game_get_bomb(game);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				game->bomb=set_bomb(player,map,bomb);
				break;
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

void game_update_monsters(struct monster* monster, struct game* game,struct map* map){
	assert(game);
	while(monster != NULL){
		monster_move(monster, game_get_current_map(game), game->player);
		window_display_image(sprite_get_monster(monster_get_direction(monster)),monster_get_x(monster)*40,monster_get_y(monster)*40);
		monster = monster_get_next_monster(monster);
	}
	game->monster = kill_monster(game->monster,game_get_current_map(game));
}
int game_update(struct game* game) {
	if (input_keyboard(game))
		return 1; // exit game

	return 0;
}
