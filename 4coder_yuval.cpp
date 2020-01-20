#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "4coder_default_include.cpp"
#include "generated/managed_id_metadata.cpp"

#pragma warning(disable : 4706)

#include "4coder_fleury_utilities.cpp"
#include "4coder_yuval_ubiquitous.cpp" // TODO(yuval): Move this to my includes section
#include "4coder_fleury_power_mode.cpp"
#include "4coder_fleury_divider_comments.cpp"
#include "4coder_fleury_plot.cpp"
#include "4coder_fleury_calc.cpp"

#define YUVAL_PRINTABLE_STRING(string) ((i32)((string).size)), ((string).str)

global_const f32 BUILD_PANEL_COLLAPSED_P = 0.05f;
global_const f32 BUILD_PANEL_EXPANDED_P = 0.85f;
global_const ARGB_Color BUILD_VIEW_BACKGROUND_COLOR = 0xFF121212;

// TODO(yuval): Make this changeable somehome (maybe through command line arguments?) 
global_const String_Const_u8 global_projects_master_file_path = string_u8_litinit("/Users/yuvaldolev/4ed/build/projects.prj");

global b32 global_edit_mode = true;

global View_ID global_build_view;
global u8 global_build_file_path[4096] = "./build.sh";

#include "4coder_yuval_theme.cpp"
#include "4coder_yuval_startup.cpp"
#include "4coder_yuval_commands.cpp"
#include "4coder_yuval_bindings.cpp"
#include "4coder_yuval_cursor.cpp"
#include "4coder_yuval_code_peek.cpp"
#include "4coder_yuval_brace.cpp"

//~ NOTE(rjf): Error annotations

static void
Fleury4RenderErrorAnnotations(Application_Links *app, Buffer_ID buffer,
                              Text_Layout_ID text_layout_id,
                              Buffer_ID jump_buffer)
{
    Heap *heap = &global_heap;
    Scratch_Block scratch(app);
    Locked_Jump_State jump_state = get_locked_jump_state(app, heap);
    
    Face_ID face = global_small_code_face;
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(jump_buffer != 0 && jump_state.view != 0)
    {
        Managed_Scope buffer_scopes[2] =
        {
            buffer_get_managed_scope(app, jump_buffer),
            buffer_get_managed_scope(app, buffer),
        };
        
        Managed_Scope comp_scope = get_managed_scope_with_multiple_dependencies(app, buffer_scopes, ArrayCount(buffer_scopes));
        Managed_Object *buffer_markers_object = scope_attachment(app, comp_scope, sticky_jump_marker_handle, Managed_Object);
        
        // NOTE(rjf): Get buffer markers (locations where jumps point at).
        i32 buffer_marker_count = 0;
        Marker *buffer_markers = 0;
        {
            buffer_marker_count = managed_object_get_item_count(app, *buffer_markers_object);
            buffer_markers = push_array(scratch, Marker, buffer_marker_count);
            managed_object_load_data(app, *buffer_markers_object, 0, buffer_marker_count, buffer_markers);
        }
        
        i64 last_line = -1;
        
        for(i32 i = 0; i < buffer_marker_count; i += 1)
        {
            i64 jump_line_number = get_line_from_list(app, jump_state.list, i);
            i64 code_line_number = get_line_number_from_pos(app, buffer, buffer_markers[i].pos);
            
            if(code_line_number != last_line)
            {
                
                String_Const_u8 jump_line = push_buffer_line(app, scratch, jump_buffer, jump_line_number);
                
                // NOTE(rjf): Remove file part of jump line.
                {
                    u64 index = string_find_first(jump_line, string_u8_litexpr("error"), StringMatch_CaseInsensitive);
                    if(index == jump_line.size)
                    {
                        index = string_find_first(jump_line, string_u8_litexpr("warning"), StringMatch_CaseInsensitive);
                        if(index == jump_line.size)
                        {
                            index = 0;
                        }
                    }
                    jump_line.str += index;
                    jump_line.size -= index;
                }
                
                // NOTE(rjf): Render annotation.
                {
                    Range_i64 line_range = Ii64(code_line_number);
                    Range_f32 y1 = text_layout_line_on_screen(app, text_layout_id, line_range.min);
                    Range_f32 y2 = text_layout_line_on_screen(app, text_layout_id, line_range.max);
                    Range_f32 y = range_union(y1, y2);
                    if(range_size(y) > 0.f)
                    {
                        Rect_f32 region = text_layout_region(app, text_layout_id);
                        draw_string(app, face, jump_line,
                                    V2f32(region.x1 - metrics.max_advance*jump_line.size -
                                          (y.max-y.min)/2 - metrics.line_height/2,
                                          y.min + (y.max-y.min)/2 - metrics.line_height/2),
                                    0xffff0000);
                    }
                }
            }
            
            last_line = code_line_number;
        }
    }
}


//~ NOTE(rjf): Function Helper

static Range_i64_Array
Fleury4GetLeftParens(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 pos, u32 flags)
{
    Range_i64_Array array = {};
    i32 max = 100;
    array.ranges = push_array(arena, Range_i64, max);
    
    for(;;)
    {
        Range_i64 range = {};
        range.max = pos;
        if(find_nest_side(app, buffer, pos - 1, flags | FindNest_Balanced,
                          Scan_Backward, NestDelim_Open, &range.start))
        {
            array.ranges[array.count] = range;
            array.count += 1;
            pos = range.first;
            if (array.count >= max)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return(array);
}

static String_Const_u8
Fleury4CopyStringButOnlyAllowOneSpace(Arena *arena, String_Const_u8 string)
{
    String_Const_u8 result = {0};
    
    u64 space_to_allocate = 0;
    u64 spaces_left_this_gap = 1;
    
    for(u64 i = 0; i < string.size; ++i)
    {
        if(string.str[i] <= 32)
        {
            if(spaces_left_this_gap > 0)
            {
                --spaces_left_this_gap;
                ++space_to_allocate;
            }
        }
        else
        {
            spaces_left_this_gap = 1;
            ++space_to_allocate;
        }
    }
    
    result.data = push_array(arena, u8, space_to_allocate);
    for(u64 i = 0; i < string.size; ++i)
    {
        if(string.str[i] <= 32)
        {
            if(spaces_left_this_gap > 0)
            {
                --spaces_left_this_gap;
                result.str[result.size++] = string.str[i];
            }
        }
        else
        {
            spaces_left_this_gap = 1;
            result.str[result.size++] = string.str[i];
        }
    }
    
    return result;
}

static void
Fleury4RenderFunctionHelper(Application_Links *app, View_ID view, Buffer_ID buffer,
                            Text_Layout_ID text_layout_id, i64 pos)
{
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Token_Iterator_Array it;
    Token *token = 0;
    
    Rect_f32 view_rect = view_get_screen_rect(app, view);
    
    Face_ID face = global_small_code_face;
    Face_Metrics metrics = get_face_metrics(app, face);
    
    if(token_array.tokens != 0)
    {
        it = token_iterator_pos(0, &token_array, pos);
        token = token_it_read(&it);
        
        if(token != 0 && token->kind == TokenBaseKind_ParentheticalOpen)
        {
            pos = token->pos + token->size;
        }
        else
        {
            if (token_it_dec_all(&it))
            {
                token = token_it_read(&it);
                if (token->kind == TokenBaseKind_ParentheticalClose &&
                    pos == token->pos + token->size)
                {
                    pos = token->pos;
                }
            }
        }
    }
    
    Scratch_Block scratch(app);
    Range_i64_Array ranges = Fleury4GetLeftParens(app, scratch, buffer, pos, FindNest_Paren);
    
    for(int range_index = 0; range_index < ranges.count; ++range_index)
    {
        it = token_iterator_pos(0, &token_array, ranges.ranges[range_index].min);
        token_it_dec_non_whitespace(&it);
        token = token_it_read(&it);
        if(token->kind == TokenBaseKind_Identifier)
        {
            Range_i64 function_name_range = Ii64(token->pos, token->pos+token->size);
            String_Const_u8 function_name = push_buffer_range(app, scratch, buffer, function_name_range);
            
            if (global_edit_mode) {
                Fleury4RenderRangeHighlight(app, view, text_layout_id, function_name_range);
            }
            
            // NOTE(rjf): Find active parameter.
            int active_parameter_index = 0;
            static int last_active_parameter = -1;
            {
                it = token_iterator_pos(0, &token_array, function_name_range.min);
                int paren_nest = 0;
                for(;token_it_inc_non_whitespace(&it);)
                {
                    token = token_it_read(&it);
                    if(token->pos + token->size > pos)
                    {
                        break;
                    }
                    
                    if(token->kind == TokenBaseKind_ParentheticalOpen)
                    {
                        ++paren_nest;
                    }
                    else if(token->kind == TokenBaseKind_StatementClose)
                    {
                        if(paren_nest == 1)
                        {
                            ++active_parameter_index;
                        }
                    }
                    else if(token->kind == TokenBaseKind_ParentheticalClose)
                    {
                        if(!--paren_nest)
                        {
                            break;
                        }
                    }
                }
            }
            b32 active_parameter_has_increased_by_one = active_parameter_index == last_active_parameter + 1;
            last_active_parameter = active_parameter_index;
            
            for(Buffer_ID buffer_it = get_buffer_next(app, 0, Access_Always);
                buffer_it != 0; buffer_it = get_buffer_next(app, buffer_it, Access_Always))
            {
                Code_Index_File *file = code_index_get_file(buffer_it);
                if(file != 0)
                {
                    for(i32 i = 0; i < file->note_array.count; i += 1)
                    {
                        Code_Index_Note *note = file->note_array.ptrs[i];
                        
                        if((note->note_kind == CodeIndexNote_Function ||
                            note->note_kind == CodeIndexNote_Macro) &&
                           string_match(note->text, function_name))
                        {
                            Range_i64 function_def_range;
                            function_def_range.min = note->pos.min;
                            function_def_range.max = note->pos.max;
                            
                            Range_i64 highlight_parameter_range = {0};
                            
                            Token_Array find_token_array = get_token_array_from_buffer(app, buffer_it);
                            it = token_iterator_pos(0, &find_token_array, function_def_range.min);
                            
                            int paren_nest = 0;
                            int param_index = 0;
                            for(;token_it_inc_non_whitespace(&it);)
                            {
                                token = token_it_read(&it);
                                if(token->kind == TokenBaseKind_ParentheticalOpen)
                                {
                                    if(++paren_nest == 1)
                                    {
                                        if(active_parameter_index == param_index)
                                        {
                                            highlight_parameter_range.min = token->pos+1;
                                        }
                                    }
                                }
                                else if(token->kind == TokenBaseKind_ParentheticalClose)
                                {
                                    if(!--paren_nest)
                                    {
                                        function_def_range.max = token->pos + token->size;
                                        if(param_index == active_parameter_index)
                                        {
                                            highlight_parameter_range.max = token->pos;
                                        }
                                        break;
                                    }
                                }
                                else if(token->kind == TokenBaseKind_StatementClose)
                                {
                                    if(param_index == active_parameter_index)
                                    {
                                        highlight_parameter_range.max = token->pos;
                                    }
                                    
                                    if(paren_nest == 1)
                                    {
                                        ++param_index;
                                    }
                                    
                                    if(param_index == active_parameter_index)
                                    {
                                        highlight_parameter_range.min = token->pos+1;
                                    }
                                }
                            }
                            
                            if(highlight_parameter_range.min > function_def_range.min &&
                               function_def_range.max > highlight_parameter_range.max)
                            {
                                
                                String_Const_u8 function_def = push_buffer_range(app, scratch, buffer_it,
                                                                                 function_def_range);
                                String_Const_u8 highlight_param_untrimmed = push_buffer_range(app, scratch, buffer_it,
                                                                                              highlight_parameter_range);
                                
                                String_Const_u8 pre_highlight_def_untrimmed =
                                {
                                    function_def.str,
                                    (u64)(highlight_parameter_range.min - function_def_range.min),
                                };
                                
                                String_Const_u8 post_highlight_def_untrimmed =
                                {
                                    function_def.str + highlight_parameter_range.max - function_def_range.min,
                                    (u64)(function_def_range.max - highlight_parameter_range.max),
                                };
                                
                                String_Const_u8 highlight_param = Fleury4CopyStringButOnlyAllowOneSpace(scratch, highlight_param_untrimmed);
                                String_Const_u8 pre_highlight_def = Fleury4CopyStringButOnlyAllowOneSpace(scratch, pre_highlight_def_untrimmed);
                                String_Const_u8 post_highlight_def = Fleury4CopyStringButOnlyAllowOneSpace(scratch, post_highlight_def_untrimmed);
                                
                                Rect_f32 helper_rect =
                                {
                                    global_cursor_position.x + 16,
                                    global_cursor_position.y + 16,
                                    global_cursor_position.x + 16,
                                    global_cursor_position.y + metrics.line_height + 26,
                                };
                                
                                f32 padding = (helper_rect.y1 - helper_rect.y0)/2 -
                                    metrics.line_height/2;
                                
                                // NOTE(rjf): Size helper rect by how much text to draw.
                                {
                                    helper_rect.x1 += get_string_advance(app, face, highlight_param);
                                    helper_rect.x1 += get_string_advance(app, face, pre_highlight_def);
                                    helper_rect.x1 += get_string_advance(app, face, post_highlight_def);
                                    helper_rect.x1 += 2 * padding;
                                }
                                
                                if(helper_rect.x1 > view_get_screen_rect(app, view).x1)
                                {
                                    f32 difference = helper_rect.x1 - view_get_screen_rect(app, view).x1;
                                    helper_rect.x0 -= difference;
                                    helper_rect.x1 -= difference;
                                }
                                
                                Vec2_f32 text_position =
                                {
                                    helper_rect.x0 + padding,
                                    helper_rect.y0 + padding,
                                };
                                
                                Fleury4DrawTooltipRect(app, helper_rect);
                                
                                text_position = draw_string(app, face, pre_highlight_def,
                                                            text_position, finalize_color(defcolor_comment, 0));
                                
                                // NOTE(rjf): Spawn power mode particles if we've changed active parameters.
                                if(active_parameter_has_increased_by_one && global_power_mode_enabled)
                                {
                                    Vec2_f32 camera = Fleury4GetCameraFromView(app, view);
                                    
                                    f32 text_width = get_string_advance(app, face, highlight_param);
                                    
                                    for(int particle_i = 0; particle_i < 600; ++particle_i)
                                    {
                                        f32 movement_angle = RandomF32(-3.1415926535897f*3.f/2.f, 3.1415926535897f*1.f/3.f);
                                        f32 velocity_magnitude = RandomF32(20.f, 180.f);
                                        f32 velocity_x = cosf(movement_angle)*velocity_magnitude;
                                        f32 velocity_y = sinf(movement_angle)*velocity_magnitude;
                                        Fleury4Particle(text_position.x + 4 + camera.x + (particle_i/500.f)*text_width,
                                                        text_position.y + 8 + camera.y,
                                                        velocity_x, velocity_y,
                                                        0xffffffff,
                                                        RandomF32(1.5f, 8.f),
                                                        RandomF32(0.5f, 6.f));
                                    }
                                    
                                    global_power_mode.screen_shake += RandomF32(20.f, 40.f);
                                }
                                
                                text_position = draw_string(app, face, highlight_param,
                                                            text_position, finalize_color(defcolor_comment_pop, 1));
                                text_position = draw_string(app, face, post_highlight_def,
                                                            text_position, finalize_color(defcolor_comment, 0));
                                
                                goto end_lookup;
                            }
                        }
                    }
                }
            }
            
            end_lookup:;
            break;
        }
    }
}

//~ NOTE(rjf): Buffer Render

static void
Fleury4RenderBuffer(Application_Links *app, View_ID view_id, Face_ID face_id,
                    Buffer_ID buffer, Text_Layout_ID text_layout_id,
                    Rect_f32 rect, Frame_Info frame_info)
{
    ProfileScope(app, "[Yuval] Render Buffer");
    
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    
    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if(token_array.tokens != 0) {
        yuval_draw_cpp_token_colors(app, text_layout_id, &token_array);
        
        // NOTE(yuval): Scan for TODOs, NOTEs, IMPORTANTs and STUDYs
        if(global_config.use_comment_keyword) {
            char user_string_buf[256] = {0};
            String_Const_u8 user_string = {0};
            {
                user_string.data = user_string_buf;
                user_string.size = snprintf(user_string_buf, sizeof(user_string_buf), "(%.*s)",
                                            string_expand(global_config.user_name));
            }
            
            Comment_Highlight_Pair pairs[] =
            {
                {string_u8_litexpr("NOTE"), 0xFF459b54},
                {string_u8_litexpr("TODO"), 0xFFD61B1B},
                {string_u8_litexpr("IMPORTANT"), 0xFFC2C22D},
                {string_u8_litexpr("STUDY"), 0xFF259FAE},
                {user_string, 0xFF007878},
            };

            draw_comment_highlights(app, buffer, text_layout_id,
                                    &token_array, pairs, ArrayCount(pairs));
        }
    } else {
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }
    
    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);
    
    // NOTE(allen): Scope highlight
    if(global_config.use_scope_highlight)
    {
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(rjf): Brace highlight
    {
        ARGB_Color colors[] =
        {
            0xff8ffff2,
        };
        
        Fleury4RenderBraceHighlight(app, buffer, text_layout_id, cursor_pos,
                                    colors, sizeof(colors)/sizeof(colors[0]));
    }
    
    // NOTE(allen): Line highlight
    if(global_config.highlight_line_at_cursor && is_active_view)
    {
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
        draw_line_highlight(app, text_layout_id, line_number,
                            fcolor_id(defcolor_highlight_cursor_line));
    }
    
    if(global_config.use_error_highlight || global_config.use_jump_highlight)
    {
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if(global_config.use_error_highlight)
        {
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                 fcolor_id(defcolor_highlight_junk));
        }
        
        // NOTE(allen): Search highlight
        if(global_config.use_jump_highlight)
        {
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if(jump_buffer != compilation_buffer)
            {
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                     fcolor_id(defcolor_highlight_white));
            }
        }
    }
    
    // NOTE(rjf): Error annotations
    {
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        Fleury4RenderErrorAnnotations(app, buffer, text_layout_id, compilation_buffer);
    }
    
    // NOTE(rjf): Token highlight
    if (global_edit_mode) {
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, cursor_pos);
        Token *token = token_it_read(&it);
        if(token && token->kind == TokenBaseKind_Identifier)
        {
            Fleury4RenderRangeHighlight(app, view_id, text_layout_id,
                                        Ii64(token->pos, token->pos + token->size));
        }
    }

    // NOTE(allen): Color parens
    if(global_config.use_paren_helper)
    {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(rjf): Divider Comments
    {
        Fleury4RenderDividerComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    f32 cursor_roundness = (metrics.normal_advance*0.5f)*0.9f;
    f32 mark_thickness = 2.f;
    
    // NOTE(allen): Cursor
    switch (fcoder_mode)
    {
        case FCoderMode_Original:
        {
            yuval_render_cursor(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness, frame_info);
            break;
        }
        
        case FCoderMode_NotepadLike:
        {
            draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
            break;
        }
    }
    
    // NOTE(rjf): Brace annotations
    {
        Fleury4RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
    }
    
    // NOTE(rjf): Brace lines
    {
        Fleury4RenderBraceLines(app, buffer, view_id, text_layout_id, cursor_pos);
    }
    
    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    // NOTE(rjf): Update calc (once per frame).
    {
        static i32 last_frame_index = -1;
        if(last_frame_index != frame_info.index)
        {
            CalcUpdateOncePerFrame(frame_info);
        }
        last_frame_index = frame_info.index;
    }
    
    // NOTE(rjf): Interpret buffer as calc code, if it's the calc buffer.
    {
        Buffer_ID calc_buffer_id = get_buffer_by_name(app, string_u8_litexpr("*calc*"), AccessFlag_Read);
        if(calc_buffer_id == buffer)
        {
            Fleury4RenderCalcBuffer(app, buffer, view_id, text_layout_id, frame_info);
        }
    }
    
    // NOTE(rjf): Draw calc comments.
    {
        Fleury4RenderCalcComments(app, buffer, view_id, text_layout_id, frame_info);
    }
    
    draw_set_clip(app, prev_clip);
    
    // NOTE(rjf): Draw tooltips and stuff.
    if(active_view == view_id)
    {
        if(global_code_peek_open)
        {
            if(buffer == global_code_peek_token_buffer)
            {
                Fleury4RenderRangeHighlight(app, view_id, text_layout_id, global_code_peek_token_range);
            }
            Fleury4RenderCodePeek(app, view_id, global_small_code_face, buffer, frame_info);
        }
        else
        {
            // NOTE(rjf): Function helper
            {
                Fleury4RenderFunctionHelper(app, view_id, buffer, text_layout_id, cursor_pos);
            }
        }
    }
    
    // NOTE(rjf): Draw power mode.
    {
        Fleury4RenderPowerMode(app, view_id, face_id, frame_info);
    }
    
}

//~ NOTE(rjf): Render hook

static void
Fleury4Render(Application_Links *app, Frame_Info frame_info, View_ID view_id)
{
    ProfileScope(app, "[Yuval] Render");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 region = rect_inner(view_rect, 1.f);
    
    // NOTE(rjf): Draw background.
    {
        ARGB_Color color;
        if (view_id == global_build_view) {
            color = BUILD_VIEW_BACKGROUND_COLOR;
        } else {
            color = fcolor_resolve(fcolor_id(defcolor_back));
        }

        draw_rectangle(app, region, 0.f, color);
        draw_margin(app, view_rect, region, color);
    }
    
    Rect_f32 prev_clip = draw_set_clip(app, region);
    
    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;
    
    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar)
    {
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }
    
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
    
    Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);
    
    if(!block_match_struct(&scroll.position, &delta.point))
    {
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }
    
    if(delta.still_animating)
    {
        animate_in_n_milliseconds(app, 0);
    }
    
    // NOTE(allen): query bars
    {
        Query_Bar *space[32];
        Query_Bar_Ptr_Array query_bars = {};
        query_bars.ptrs = space;
        if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars))
        {
            for (i32 i = 0; i < query_bars.count; i += 1)
            {
                Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
                draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
                region = pair.max;
            }
        }
    }
    
    // NOTE(allen): FPS hud
    if(show_fps_hud)
    {
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }
    
    // NOTE(allen): layout line numbers
    Rect_f32 line_number_rect = {};
    if(global_config.show_line_number_margins)
    {
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }
    
    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    if(is_active_view)
    {
        buffer_point.pixel_shift.y += global_power_mode.screen_shake*1.f;
        global_power_mode.screen_shake -= global_power_mode.screen_shake * frame_info.animation_dt * 12.f;
    }
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);
    
    // NOTE(allen): draw line numbers
    if(global_config.show_line_number_margins)
    {
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }
    
    // NOTE(allen): draw the buffer
    Fleury4RenderBuffer(app, view_id, face_id, buffer, text_layout_id, region, frame_info);
    
    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

//~ NOTE(rjf): Begin buffer hook

BUFFER_HOOK_SIG(Fleury4BeginBuffer)
{
    ProfileScope(app, "[Yuval] Begin Buffer");
    
    Scratch_Block scratch(app);
    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
    
    if(file_name.size > 0)
    {
        String_Const_u8_Array extensions = global_config.code_exts;
        String_Const_u8 ext = string_file_extension(file_name);
        
        for(i32 i = 0; i < extensions.count; ++i)
        {
            if(string_match(ext, extensions.strings[i]))
            {
                treat_as_code = true;
                break;
            }
        }
    }
    
    Command_Map_ID map_id = (treat_as_code) ? (mapid_code) : (mapid_file);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;
    
    Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    *eol_setting = setting;
    
    // NOTE(allen): Decide buffer settings
    b32 wrap_lines = true;
    b32 use_virtual_whitespace = false;
    b32 use_lexer = false;
    if(treat_as_code)
    {
        wrap_lines = global_config.enable_code_wrapping;
        use_virtual_whitespace = global_config.enable_virtual_whitespace;
        use_lexer = true;
    }
    
    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
    if(string_match(buffer_name, string_u8_litexpr("*compilation*")))
    {
        wrap_lines = false;
    }
    
    if(use_lexer)
    {
        ProfileBlock(app, "begin buffer kick off lexer");
        Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
        *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
    }
    
    {
        b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
        *wrap_lines_ptr = wrap_lines;
    }
    
    if (use_virtual_whitespace)
    {
        if (use_lexer)
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
        }
        else
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
        }
    }
    else
    {
        buffer_set_layout(app, buffer_id, layout_generic);
    }
    
    // no meaning for return
    return(0);
}


//~ NOTE(rjf): Layout

static Layout_Item_List
Fleury4LayoutInner(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width, Layout_Virtual_Indent virt_indent){
    Layout_Item_List list = get_empty_item_list(range);
    
    Scratch_Block scratch(app);
    String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);
    
    Face_Advance_Map advance_map = get_face_advance_map(app, face);
    Face_Metrics metrics = get_face_metrics(app, face);
    LefRig_TopBot_Layout_Vars pos_vars = get_lr_tb_layout_vars(&advance_map, &metrics, width);
    
    if (text.size == 0){
        lr_tb_write_blank(&pos_vars, face, arena, &list, range.first);
    }
    else{
        b32 skipping_leading_whitespace = (virt_indent == LayoutVirtualIndent_On);
        Newline_Layout_Vars newline_vars = get_newline_layout_vars();
        
        u8 *ptr = text.str;
        u8 *end_ptr = ptr + text.size;
        for (;ptr < end_ptr;){
            Character_Consume_Result consume = utf8_consume(ptr, (u64)(end_ptr - ptr));
            
            i64 index = layout_index_from_ptr(ptr, text.str, range.first);
            switch (consume.codepoint){
                case '\t':
                case ' ':
                {
                    newline_layout_consume_default(&newline_vars);
                    f32 advance = lr_tb_advance(&pos_vars, face, consume.codepoint);
                    if (!skipping_leading_whitespace){
                        lr_tb_write_with_advance(&pos_vars, face, advance, arena, &list, index, consume.codepoint);
                    }
                    else{
                        lr_tb_advance_x_without_item(&pos_vars, advance);
                    }
                }break;
                
                default:
                {
                    newline_layout_consume_default(&newline_vars);
                    lr_tb_write(&pos_vars, face, arena, &list, index, consume.codepoint);
                }break;
                
                case '\r':
                {
                    newline_layout_consume_CR(&newline_vars, index);
                }break;
                
                case '\n':
                {
                    i64 newline_index = newline_layout_consume_LF(&newline_vars, index);
                    lr_tb_write_blank(&pos_vars, face, arena, &list, newline_index);
                    lr_tb_next_line(&pos_vars);
                }break;
                
                case max_u32:
                {
                    newline_layout_consume_default(&newline_vars);
                    lr_tb_write_byte(&pos_vars, face, arena, &list, index, *ptr);
                }break;
            }
            
            ptr += consume.inc;
        }
        
        if (newline_layout_consume_finish(&newline_vars)){
            i64 index = layout_index_from_ptr(ptr, text.str, range.first);
            lr_tb_write_blank(&pos_vars, face, arena, &list, index);
        }
    }
    
    layout_item_list_finish(&list, -pos_vars.line_to_text_shift);
    
    return(list);
}

static Layout_Item_List
Fleury4Layout(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width)
{
    return(Fleury4LayoutInner(app, arena, buffer, range, face, width, LayoutVirtualIndent_Off));
}

//~ NOTE(rjf): Custom layer initialization

void
custom_layer_init(Application_Links *app)
{
    Thread_Context *tctx = get_thread_context(app);
    
    // NOTE(allen): setup for default framework
    {
        async_task_handler_init(app, &global_async_system);
        code_index_init();
        buffer_modified_set_init();
        Profile_Global_List *list = get_core_profile_list(app);
        ProfileThreadName(tctx, list, string_u8_litexpr("main"));
        initialize_managed_id_metadata(app);
        set_default_color_scheme(app);
    }
    
    // NOTE(allen): default hooks and command maps
    {
        set_all_default_hooks(app);
        set_custom_hook(app, HookID_RenderCaller,  Fleury4Render);
        set_custom_hook(app, HookID_BeginBuffer,   Fleury4BeginBuffer);
        set_custom_hook(app, HookID_Layout,        Fleury4Layout);
        mapping_init(tctx, &framework_mapping);
        yuval_set_bindings(&framework_mapping);
    }
    
    // NOTE(rjf): Open calc buffer.
    {
        Buffer_ID calc_buffer = create_buffer(app, string_u8_litexpr("*calc*"),
                                              BufferCreate_NeverAttachToFile |
                                              BufferCreate_AlwaysNew);
        buffer_set_setting(app, calc_buffer, BufferSetting_Unimportant, true);
        // (void)calc_buffer;
    }
    
    yuval_update_dark_theme(app);
}
