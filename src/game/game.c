#include "./game.h"
#include "../engine/engine.h"
#include "shader.h"

u32 id, id_two;
#define PLAYER1_TEXTURE_SLOT 0
#define PLAYER2_TEXTURE_SLOT 1
#define TILES_TEXTURE_SLOT 2
#define END_TILE_TEXTURE_SLOT 3
#define FAILED_SLOT 4
#define SUCCEEDED_SLOT 5
#define TEXT1_TEXTURE_SLOT 6
#define TEXT2_TEXTURE_SLOT 7
#define TEXT3_TEXTURE_SLOT 8
#define TEXT4_TEXTURE_SLOT 9

#define SCREEN_WIDTH  1280 
#define SCREEN_HEIGHT 720 
#define ASPECT_RATIO (f32) SCREEN_WIDTH/SCREEN_HEIGHT
#define TSX 36
#define TSY 36
/*
    16 textures:
        - 1 player (2 textures)
        - 8 textures for tileset
        - package (1 texture)
        - level items (2)
        - 3 textures for the text..etc..
*/

void game_get_camera_pos(uv2_t * c, SDL_Window * w, uphys_obj_t * o, u32 tsx, u32 tsy, f64 dt) {
    i32 s_w, s_h;
    SDL_GetWindowSize(w, &s_w, &s_h);
    // c -> x += (((o -> pos.x - c -> x - ((s_w * 0.5) - (tsx * 0.5))) / 5) * dt);
    // c -> y += (((o -> pos.y - c -> y - ((s_h * 0.5) - (tsy * 0.5))) / 5) * dt); 

    c -> x += round(((o -> pos.x - c -> x - ((s_w * 0.5) - (tsx * 0.5))) / 10) * 1);
    c -> y += round(((o -> pos.y - c -> y - ((s_h * 0.5) - (tsy * 0.5))) / 10) * 1); 
}

void game_lvls_load(lvl_t * l, const char * path) {
    l -> map_str = utxt_file_query(path);
    l -> coll_tiles = calloc(strlen(l -> map_str), sizeof(uphys_obj_t));
    l -> tile_len = 0;
}

void game_lvls_create(game_lvls_t * l, const char * lvls_folder_path, u32 number_of_lvls) {
    char * current_map = calloc(128, sizeof(char));
    const char * prefix = lvls_folder_path;
    char * final = calloc(129, sizeof(char));

    l -> number_of_lvls = number_of_lvls;
    l -> lvls = calloc(number_of_lvls, sizeof(lvl_t));

    // SDL_SetTextureBlendMode(l -> default_img.tex, SDL_BLENDMODE_ADD);
    // SDL_SetTextureAlphaMod(l -> default_img.tex, 50);

    for (u08 u = 0; u < number_of_lvls; u++) {
        char * curr_frame = ui32_to_str(u);
        // strncat(current_map, prefix, strlen(prefix));
        // strncat(current_map, curr_frame, strlen(curr_frame));
        // strncat(current_map, ".txt", strlen(".txt"));
        strcat(current_map, prefix);
        strcat(current_map, curr_frame);
        strcat(current_map, ".txt");

        memcpy(final, current_map, strlen(current_map));
        final[strlen(current_map) + 1] = '\0';

        game_lvls_load(&l -> lvls[u], final);

        memset(current_map, 0, strlen(current_map) * sizeof(char));
        memset(final, 0, strlen(final) * sizeof(char));
    }

    i32 x = 0;
    i32 y = 0;
    
    for (u08 u = 0; u < number_of_lvls; u++) {
        sz_t map_len = strlen(l -> lvls[u].map_str); // Reading the file
        for (u32 t = 0; t < map_len; t++) {
            if (l -> lvls[u].map_str[t] != '0' && l -> lvls[u].map_str[t] != '3' && l -> lvls[u].map_str[t] != '5') {
                uphys_obj_mk(&l -> lvls[u].coll_tiles[l -> lvls[u].tile_len], uv2_mk(x * TSX, y * TSY), uv2_mk(0, 0), uv2_mk(TSX, TSY));
                l -> lvls[u].tile_len++;
            }

            if (l -> lvls[u].map_str[t] == '5') {
                l -> lvls[u].end_pos.x = x * TSX;
                l -> lvls[u].end_pos.y = y * TSY;               
                uphys_obj_mk(&l -> lvls[u].end_tile, l -> lvls[u].end_pos, uv2_mk(0, 0), uv2_mk(TSX, TSY));
            }

            if (l -> lvls[u].map_str[t] == '3') {
                l -> lvls[u].player_pos.x = x * TSX;
                l -> lvls[u].player_pos.y = y * TSY;
            }

            if (l -> lvls[u].map_str[t] == '4') {
                l -> lvls[u].enmy_pos.x = x * TSX;
                l -> lvls[u].enmy_pos.y = y * TSY;
            }

            if (l -> lvls[u].map_str[t] != '\n') {
                x++;
            }

            if (l -> lvls[u].map_str[t] == '\n') {
                x = 0;
                y++;
            }
        }
        x = 0;
        y = 0;
    } 

    l -> curr_lvl = 0;
    // l -> lvls[l -> curr_lvl].tile_ogl.pts = calloc(6, sizeof(sgfx_ogl_pt_t));
}

void game_lvls_render(game_lvls_t * l, sgfx_ogl_renderer_t * r, uv2_t * camera) {
    for (u32 t = 0; t < l -> lvls[l -> curr_lvl].tile_len; t++) {
        uphys_obj_t s = l -> lvls[l -> curr_lvl].coll_tiles[t];
        // gsdl_draw_phys_obj_rect(&s, 21, 255, 55, 255, &p -> camera, p -> renderer);

        sgfx_convert_phys_obj_to_ogl(&s, &l -> lvls[l -> curr_lvl].tile_ogl, TILES_TEXTURE_SLOT);
        l -> lvls[l -> curr_lvl].tile_ogl.translation.x -= camera -> x; 
        l -> lvls[l -> curr_lvl].tile_ogl.translation.y -= camera -> y; 
        sgfx_add_prim_to_ogl_renderer(r, &l -> lvls[l -> curr_lvl].tile_ogl);        

    }


    // gsdl_draw_phys_obj_rect(&s, 21, 255, 55, 255, &p -> camera, p -> renderer);

    sgfx_convert_phys_obj_to_ogl(&l -> lvls[l -> curr_lvl].end_tile, &l -> lvls[l -> curr_lvl].tile_ogl, END_TILE_TEXTURE_SLOT);
    l -> lvls[l -> curr_lvl].tile_ogl.translation.x -= camera -> x; 
    l -> lvls[l -> curr_lvl].tile_ogl.translation.y -= camera -> y; 
    sgfx_add_prim_to_ogl_renderer(r, &l -> lvls[l -> curr_lvl].tile_ogl);        


        
}

// max i go to is the hundreds
sgfx_ogl_prim_t numbers[3]; 
u32 current_units = 0;
u32 current_tens = 0;
u32 current_hundreds = 0;
f32 w, h;
sgfx_ogl_prim_t avoid_goal;
sgfx_ogl_prim_t kill_goal;
sgfx_ogl_prim_t failed_goal;

sgfx_ogl_prim_t game_finished;


Mix_Chunk  * failed_sound;
Mix_Chunk * jump_sound;
Mix_Chunk * lvl_completed_sound;
sgfx_ogl_prim_t p;
void game_init(game_t * g) {
    sgfx_alloc_prim(&p, 6);
    sgfx_alloc_prim(&game_finished, 6);
    sgfx_alloc_prim(&failed_goal, 6);
    sgfx_alloc_prim(&kill_goal, 6);
    sgfx_alloc_prim(&avoid_goal, 6);

    { // Initializing SDL2
        g -> s = calloc(1, sizeof(game_state_t));
        g -> s -> info = (sgfx_init_info_t) { 
            "dodgy3quares", { SCREEN_WIDTH, SCREEN_HEIGHT }, SDL_WINDOW_OPENGL 
        };
        
        g -> p = sgfx_init(&g -> s -> info);
    }
    { // OpenGL Renderer Initialization & Game stuff 
        sgfx_mk_ogl_ctx(g -> p -> win);
        huh("[OpenGL Version] %s", glGetString(GL_VERSION));   
        sgfx_mk_ogl_renderer(&g -> s -> renderer, SCREEN_WIDTH, SCREEN_HEIGHT, vs, fs);

        i32 active_texture_slot = glGetUniformLocation(g -> s -> renderer.shader, "texture_slots");
        i32 slots[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        glUniform1iv(active_texture_slot, 10, slots);

        g -> s -> projection = um4x4_orthographic_projection(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
        i32 loc = glGetUniformLocation(g -> s -> renderer.shader, "projection");
        glUniformMatrix4fv(loc, 1, 0, &g -> s -> projection.elems[0][0]);
        g -> s -> font = TTF_OpenFont("res/m6x11.ttf", 24);
    }
    { // Font   
        /*SDL_Surface * surf = TTF_RenderText_Blended(g -> s -> font, "h", (SDL_Color) { 255, 255, 255, 255 });
        for (u32 u = 0; u < 10; u++) {
            char * strn = ui32_to_str(u);
            sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
                SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
            sgfx_end_swe_render(&g -> s -> texture);   
            g -> s -> number_textures[u] = sgfx_mk_ogl_texture(13);
            sgfx_set_ogl_texture_data(surf -> w, surf -> h, surf -> pixels);
        } */

        
        
        char stringy[128] = {0};  
        strcat(stringy, "0123456789");

        SDL_Surface * surf = TTF_RenderText_Blended(g -> s -> font, stringy, (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        w = surf -> w / 10;
        w = w / surf -> w;
        sgfx_mk_ogl_texture(TEXT1_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);


        char * time_passed = "Time Passed (seconds): ";
        surf = TTF_RenderText_Blended(g -> s -> font, time_passed, (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        sgfx_mk_ogl_texture(TEXT2_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);

        char * goal = "Goal: Avoid the pink square & reach the end tile (blue)";
        surf = TTF_RenderText_Blended(g -> s -> font, goal, (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        sgfx_mk_ogl_texture(TEXT3_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);
        sgfx_mk_textured_quad(&avoid_goal, SCREEN_WIDTH / 2, 80, surf -> w, surf -> h, TEXT3_TEXTURE_SLOT);

        char * goal2 = "Goal: Kill (by colliding with it) the pink square";
        surf = TTF_RenderText_Blended(g -> s -> font, goal2, (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        sgfx_mk_ogl_texture(TEXT4_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);
        sgfx_mk_textured_quad(&kill_goal, SCREEN_WIDTH / 2, 80, surf -> w, surf -> h, TEXT4_TEXTURE_SLOT);


        char * failed = "Goal Failed";
        surf = TTF_RenderText_Blended(g -> s -> font, failed, (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        sgfx_mk_ogl_texture(FAILED_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);
        sgfx_mk_textured_quad(&failed_goal, SCREEN_WIDTH / 2, 300, surf -> w, surf -> h, FAILED_SLOT);


        surf = TTF_RenderText_Blended(g -> s -> font, "GAME COMPLETED, TRY TO BEAT YOUR TIME!!", (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, surf -> w, surf -> h);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);       

        sgfx_mk_ogl_texture(SUCCEEDED_SLOT);
        sgfx_set_ogl_texture_data(surf -> w, surf -> h, g -> s -> texture.s -> pixels);
        sgfx_mk_textured_quad(&game_finished, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, surf -> w, surf -> h, SUCCEEDED_SLOT);



    }

    { // Textures 
        SDL_Surface * surf = TTF_RenderText_Blended(g -> s -> font, "(_)", (SDL_Color) { 120, 90, 80, 255 });

        // sgfx_mk_ogl_texture(TEXT1_TEXTURE_SLOT);
        // sgfx_set_ogl_texture_data(surf -> w, surf -> h, surf -> pixels);


        // PLAYER
        SDL_Surface * s_urf = TTF_RenderText_Blended(g -> s -> font, "--_--", (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, 16 + surf -> w, 16 + surf -> h);
            sgfx_start_swe_render(&g -> s -> texture, 180, 128, 255, 255);
            SDL_BlitSurface(s_urf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);

        sgfx_mk_ogl_texture(PLAYER1_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(16 + surf -> w, 16 + surf -> h, g -> s -> texture.s -> pixels);


        // PLAYER
        s_urf = TTF_RenderText_Blended(g -> s -> font, "OoO", (SDL_Color) { 255, 255, 255, 255 });
        sgfx_mk_swe_renderer(&g -> s -> texture, 16 + surf -> w, 16 + surf -> h);
            sgfx_start_swe_render(&g -> s -> texture, 255,8,105, 255);
            SDL_BlitSurface(s_urf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);

        sgfx_mk_ogl_texture(PLAYER2_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(16 + surf -> w, 16 + surf -> h, g -> s -> texture.s -> pixels);

        // TILES
        sgfx_mk_swe_renderer(&g -> s -> texture, 16, 16);
            sgfx_start_swe_render(&g -> s -> texture, 118, 57, 49, 255);
            SDL_BlitSurface(surf, NULL, g -> s -> texture.s, NULL); 
        sgfx_end_swe_render(&g -> s -> texture);

        sgfx_mk_ogl_texture(TILES_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(16, 16, g -> s -> texture.s -> pixels);

        // TILES
        sgfx_mk_swe_renderer(&g -> s -> texture, 16, 16);
            sgfx_start_swe_render(&g -> s -> texture, 100, 100, 255, 255);
        sgfx_end_swe_render(&g -> s -> texture);

        sgfx_mk_ogl_texture(END_TILE_TEXTURE_SLOT);
        sgfx_set_ogl_texture_data(16, 16, g -> s -> texture.s -> pixels);


        g -> s -> finished_game = 0;

/*        u32 nOfColors;
        u32 texture_format;
        SDL_Surface * surface = g -> s -> texture.s;
        nOfColors = surface->format->BytesPerPixel;
        if (nOfColors == 4)     // contains an alpha channel
        {
            if (surface->format->Rmask == 0x000000ff) {
                texture_format = GL_RGBA;
                printf("rgba");
            }
            else {
                texture_format = GL_BGRA;
                printf("bgra");
            }
        } else if (nOfColors == 3)     // no alpha channel
        {
            if (surface->format->Rmask == 0x000000ff) {
                texture_format = GL_RGB;
                printf("rgb");
            }
            else {
                texture_format = GL_BGR;
                printf("bgr");
            }
        } else {
            printf("warning: the image is not truecolor..  this will probably break\n");
            // this error should not go unhandled*/
        //}
    }

    { // Characters/Game Objects/Primitives
        game_lvls_create(&g -> s -> lvls, "res/lvls/lvl_", 4);
        u32 l_c = g -> s -> lvls.curr_lvl;
        uphys_obj_mk(&g -> s -> player_phys, g -> s -> lvls.lvls[l_c].player_pos, uv2_mk(5, 0), uv2_mk(TSX - 6, TSY - 6));
        uphys_obj_mk(&g -> s -> enemy_phys, g -> s -> lvls.lvls[l_c].enmy_pos, uv2_mk(0, 0), uv2_mk(TSX - 5, TSY -5));
        // g -> s -> player_ogl.pts = calloc(6, sizeof(sgfx_ogl_pt_t));
        g -> s -> current_goal = 0;
        g -> s -> camera = uv2_mk(0, 0);
        game_get_camera_pos(&g -> s -> camera, g -> p -> win, &g -> s -> player_phys, TSX, TSY, 1);
        // uphys_obj_mk(&g -> s -> tile_phys, uv2_mk(200, 200 + 32*2), uv2_mk(20, 0), uv2_mk(TSX, TSY));

    }
    for (u32 u = 0; u < 3; u++) {
        sgfx_alloc_prim(&numbers[u], 6);
        sgfx_mk_textured_clip_quad(&numbers[u], 100, 100, 24, 44, 13 + u, w * current_units, w * (current_units + 1));
        //for (u32 n = 0; n < 6; n++) {
        //    numbers[u].pts[n].texture.x = numbers[u].pts[n].texture.x/10 * 2;
        //}
    }
    failed_sound = Mix_LoadWAV("res/game_failed.wav");
    jump_sound = Mix_LoadWAV("res/jump.wav");
    lvl_completed_sound = Mix_LoadWAV("res/lvl_complete.wav");
    // Mix_PlayChannel(-1, game_failed_sound, 0);
    sgfx_alloc_prim(&g -> s -> player_ogl, 6);
    sgfx_alloc_prim(&g -> s -> enemy_ogl, 6);
    for (u32 u = 0; u < g -> s -> lvls.number_of_lvls; u++) {
       sgfx_alloc_prim(&g -> s -> lvls.lvls[u].tile_ogl, 6);
    }


}

u08 tmp;
u08 failed_splayd = 0;

void game_run(void * _g) {
    game_t * g = (game_t *) _g;
    // EVENT 
    //       HANDLING
    { 
        i32 mx, my;
        while (SDL_PollEvent(&g -> p -> e)) {
            switch (g -> p -> e.type) {
                case SDL_QUIT:
                    g -> p-> running = 0;
                    break;
                
                default:
                    break;
            }
        }
        g -> p -> keys_pressed = SDL_GetKeyboardState(NULL);
        g -> s -> player_phys.h_move = 0;

        if (g -> p -> keys_pressed[SDL_SCANCODE_A]) g -> s -> player_phys.h_move = -1;
        if (g -> p -> keys_pressed[SDL_SCANCODE_D]) g -> s -> player_phys.h_move = +1;
        if (g -> p -> keys_pressed[SDL_SCANCODE_SPACE] && g -> s -> air_time < 2) { 
            g -> s -> player_phys.extra_momentum.y = -12.5;
            Mix_PlayChannel(-1, jump_sound, 0);
        }
        if (g -> p -> keys_pressed[SDL_SCANCODE_R]) {
            g -> s -> player_phys.pos = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].player_pos;
            g -> s -> enemy_phys.pos = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].enmy_pos;
            g -> s -> lvl_complete = 0;
            tmp = 0;
            g -> s -> player_phys.moving = 0;
            failed_splayd = 0;
        }

    }

    // GAME
    //      LOGIC    
    if (g -> s -> lvl_complete == 0 && g -> s -> finished_game == 0) {
        f32 dt = sgfx_get_dt(&g -> p -> fps, 60);
        game_get_camera_pos(&g -> s -> camera, g -> p -> win, &g -> s -> player_phys, TSX, TSY, 1);
        
        sgfx_update_timer(&g -> s -> timer, g -> p -> fps.ms_dt); 
        current_units = g -> s -> timer.actual_time;

        if (((i32) g -> s -> timer.count) % 400 == 0) {
            if (g -> s -> current_goal == 0) {
                g -> s -> last_recorded_pos.x = g -> s -> player_phys.pos.x;
                g -> s -> last_recorded_pos.y = g -> s -> player_phys.pos.y;
            } else {
                g -> s -> last_recorded_pos.x = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].end_tile.pos.x;
                g -> s -> last_recorded_pos.y = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].end_tile.pos.y;
            }
        }

        if (g -> s -> timer.actual_time % 2 && g -> s -> timer.count == 0) {
            huh("Frame Time (ms): %f", g -> p -> fps.ms_dt); 
        }
        
        if (current_units >= 10) {
            current_units = 0; 
            current_tens++;
            g -> s -> timer.actual_time = 0;
            g -> s -> timer.count = 0;
        }

        if (current_tens >= 10) {
            current_tens = 0;
            current_units = 0; 
            current_hundreds++;
        }
        sgfx_mk_textured_clip_quad(&numbers[0], 150, 110, numbers[0].scale.x, numbers[0].scale.y, TEXT1_TEXTURE_SLOT, w * current_units, w * (current_units + 1));
        sgfx_mk_textured_clip_quad(&numbers[1], 120, 110, numbers[1].scale.x, numbers[1].scale.y, TEXT1_TEXTURE_SLOT, w * current_tens, w * (current_tens + 1));
        sgfx_mk_textured_clip_quad(&numbers[2],  90, 110, numbers[2].scale.x, numbers[2].scale.y, TEXT1_TEXTURE_SLOT, w * current_hundreds, w * (current_hundreds + 1));
        

        uphys_obj_update_vel(&g -> s -> player_phys, uv2_mk(1, 1), 1);

        g -> s -> player_phys.extra_momentum.y += 0.4;
        if (g -> s -> player_phys.v_coll == 1) {
            g -> s -> player_phys.extra_momentum.y = 0;
            g -> s -> air_time = 0;
        } else {
            g -> s -> air_time += 0.25;
        } 

        if (g -> s -> player_phys.v_coll == -1) {
            g -> s -> player_phys.extra_momentum.y += 0.4;
        }

        lvl_t l = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl];
        uphys_obj_update_pos(&g -> s -> player_phys, l.coll_tiles, l.tile_len, 1);
        if (g -> s -> player_phys.moving) { tmp = 1; } 

        if (tmp) {
            // uv2_dist_between(g -> s -> player_phys.pos, g -> s -> playe)
            f32 distx = g -> s -> last_recorded_pos.x - g -> s -> enemy_phys.pos.x;
            f32 disty = g -> s -> last_recorded_pos.y - g -> s -> enemy_phys.pos.y;
        
            f32 direction = atan2(disty, distx);

            g -> s -> enemy_phys.pos.x += cos(direction) * 5; 
            g -> s -> enemy_phys.pos.y += sin(direction) * 5; 
        }

        if (g -> s -> current_goal == 0) {
            if (uphys_obj_coll_detect((g -> s -> player_phys), (g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].end_tile))) {
                g -> s -> lvl_complete = 1; 
            } 
            if (uphys_obj_coll_detect((g -> s -> player_phys), (g -> s -> enemy_phys))) {
                g -> s -> lvl_complete = -1;
            }
        } else {
            if (uphys_obj_coll_detect((g -> s -> player_phys), (g -> s -> enemy_phys))) {
                g -> s -> lvl_complete = 1;
            } 
            if (uphys_obj_coll_detect((g -> s -> enemy_phys), (g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].end_tile))) {
                g -> s -> lvl_complete = -1; 
            } 
        }



    }

    if (g -> s -> lvl_complete == 1) {
        printf("%u\n", g -> s -> lvls.curr_lvl);
        printf("%f %f\n", g -> s -> player_phys.pos.x, g -> s -> player_phys.pos.y);
        g -> s -> lvls.curr_lvl++;
        u32 lvl = g -> s -> lvls.curr_lvl;
        Mix_PlayChannel(-1, lvl_completed_sound, 0);
        if (g -> s -> lvls.curr_lvl == g -> s -> lvls.number_of_lvls) {
            g -> s -> finished_game = 1;
            g -> s -> lvls.curr_lvl--;
        } else {
            g -> s -> player_phys.pos = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].player_pos;
            g -> s -> enemy_phys.pos = g -> s -> lvls.lvls[g -> s -> lvls.curr_lvl].enmy_pos;
            tmp = 0;
            g -> s -> player_phys.moving = 0;
        }
        g -> s -> lvl_complete = 0;
        if ((g -> s -> lvls.curr_lvl + 1) % 2 == 0) {
            g -> s -> current_goal = 1;
        } else {
            g -> s -> current_goal = 0;
        }
        g -> s -> player_phys.pos = g -> s -> lvls.lvls[lvl].player_pos;
        g -> s -> enemy_phys.pos = g -> s -> lvls.lvls[lvl].enmy_pos;
        printf("%f %f\n", g -> s -> player_phys.pos.x, g -> s -> player_phys.pos.y);
    }

    // Rendering

    // GAME
    //      RENDERING 
    // 3 following render parts        
        // levels
        // level items
        // scores
        // players 

    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.1, 0.4, 1.0);
        sgfx_start_render(&g -> s -> renderer);
            game_lvls_render(&g -> s -> lvls, &g -> s -> renderer, &g -> s -> camera);
        sgfx_end_render(&g -> s -> renderer);

        sgfx_start_render(&g -> s -> renderer);
            sgfx_convert_phys_obj_to_ogl(&g -> s -> player_phys, &g -> s -> player_ogl, PLAYER1_TEXTURE_SLOT);
            // g -> s -> player_ogl.rotation.z = 1;
            // g -> s -> player_ogl.rotation.w = -30;           
            g -> s -> player_ogl.translation.x -= g -> s -> camera.x;
            g -> s -> player_ogl.translation.y -= g -> s -> camera.y;
            sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &g -> s -> player_ogl);        

            sgfx_convert_phys_obj_to_ogl(&g -> s -> enemy_phys, &g -> s -> enemy_ogl, PLAYER2_TEXTURE_SLOT);
            // g -> s -> player_ogl.rotation.z = 1;
            // g -> s -> player_ogl.rotation.w = -30;           
            g -> s -> enemy_ogl.translation.x -= g -> s -> camera.x;
            g -> s -> enemy_ogl.translation.y -= g -> s -> camera.y;
            sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &g -> s -> enemy_ogl);        




            // sgfx_convert_phys_obj_to_ogl(&g -> s -> player_phys, &g -> s -> player_ogl, PLAYER1_TEXTURE_SLOT);
            // g -> s -> player_ogl.rotation.z = 1;
            // g -> s -> player_ogl.rotation.w = -30;           

            sgfx_mk_textured_quad(&p, 200, 50, 300, 50, TEXT2_TEXTURE_SLOT);
            sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &p);
            for (u32 u = 0; u < 3; u++) {
                sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &numbers[u]);        
            }
            if (g -> s -> current_goal == 0) {
                sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &avoid_goal);        
            } else {
                sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &kill_goal);        
            }

            if (g -> s -> lvl_complete == -1) {
                if (failed_splayd == 0) {
                    Mix_PlayChannel(-1, failed_sound, 0);
                    failed_splayd = 1;
                }               
                
                sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &failed_goal);
            }

            if (g -> s -> finished_game) {

                sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &game_finished);
            } 

            // sgfx_convert_phys_obj_to_ogl(&g -> s -> tile_phys, &g -> s -> tile_ogl, TILES_TEXTURE_SLOT);
            // sgfx_add_prim_to_ogl_renderer(&g -> s -> renderer, &g -> s -> tile_ogl);        

        sgfx_end_render(&g -> s -> renderer);
    }

    SDL_GL_SwapWindow(g -> p -> win);

    if (g -> p -> fps.ms_dt < 16) {
        SDL_Delay(16 - g -> p -> fps.ms_dt);
    }
}

void game_close(game_t * g) {
    free(g -> s);
    sgfx_rm(g -> p);   
}
