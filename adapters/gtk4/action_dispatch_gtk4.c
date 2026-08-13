/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/action_dispatch_gtk4.c
 *
 * PURPOSE:
 *   Collect generic action arguments with native GTK4 prompts, then execute the
 *   canonical Framework action and refresh the toolkit-neutral workbench model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <string.h>

#include "umicom/document/file_filter.h"

typedef struct UmiGtk4ActionPrompt {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
    UmiUiActionArgumentKind kind;
    GtkWindow *window;
    GtkWidget *primary;
    GtkWidget *secondary;
} UmiGtk4ActionPrompt;

static void execute_action(UmiGtk4Adapter *adapter,
                           const char *action_id,
                           const char *argument)
{
    UmiUiWorkbench *workbench;
    char message[512] = "";
    UmiStatus status;
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_workbench_execute_action(workbench, action_id,
                                             argument != NULL ? argument : "",
                                             message, sizeof(message));
    if (message[0] == '\0') {
        (void)g_snprintf(message, sizeof(message), "%s: %s",
                         action_id, umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
    if (status == UMI_STATUS_OK) (void)umi_gtk4_refresh_workbench(adapter);
}

static void file_prompt_response(GtkNativeDialog *dialog,
                                 int response,
                                 gpointer user_data)
{
    UmiGtk4ActionPrompt *prompt = (UmiGtk4ActionPrompt *)user_data;
    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        if (file != NULL) {
            char *path = g_file_get_path(file);
            if (path != NULL) execute_action(prompt->adapter, prompt->action_id, path);
            g_free(path);
            g_object_unref(file);
        }
    }
    g_object_unref(dialog);
    g_free(prompt);
}

static void add_file_filters(GtkFileChooser *chooser)
{
    UmiDocumentFileFilterSet filters;
    size_t filter_index;
    if (umi_document_file_filters_default(&filters) != UMI_STATUS_OK) return;
    for (filter_index = 0U; filter_index < filters.count; ++filter_index) {
        GtkFileFilter *native = gtk_file_filter_new();
        size_t pattern_index;
        gtk_file_filter_set_name(native, filters.filters[filter_index].name);
        for (pattern_index = 0U;
             pattern_index < filters.filters[filter_index].pattern_count;
             ++pattern_index) {
            gtk_file_filter_add_pattern(native,
                filters.filters[filter_index].patterns[pattern_index]);
        }
        gtk_file_chooser_add_filter(chooser, native);
        g_object_unref(native);
    }
}

static void show_file_prompt(UmiGtk4Adapter *adapter,
                             const UmiUiActionSnapshot *action)
{
    GtkFileChooserAction chooser_action =
        action->argument_kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH
        ? GTK_FILE_CHOOSER_ACTION_OPEN : GTK_FILE_CHOOSER_ACTION_SAVE;
    GtkFileChooserNative *chooser = gtk_file_chooser_native_new(
        action->label, adapter->window, chooser_action,
        chooser_action == GTK_FILE_CHOOSER_ACTION_OPEN ? "Open" : "Save",
        "Cancel");
    UmiGtk4ActionPrompt *prompt;
    if (chooser == NULL) return;
    prompt = g_new0(UmiGtk4ActionPrompt, 1);
    if (prompt == NULL) {
        g_object_unref(chooser);
        return;
    }
    prompt->adapter = adapter;
    prompt->kind = action->argument_kind;
    (void)g_strlcpy(prompt->action_id, action->action_id,
                    sizeof(prompt->action_id));
    add_file_filters(GTK_FILE_CHOOSER(chooser));
    g_signal_connect(chooser, "response",
                     G_CALLBACK(file_prompt_response), prompt);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(chooser));
}

static void text_prompt_close(GtkButton *button, gpointer user_data)
{
    UmiGtk4ActionPrompt *prompt = (UmiGtk4ActionPrompt *)user_data;
    (void)button;
    gtk_window_destroy(prompt->window);
    g_free(prompt);
}

static void text_prompt_accept(GtkButton *button, gpointer user_data)
{
    UmiGtk4ActionPrompt *prompt = (UmiGtk4ActionPrompt *)user_data;
    const char *primary;
    const char *secondary;
    char argument[UMI_UI_DESCRIPTION_CAPACITY];
    (void)button;
    primary = gtk_editable_get_text(GTK_EDITABLE(prompt->primary));
    secondary = prompt->secondary != NULL
        ? gtk_editable_get_text(GTK_EDITABLE(prompt->secondary)) : NULL;
    if (secondary != NULL) {
        (void)g_snprintf(argument, sizeof(argument), "%s\n%s", primary, secondary);
    } else {
        (void)g_strlcpy(argument, primary, sizeof(argument));
    }
    execute_action(prompt->adapter, prompt->action_id, argument);
    gtk_window_destroy(prompt->window);
    g_free(prompt);
}

static void show_text_prompt(UmiGtk4Adapter *adapter,
                             const UmiUiActionSnapshot *action)
{
    UmiGtk4ActionPrompt *prompt = g_new0(UmiGtk4ActionPrompt, 1);
    GtkWidget *box;
    GtkWidget *buttons;
    GtkWidget *accept;
    GtkWidget *cancel;
    if (prompt == NULL) return;
    prompt->adapter = adapter;
    prompt->kind = action->argument_kind;
    (void)g_strlcpy(prompt->action_id, action->action_id,
                    sizeof(prompt->action_id));
    prompt->window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(prompt->window, action->label);
    gtk_window_set_transient_for(prompt->window, adapter->window);
    gtk_window_set_modal(prompt->window, TRUE);
    gtk_window_set_default_size(prompt->window, 460, 160);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    prompt->primary = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(prompt->primary),
        action->argument_kind == UMI_UI_ACTION_ARGUMENT_LINE_NUMBER
            ? "Line number" : "Find text");
    gtk_box_append(GTK_BOX(box), prompt->primary);
    if (action->argument_kind == UMI_UI_ACTION_ARGUMENT_FIND_REPLACE) {
        prompt->secondary = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(prompt->secondary), "Replace with");
        gtk_box_append(GTK_BOX(box), prompt->secondary);
    }
    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(buttons, GTK_ALIGN_END);
    cancel = gtk_button_new_with_label("Cancel");
    accept = gtk_button_new_with_label(action->label);
    g_signal_connect(cancel, "clicked", G_CALLBACK(text_prompt_close), prompt);
    g_signal_connect(accept, "clicked", G_CALLBACK(text_prompt_accept), prompt);
    gtk_box_append(GTK_BOX(buttons), cancel);
    gtk_box_append(GTK_BOX(buttons), accept);
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_child(prompt->window, box);
    gtk_window_present(prompt->window);
    gtk_widget_grab_focus(prompt->primary);
}

void umi_gtk4_dispatch_action(UmiGtk4Adapter *adapter, const char *action_id)
{
    UmiUiWorkbench *workbench;
    UmiUiActionSnapshot action;
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_action_model_find(umi_ui_workbench_actions(workbench),
                                 action_id, &action) != UMI_STATUS_OK) return;
    if (action.argument_kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH ||
        action.argument_kind == UMI_UI_ACTION_ARGUMENT_SAVE_PATH) {
        show_file_prompt(adapter, &action);
    } else if (action.argument_kind == UMI_UI_ACTION_ARGUMENT_TEXT ||
               action.argument_kind == UMI_UI_ACTION_ARGUMENT_FIND_REPLACE ||
               action.argument_kind == UMI_UI_ACTION_ARGUMENT_LINE_NUMBER) {
        show_text_prompt(adapter, &action);
    } else {
        execute_action(adapter, action.action_id, action.argument);
    }
}
