
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
DEFINE_MODAL_KEY(yuval_modal_h, yuval_jump_lister_other_panel);
DEFINE_MODAL_KEY(yuval_modal_i, replace_in_range);
DEFINE_MODAL_KEY(yuval_modal_j, yuval_jump_lister);
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

DEFINE_MODAL_KEY(yuval_modal_1, leave_current_input_unhandled); // TODO(yuval): Shouldn't need to bind a key for this? (casey_build_search)
DEFINE_MODAL_KEY(yuval_modal_2, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_3, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_4, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_5, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_6, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_7, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_8, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_9, leave_current_input_unhandled); // TODO(yuval): Available
DEFINE_MODAL_KEY(yuval_modal_0, kill_buffer);
DEFINE_MODAL_KEY(yuval_modal_minus, command_lister);
DEFINE_MODAL_KEY(yuval_modal_equals, yuval_command_lister);

DEFINE_MODAL_KEY(yuval_modal_space, set_mark);
DEFINE_MODAL_KEY(yuval_modal_back_slash, yuval_clean_and_save);
DEFINE_MODAL_KEY(yuval_modal_single_quote, keyboard_macro_replay);
DEFINE_MODAL_KEY(yuval_modal_comma, goto_beginning_of_file);
DEFINE_MODAL_KEY(yuval_modal_period, goto_end_of_file);
DEFINE_MODAL_KEY(yuval_modal_forward_slash, change_active_panel);
DEFINE_MODAL_KEY(yuval_modal_semicolon, list_all_locations);
DEFINE_MODAL_KEY(yuval_modal_open_bracket, keyboard_macro_start_recording);
DEFINE_MODAL_KEY(yuval_modal_close_bracket, keyboard_macro_finish_recording);

DEFINE_BIMODAL_KEY(yuval_modal_backspace, backspace_alpha_numeric_boundary, backspace_char);
DEFINE_BIMODAL_KEY(yuval_modal_up, move_up, move_up);
DEFINE_BIMODAL_KEY(yuval_modal_down, move_down, move_down);
DEFINE_BIMODAL_KEY(yuval_modal_left, move_left_whitespace_boundary, move_left);
DEFINE_BIMODAL_KEY(yuval_modal_right, move_right_whitespace_boundary, move_right);
DEFINE_BIMODAL_KEY(yuval_modal_delete, delete_alpha_numeric_boundary, delete_char);
DEFINE_BIMODAL_KEY(yuval_modal_home, fleury_home, yuval_home_and_tab);
DEFINE_BIMODAL_KEY(yuval_modal_end, seek_end_of_line, seek_end_of_line);
DEFINE_BIMODAL_KEY(yuval_modal_tab, word_complete, word_complete);

//~ NOTE(yuval): Bindings

function void
yuval_set_bindings(Mapping *mapping) {
    MappingScope();
    SelectMapping(mapping);

    SelectMap(mapid_global);
    BindCore(yuval_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    Bind(keyboard_macro_start_recording , KeyCode_U, KeyCode_Control);
    Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Control, KeyCode_Shift);
    Bind(keyboard_macro_replay,           KeyCode_U, KeyCode_Alt);
    Bind(change_active_panel,           KeyCode_Comma, KeyCode_Control);
    Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Control, KeyCode_Shift);
    Bind(interactive_new,               KeyCode_N, KeyCode_Control);
    Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Control);
    Bind(open_in_other,                 KeyCode_O, KeyCode_Alt);
    Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
    Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Control);
    Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
    Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
    Bind(close_build_panel,             KeyCode_Comma, KeyCode_Alt);
    Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
    Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
    Bind(build_in_build_panel,          KeyCode_M, KeyCode_Alt);
    Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
    Bind(toggle_filebar,                KeyCode_B, KeyCode_Alt);
    Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
    Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
    Bind(command_lister,                KeyCode_X, KeyCode_Alt);
    Bind(project_command_lister,        KeyCode_X, KeyCode_Alt, KeyCode_Shift);
    Bind(list_all_functions_current_buffer_lister, KeyCode_I, KeyCode_Control, KeyCode_Shift);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(project_fkey_command, KeyCode_F3);
    Bind(project_fkey_command, KeyCode_F4);
    Bind(project_fkey_command, KeyCode_F5);
    Bind(project_fkey_command, KeyCode_F6);
    Bind(project_fkey_command, KeyCode_F7);
    Bind(project_fkey_command, KeyCode_F8);
    Bind(project_fkey_command, KeyCode_F9);
    Bind(project_fkey_command, KeyCode_F10);
    Bind(project_fkey_command, KeyCode_F11);
    Bind(project_fkey_command, KeyCode_F12);
    Bind(project_fkey_command, KeyCode_F13);
    Bind(project_fkey_command, KeyCode_F14);
    Bind(project_fkey_command, KeyCode_F15);
    Bind(project_fkey_command, KeyCode_F16);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
    
    // NOTE(yuval): Custom bindings.
    {
        Bind(open_panel_vsplit, KeyCode_P, KeyCode_Control);
        Bind(open_panel_hsplit, KeyCode_Minus, KeyCode_Control);
        Bind(close_panel, KeyCode_P, KeyCode_Control, KeyCode_Shift);
        // Bind(fleury_toggle_colors, KeyCode_Tick, KeyCode_Control);
        Bind(fleury_place_cursor, KeyCode_Tick, KeyCode_Alt);
        Bind(fleury_toggle_power_mode, KeyCode_P, KeyCode_Alt);
        Bind(jump_to_definition, KeyCode_J, KeyCode_Control);
    }
    
    SelectMap(mapid_file);
    ParentMap(mapid_global);
    BindTextInput(fleury_write_text_input);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);
        
    // NOTE(yuval) Non-Modal Key Bindings
    {
        Bind(yuval_toggle_edit_mode, KeyCode_Escape);

        Bind(fleury_code_peek,          KeyCode_Alt, KeyCode_Control);
        //Bind(fleury_close_code_peek,    KeyCode_Escape);
        Bind(fleury_code_peek_go,       KeyCode_Return, KeyCode_Control);
        Bind(fleury_code_peek_go_same_panel, KeyCode_Return, KeyCode_Control, KeyCode_Shift);
        Bind(fleury_write_zero_struct,  KeyCode_0, KeyCode_Control);
    }

    // NOTE(yuval): Modal Key Bindings
    {
        Bind(yuval_modal_space, KeyCode_Space);
        Bind(yuval_modal_space, KeyCode_Space, KeyCode_Shift);
        
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
    BindTextInput(fleury_write_text_and_auto_indent);
    Bind(move_left_alpha_numeric_boundary,           KeyCode_Left, KeyCode_Control);
    Bind(move_right_alpha_numeric_boundary,          KeyCode_Right, KeyCode_Control);
    Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Alt);
    Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt);
    Bind(comment_line_toggle,        KeyCode_Semicolon, KeyCode_Control);
    Bind(word_complete,              KeyCode_Tab);
    Bind(auto_indent_range,          KeyCode_Tab, KeyCode_Control);
    Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
    Bind(word_complete_drop_down,    KeyCode_Tab, KeyCode_Shift, KeyCode_Control);
    Bind(write_block,                KeyCode_R, KeyCode_Alt);
    Bind(write_todo,                 KeyCode_T, KeyCode_Alt);
    Bind(write_note,                 KeyCode_Y, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
    Bind(open_long_braces,           KeyCode_LeftBracket, KeyCode_Control);
    Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(open_long_braces_break,     KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
    Bind(select_surrounding_scope,   KeyCode_LeftBracket, KeyCode_Alt);
    Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Alt);
    Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Alt);
    Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
    Bind(place_in_scope,             KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(delete_current_scope,       KeyCode_Minus, KeyCode_Alt);
    Bind(if0_off,                    KeyCode_I, KeyCode_Alt);
    Bind(open_file_in_quotes,        KeyCode_1, KeyCode_Alt);
    Bind(open_matching_file_cpp,     KeyCode_2, KeyCode_Alt);
}
