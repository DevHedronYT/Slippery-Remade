#ifndef STATE_H 
    #define STATE_H
    // Often represented as s
    #include <engine.h> 
    typedef struct {
        config_file_t cfg_file; 
        gsdl_timer_t log_timer;
        u08 stats_logged;

        // menu
        f32 initial_slip_away_btn_y;
        f32 position_delta;

        gsdl_ui_label_t menu_game_title;

        gsdl_img_t menu_btn_bg; 

        gsdl_ui_label_t menu_slip_away_btn_text;
        gsdl_phys_obj_t menu_slip_away_btn;

        gsdl_ui_label_t menu_options_btn_text;
        gsdl_phys_obj_t menu_options_btn;

        gsdl_ui_label_t menu_exit_btn_text;
        gsdl_phys_obj_t menu_exit_btn;

        // actual game
        gsdl_cam_t camera;

        char * player_state; 
        gsdl_phys_obj_t player_obj;
        gsdl_img_t player_img;
        f32 player_stretch;
        v2_t max_player_dimensions;
        v2_t min_player_dimensions;

        ht_t player_events;

    } game_state_t; 

    typedef struct {
        gsdl_props_t * props;
        game_state_t * state;
    } game_t;

    #define GAME_STATE_MENU 0
    #define GAME_STATE_OPTIONS 1
    #define GAME_STATE_PLAY 2
#endif