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

CUSTOM_COMMAND_SIG(ryanb_interactive_open_all_code) {
    close_all_code(app);
    interactive_open(app);
    open_all_code(app);
}

CUSTOM_COMMAND_SIG(ryanb_kill_to_end_of_line) {
    Scratch_Block scratch(app);
    current_view_boundary_delete(app, Scan_Forward, push_boundary_list(scratch, boundary_line));
}

CUSTOM_COMMAND_SIG(ryanb_open_panel_vsplit) {
    View_ID view = get_active_view(app, Access_Always);
    View_ID next_view = get_next_view_looped_primary_panels(app, view, Access_Always);
    
    if (view == next_view) {
        open_panel_vsplit(app);
    }
    else {
        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
        view_set_active(app, next_view);
        view_set_buffer(app, next_view, buffer, 0);
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
    
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    
    BindMouseWheel(mouse_wheel_scroll);
    
    Bind(build_in_build_panel,            KeyCode_B,           KeyCode_Control);                // ctrl + b               : execute build in build panel
    Bind(change_active_panel,             KeyCode_Comma,       KeyCode_Control);                // ctrl + ,               : switch active panel
    Bind(close_build_footer_panel,        KeyCode_Escape);                                      // esc                    : close open build panel and toggle off tooltips
    Bind(close_panel,                     KeyCode_NumPadMinus, KeyCode_Control);                // ctrl + numpad -        : close panel
    Bind(exit_4coder,                     KeyCode_F4,          KeyCode_Alt);                    // alt  + f4              : close 4coder
    Bind(exit_4coder,                     KeyCode_Q,           KeyCode_Control);                // ctrl + q               : try to quit
    Bind(interactive_new,                 KeyCode_N,           KeyCode_Control);                // ctrl + n               : open new file prompt
    Bind(interactive_open,                KeyCode_O,           KeyCode_Control);                // ctrl + o               : open existing file prompt
    Bind(interactive_switch_buffer,       KeyCode_W,           KeyCode_Control);                // ctrl + w               : switch buffer prompt
    Bind(kill_buffer,                     KeyCode_NumPadStar,  KeyCode_Control);                // ctrl + numpad *        : close file
    Bind(quick_swap_buffer,               KeyCode_W,           KeyCode_Control, KeyCode_Shift); // ctrl + shift + ,       : swap panels
    Bind(swap_panels,                     KeyCode_Comma,       KeyCode_Control, KeyCode_Shift); // ctrl + shift + w       : quick swap buffer to previous buffer
    Bind(toggle_fullscreen,               KeyCode_F11);                                         // f11                    : toggle full screen
    Bind(ryanb_command_lister,            KeyCode_F12);                                         // f12                    : open custom command lister
    Bind(ryanb_interactive_open_all_code, KeyCode_O,           KeyCode_Control, KeyCode_Shift); // ctrl + shift + o       : open existing file prompt and open all code near that file
    Bind(ryanb_open_panel_vsplit,         KeyCode_NumPadPlus,  KeyCode_Control);                // ctrl + numpad +        : open split panel
    
    // plain text file bindings
    SelectMap(file_id);
    ParentMap(global_id);
    
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseMove(click_set_cursor_if_lbutton);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    
    BindTextInput(write_text_input);
    
    Bind(backspace_char,                 KeyCode_Backspace);                                 // backspace        : delete previous character
    Bind(center_view,                    KeyCode_Space,     KeyCode_Control);                // ctrl + space     : center view
    Bind(copy,                           KeyCode_C,         KeyCode_Control);                // ctrl + c         : copy selection
    Bind(cut,                            KeyCode_X,         KeyCode_Control);                // ctrl + x         : cut current selection
    Bind(delete_char,                    KeyCode_Delete);                                    // del              : delete next character
    Bind(delete_line,                    KeyCode_Delete,    KeyCode_Shift);                  // shift + del      : delete line
    Bind(duplicate_line,                 KeyCode_D,         KeyCode_Control);                // ctrl + d         : duplicate line
    Bind(exit_4coder,                    KeyCode_Q,         KeyCode_Control);                // ctrl + q         : try to quit
    Bind(goto_beginning_of_file,         KeyCode_Home,      KeyCode_Control);                // ctrl + home      : seek top of file and
    Bind(goto_end_of_file,               KeyCode_End,       KeyCode_Control);                // ctrl + end       : seek bottom of file
    Bind(goto_line,                      KeyCode_G,         KeyCode_Control);                // ctrl + g         : go to line dialog
    Bind(interactive_switch_buffer,      KeyCode_W,         KeyCode_Control);                // ctrl + w         : switch buffer prompt
    Bind(list_all_locations,             KeyCode_F,         KeyCode_Control, KeyCode_Shift); // ctrl + shift + f : find in every buffer
    Bind(move_down,                      KeyCode_Down);                                      // down             : seek line down
    Bind(move_down_to_blank_line,        KeyCode_Down,      KeyCode_Control);                // ctrl + down      : seek whitespace down and center view
    Bind(move_left,                      KeyCode_Left);                                      // left             : seek character left
    Bind(move_left_token_boundary,       KeyCode_Left,      KeyCode_Control);                // ctrl + left      : seek token left
    Bind(move_line_down,                 KeyCode_Down,      KeyCode_Alt);                    // alt  + down      : move line down
    Bind(move_line_up,                   KeyCode_Up,        KeyCode_Alt);                    // alt  + up        : move line up
    Bind(move_right,                     KeyCode_Right);                                     // right            : seek character right
    Bind(move_right_token_boundary,      KeyCode_Right,     KeyCode_Control);                // ctrl + right     : seek token right
    Bind(move_up,                        KeyCode_Up);                                        // up               : seek line up
    Bind(move_up_to_blank_line,          KeyCode_Up,        KeyCode_Control);                // ctrl + up        : seek whitespace up and center view
    Bind(page_down,                      KeyCode_PageDown);                                  // page down        : page down
    Bind(page_up,                        KeyCode_PageUp);                                    // page up          : page up
    Bind(paste,                          KeyCode_V,         KeyCode_Control);                // ctrl + v         : paste
    Bind(query_replace,                  KeyCode_H,         KeyCode_Control);                // ctrl + h         : find and replace prompt
    Bind(query_replace_identifier,       KeyCode_H,         KeyCode_Control, KeyCode_Alt);   // ctrl + alt + h   : find and replace prompt, with current identifier autopopulated as find
    Bind(redo,                           KeyCode_Z,         KeyCode_Control, KeyCode_Shift); // ctrl + shift + z : redo
    Bind(rename_file_query,              KeyCode_F2,        KeyCode_Control);                // ctrl + f2        : rename file prompt
    Bind(reopen,                         KeyCode_T,         KeyCode_Control, KeyCode_Shift); // ctrl + shift + t : reopen closed file
    Bind(save,                           KeyCode_S,         KeyCode_Control);                // ctrl + s         : save current buffer
    Bind(save_all_dirty_buffers,         KeyCode_S,         KeyCode_Control, KeyCode_Shift); // ctrl + shift + s : save all buffers
    Bind(search,                         KeyCode_F,         KeyCode_Control);                // ctrl + f         : find in current buffer
    Bind(seek_beginning_of_textual_line, KeyCode_Home);                                      // home             : seek line start
    Bind(seek_end_of_line,               KeyCode_End);                                       // end              : seek line end
    Bind(select_all,                     KeyCode_A,         KeyCode_Control);                // ctrl + a         : select all
    Bind(undo,                           KeyCode_Z,         KeyCode_Control);                // ctrl + z         : undo
    
    Bind(ryanb_kill_to_end_of_line,      KeyCode_K,         KeyCode_Control);                // ctrl + k         : delete characters from cursor to end of line
    
    // code file bindings
    SelectMap(code_id);
    ParentMap(file_id);
    
    Bind(comment_line_toggle,       KeyCode_ForwardSlash, KeyCode_Alt);     // alt  + /               : toggle line comment
    Bind(move_left_token_boundary,  KeyCode_Left,         KeyCode_Control); // ctrl + left            : seek token left
    Bind(move_right_token_boundary, KeyCode_Right,        KeyCode_Control); // ctrl + right           : seek token right
    Bind(paste_and_indent,          KeyCode_V,            KeyCode_Control); // ctrl + v               : paste and indent
    Bind(word_complete,             KeyCode_Tab);                           // tab                    : auto-complete current word
    Bind(write_note,                KeyCode_N,            KeyCode_Alt);     // alt  + t               : write a NOTE comment
    Bind(write_todo,                KeyCode_T,            KeyCode_Alt);     // alt  + t               : write a TODO comment
    
}

/////////////////////////////////////////////////////////////////////////////
// CUSTOM LAYER INIT                                                       //
/////////////////////////////////////////////////////////////////////////////

void custom_layer_init(Application_Links* app) {
    Thread_Context* tctx = get_thread_context(app);
    
    default_framework_init(app);
    set_all_default_hooks(app);
    mapping_init(tctx, &framework_mapping);
    setup_ryanb_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
}