static Face_ID global_styled_title_face = 0;
static Face_ID global_styled_label_face = 0;
static Face_ID global_small_code_face = 0;
static Vec2_f32 global_cursor_position = {0};
static Vec2_f32 global_last_cursor_position = {0};

#if USE_YUVAL_CODE_INDEX
static Code_Index_Note *
yuval_lookup_string_in_code_index(Application_Links *app, String_Const_u8 string) {
    Code_Index_Note *note = 0;
    
    if (string.str) {
        for (Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always);
             buffer_it;
             buffer_it = get_buffer_next(app, buffer_it, Access_Always)) {
            Code_Index_File* file = code_index_get_file(buffer_it);
            if (file) {
                Data data = make_data(string.str, string.size);
                Table_Lookup lookup = table_lookup(&file->string_to_index_note, data);
                if (lookup.found_match){
                    u64 val = 0;
                    table_read(&file->string_to_index_note, lookup, &val);
                    note = (Code_Index_Note*)IntAsPtr(val);
                    break;
                }
            }
        }
    }
    return note;
}
#else
static Code_Index_Note *
yuval_lookup_string_in_code_index(Application_Links *app, String_Const_u8 string) {
    Code_Index_Note *note = 0;
    
	if (string.str) {
		for (Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always);
             buffer_it != 0; buffer_it = get_buffer_next(app, buffer_it, Access_Always))
		{
			Code_Index_File* file = code_index_get_file(buffer_it);
			if (file != 0)
			{
				for (i32 i = 0; i < file->note_array.count; i += 1)
				{
					Code_Index_Note* this_note = file->note_array.ptrs[i];
                    
					if (string_match(this_note->text, string))
					{
						note = this_note;
						break;
					}
				}
			}
		}
	}
    return note;
}
#endif

static Code_Index_Note *
yuval_lookup_token_in_code_index(Application_Links *app, Buffer_ID buffer, Token token) {
    Code_Index_Note *note = 0;
    Scratch_Block scratch(app);
    String_Const_u8 string = push_buffer_range(app, scratch, buffer, Ii64(token.pos, token.pos + token.size));
    note = yuval_lookup_string_in_code_index(app, string);
    return note;
}

static ARGB_Color
yuval_get_cpp_token_color(Token token) {
    ARGB_Color color = ARGBFromID(defcolor_text_default);
    
    switch(token.kind)
    {
        case TokenBaseKind_Preprocessor:     color = ARGBFromID(defcolor_preproc); break;
        case TokenBaseKind_Keyword:          color = ARGBFromID(defcolor_keyword); break;
        case TokenBaseKind_Comment:          color = ARGBFromID(defcolor_comment); break;
        case TokenBaseKind_LiteralString:    color = ARGBFromID(defcolor_str_constant); break;
        case TokenBaseKind_LiteralInteger:   color = ARGBFromID(defcolor_int_constant); break;
        case TokenBaseKind_LiteralFloat:     color = ARGBFromID(defcolor_float_constant); break;
        case TokenBaseKind_Operator:         color = ARGBFromID(defcolor_preproc); break;
        
        case TokenBaseKind_ScopeOpen:
        case TokenBaseKind_ScopeClose:
        case TokenBaseKind_ParentheticalOpen:
        case TokenBaseKind_ParentheticalClose:
        case TokenBaseKind_StatementClose:
        {
            u32 r = (color & 0x00ff0000) >> 16;
            u32 g = (color & 0x0000ff00) >>  8;
            u32 b = (color & 0x000000ff) >>  0;
            
            r = (r * 4) / 5;
            g = (g * 4) / 5;
            b = (b * 4) / 5;
            
            
            color = 0xff000000 | (r << 16) | (g << 8) | (b << 0);
            
            break;
        }
        
        default:
        {
            switch(token.sub_kind)
            {
                case TokenCppKind_LiteralTrue:
                case TokenCppKind_LiteralFalse:
                {
                    color = ARGBFromID(defcolor_bool_constant);
                    break;
                }
                case TokenCppKind_LiteralCharacter:
                case TokenCppKind_LiteralCharacterWide:
                case TokenCppKind_LiteralCharacterUTF8:
                case TokenCppKind_LiteralCharacterUTF16:
                case TokenCppKind_LiteralCharacterUTF32:
                {
                    color = ARGBFromID(defcolor_char_constant);
                    break;
                }
                case TokenCppKind_PPIncludeFile:
                {
                    color = ARGBFromID(defcolor_include);
                    break;
                }
            }
            break;
        }
    }
    
    return color;
}

static void
yuval_draw_cpp_token_colors(Application_Links *app, Text_Layout_ID text_layout_id, Token_Array *array) {
    local_const ARGB_Color TYPE_COLOR = 0xFFBA9341;
    local_const ARGB_Color FUNCTION_COLOR = 0xFFB74D45;
    local_const ARGB_Color MACRO_COLOR = 0xFF4F8C7C;
    
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 first_index = token_index_from_pos(array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, array, first_index);
    
    for (;;)
    {
        Token *token = token_it_read(&it);
        if (!token || token->pos >= visible_range.one_past_last)
        {
            break;
        }
        
        ARGB_Color argb = yuval_get_cpp_token_color(*token);
        
        if (token->kind == TokenBaseKind_Identifier) {           
            // NOTE(yuval): Is this a function, type, or macro?
            Buffer_ID buffer = text_layout_get_buffer(app, text_layout_id);
            if (buffer) {
                Code_Index_Note *note = 0;
                
                // NOTE(rjf): Look up token.
                {
                    ProfileScope(app, "[Yuval] Code Index Token Look-Up");
                    note = yuval_lookup_token_in_code_index(app, buffer, *token);
                }
                
                if (note) {
                    switch (note->note_kind) {
                        case CodeIndexNote_Type: { argb = TYPE_COLOR; } break;
                        case CodeIndexNote_Function: { argb = FUNCTION_COLOR; } break;
                        case CodeIndexNote_Macro: { argb = MACRO_COLOR; } break;
                    }
                }
            }
        }
        
        paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
        if(!token_it_inc_all(&it))
        {
            break;
        }
    }
}

static void
Fleury4DrawTooltipRect(Application_Links *app, Rect_f32 rect)
{
    ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_back));
    ARGB_Color border_color = 0xFFFFFFFF;
    
    background_color &= 0x00ffffff;
    background_color |= 0xd0000000;
    
    border_color &= 0x00ffffff;
    border_color |= 0x30000000;
    
    draw_rectangle(app, rect, 4.f, background_color);
    draw_rectangle_outline(app, rect, 4.f, 3.f, border_color);
}

static void
Fleury4RenderRangeHighlight(Application_Links *app, View_ID view_id, Text_Layout_ID text_layout_id,
                            Range_i64 range)
{
    Rect_f32 range_start_rect = text_layout_character_on_screen(app, text_layout_id, range.start);
    Rect_f32 range_end_rect = text_layout_character_on_screen(app, text_layout_id, range.end-1);
    Rect_f32 total_range_rect = {0};
    total_range_rect.x0 = MinimumF32(range_start_rect.x0, range_end_rect.x0);
    total_range_rect.y0 = MinimumF32(range_start_rect.y0, range_end_rect.y0);
    total_range_rect.x1 = MaximumF32(range_start_rect.x1, range_end_rect.x1);
    total_range_rect.y1 = MaximumF32(range_start_rect.y1, range_end_rect.y1);
    
    ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_pop2));
    float background_color_r = (float)((background_color & 0x00ff0000) >> 16) / 255.f;
    float background_color_g = (float)((background_color & 0x0000ff00) >>  8) / 255.f;
    float background_color_b = (float)((background_color & 0x000000ff) >>  0) / 255.f;
    background_color_r += (1.f - background_color_r) * 0.5f;
    background_color_g += (1.f - background_color_g) * 0.5f;
    background_color_b += (1.f - background_color_b) * 0.5f;
    ARGB_Color highlight_color = (0x55000000 |
                                  (((u32)(background_color_r * 255.f)) << 16) |
                                  (((u32)(background_color_g * 255.f)) <<  8) |
                                  (((u32)(background_color_b * 255.f)) <<  0));
    draw_rectangle(app, total_range_rect, 4.f, highlight_color);
}
