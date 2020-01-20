
//~ Yuval's 4coder Dark Theme

static void
yuval_update_dark_theme(Application_Links *app) {
    // NOTE(yuval): Color Constants
    ARGB_Color background_color = 0xFF201F21;
    
    ARGB_Color text_default_color = 0xFFA89A8A;
    ARGB_Color comment_color = 0xFF7D7D7D;
    ARGB_Color keyword_color = 0xFFB5913B;
    ARGB_Color constant_color = 0xFF718C40;
    
    ARGB_Color cursor_color = (global_edit_mode ? 0xFFFF4242 : 0xFF78D875);
    ARGB_Color at_cursor_color = (global_edit_mode ? background_color : text_default_color);
    ARGB_Color list_item_active_color = 0xFF934420;
    
    // NOTE(yuval): Make color table
    Color_Table *table = &active_color_table;
    Arena *arena = &global_theme_arena;
    linalloc_clear(arena);
    *table = make_color_table(app, arena);
    
    table->arrays[defcolor_bar]                   = make_colors(arena, background_color);
    table->arrays[defcolor_base]                  = make_colors(arena, text_default_color);
    table->arrays[defcolor_pop1]                  = make_colors(arena, 0xFF03CF0C);
    table->arrays[defcolor_pop2]                  = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_back]                  = make_colors(arena, background_color);
    table->arrays[defcolor_margin]                = make_colors(arena, background_color);
    table->arrays[defcolor_margin_hover]          = make_colors(arena, background_color);
    table->arrays[defcolor_margin_active]         = make_colors(arena, background_color);
    table->arrays[defcolor_list_item]             = make_colors(arena, background_color);
    table->arrays[defcolor_list_item_hover]       = make_colors(arena, list_item_active_color);
    table->arrays[defcolor_list_item_active]      = make_colors(arena, list_item_active_color);
    table->arrays[defcolor_cursor]                = make_colors(arena, cursor_color);
    table->arrays[defcolor_at_cursor]             = make_colors(arena, at_cursor_color);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, background_color);
    table->arrays[defcolor_highlight]             = make_colors(arena, 0xFF703419);
    table->arrays[defcolor_at_highlight]          = make_colors(arena, 0xFFCDAA7D);
    table->arrays[defcolor_mark]                  = make_colors(arena, 0xFFFF4242);
    table->arrays[defcolor_text_default]          = make_colors(arena, text_default_color);
    table->arrays[defcolor_comment]               = make_colors(arena, comment_color);
    table->arrays[defcolor_comment_pop]           = make_colors(arena, comment_color, comment_color);
    table->arrays[defcolor_keyword]               = make_colors(arena, keyword_color);
    table->arrays[defcolor_str_constant]          = make_colors(arena, constant_color);
    table->arrays[defcolor_char_constant]         = make_colors(arena, constant_color);
    table->arrays[defcolor_int_constant]          = make_colors(arena, constant_color);
    table->arrays[defcolor_float_constant]        = make_colors(arena, constant_color);
    table->arrays[defcolor_bool_constant]         = make_colors(arena, constant_color);
    table->arrays[defcolor_preproc]               = make_colors(arena, 0xFFC1B3A2);
    table->arrays[defcolor_include]               = make_colors(arena, constant_color);
    table->arrays[defcolor_special_character]     = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_ghost_character]       = make_colors(arena, 0xFF5B4D3C);
    table->arrays[defcolor_highlight_junk]        = make_colors(arena, 0xFF3A0000);
    table->arrays[defcolor_highlight_white]       = make_colors(arena, 0xFF003A3A);
    table->arrays[defcolor_paste]                 = make_colors(arena, 0xFFDDEE00);
    table->arrays[defcolor_undo]                  = make_colors(arena, 0xFF00DDEE);
    table->arrays[defcolor_back_cycle]            = make_colors(arena, 0x0CA00000, 0x0800A000, 0x080000A0, 0x08A0A000);
    table->arrays[defcolor_text_cycle]            = make_colors(arena, 0xFFEA3C3C, 0xFF3EC48C, 0xFF2E84BA, 0xFFFFA000);
    table->arrays[defcolor_line_numbers_back]     = make_colors(arena, background_color);
    table->arrays[defcolor_line_numbers_text]     = make_colors(arena, comment_color);
}

#if 0
CUSTOM_COMMAND_SIG(fleury_toggle_colors)
CUSTOM_DOC("Toggles light/dark mode.")
{
    if(global_dark_mode)
    {
        Fleury4LightMode(app);
        global_dark_mode = 0;
    }
    else
    {
        Fleury4DarkMode(app);
        global_dark_mode = 1;
    }
}
#endif
