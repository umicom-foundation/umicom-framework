/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/action_dispatch_gtk4.c
 *
 * PURPOSE:
 *   Collect generic action arguments with native GTK4 prompts, then execute the
 *   canonical Framework action and refresh the toolkit-neutral workbench model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the execute action operation used by this module and its client applications. */
static void execute_action(UmiGtk4Adapter *adapter,
                           const char *action_id,
                           const char *argument)
{
    UmiUiWorkbench *workbench;
    char message[512] = "";
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_workbench_execute_action(workbench, action_id,
                                             argument != NULL ? argument : "",
                                             message, sizeof(message));
    /* Apply this branch only when its contract condition is satisfied. */
    if (message[0] == '\0') {
        (void)g_snprintf(message, sizeof(message), "%s: %s",
                         action_id, umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) (void)umi_gtk4_refresh_workbench(adapter);
}

/*
 * Provide the file prompt complete operation used by this module and its client
 * applications.
 */
static void file_prompt_complete(GObject *source_object,
                                 GAsyncResult *result,
                                 gpointer user_data)
{
    UmiGtk4ActionPrompt *prompt = (UmiGtk4ActionPrompt *)user_data;
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GError *error = NULL;
    GFile *file;

    file = prompt->kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH
        ? gtk_file_dialog_open_finish(dialog, result, &error)
        : gtk_file_dialog_save_finish(dialog, result, &error);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file != NULL) {
        char *path = g_file_get_path(file);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (path != NULL) execute_action(prompt->adapter, prompt->action_id, path);
        g_free(path);
        g_object_unref(file);
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (error != NULL &&
               !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(prompt->adapter->status_label),
                           error->message);
    }
    g_clear_error(&error);
    g_free(prompt);
}

/*
 * Provide the create file filters operation used by this module and its client
 * applications.
 */
static GListStore *create_file_filters(void)
{
    UmiDocumentFileFilterSet filters;
    GListStore *model;
    size_t filter_index;

    model = g_list_store_new(GTK_TYPE_FILE_FILTER);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_document_file_filters_default(&filters) != UMI_STATUS_OK) {
        return model;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (filter_index = 0U; filter_index < filters.count; ++filter_index) {
        GtkFileFilter *native = gtk_file_filter_new();
        size_t pattern_index;
        gtk_file_filter_set_name(native, filters.filters[filter_index].name);
        /* Visit each bounded item once so every record receives the same rule. */
        for (pattern_index = 0U;
             pattern_index < filters.filters[filter_index].pattern_count;
             ++pattern_index) {
            gtk_file_filter_add_pattern(native,
                filters.filters[filter_index].patterns[pattern_index]);
        }
        g_list_store_append(model, native);
        g_object_unref(native);
    }
    return model;
}

/* Provide the show file prompt operation used by this module and its client applications. */
static void show_file_prompt(UmiGtk4Adapter *adapter,
                             const UmiUiActionSnapshot *action)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    GListStore *filters;
    UmiGtk4ActionPrompt *prompt;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dialog == NULL) return;
    prompt = g_new0(UmiGtk4ActionPrompt, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (prompt == NULL) {
        g_object_unref(dialog);
        return;
    }
    prompt->adapter = adapter;
    prompt->kind = action->argument_kind;
    (void)g_strlcpy(prompt->action_id, action->action_id,
                    sizeof(prompt->action_id));
    gtk_file_dialog_set_title(dialog, action->label);
    filters = create_file_filters();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filters != NULL) {
        gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
        g_object_unref(filters);
    }

    /* GtkFileDialog is GTK 4.10's non-deprecated asynchronous replacement for
     * GtkFileChooserNative.  The operation retains the dialog until the
     * callback completes, so the caller can release its reference now. */
    if (action->argument_kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH) {
        gtk_file_dialog_open(dialog,
                             adapter->window,
                             NULL,
                             file_prompt_complete,
                             prompt);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_file_dialog_save(dialog,
                             adapter->window,
                             NULL,
                             file_prompt_complete,
                             prompt);
    }
    g_object_unref(dialog);
}

/* Provide the text prompt close operation used by this module and its client applications. */
static void text_prompt_close(GtkButton *button, gpointer user_data)
{
    UmiGtk4ActionPrompt *prompt = (UmiGtk4ActionPrompt *)user_data;
    (void)button;
    gtk_window_destroy(prompt->window);
    g_free(prompt);
}

/*
 * Provide the text prompt accept operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secondary != NULL) {
        (void)g_snprintf(argument, sizeof(argument), "%s\n%s", primary, secondary);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)g_strlcpy(argument, primary, sizeof(argument));
    }
    execute_action(prompt->adapter, prompt->action_id, argument);
    gtk_window_destroy(prompt->window);
    g_free(prompt);
}

/* Provide the show text prompt operation used by this module and its client applications. */
static void show_text_prompt(UmiGtk4Adapter *adapter,
                             const UmiUiActionSnapshot *action)
{
    UmiGtk4ActionPrompt *prompt = g_new0(UmiGtk4ActionPrompt, 1);
    GtkWidget *box;
    GtkWidget *buttons;
    GtkWidget *accept;
    GtkWidget *cancel;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
            ? "Line number"
            : (action->tooltip[0] != '\0' ? action->tooltip : "Enter text"));
    gtk_box_append(GTK_BOX(box), prompt->primary);
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the gtk4 dispatch action operation used by this module and its client
 * applications.
 */
void umi_gtk4_dispatch_action(UmiGtk4Adapter *adapter, const char *action_id)
{
    UmiUiWorkbench *workbench;
    UmiUiActionSnapshot action;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_action_model_find(umi_ui_workbench_actions(workbench),
                                 action_id, &action) != UMI_STATUS_OK) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action.argument_kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH ||
        action.argument_kind == UMI_UI_ACTION_ARGUMENT_SAVE_PATH) {
        show_file_prompt(adapter, &action);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (action.argument_kind == UMI_UI_ACTION_ARGUMENT_TEXT ||
               action.argument_kind == UMI_UI_ACTION_ARGUMENT_FIND_REPLACE ||
               action.argument_kind == UMI_UI_ACTION_ARGUMENT_LINE_NUMBER) {
        show_text_prompt(adapter, &action);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        execute_action(adapter, action.action_id, action.argument);
    }
}
