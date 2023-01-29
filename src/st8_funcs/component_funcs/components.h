#ifndef COMPONENTS_H
    #define COMPONENTS_H
    #include <engine.h>
    u08 log_debug_stats_at_interval(gsdl_props_t * p, gsdl_timer_t * timer, u08 stats_logged);
    void animate_main_menu_btn(gsdl_phys_obj_t * btn, f32 * position_delta, f32 initial_y_val, f64 dt);

#endif
