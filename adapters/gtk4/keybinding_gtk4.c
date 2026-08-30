/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/keybinding_gtk4.c
 *
 * PURPOSE:
 *   Translate GTK4 key events into toolkit-neutral Umicom keybinding chords,
 *   resolve them through UmiUiKeybindingRegistry, and execute the canonical
 *   Framework command plus argument.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * GTK knows which physical key was pressed, but it does not own Umicom's
 * shortcut policy.  This adapter converts the event to a stable chord such as
 * "Ctrl+Shift+E". Framework then decides whether that chord is enabled in the
 * current context and returns the command ID plus argument to execute.
 */

#include "gtk4_internal.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int append_token(char *buffer,
                        size_t capacity,
                        size_t *length,
                        const char *token)
{
    int written;

    if (buffer == NULL || length == NULL || token == NULL || *length >= capacity) {
        return 0;
    }
    /* snprintf accepts size_t.  g_snprintf accepts gulong on Windows, which
     * made a 64-bit size_t-to-32-bit conversion visible under -Wconversion. */
    written = snprintf(buffer + *length,
                       capacity - *length,
                       "%s%s",
                       *length > 0U ? "+" : "",
                       token);
    if (written < 0 || (size_t)written >= capacity - *length) {
        return 0;
    }
    *length += (size_t)written;
    return 1;
}

static int append_key_name(char *buffer,
                           size_t capacity,
                           size_t *length,
                           guint keyval)
{
    const char *name;
    char normalised[64];
    size_t index;
    size_t name_length;

    name = gdk_keyval_name(keyval);
    if (name == NULL || name[0] == '\0') {
        return 0;
    }

    name_length = strlen(name);
    if (name_length >= sizeof(normalised)) {
        return 0;
    }

    for (index = 0U; index < name_length; ++index) {
        unsigned char ch = (unsigned char)name[index];
        normalised[index] = name_length == 1U
            ? (char)toupper(ch)
            : (char)ch;
    }
    normalised[name_length] = '\0';
    return append_token(buffer, capacity, length, normalised);
}

static int chord_from_event(guint keyval,
                            GdkModifierType state,
                            char *out_chord,
                            size_t capacity)
{
    size_t length = 0U;
    GdkModifierType modifiers;

    if (out_chord == NULL || capacity == 0U) {
        return 0;
    }
    out_chord[0] = '\0';

    modifiers = state & gtk_accelerator_get_default_mod_mask();
    if ((modifiers & GDK_CONTROL_MASK) != 0 &&
        !append_token(out_chord, capacity, &length, "Ctrl")) {
        return 0;
    }
    if ((modifiers & GDK_ALT_MASK) != 0 &&
        !append_token(out_chord, capacity, &length, "Alt")) {
        return 0;
    }
    if ((modifiers & GDK_SHIFT_MASK) != 0 &&
        !append_token(out_chord, capacity, &length, "Shift")) {
        return 0;
    }
    if ((modifiers & GDK_SUPER_MASK) != 0 &&
        !append_token(out_chord, capacity, &length, "Super")) {
        return 0;
    }

    return append_key_name(out_chord, capacity, &length, keyval);
}

static gboolean on_key_pressed(GtkEventControllerKey *controller,
                               guint keyval,
                               guint keycode,
                               GdkModifierType state,
                               gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiKeybindingResolution resolution;
    char chord[UMI_UI_CHORD_CAPACITY];
    char message[512] = "";
    UmiStatus status;

    (void)controller;
    (void)keycode;

    if (adapter == NULL || adapter->shell == NULL) {
        return FALSE;
    }
    if (!chord_from_event(keyval, state, chord, sizeof(chord))) {
        return FALSE;
    }

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_workbench_resolve_keybinding(workbench,
                                                 chord,
                                                 &resolution);
    if (status != UMI_STATUS_OK) {
        return FALSE;
    }

    status = umi_command_registry_execute(
        umi_ui_workbench_commands(workbench),
        resolution.command_id,
        resolution.argument[0] != '\0' ? resolution.argument : NULL,
        message,
        sizeof(message));

    if (message[0] == '\0') {
        (void)g_snprintf(message,
                         sizeof(message),
                         "%s: %s",
                         resolution.command_id,
                         umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);

    if (status == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(adapter);
        return TRUE;
    }
    return FALSE;
}

UmiStatus umi_gtk4_install_keybindings(UmiGtk4Adapter *adapter)
{
    GtkEventController *controller;

    if (adapter == NULL || adapter->window == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    controller = gtk_event_controller_key_new();
    if (controller == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    /* Capture before focused child widgets so global workbench shortcuts remain
     * available while the editor, search entry or terminal has focus.
     */
    gtk_event_controller_set_propagation_phase(controller, GTK_PHASE_CAPTURE);
    g_signal_connect(controller,
                     "key-pressed",
                     G_CALLBACK(on_key_pressed),
                     adapter);
    gtk_widget_add_controller(GTK_WIDGET(adapter->window), controller);
    return UMI_STATUS_OK;
}
