#include <game.h>

i32 main(i32 argv, char ** args) {
    game_t game; 
    game_init(&game);
    game_run(&game);
    game_close(&game);
    return 0;
}