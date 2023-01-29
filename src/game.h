#ifndef GAME_H
    #define GAME_H

    #include <st8.h>

    #define EVENT_FUNCTIONS_NUM 3

    void game_init(game_t * game);
    // these functions will have sub-functions depending on game stage
    void game_run(game_t * game);
//    void game_handle_events(game_t * game);
//    void game_update(game_t * game);
//    void game_render(game_t * game);
    void game_close(game_t * game); 

#endif