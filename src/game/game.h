#pragma once
#include "../engine/engine.h"

typedef struct {
    char * map_str;
    sgfx_ogl_prim_t tile_ogl;
    uphys_obj_t * coll_tiles;
    u32 tile_len;
    uv2_t player_pos;
    uv2_t enmy_pos;
    uv2_t end_pos;
    uphys_obj_t end_tile;
} lvl_t;

typedef struct {
    lvl_t * lvls;
    u32 number_of_lvls;
    u32 curr_lvl;
} game_lvls_t;

typedef struct {
    // config_file_t config;
    sgfx_init_info_t info;
    sgfx_timer_t timer;

    sgfx_swe_renderer_t texture;
    sgfx_ogl_renderer_t renderer;
    TTF_Font * font;

    // 3 types for each entity 
    sgfx_ogl_prim_t player_ogl;
    uphys_obj_t player_phys;
    u32 current_goal;

    i32 lvl_complete;
    u08 finished_game;
    uv2_t last_recorded_pos;
    sgfx_ogl_prim_t enemy_ogl;
    uphys_obj_t enemy_phys;

    f32 air_time;
    uv2_t camera;

    u32 number_textures[10];

    game_lvls_t lvls;

    um4x4_t projection;

} game_state_t;

typedef struct {
    game_state_t * s;
    sgfx_program_t * p;
} game_t;

void game_init(game_t * g);
void game_run(void * _g);
void game_close(game_t * g);

