// TODO(rjf): This is only being used to check if a font file exists because
// there's a bug in try_create_new_face that crashes the program if a font is
// not found. This function is only necessary until that is fixed.
static b32
IsFileReadable(String_Const_u8 path)
{
    b32 result = 0;
    FILE *file = fopen((char *)path.str, "r");
    if(file)
    {
        result = 1;
        fclose(file);
    }
    return result;
}

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
            yuval_build_view_open(app, left_code_view);
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
            
            // NOTE(rjf): Fallback font.
            Face_ID face_that_should_totally_be_there = get_face_id(app, 0);
            
            // NOTE(rjf): Title font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 18;
                    desc.parameters.bold = 0;
                    desc.parameters.italic = 0;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_styled_title_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_styled_title_face = face_that_should_totally_be_there;
                }
            }
            
            // NOTE(rjf): Label font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 10;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_styled_label_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_styled_label_face = face_that_should_totally_be_there;
                }
            }
            
            // NOTE(rjf): Small code font.
            {
                Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
                
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/Inconsolata-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = normal_code_desc.parameters.pt_size;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name))
                {
                    global_small_code_face = try_create_new_face(app, &desc);
                }
                else
                {
                    global_small_code_face = face_that_should_totally_be_there;
                }
            }
        }
    }
}
