#ifndef COMPONENTS_H
    #define COMPONENTS_H
    #include <engine.h>
    u08 log_debug_stats_at_interval(gsdl_props_t * p, gsdl_timer_t * timer, u08 stats_logged);
    void animate_play_btn(gsdl_phys_obj_t * btn, f32 * position_delta, f32 initial_y_val, f64 dt);
    void update_ui_label_centered_with_phys_obj(gsdl_ui_label_t * label, gsdl_phys_obj_t * obj);

    void animate_player(gsdl_phys_obj_t * player, f32 * increment, v2_t min_player_dimension, v2_t max_player_dimension, f64 dt);

#endif
