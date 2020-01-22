
//~ NOTE(yuval): Build view constants & globals

global_const f32 YUVAL_BUILD_VIEW__COLLAPSED_P = 0.05f;
global_const f32 YUVAL_BUILD_VIEW__EXPANDED_P = 0.85f;

global f32 yuval_build_view__open_t_target = YUVAL_BUILD_VIEW__COLLAPSED_P;

global View_ID global_build_view;

//~ NOTE(yuval): Build view functions

function void
yuval_build_view_open(Application_Links* app, View_ID view){
    global_build_view = open_footer_panel(app, view);
    
    view_set_split_proportion(app, global_build_view, YUVAL_BUILD_VIEW__COLLAPSED_P);
    hide_scrollbar(app);
    hide_filebar(app);
    
    Buffer_ID compilation_buffer = buffer_identifier_to_id_create_out_buffer(app, standard_build_build_buffer_identifier);
    view_set_buffer(app, global_build_view, compilation_buffer, 0);
}

function void
yuval_build_view_draw(Application_Links* app, Frame_Info frame_info){
    local_const ARGB_Color BACKGROUND_COLOR = 0xFF121212;
    local_const f32 DOPEN_DT= 2.7f;
    
    local_persist f32 open_t = YUVAL_BUILD_VIEW__COLLAPSED_P;
    
    f32 dopen = frame_info.animation_dt * DOPEN_DT;
    if (open_t < yuval_build_view__open_t_target) {
        open_t += dopen;
        if (open_t > yuval_build_view__open_t_target) {
            open_t = yuval_build_view__open_t_target;
        }
    } else if (open_t > yuval_build_view__open_t_target) {
        open_t -= dopen;
        if (open_t < yuval_build_view__open_t_target) {
            open_t = yuval_build_view__open_t_target;
        }
    }
    
    if (open_t != yuval_build_view__open_t_target) {
        view_set_split_proportion(app, global_build_view, open_t);
        animate_in_n_milliseconds(app, 0);
    }
    
    Rect_f32 view_rect = view_get_screen_rect(app, global_build_view);
    Rect_f32 region = rect_inner(view_rect, 1.f);
    
    draw_rectangle(app, region, 0.f, BACKGROUND_COLOR);
    draw_margin(app, view_rect, region, BACKGROUND_COLOR);
}

CUSTOM_COMMAND_SIG(yuval_toggle_build_panel_display) {
    if (yuval_build_view__open_t_target == YUVAL_BUILD_VIEW__COLLAPSED_P) {
        yuval_build_view__open_t_target = YUVAL_BUILD_VIEW__EXPANDED_P;
    } else {
        yuval_build_view__open_t_target = YUVAL_BUILD_VIEW__COLLAPSED_P;
    }
}
