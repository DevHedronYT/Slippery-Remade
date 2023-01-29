#include <events.h>


void game_events_handle_keys_in_menu(game_state_t * s, gsdl_props_t * p) {
    if (p -> keys_pressed[SDL_SCANCODE_Q]) {
        p -> running = 0;
    }
} 

void game_events_handle_mbtn_in_menu(game_state_t * s, gsdl_props_t * p) {
    i32 x, y;
    SDL_GetMouseState(&x, &y);

    gsdl_create_phys_obj(&p -> mouse, mk_v2(x, y), mk_v2(0, 0), 5, 5);

    if (gsdl_phys_obj_coll_detect(p -> mouse, s -> menu_slip_away_btn)) {
        p -> game_stage = 2;
    }
    if (gsdl_phys_obj_coll_detect(p -> mouse, s -> menu_options_btn)) {
        p -> game_stage = 1;
    }
    if (gsdl_phys_obj_coll_detect(p -> mouse, s -> menu_exit_btn)) {
        p -> running = 0;
    }
}


void game_events_handle_keys_in_play(game_state_t * s, gsdl_props_t * p) {
    if (p -> keys_pressed[SDL_SCANCODE_ESCAPE]) {
        p -> game_stage = 0;
    }
} 