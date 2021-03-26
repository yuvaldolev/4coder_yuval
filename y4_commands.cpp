CUSTOM_COMMAND_SIG(y4_interactive_switch_buffer_other_panel) {
    change_active_panel_send_command(app, interactive_switch_buffer);
}

CUSTOM_COMMAND_SIG(y4_interactive_open_or_new_other_panel) {
    change_active_panel_send_command(app, interactive_open_or_new);
}
