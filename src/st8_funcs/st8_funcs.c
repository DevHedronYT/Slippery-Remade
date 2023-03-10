#include <st8_funcs.h>
#include <components.h>
#include <events.h>


void game_events_handle_quit(game_state_t * s, gsdl_props_t * p) {
    p -> running = 0;
}

void game_events_handle_keys(game_state_t * s, gsdl_props_t * p) {
    switch (p -> game_stage) {
        case GAME_STATE_MENU: 
            game_events_handle_keys_in_menu(s, p);
            break;
    
        case GAME_STATE_PLAY:
            game_events_handle_keys_in_play(s, p);
            break;

        default:
            break;
    }
}

// Add if-statements for game_stages here
void game_events_handle_mbtn(game_state_t * s, gsdl_props_t * p) {
    switch (p -> game_stage) {
        case GAME_STATE_MENU: 
            game_events_handle_mbtn_in_menu(s, p);
            break;

        default:
            break;
    }
}


void game_state_menu(game_t * game) {
    gsdl_props_t * p = game -> props;    
    game_state_t * s = game -> state;    

    gsdl_start_frame_time_rec(p);

    { // Event handling
        gsdl_start_event_handling(p);
            gsdl_handle_events(s, p);
        gsdl_end_event_handling(p);
    }

    {
        s -> stats_logged = log_debug_stats_at_interval(p, &s -> log_timer, s -> stats_logged);   

        animate_play_btn(&s -> menu_slip_away_btn, &s -> position_delta, s -> initial_slip_away_btn_y, p -> special_dt_ratio);        
        update_ui_label_centered_with_phys_obj(&s -> menu_slip_away_btn_text, &s -> menu_slip_away_btn);

    }

    { // Rendering
        gsdl_start_render(p, cBLACK, p -> render_texture.tex);

            gsdl_draw_ui_label(&s -> menu_game_title, p -> renderer);

            // gsdl_draw_phys_obj_rect(&s -> menu_slip_away_btn, 255, 0, 255, 255, NULL, p -> renderer);
            gsdl_draw_phys_obj(&s -> menu_btn_bg, &s -> menu_slip_away_btn, NULL, p -> renderer);
            gsdl_draw_ui_label(&s -> menu_slip_away_btn_text, p -> renderer);

            // gsdl_draw_phys_obj_rect(&s -> menu_options_btn, 255, 0, 255, 255, NULL, p -> renderer);
            gsdl_draw_phys_obj(&s -> menu_btn_bg, &s -> menu_options_btn, NULL, p -> renderer);
            gsdl_draw_ui_label(&s -> menu_options_btn_text, p -> renderer);

            // gsdl_draw_phys_obj_rect(&s -> menu_exit_btn, 255, 0, 255, 255, NULL, p -> renderer);
            gsdl_draw_phys_obj(&s -> menu_btn_bg, &s -> menu_exit_btn, NULL, p -> renderer);
            gsdl_draw_ui_label(&s -> menu_exit_btn_text, p -> renderer);



        gsdl_end_render(p);
    }

    gsdl_render_all(p, cBLACK, &p -> render_texture, &p -> render_texture_obj, NULL, 0, p -> i_w, p -> i_h);
    gsdl_stop_frame_time_rec(p);
}

void game_state_play(game_t * game) {
    gsdl_props_t * p = game -> props;    
    game_state_t * s = game -> state;    

    gsdl_start_frame_time_rec(p);

    { // Event handling

        gsdl_start_event_handling(p);
            gsdl_handle_events(s, p);
        gsdl_end_event_handling(p);
    }

    { // updating
        s -> stats_logged = log_debug_stats_at_interval(p, &s -> log_timer, s -> stats_logged);   
        if (strcmp("idle", s -> player_state) == 0) {
            animate_player(&s -> player_obj, &s -> player_stretch, s -> min_player_dimensions, s -> max_player_dimensions, p -> special_dt_ratio);
        }

        gsdl_update_phys_obj_vel(&s -> player_obj, 0);
        gsdl_update_phys_obj_pos(&s -> player_obj, NULL, 0, p -> special_dt_ratio);


    }


    { // Rendering
        gsdl_start_render(p, cWHITE, p -> render_texture.tex);
            gsdl_draw_phys_obj(&s -> player_img, &s -> player_obj, NULL, p -> renderer);
        gsdl_end_render(p);
    }

    gsdl_render_all(p, cBLACK, &p -> render_texture, &p -> render_texture_obj, NULL, 0, p -> i_w, p -> i_h);
    gsdl_stop_frame_time_rec(p);
}



