
//~ Modal Key Commands

#define DEFINE_FULL_BIMODAL_KEY(binding_name, edit_code, normal_code) \
CUSTOM_COMMAND_SIG(binding_name) \
CUSTOM_DOC("Bimodal Key Function.") \
{ \
if (global_edit_mode) { \
edit_code(app); \
} \
else { \
normal_code(app); \
} \
}

#define DEFINE_BIMODAL_KEY(binding_name, edit_code, normal_code) DEFINE_FULL_BIMODAL_KEY(binding_name, edit_code, normal_code)
#define DEFINE_MODAL_KEY(binding_name, edit_code) DEFINE_BIMODAL_KEY(binding_name, edit_code, leave_current_input_unhandled)

DEFINE_MODAL_KEY(yuval_modal_a, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_b, interactive_switch_buffer);
DEFINE_MODAL_KEY(yuval_modal_c, open_matching_file_cpp); // TODO(yuval): Override this function to handle other file types (obj-c, and so on...)
DEFINE_MODAL_KEY(yuval_modal_d, yuval_kill_to_end_of_line);
DEFINE_MODAL_KEY(yuval_modal_e, view_buffer_other_panel);
DEFINE_MODAL_KEY(yuval_modal_f, paste_and_indent);
DEFINE_MODAL_KEY(yuval_modal_g, goto_line);
DEFINE_MODAL_KEY(yuval_modal_h, yuval_jump_lister);
DEFINE_MODAL_KEY(yuval_modal_i, replace_in_range);
DEFINE_MODAL_KEY(yuval_modal_j, jump_to_definition);
DEFINE_MODAL_KEY(yuval_modal_k, delete_line);
DEFINE_MODAL_KEY(yuval_modal_l, list_all_substring_locations_case_insensitive);
DEFINE_MODAL_KEY(yuval_modal_m, yuval_save_and_make_without_asking);
DEFINE_MODAL_KEY(yuval_modal_n, goto_next_jump);
DEFINE_MODAL_KEY(yuval_modal_o, query_replace);
DEFINE_MODAL_KEY(yuval_modal_p, goto_prev_jump);
DEFINE_MODAL_KEY(yuval_modal_q, copy);
DEFINE_MODAL_KEY(yuval_modal_r, reverse_search);
DEFINE_MODAL_KEY(yuval_modal_s, search);
DEFINE_MODAL_KEY(yuval_modal_t, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_u, undo);
DEFINE_MODAL_KEY(yuval_modal_v, yuval_switch_buffer_other_panel);
DEFINE_MODAL_KEY(yuval_modal_w, cut);
DEFINE_MODAL_KEY(yuval_modal_x, yuval_open_matching_file_other_panel);
DEFINE_MODAL_KEY(yuval_modal_y, redo);
DEFINE_MODAL_KEY(yuval_modal_z, interactive_open_or_new);

DEFINE_MODAL_KEY(yuval_modal_1, fleury_code_peek); // TODO(yuval): Shouldn't need to bind a key for this? (casey_build_search)
DEFINE_MODAL_KEY(yuval_modal_2, fleury_close_code_peek); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_3, fleury_code_peek_go); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_4, fleury_code_peek_go_same_panel); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_5, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_6, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_7, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_8, yuval_jump_lister_other_panel); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_9, yuval_jump_to_definition_other_panel); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_0, kill_buffer);
DEFINE_MODAL_KEY(yuval_modal_minus, command_lister);
DEFINE_MODAL_KEY(yuval_modal_equals, yuval_command_lister);

DEFINE_MODAL_KEY(yuval_modal_space, set_mark);
DEFINE_MODAL_KEY(yuval_modal_back_slash, save); // TODO(yuval): Fix yuval_clean_and_save
DEFINE_MODAL_KEY(yuval_modal_single_quote, keyboard_macro_replay);
DEFINE_MODAL_KEY(yuval_modal_comma, goto_beginning_of_file);
DEFINE_MODAL_KEY(yuval_modal_period, goto_end_of_file);
DEFINE_MODAL_KEY(yuval_modal_forward_slash, change_active_panel);
DEFINE_MODAL_KEY(yuval_modal_semicolon, list_all_locations);
DEFINE_MODAL_KEY(yuval_modal_open_bracket, keyboard_macro_start_recording);
DEFINE_MODAL_KEY(yuval_modal_close_bracket, keyboard_macro_finish_recording);

DEFINE_BIMODAL_KEY(yuval_modal_backspace, snipe_backward_whitespace_or_token_boundary, backspace_char);
DEFINE_BIMODAL_KEY(yuval_modal_up, move_up, move_up);
DEFINE_BIMODAL_KEY(yuval_modal_down, move_down, move_down);
DEFINE_BIMODAL_KEY(yuval_modal_left, move_left_token_boundary, move_left);
DEFINE_BIMODAL_KEY(yuval_modal_right, move_right_token_boundary, move_right);
DEFINE_BIMODAL_KEY(yuval_modal_delete, snipe_forward_whitespace_or_token_boundary, delete_char);
DEFINE_BIMODAL_KEY(yuval_modal_home, fleury_home, yuval_home_and_tab);
DEFINE_BIMODAL_KEY(yuval_modal_end, seek_end_of_line, seek_end_of_line);
DEFINE_BIMODAL_KEY(yuval_modal_tab, word_complete, word_complete);

//~ NOTE(yuval): Bindings

function void
yuval_set_bindings(Mapping *mapping) {
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(mapid_global);
    {
        BindCore(yuval_startup, CoreCode_Startup);
        BindCore(default_try_exit, CoreCode_TryExit);
        
        BindMouseWheel(mouse_wheel_scroll);
        BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
        
        Bind(yuval_toggle_build_panel_display, KeyCode_F1);
        Bind(toggle_filebar, KeyCode_F2);
        
        Bind(exit_4coder, KeyCode_0, KeyCode_Alt);
        Bind(exit_4coder, KeyCode_F4, KeyCode_Alt);
        
    }
    
    SelectMap(mapid_file);
    ParentMap(mapid_global);
    {
        BindTextInput(write_text_input);
        BindMouse(click_set_cursor_and_mark, MouseCode_Left);
        BindMouseRelease(click_set_cursor, MouseCode_Left);
        BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
        BindMouseMove(click_set_cursor_if_lbutton);
    
        // NOTE(yuval) Non-Modal Key Bindings
        Bind(yuval_toggle_edit_mode, KeyCode_Escape);

        Bind(move_up_to_blank_line, KeyCode_PageUp);
        Bind(page_up, KeyCode_PageUp, KeyCode_Shift);

        Bind(move_down_to_blank_line, KeyCode_PageDown);
        Bind(page_down, KeyCode_PageUp, KeyCode_Shift);

        // NOTE(yuval): Modal Key Bindings
        Bind(yuval_modal_a, KeyCode_A);
        Bind(yuval_modal_b, KeyCode_B);
        Bind(yuval_modal_c, KeyCode_C);
        Bind(yuval_modal_d, KeyCode_D);
        Bind(yuval_modal_e, KeyCode_E);
        Bind(yuval_modal_f, KeyCode_F);
        Bind(yuval_modal_g, KeyCode_G);
        Bind(yuval_modal_h, KeyCode_H);
        Bind(yuval_modal_i, KeyCode_I);
        Bind(yuval_modal_j, KeyCode_J);
        Bind(yuval_modal_k, KeyCode_K);
        Bind(yuval_modal_l, KeyCode_L);
        Bind(yuval_modal_m, KeyCode_M);
        Bind(yuval_modal_n, KeyCode_N);
        Bind(yuval_modal_o, KeyCode_O);
        Bind(yuval_modal_p, KeyCode_P);
        Bind(yuval_modal_q, KeyCode_Q);
        Bind(yuval_modal_r, KeyCode_R);
        Bind(yuval_modal_s, KeyCode_S);
        Bind(yuval_modal_t, KeyCode_T);
        Bind(yuval_modal_u, KeyCode_U);
        Bind(yuval_modal_v, KeyCode_V);
        Bind(yuval_modal_w, KeyCode_W);
        Bind(yuval_modal_x, KeyCode_X);
        Bind(yuval_modal_y, KeyCode_Y);
        Bind(yuval_modal_z, KeyCode_Z);
        
        Bind(yuval_modal_1, KeyCode_1);
        Bind(yuval_modal_2, KeyCode_2);
        Bind(yuval_modal_3, KeyCode_3);
        Bind(yuval_modal_4, KeyCode_4);
        Bind(yuval_modal_5, KeyCode_5);
        Bind(yuval_modal_6, KeyCode_6);
        Bind(yuval_modal_7, KeyCode_7);
        Bind(yuval_modal_8, KeyCode_8);
        Bind(yuval_modal_9, KeyCode_9);
        Bind(yuval_modal_0, KeyCode_0);
        Bind(yuval_modal_minus, KeyCode_Minus);
        Bind(yuval_modal_equals, KeyCode_Equal);
        
        Bind(yuval_modal_space, KeyCode_Space);
        Bind(yuval_modal_space, KeyCode_Space, KeyCode_Shift);

        Bind(yuval_modal_back_slash, KeyCode_BackwardSlash);
        Bind(yuval_modal_single_quote, KeyCode_Quote);
        Bind(yuval_modal_comma, KeyCode_Comma);
        Bind(yuval_modal_period, KeyCode_Period);
        Bind(yuval_modal_forward_slash, KeyCode_ForwardSlash);
        Bind(yuval_modal_semicolon, KeyCode_Semicolon);
        Bind(yuval_modal_open_bracket, KeyCode_LeftBracket);
        Bind(yuval_modal_close_bracket, KeyCode_RightBracket);
        
        Bind(yuval_modal_backspace, KeyCode_Backspace);
        Bind(yuval_modal_backspace, KeyCode_Backspace, KeyCode_Shift);
        
        Bind(yuval_modal_up, KeyCode_Up);
        Bind(yuval_modal_up, KeyCode_Up, KeyCode_Shift);
        
        Bind(yuval_modal_down, KeyCode_Down);
        Bind(yuval_modal_down, KeyCode_Down, KeyCode_Shift);
        
        Bind(yuval_modal_left, KeyCode_Left);
        Bind(yuval_modal_left, KeyCode_Left, KeyCode_Shift);
        
        Bind(yuval_modal_right, KeyCode_Right);
        Bind(yuval_modal_right, KeyCode_Right, KeyCode_Shift);
        
        Bind(yuval_modal_delete, KeyCode_Delete);
        Bind(yuval_modal_delete, KeyCode_Delete, KeyCode_Shift);
        
        Bind(yuval_modal_home, KeyCode_Home);
        Bind(yuval_modal_home, KeyCode_Home, KeyCode_Shift);
        
        Bind(yuval_modal_end, KeyCode_End);
        Bind(yuval_modal_end, KeyCode_End, KeyCode_Shift);
        
        Bind(yuval_modal_tab, KeyCode_Tab);
        Bind(yuval_modal_tab, KeyCode_Tab, KeyCode_Shift);
    }
    
    SelectMap(mapid_code);
    ParentMap(mapid_file);
    {
        BindTextInput(write_text_and_auto_indent);
}
