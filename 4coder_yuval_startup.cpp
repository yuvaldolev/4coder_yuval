CUSTOM_COMMAND_SIG(yuval_startup)
CUSTOM_DOC("Yuval startup event.")
{
    ProfileScope(app, "default startup");
    User_Input input = get_current_input(app);
    if(match_core_code(&input, CoreCode_Startup)){
        String_Const_u8_Array file_names = input.event.core.file_names;
        load_themes_default_folder(app);
        default_4coder_initialize(app, file_names);

        // NOTE(yuval): Create the panels
        {
            View_ID left_code_view = get_active_view(app, Access_Always);

            // NOTE(yuval): Create the build panel
            {
                global_build_view = open_footer_panel(app, left_code_view);
            
                view_set_split_proportion(app, global_build_view, BUILD_PANEL_COLLAPSED_P);
                hide_scrollbar(app);
                hide_filebar(app);

                Buffer_ID compilation_buffer = buffer_identifier_to_id_create_out_buffer(app, standard_build_build_buffer_identifier);
                view_set_buffer(app, global_build_view, compilation_buffer, 0);
            }

            change_active_panel(app);

            // NOTE(yuval): Open side by side code panels
            default_4coder_side_by_side_panels(app, file_names);

            // NOTE(yuval): Set the left code panel's settings
            {
                hide_scrollbar(app);
                hide_filebar(app);
                change_active_panel(app);
            }

            // NOTE(yuval): Set the right code panel's settings
            {
                hide_scrollbar(app);
                hide_filebar(app);
            }

            // NOTE(yuval): Set the left code panel as the active panel
            view_set_active(app, left_code_view);
        }

        
        if (global_config.automatically_load_project){
            load_project(app);
        }
        
        // NOTE(rjf): Initialize stylish fonts.
        {
            Scratch_Block scratch(app);
            String_Const_u8 bin_path = system_get_path(scratch, SystemPath_Binary);
            
            // NOTE(rjf): Title font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 18;
                    desc.parameters.bold = 0;
                    desc.parameters.italic = 0;
                    desc.parameters.hinting = 1;
                }
                global_styled_title_face = try_create_new_face(app, &desc);
            }
            
            // NOTE(rjf): Label font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 10;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 1;
                }
                global_styled_label_face = try_create_new_face(app, &desc);
            }
            
            // NOTE(rjf): Small code font.
            {
                Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
                
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/Inconsolata-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = normal_code_desc.parameters.pt_size - 1;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 1;
                }
                global_small_code_face = try_create_new_face(app, &desc);
            }
        }
    }
}
