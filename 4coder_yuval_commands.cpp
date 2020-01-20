
//~ Commands from Ryan Fleury's Custom Layer

CUSTOM_COMMAND_SIG(fleury_write_text_input)
CUSTOM_DOC("Inserts whatever text was used to trigger this command.")
{
    write_text_input(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_text_and_auto_indent)
CUSTOM_DOC("Inserts text and auto-indents the line on which the cursor sits if any of the text contains 'layout punctuation' such as ;:{}()[]# and new lines.")
{
    write_text_and_auto_indent(app);
    Fleury4SpawnPowerModeParticles(app, get_active_view(app, Access_ReadWriteVisible));
}

CUSTOM_COMMAND_SIG(fleury_write_zero_struct)
CUSTOM_DOC("At the cursor, insert a ' = {0};'.")
{
    write_string(app, string_u8_litexpr(" = {0};"));
}

CUSTOM_COMMAND_SIG(fleury_home)
CUSTOM_DOC("Goes to the beginning of the line.")
{
    seek_pos_of_visual_line(app, Side_Min);
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    scroll.target.pixel_shift.x = 0;
    view_set_buffer_scroll(app, view, scroll, SetBufferScroll_NoCursorChange);
}

CUSTOM_COMMAND_SIG(fleury_place_cursor)
CUSTOM_DOC("Places a new cursor at the current main cursor position.")
{
    //View_ID view = get_active_view(app, Access_ReadWriteVisible);
    //i64 current_cursor_pos = view_get_cursor_pos(app, view);
    //global_cursor_positions[global_cursor_count++] = current_cursor_pos;
}

//~ Yuval's Custom Commands

CUSTOM_COMMAND_SIG(yuval_home_and_tab)
CUSTOM_DOC("Goes to the beginning of the line and tabs.")
{
    fleury_home(app);
    auto_indent_line_at_cursor(app);
}

CUSTOM_COMMAND_SIG(yuval_toggle_edit_mode)
CUSTOM_DOC("Toggles the global edit mode and upadtes the theme accordingly.")
{
    global_edit_mode = !global_edit_mode;
    yuval_update_dark_theme(app);
}

CUSTOM_COMMAND_SIG(yuval_clean_and_save)
CUSTOM_DOC("Cleans all lines in the current buffer and saves it.")
{
    auto_indent_whole_file(app);
    clean_all_lines(app);
    save(app);
}

CUSTOM_COMMAND_SIG(yuval_switch_buffer_other_panel)
CUSTOM_DOC("Displays the buffer lister in the other panel.")
{
    change_active_panel_send_command(app, interactive_switch_buffer);
}

CUSTOM_COMMAND_SIG(yuval_open_matching_file_other_panel)
CUSTOM_DOC("Opens the matching file in the other panel.")
{
    view_buffer_other_panel(app);
    open_matching_file_cpp(app);
}

CUSTOM_COMMAND_SIG(yuval_kill_to_end_of_line)
CUSTOM_DOC("Deletes all characters from the cursor position to the end of the line.")
{
    View_ID view = get_active_view(app, Access_Write);
    
    i64 start = view_get_cursor_pos(app, view);
    seek_end_of_line(app);
    i64 end = view_get_cursor_pos(app, view);

    Range_i64 range = {start, end};
    if (start == end){
        range.end += 1;
    }

    Buffer_ID buffer = view_get_buffer(app, view, Access_Write);

    String_Const_u8 zero_string = {};
    buffer_replace_range(app, buffer, range, zero_string);
}

CUSTOM_COMMAND_SIG(yuval_save_and_make_without_asking)
CUSTOM_DOC("Saves all dirty panels and executes the global build file")
{
}

function b32
yuval_commands__string_is_empty(String_Const_u8 str) {
    b32 result = (str.size == 0);
    return result;
}

function b32
yuval_commands__string_is_null(String_Const_u8 str) {
    b32 result = ((str.str == 0) && (str.size == 0));
    return result;
}

function String_Const_u8
yuval_commands__string_consume_line(String_Const_u8* source) {
    String_Const_u8 result = {};

    if (!yuval_commands__string_is_empty(*source)) {
        u64 pos = string_find_first(*source, '\n');
        result = string_substring(*source, Ii64(0, (i64)pos));

        *source = string_skip(*source, result.size + 1);

        if ((result.size != 0) && (result.str[result.size - 1] == '\r')) {
            --result.size;
        }
    }

    return result;
}

function b32
yuval_commands__string_consume_non_empty_line(String_Const_u8* source, String_Const_u8* out_line) {
    b32 result;

    for (;;) {
        String_Const_u8 line = yuval_commands__string_consume_line(source);
        if (!yuval_commands__string_is_empty(line)) {
            // NOTE(yuval): We found a non-empty line. Break.
            *out_line = line;
            result = true;
            break;
        } else if (yuval_commands__string_is_null(line)) {
            // NOTE(yuval): We hit the end of the string. Stop reading lines immediately!
            result = false;
            break;
        }
    }

    return result;
}

function String_Const_u8
yuval_commands__string_consume_word(String_Const_u8* source) {
    String_Const_u8 result = {};

    u64 start = 0;
    for (; start < source->size; ++start) {
        u8 c = source->str[start];
        if (!character_is_whitespace(c)) {
            break;
        }
    }

    if (start < source->size) {
        u64 end = start;
        for (; end < source->size; ++end) {
            u8 c = source->str[end];
            if (character_is_whitespace(c)) {
                break;
            }
        }

        result = string_substring(*source, Ii64((i64)start, (i64)end));
        *source = string_skip(*source, end + 1);
    } else {
        *source = string_skip(*source, source->size);
    }

    return result;
}

CUSTOM_COMMAND_SIG(yuval_switch_project)
CUSTOM_DOC("Closes the current project and displays the project list")
{
    local_const String_Const_u8 BUILD_FILE_OS_MAC = string_u8_litinit("mac");
    local_const String_Const_u8 BUILD_FILE_OS_WINDOWS = string_u8_litinit("windows");

    Scratch_Block scratch(app);

    // NOTE(yuval): Open the projects master file
    Plat_Handle projects_master_file_handle;
    if (!system_load_handle(scratch, global_projects_master_file_path, &projects_master_file_handle)) {
        print_message(app, string_u8_litexpr("Failed to open the Projects Master File!\n"));
        return;
    }

    // NOTE(yuval): Read the projects master file
    String_Const_u8 projects_master_file;
    {
        File_Attributes attribs = system_load_attributes(projects_master_file_handle);
        projects_master_file = string_const_u8_push(scratch, attribs.size);
        if (!system_load_file(projects_master_file_handle, (char*)projects_master_file.str, attribs.size)) {
            print_message(app, string_u8_litexpr("Failed to read the Projects Master File!\n"));
            return;
        }
    }

    system_load_close(projects_master_file_handle);

    //
    // NOTE(yuval): Find and list all projects in the master file
    //

    // NOTE(yuval): Begin the project lister
    Lister* lister = begin_lister(app, scratch);
    {
        lister_set_query(lister, string_u8_litexpr("Project:"));
        lister->handlers = lister_get_default_handlers();
    }

    // NOTE(yuval): Add the projects
    for (;;) {
        // NOTE(yuval): Read the project name
        String_Const_u8 project_name;
        {
            b32 found = yuval_commands__string_consume_non_empty_line(&projects_master_file, &project_name);
            if (!found) {
                break;
            }
        }

        // NOTE(yuval): Read the path to the project file
        String_Const_u8* project_file = push_array(scratch, String_Const_u8, 1);
        {
            b32 found = yuval_commands__string_consume_non_empty_line(&projects_master_file, project_file);
            if (!found) {
                break;
            }
        }

        lister_add_item(lister, project_name, *project_file, project_file, 0);
    }

    // NOTE(yuval): Run the lister
    Lister_Result result = run_lister(app, lister);
    if (result.canceled) {
        return;
    }

    String_Const_u8* project_file_path = (String_Const_u8*)result.user_data;
    if (!project_file_path) {
        print_message(app, string_u8_litexpr("The selected project has an invalid path!\n"));
        return;
    }

    //
    // NOTE(yuval): Open the project
    //
    
    // NOTE(yuval): Open the selected project file
    Plat_Handle project_file_handle;
    {
        u8 *c_project_file_path = push_array(scratch, u8, project_file_path->size + 1);
        block_copy(c_project_file_path, project_file_path->str, project_file_path->size);
        c_project_file_path[project_file_path->size] = 0;

        if (!system_load_handle(scratch, (char*)c_project_file_path, &project_file_handle)) {
            print_message(app, string_u8_litexpr("Failed to open the selected project file!\n"));
            return;
        }
    }

    // NOTE(yuval): Read the project file
    String_Const_u8 project_file;
    {
        File_Attributes attribs = system_load_attributes(project_file_handle);
        project_file = string_const_u8_push(scratch, attribs.size);
        if (!system_load_file(project_file_handle, (char*)project_file.str, attribs.size)) {
            print_message(app, string_u8_litexpr("Failed to read the selected project file!\n"));
            return;
        }
    }

    system_load_close(project_file_handle);

    // NOTE(yuval): Parse the project file
    // [] Find Build Script Path
    // [] Find & Open Code Files

    b32 parsing_code_section = false;
    b32 parsing_build_section = false;

    for (;;) {
        String_Const_u8 line = yuval_commands__string_consume_line(&project_file);
        if (yuval_commands__string_is_null(line)){
            // NOTE(yuval): Reached the end of the file. Stop reading lines immediately.
            break;
        }

        line = string_skip_chop_whitespace(line);
        if (yuval_commands__string_is_empty(line)){
            continue;
        }

        if (line.str[0] == '#') {
            continue;
        }

        if (line.str[0] == ':') {
            parsing_code_section = false;
            parsing_build_section = false;

            line = string_skip(line, 1);
            line = string_skip_whitespace(line);

            String_Const_u8 section = yuval_commands__string_consume_word(&line);
            if (string_match(section, string_u8_litexpr("code"))) {
                parsing_code_section = true;
            } else if (string_match(section, string_u8_litexpr("build"))) {
                parsing_build_section = true;
            }
        } else {
            if (parsing_build_section) {
                String_Const_u8 os = yuval_commands__string_consume_word(&line);
                b32 this_os = false;
#if OS_MAC
                if (string_match(os, BUILD_FILE_OS_MAC)) {
                    this_os = true;
                }
#elif OS_WINDOWS
                if (string_match(os, BUILD_FILE_WINDOWS)) {
                    this_os = true;
                }
#endif
                
                if (this_os) {
                    String_Const_u8 build_file_path = yuval_commands__string_consume_word(&line);
                    printf("build_file_path: '%.*s'\n",
                        (i32)build_file_path.size, build_file_path.str);
                }
            }
        }
    }
}

CUSTOM_COMMAND_SIG(yuval_modify_projects)
CUSTOM_DOC("Opens the project list file in the current panel")
{
    
}

CUSTOM_COMMAND_SIG(yuval_command_lister)
CUSTOM_DOC("Displays yuval's command lister in the current panel")
{
    Scratch_Block scratch(app);

    // NOTE(yuval): Begin the commad lister
    Lister* lister = begin_lister(app, scratch);
    {
        lister_set_query(lister, string_u8_litexpr("Command:"));
        lister->handlers = lister_get_default_handlers();
    }

    // NOTE(yuval): Add commands
    {
        // NOTE(yuval): switch_project command
        {
            String_Const_u8 string = string_u8_litexpr("switch_project");
            String_Const_u8 desc = string_u8_litexpr("Closes the current project and displays the project list.");
            void* func = (void*)yuval_switch_project;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): modify_projects command
        {
            String_Const_u8 string = string_u8_litexpr("modify_projects");
            String_Const_u8 desc = string_u8_litexpr("Opens the project list file in the current panel.");
            void* func = (void*)yuval_modify_projects;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): toggle_fullscreen command
        {
            String_Const_u8 string = string_u8_litexpr("toggle_fullscreen");
            String_Const_u8 desc = string_u8_litexpr("Toggles 4coder between fullscreen and windowed modes.");
            void* func = (void*)toggle_fullscreen;
            lister_add_item(lister, string, desc, func, 0);
        }
    }

    // NOTE(yuval): Run the lister
    Lister_Result result = run_lister(app, lister);
    if (!result.canceled) {
        Custom_Command_Function* command = (Custom_Command_Function*)result.user_data;
        command(app);
    }
}

CUSTOM_COMMAND_SIG(yuval_jump_to_definition_other_panel)
CUSTOM_DOC("Display the jump to defintion lister on the panel panel.")
{
    change_active_panel_send_command(app, jump_to_definition);
}

CUSTOM_COMMAND_SIG(yuval_list_all_type_definitions_lister)
CUSTOM_DOC("Creates a lister of locations that look like type definitions.")
{
    char *query = "Type:";
    
    Scratch_Block scratch(app);
    Lister *lister = begin_lister(app, scratch);
    lister_set_query(lister, query);
    lister->handlers = lister_get_default_handlers();
    
    Token_Iterator_Array it;
    Token *token = 0;

    code_index_lock();
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_Always)) {
        Code_Index_File *file = code_index_get_file(buffer);
        Token_Array token_array = get_token_array_from_buffer(app, buffer);

        if (file != 0) {
            for (i32 i = 0; i < file->note_array.count; i += 1) {
                Code_Index_Note *note = file->note_array.ptrs[i];
                Tiny_Jump *jump = push_array(scratch, Tiny_Jump, 1);
                jump->buffer = buffer;
                jump->pos = note->pos.first;
                
                Range_i64 def_range = {0, note->pos.end}; 
                if (note->note_kind == CodeIndexNote_Type) {
                    it = token_iterator_pos(0, &token_array, note->pos.min);
                    while (token_it_dec_non_whitespace(&it)) {
                        token = token_it_read(&it);
                        if ((token->sub_kind == TokenCppKind_Struct) ||
                            (token->sub_kind == TokenCppKind_Enum) ||
                            (token->sub_kind == TokenCppKind_Union) ||
                            (token->sub_kind == TokenCppKind_Typedef)) {
                            def_range.start = token->pos;
                            break;
                        }
                    }
                    String_Const_u8 def = push_buffer_range(app, scratch, buffer, def_range);
                    lister_add_item(lister, note->text, def, jump, 0);
                }
            }
        }
    }
    code_index_unlock();
    
    Lister_Result l_result = run_lister(app, lister);
    Tiny_Jump result = {};
    if (!l_result.canceled && l_result.user_data != 0){
        block_copy_struct(&result, (Tiny_Jump*)l_result.user_data);
    }
    
    if (result.buffer != 0){
        View_ID view = get_this_ctx_view(app, Access_Always);
        jump_to_location(app, view, result.buffer, result.pos);
    }
}

CUSTOM_COMMAND_SIG(yuval_list_all_macros_lister)
CUSTOM_DOC("Creates a lister of locations that look like macros.")
{
    char *query = "Macro:";
    
    Scratch_Block scratch(app);
    Lister *lister = begin_lister(app, scratch);
    lister_set_query(lister, query);
    lister->handlers = lister_get_default_handlers();
    
    Token_Iterator_Array it;
    Token *token = 0;

    code_index_lock();
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_Always)) {
        Code_Index_File *file = code_index_get_file(buffer);
        Token_Array token_array = get_token_array_from_buffer(app, buffer);

        if (file != 0) {
            for (i32 i = 0; i < file->note_array.count; i += 1) {
                Code_Index_Note *note = file->note_array.ptrs[i];
                Tiny_Jump *jump = push_array(scratch, Tiny_Jump, 1);
                jump->buffer = buffer;
                jump->pos = note->pos.first;
                
                Range_i64 def_range = {0, note->pos.end}; 
                if (note->note_kind == CodeIndexNote_Macro) {
                    it = token_iterator_pos(0, &token_array, note->pos.min);
                    while (token_it_dec_non_whitespace(&it)) {
                        token = token_it_read(&it);
                        if (token->sub_kind == TokenCppKind_PPDefine) {
                            def_range.start = token->pos;
                            break;
                        }
                    }
                    String_Const_u8 def = push_buffer_range(app, scratch, buffer, def_range);
                    lister_add_item(lister, note->text, def, jump, 0);
                }
            }
        }
    }
    code_index_unlock();
    
    Lister_Result l_result = run_lister(app, lister);
    Tiny_Jump result = {};
    if (!l_result.canceled && l_result.user_data != 0){
        block_copy_struct(&result, (Tiny_Jump*)l_result.user_data);
    }
    
    if (result.buffer != 0){
        View_ID view = get_this_ctx_view(app, Access_Always);
        jump_to_location(app, view, result.buffer, result.pos);
    }
}

CUSTOM_COMMAND_SIG(yuval_list_all_notes_lister)
CUSTOM_DOC("Creates a lister of locations that look like NOTE comments.")
{
    String_Const_u8 keyword_strings[] = {
        string_u8_litinit("NOTE")
    };

    String_Const_u8_Array keywords = array_initr(keyword_strings);
    list_all_locations__generic(app, keywords, ListAllLocationsFlag_CaseSensitive);

    view_jump_list_with_lister(app);
}

CUSTOM_COMMAND_SIG(yuval_list_all_todos_lister)
CUSTOM_DOC("Creates a lister of locations that look like TODO comments.")
{
    String_Const_u8 keyword_strings[] = {
        string_u8_litinit("TODO")
    };

    String_Const_u8_Array keywords = array_initr(keyword_strings);
    list_all_locations__generic(app, keywords, ListAllLocationsFlag_CaseSensitive);

    view_jump_list_with_lister(app);
}

CUSTOM_COMMAND_SIG(yuval_list_all_importants_lister)
CUSTOM_DOC("Creates a lister of locations that look like IMPORTANT comments.")
{
    String_Const_u8 keyword_strings[] = {
        string_u8_litinit("IMPORTANT")
    };

    String_Const_u8_Array keywords = array_initr(keyword_strings);
    list_all_locations__generic(app, keywords, ListAllLocationsFlag_CaseSensitive);

    view_jump_list_with_lister(app);
}

CUSTOM_COMMAND_SIG(yuval_list_all_studies_lister)
CUSTOM_DOC("Creates a lister of locations that look like STUDY comments.")
{
    String_Const_u8 keyword_strings[] = {
        string_u8_litinit("STUDY")
    };

    String_Const_u8_Array keywords = array_initr(keyword_strings);
    list_all_locations__generic(app, keywords, ListAllLocationsFlag_CaseSensitive);

    view_jump_list_with_lister(app);
}

CUSTOM_COMMAND_SIG(yuval_jump_lister)
CUSTOM_DOC("Displays yuval's jump lister in the current panel.")
{
    Scratch_Block scratch(app);

    // NOTE(yuval): Begin the jump lister
    Lister* lister = begin_lister(app, scratch);
    {
        lister_set_query(lister, string_u8_litexpr("Jump:"));
        lister->handlers = lister_get_default_handlers();
    }

    // NOTE(yuval): Add jump categories
    {
        String_Const_u8 desc = SCu8();

        // NOTE(yuval): Functions
        {
            String_Const_u8 string = string_u8_litexpr("Function");
            void* func = (void*)list_all_functions_all_buffers_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): Types
        {
            String_Const_u8 string = string_u8_litexpr("Type");
            void* func = (void*)yuval_list_all_type_definitions_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): Macros
        {
            String_Const_u8 string = string_u8_litexpr("Macro");
            void* func = (void*)yuval_list_all_macros_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): NOTEs
        {
            String_Const_u8 string = string_u8_litexpr("NOTE");
            void* func = (void*)yuval_list_all_notes_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): TODOs
        {
            String_Const_u8 string = string_u8_litexpr("TODO");
            void* func = (void*)yuval_list_all_todos_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): IMPORTANTs
        {
            String_Const_u8 string = string_u8_litexpr("IMPORTANT");
            void* func = (void*)yuval_list_all_importants_lister;
            lister_add_item(lister, string, desc, func, 0);
        }

        // NOTE(yuval): STUDYs
        {
            String_Const_u8 string = string_u8_litexpr("STUDY");
            void* func = (void*)yuval_list_all_studies_lister;
            lister_add_item(lister, string, desc, func, 0);
        }
    }

    // NOTE(yuval): Run the lister
    Lister_Result result = run_lister(app, lister);
    if (!result.canceled) {
        Custom_Command_Function* command = (Custom_Command_Function*)result.user_data;
        command(app);
    }
}

CUSTOM_COMMAND_SIG(yuval_jump_lister_other_panel)
CUSTOM_DOC("Displays yuval's jump lister in the other panel.")
{
    change_active_panel_send_command(app, yuval_jump_lister);
}
