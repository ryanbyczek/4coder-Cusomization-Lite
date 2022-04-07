#include "4coder_default_include.cpp"
#include "generated/managed_id_metadata.cpp"

/////////////////////////////////////////////////////////////////////////////
// CUSTOM COMMANDS                                                         //
/////////////////////////////////////////////////////////////////////////////

CUSTOM_COMMAND_SIG(ryanb_command_set_line_endings_crlf) {
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    rewrite_lines_to_crlf(app, buffer);
    set_eol_mode_to_crlf(app);
}

CUSTOM_COMMAND_SIG(ryanb_command_set_line_endings_lf) {
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    rewrite_lines_to_lf(app, buffer);
    set_eol_mode_to_lf(app);
}

CUSTOM_COMMAND_SIG(ryanb_command_lister) {
    View_ID view = get_this_ctx_view(app, Access_Always);
    if (view == 0) return;
    Buffer_ID buffer = buffer = view_get_buffer(app, view, Access_ReadVisible);
    
    Scratch_Block scratch(app);
    
    Lister_Block lister(app, scratch);
    lister_set_query(lister, string_u8_litexpr("Select a command..."));
    lister_set_default_handlers(lister);
    
    // apply theme
    {
        String_Const_u8 fcoder_extension = string_u8_litexpr(".4coder");
        String_Const_u8 file_name = push_buffer_unique_name(app, scratch, buffer);
        if (string_match(string_postfix(file_name, fcoder_extension.size), fcoder_extension)) {
            lister_add_item(lister, string_u8_litexpr("apply theme"), file_name, (void*)load_theme_current_buffer, 0);
        }
    }
    
    // change line endings
    {
        Managed_Scope scope = buffer_get_managed_scope(app, buffer);
        Line_Ending_Kind* eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
        switch (*eol_setting) {
            case LineEndingKind_Binary: {
                lister_add_item(lister, string_u8_litexpr("change line endings to CRLF"), string_u8_litexpr(""), (void*)ryanb_command_set_line_endings_crlf, 0);
                lister_add_item(lister, string_u8_litexpr("change line endings to LF"),   string_u8_litexpr(""), (void*)ryanb_command_set_line_endings_lf, 0);
            }
            break;
            
            case LineEndingKind_LF: {
                lister_add_item(lister, string_u8_litexpr("change line endings to CRLF"), string_u8_litexpr(""), (void*)ryanb_command_set_line_endings_crlf, 0);
            }
            break;
            
            case LineEndingKind_CRLF: {
                lister_add_item(lister, string_u8_litexpr("change line endings to LF"), string_u8_litexpr(""), (void*)ryanb_command_set_line_endings_lf, 0);
            }
            break;
        }
    }
    
    Lister_Result result = run_lister(app, lister);
    
    if (!result.canceled) {
        Custom_Command_Function* command = (Custom_Command_Function*)(result.user_data);
        if (command != 0) {
            view_enqueue_command_function(app, view, command);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CUSTOM HOTKEY FUNCTIONS                                                 //
/////////////////////////////////////////////////////////////////////////////

CUSTOM_COMMAND_SIG(ryanb_build_in_build_panel) {
    build_in_build_panel(app);
    change_to_build_panel(app);
}

CUSTOM_COMMAND_SIG(ryanb_goto_line) {
    goto_line(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_interactive_open_all_code) {
    close_all_code(app);
    interactive_open(app);
    open_all_code(app);
}

CUSTOM_COMMAND_SIG(ryanb_jump_to_definition_at_cursor) {
    jump_to_definition_at_cursor(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_jump_to_last_point) {
    jump_to_last_point(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_kill_to_end_of_line) {
    Scratch_Block scratch(app);
    current_view_boundary_delete(app, Scan_Forward, push_boundary_list(scratch, boundary_line));
}

CUSTOM_COMMAND_SIG(ryanb_move_down_to_blank_line) {
    move_down_to_blank_line(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_move_up_to_blank_line) {
    move_up_to_blank_line(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_page_down) {
    page_down(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_page_up) {
    page_up(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_paste_and_replace_token) {
    move_left_token_boundary(app);
    set_mark(app);
    move_right_token_boundary(app);
    delete_char(app);
    paste_and_indent(app);
}

CUSTOM_COMMAND_SIG(ryanb_search) {
    search(app);
    center_view(app);
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ryanb_seek_beginning_of_line) {
    seek_beginning_of_line(app);
    left_adjust_view(app);
}

CUSTOM_COMMAND_SIG(ryanb_swap_panels) {
    swap_panels(app);
    change_active_panel(app);
}

CUSTOM_COMMAND_SIG(ryanb_toggle_panel) {
    View_ID view = get_active_view(app, Access_Always);
    View_ID next_view = get_next_view_looped_primary_panels(app, view, Access_Always);
    
    if (view == next_view) {
        open_panel_vsplit(app);
        change_active_panel(app);
    }
    else {
        change_active_panel(app);
        close_panel(app);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CUSTOM MAPPINGS                                                         //
/////////////////////////////////////////////////////////////////////////////

void setup_ryanb_mapping(Mapping* mapping, i64 global_id, i64 file_id, i64 code_id) {
    MappingScope();
    SelectMapping(mapping);
    
    // global bindings
    SelectMap(global_id);
    
    Bind(ryanb_build_in_build_panel,      KeyCode_B,       KeyCode_Control);
    Bind(change_active_panel,             KeyCode_Comma,   KeyCode_Control);
    Bind(change_active_panel,             KeyCode_Period,  KeyCode_Control);
    Bind(close_build_footer_panel,        KeyCode_Escape);
    Bind(exit_4coder,                     KeyCode_F4,      KeyCode_Alt);
    Bind(kill_buffer,                     KeyCode_F4,      KeyCode_Control);
    Bind(toggle_fullscreen,               KeyCode_F11);
    Bind(ryanb_command_lister,            KeyCode_F12);
    Bind(interactive_new,                 KeyCode_N,       KeyCode_Control);
    Bind(interactive_open,                KeyCode_O,       KeyCode_Control);
    Bind(ryanb_interactive_open_all_code, KeyCode_O,       KeyCode_Control, KeyCode_Shift);
    Bind(ryanb_toggle_panel,              KeyCode_P,       KeyCode_Control);
    Bind(ryanb_swap_panels,               KeyCode_P,       KeyCode_Control, KeyCode_Shift);
    Bind(exit_4coder,                     KeyCode_Q,       KeyCode_Control);
    Bind(change_active_panel,             KeyCode_Tab,     KeyCode_Control);
    Bind(interactive_switch_buffer,       KeyCode_W,       KeyCode_Control);
    Bind(kill_buffer,                     KeyCode_W,       KeyCode_Control, KeyCode_Shift);
    
    // plain text file bindings
    SelectMap(file_id);
    ParentMap(global_id);
    
    Bind(select_all,                     KeyCode_A,         KeyCode_Control);
    Bind(backspace_char,                 KeyCode_Backspace);
    Bind(copy,                           KeyCode_C,         KeyCode_Control);
    Bind(duplicate_line,                 KeyCode_D,         KeyCode_Control);
    Bind(delete_char,                    KeyCode_Delete);
    Bind(delete_line,                    KeyCode_Delete,    KeyCode_Shift);
    Bind(move_down,                      KeyCode_Down);
    Bind(move_line_down,                 KeyCode_Down,      KeyCode_Alt);
    Bind(ryanb_move_down_to_blank_line,  KeyCode_Down,      KeyCode_Control);
    Bind(seek_end_of_line,               KeyCode_End);
    Bind(goto_end_of_file,               KeyCode_End,       KeyCode_Control);
    Bind(ryanb_search,                   KeyCode_F,         KeyCode_Control);
    Bind(list_all_locations,             KeyCode_F,         KeyCode_Control, KeyCode_Shift);
    Bind(rename_file_query,              KeyCode_F2,        KeyCode_Control);
    Bind(ryanb_goto_line,                KeyCode_G,         KeyCode_Control);
    Bind(query_replace,                  KeyCode_H,         KeyCode_Control);
    Bind(query_replace_identifier,       KeyCode_H,         KeyCode_Control, KeyCode_Alt);
    Bind(seek_beginning_of_textual_line, KeyCode_Home);
    Bind(goto_beginning_of_file,         KeyCode_Home,      KeyCode_Control);
    Bind(ryanb_kill_to_end_of_line,      KeyCode_K,         KeyCode_Control);
    Bind(move_left,                      KeyCode_Left);
    Bind(move_left_token_boundary,       KeyCode_Left,      KeyCode_Control);
    Bind(ryanb_page_down,                KeyCode_PageDown);
    Bind(goto_end_of_file,               KeyCode_PageDown,  KeyCode_Control);
    Bind(ryanb_page_up,                  KeyCode_PageUp);
    Bind(goto_beginning_of_file,         KeyCode_PageUp,    KeyCode_Control);
    Bind(move_right,                     KeyCode_Right);
    Bind(move_right_token_boundary,      KeyCode_Right,     KeyCode_Control);
    Bind(exit_4coder,                    KeyCode_Q,         KeyCode_Control);
    Bind(save,                           KeyCode_S,         KeyCode_Control);
    Bind(save_all_dirty_buffers,         KeyCode_S,         KeyCode_Control, KeyCode_Shift);
    Bind(center_view,                    KeyCode_Space,     KeyCode_Control);
    Bind(move_up,                        KeyCode_Up);
    Bind(move_line_up,                   KeyCode_Up,        KeyCode_Alt);
    Bind(ryanb_move_up_to_blank_line,    KeyCode_Up,        KeyCode_Control);
    Bind(paste,                          KeyCode_V,         KeyCode_Control);
    Bind(interactive_switch_buffer,      KeyCode_W,         KeyCode_Control);
    Bind(cut,                            KeyCode_X,         KeyCode_Control);
    Bind(undo,                           KeyCode_Z,         KeyCode_Control);
    Bind(redo,                           KeyCode_Z,         KeyCode_Control, KeyCode_Shift);
    
    // code file bindings
    SelectMap(code_id);
    ParentMap(file_id);
    
    Bind(comment_line_toggle,                KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(move_left_whitespace_boundary,      KeyCode_Left,         KeyCode_Control);
    Bind(write_note,                         KeyCode_N,            KeyCode_Alt);
    Bind(ryanb_jump_to_definition_at_cursor, KeyCode_Return,       KeyCode_Control);
    Bind(ryanb_jump_to_last_point,           KeyCode_Return,       KeyCode_Control, KeyCode_Shift);
    Bind(move_right_whitespace_boundary,     KeyCode_Right,        KeyCode_Control);
    Bind(ryanb_jump_to_last_point,           KeyCode_Minus,        KeyCode_Control);
    Bind(write_todo,                         KeyCode_T,            KeyCode_Alt);
    Bind(word_complete,                      KeyCode_Tab);
    Bind(paste_and_indent,                   KeyCode_V,            KeyCode_Control);
    Bind(ryanb_paste_and_replace_token,      KeyCode_V,            KeyCode_Control, KeyCode_Shift);
}

/////////////////////////////////////////////////////////////////////////////
// CUSTOM LAYER INIT                                                       //
/////////////////////////////////////////////////////////////////////////////

void custom_layer_init(Application_Links* app) {
    Thread_Context* tctx = get_thread_context(app);
    
    default_framework_init(app);
    set_all_default_hooks(app);
    mapping_init(tctx, &framework_mapping);
    
    String_ID mapid_global = vars_save_string_lit("keys_global");
    String_ID mapid_file = vars_save_string_lit("keys_file");
    String_ID mapid_code = vars_save_string_lit("keys_code");
    
    setup_essential_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    setup_ryanb_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    //setup_default_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
}
