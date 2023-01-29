#include <components.h>

u08 log_debug_stats_at_interval(gsdl_props_t * p, gsdl_timer_t * timer, u08 stats_logged) {
    gsdl_update_timer(timer, p -> ms_dt);
    if (timer -> actual_time > 0 && timer -> actual_time % 5 == 0 && !stats_logged) {
        // 1 frame is taking 0.7 ms
        // fps means frames per sec, 1000/0.7 s frame rate
        logger_log(LOG_DBG, "Stats:\n \tMem Used (mb): %d\n \tFrame Time (ms): %f\n \tFPS: %f\n", gsdl_get_mem_used(p), p -> ms_dt, 1/(p -> ms_dt/1000));
        stats_logged = 1;
    }

    if (timer -> actual_time % 5 != 0) {
        stats_logged = 0;
    }
    return stats_logged;
}

void animate_main_menu_btn(gsdl_phys_obj_t * btn, f32 * position_delta, f32 initial_y_val, f64 dt) {
    if (dt < 1) {
        if (btn -> pos.y <= initial_y_val) {
            *position_delta = 1;
        }
        if (btn -> pos.y >= initial_y_val + 50) {
            *position_delta = -1;
        }

        btn -> pos.y += *position_delta * dt;



    }
}
