#ifndef ST8_FUNCS_H 
    #define ST8_FUNCS_H 
    #include <st8.h>
    void game_events_handle_quit(game_state_t * s, gsdl_props_t * p);
    void game_events_handle_keys(game_state_t * s, gsdl_props_t * p);
    void game_events_handle_mbtn(game_state_t * s, gsdl_props_t * p);

    void game_state_menu(game_t * game);
    void game_state_play(game_t * game);

#endif
