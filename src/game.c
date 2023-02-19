#include <game.h>
#include <st8_funcs.h>



// User-handled Functions
void game_init(game_t * game) {
    game -> props = calloc(1, sizeof(gsdl_props_t)); 
    game -> state = calloc(1, sizeof(game_state_t));

    gsdl_props_t * p = game -> props;    
    game_state_t * s = game -> state;    

    // In order of handling
    // mapping of structs from cfg file? 
    // Reloading whole game to hot reload 
    // cfg file
    gsdl_init_info_t initialization_info;
    s -> cfg_file = parse_config_file("res/settings.txt");
    initialization_info.title = config_file_get(&s -> cfg_file, "title"); 
    initialization_info.w = str_to_i32(config_file_get(&s -> cfg_file, "size")[1]);
    initialization_info.h = str_to_i32(config_file_get(&s -> cfg_file, "size")[2]);
    initialization_info.renderer_accel = str_to_i32(config_file_get(&s -> cfg_file, "renderer_accel")[1]);
    initialization_info.fps = str_to_i32(config_file_get(&s -> cfg_file, "fps")[1]);
    initialization_info.vsync = str_to_i32(config_file_get(&s -> cfg_file, "vsync")[1]);
    initialization_info.resizable = str_to_i32(config_file_get(&s -> cfg_file, "resizable")[1]);


    initialization_info.event_functions_num = EVENT_FUNCTIONS_NUM;
    initialization_info.event_functions = calloc(EVENT_FUNCTIONS_NUM, sizeof(event_function_type));
        initialization_info.event_functions[0] = game_events_handle_quit;
        initialization_info.event_functions[1] = game_events_handle_keys;
        initialization_info.event_functions[2] = game_events_handle_mbtn;
    initialization_info.event_type_strings = calloc(EVENT_FUNCTIONS_NUM, sizeof(char) * 128);
        initialization_info.event_type_strings[0] = "SDL_QUIT";
        initialization_info.event_type_strings[1] = "SDL_KEYDOWN";
        initialization_info.event_type_strings[2] = "SDL_MOUSEBUTTONDOWN";
    initialization_info.event_type_codes = calloc(EVENT_FUNCTIONS_NUM, sizeof(u32));
        initialization_info.event_type_codes[0] = SDL_QUIT;
        initialization_info.event_type_codes[1] = SDL_KEYDOWN;
        initialization_info.event_type_codes[2] = SDL_MOUSEBUTTONDOWN;

    gsdl_init(initialization_info, p);

    SDL_SetWindowBordered(p -> win, SDL_FALSE);

    // Initializing game-related variables

    s -> initial_slip_away_btn_y = p -> i_h / 2 - 50 / 2;
    s -> log_timer = gsdl_create_timer();
    s -> stats_logged = 0;
    { // Menu 
        gsdl_create_ui_label(&s -> menu_game_title, p -> i_w / 2 + p -> i_w / 4, p -> i_h / 4, "Slippery", "res/m5x7.ttf", 255, 255, 255, 255, 140, p -> renderer, &p -> texture_storage, &p -> ptr_storage);
        gsdl_center_ui_label(&s -> menu_game_title);

        gsdl_create_ui_label(&s -> menu_slip_away_btn_text, p -> i_w / 4, p -> i_h / 2, "Slip Away!", "res/m5x7.ttf", 255, 255, 255, 255, 70, p -> renderer, &p -> texture_storage, &p -> ptr_storage);
        gsdl_center_ui_label(&s -> menu_slip_away_btn_text);

        gsdl_create_phys_obj(&s -> menu_slip_away_btn, mk_v2(p -> i_w / 4 - 300 / 2, p -> i_h / 2 - 50 / 2), mk_v2(0, 0), 300, 50);

        gsdl_create_ui_label(&s -> menu_options_btn_text, p -> i_w / 4, p -> i_h / 2 + 150, "Options", "res/m5x7.ttf", 255, 255, 255, 255, 70, p -> renderer, &p -> texture_storage, &p -> ptr_storage);
        gsdl_center_ui_label(&s -> menu_options_btn_text);

        gsdl_create_phys_obj(&s -> menu_options_btn, mk_v2(p -> i_w / 4 - 300 / 2, p -> i_h / 2 - 50 / 2 + 125), mk_v2(0, 0), 350, 50);


        // Label coords are pointless
        gsdl_create_ui_label(&s -> menu_exit_btn_text, p -> i_w / 4, p -> i_h / 2 + 150, "Exit", "res/m5x7.ttf", 255, 255, 255, 255, 70, p -> renderer, &p -> texture_storage, &p -> ptr_storage);
        gsdl_center_ui_label(&s -> menu_exit_btn_text);

        gsdl_create_phys_obj(&s -> menu_exit_btn, mk_v2(p -> i_w / 4 - 300 / 2, s -> menu_options_btn.pos.y + 75), mk_v2(0, 0), 400, 50);

        gsdl_load_tex(&s -> menu_btn_bg, "res/btn_bg.png", 0, 0, 30, 25, p -> renderer, &p -> texture_storage, &p -> ptr_storage);

        // setting the positions
        s -> menu_options_btn_text.pos.x = s -> menu_options_btn.pos.x + s -> menu_options_btn.w / 2;
        s -> menu_options_btn_text.pos.y = s -> menu_options_btn.pos.y + s -> menu_options_btn.h / 2;
        gsdl_center_ui_label(&s -> menu_options_btn_text);

        s -> menu_exit_btn_text.pos.x = s -> menu_exit_btn.pos.x + s -> menu_exit_btn.w / 2;
        s -> menu_exit_btn_text.pos.y = s -> menu_exit_btn.pos.y + s -> menu_exit_btn.h / 2;
        gsdl_center_ui_label(&s -> menu_exit_btn_text);

    }

    { // Game
        s -> player_state = calloc(24, sizeof(char));
        s -> player_state = "idle";
        gsdl_create_phys_obj(&s -> player_obj, mk_v2(100, 100), mk_v2(8, 0), 4*16, 40); 
        gsdl_load_tex(&s -> player_img, "res/player.png", 0, 0, 16, 10, p -> renderer, &p -> texture_storage, &p -> ptr_storage);
        s -> player_stretch = 1;
        s -> max_player_dimensions = mk_v2(s -> player_obj.w + 10, s -> player_obj.h + 10);
        s -> min_player_dimensions = mk_v2(s -> player_obj.w, s -> player_obj.h);

        s -> player_events = ht_create(16);


        // statically allocated so it doesnt stay in memory, fuck
        /* u32 move_left_keys[3] = { 2, SDL_SCANCODE_A, SDL_SCANCODE_LEFT };
        u32 move_right_keys[3] = { 2, SDL_SCANCODE_D, SDL_SCANCODE_RIGHT };
        u32 jump_keys[4] = { 3, SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_SPACE }; */

        u32 * move_left_keys = calloc(3, sizeof(u32));            
        move_left_keys[0] = 2;
        move_left_keys[1] = SDL_SCANCODE_A;
        move_left_keys[2] = SDL_SCANCODE_LEFT;

        u32 * move_right_keys = calloc(3, sizeof(u32));            
        move_right_keys[0] = 2;
        move_right_keys[1] = SDL_SCANCODE_D;
        move_right_keys[2] = SDL_SCANCODE_RIGHT;

        u32 * jump_keys = calloc(4, sizeof(u32));            
        jump_keys[0] = 3;
        jump_keys[1] = SDL_SCANCODE_W;
        jump_keys[2] = SDL_SCANCODE_UP;
        jump_keys[3] = SDL_SCANCODE_SPACE;

        // u32 [4] = { 3, SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_SPACE };
        ht_insert(&s -> player_events, "move_left", move_left_keys);
        ht_insert(&s -> player_events, "move_right", move_right_keys);
        ht_insert(&s -> player_events, "jump", jump_keys);
    }

    if (!p || !s) {
        logger_log(LOG_ERR, "Failed to initialize");
    } else {
        logger_log(LOG_SUCCESS, "Game state successfully initialized");
    }   

}

void game_run(game_t * game) {
    gsdl_props_t * p = game -> props;    
    game_state_t * s = game -> state;    

    while (p -> running) {
        switch (p -> game_stage) {
            case GAME_STATE_MENU:
                game_state_menu(game);
                break;
            
            case GAME_STATE_PLAY:
                game_state_play(game);
                break;

            default:
                p -> running = 0;
                break;
        }
    }
}

void game_close(game_t * game) {
    gsdl_destroy(game -> props);
    free(game -> props);
    free(game -> state);
    logger_log(LOG_SUCCESS, "Successfully exited with game stage: %u\n", game -> props -> game_stage);
}
