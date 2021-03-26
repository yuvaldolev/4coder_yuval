CUSTOM_COMMAND_SIG(y4_interactive_switch_buffer_other_panel) {
    change_active_panel_send_command(app, interactive_switch_buffer);
}

CUSTOM_COMMAND_SIG(y4_interactive_open_or_new_other_panel) {
    change_active_panel_send_command(app, interactive_open_or_new);
}

CUSTOM_COMMAND_SIG(y4_search_for_definition__project_wide_other_panel) {
    change_active_panel_send_command(app, f4_search_for_definition__project_wide);
}

CUSTOM_COMMAND_SIG(y4_search_for_definition__current_file_other_panel) {
    change_active_panel_send_command(app, f4_search_for_definition__current_file);
}

CUSTOM_COMMAND_SIG(y4_find_matching_file_other_panel) {
    view_buffer_other_panel(app);
    casey_find_matching_file(app);
}
