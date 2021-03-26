/* date = March 26th 2021 5:01 pm */

#ifndef Y4_COMMANDS_H
#define Y4_COMMANDS_H

CUSTOM_COMMAND_SIG(y4_interactive_switch_buffer_other_panel)
CUSTOM_DOC("Switch buffer in the other panel");

CUSTOM_COMMAND_SIG(y4_interactive_open_or_new_other_panel)
CUSTOM_DOC("Open an existing file or create a new one in the other panel");

CUSTOM_COMMAND_SIG(y4_search_for_definition__project_wide_other_panel)
CUSTOM_DOC("Search for a definition throughout the whole project in the other panel");

CUSTOM_COMMAND_SIG(y4_search_for_definition__current_file_other_panel)
CUSTOM_DOC("Search for a definition throughout the current file in the other panel");

CUSTOM_COMMAND_SIG(y4_find_matching_file_other_panel)
CUSTOM_DOC("Find the matching file and open it in the other panel");

#endif //Y4_COMMANDS_H
