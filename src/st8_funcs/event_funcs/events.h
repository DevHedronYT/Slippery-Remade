#ifndef EVENTS_H
    #define EVENTS_H

    #include <engine.h>
    #include <st8.h> 
    
    void game_events_handle_keys_in_menu(game_state_t * s, gsdl_props_t * p); 
    void game_events_handle_mbtn_in_menu(game_state_t * s, gsdl_props_t * p);

   
    void game_events_handle_keys_in_play(game_state_t * s, gsdl_props_t * p); 


#endif
