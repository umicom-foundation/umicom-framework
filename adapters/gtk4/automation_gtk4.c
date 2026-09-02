/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/automation_gtk4.c
 *
 * PURPOSE:
 *   Perform Umicom user-acceptance actions inside a running GTK4 application.
 *   No global mouse or keyboard hooks are used, so tests stay inside the target
 *   application and do not interfere with the user's other desktop programs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_GTK4_AUTOMATION_ID_KEY "umicom-automation-id"

struct UmiGtk4AutomationDriver {
    GtkWidget *root;
    uint64_t revision;
};

/* Copy report text with a guaranteed terminator and report truncation clearly. */
static int automation_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U) return 0;
    written = snprintf(destination, capacity, "%s", source != NULL ? source : "");
    return written >= 0 && (size_t)written < capacity;
}

/* Walk the GTK child tree until the requested stable automation ID is found. */
static GtkWidget *automation_find_widget(GtkWidget *widget, const char *target_id)
{
    GtkWidget *child;
    const char *widget_id;

    if (widget == NULL || target_id == NULL) return NULL;

    widget_id = (const char *)g_object_get_data(
        G_OBJECT(widget),
        UMI_GTK4_AUTOMATION_ID_KEY);
    if (widget_id != NULL && strcmp(widget_id, target_id) == 0) return widget;

    /* GTK4 exposes children through sibling navigation rather than GtkContainer. */
    for (child = gtk_widget_get_first_child(widget);
         child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = automation_find_widget(child, target_id);
        if (match != NULL) return match;
    }

    return NULL;
}

/* Check whether a top-level window belongs to the driver's window family. */
static int automation_window_is_related(
    GtkWindow *candidate,
    GtkWindow *driver_window)
{
    GtkWindow *current = candidate;
    size_t depth = 0U;

    if (candidate == NULL || driver_window == NULL) return 0;
    while (current != NULL && depth < 32U) {
        if (current == driver_window) return 1;
        current = gtk_window_get_transient_for(current);
        depth += 1U;
    }
    return 0;
}

/*
 * Resolve a control in the main tree or a related modal window. Text prompts
 * are separate GTK top levels, so checking only normal children would make a
 * click-then-type journey stop immediately after opening its dialog.
 */
static GtkWidget *automation_resolve_widget(
    UmiGtk4AutomationDriver *driver,
    const char *target_id)
{
    GtkWidget *match;
    GtkRoot *native_root;
    GtkWindow *driver_window = NULL;
    GListModel *top_levels;
    guint index;

    if (driver == NULL || driver->root == NULL) return NULL;

    match = automation_find_widget(driver->root, target_id);
    if (match != NULL) return match;

    native_root = gtk_widget_get_root(driver->root);
    if (native_root != NULL && GTK_IS_WINDOW(native_root)) {
        driver_window = GTK_WINDOW(native_root);
    } else if (GTK_IS_WINDOW(driver->root)) {
        driver_window = GTK_WINDOW(driver->root);
    }

    top_levels = gtk_window_get_toplevels();
    if (top_levels == NULL) return NULL;

    for (index = 0U; index < g_list_model_get_n_items(top_levels); ++index) {
        GtkWindow *window = GTK_WINDOW(g_list_model_get_item(top_levels, index));

        if (window == NULL) continue;
        /* A parented root searches only its own window and transient dialogs. */
        if (driver_window == NULL ||
            automation_window_is_related(window, driver_window)) {
            match = automation_find_widget(GTK_WIDGET(window), target_id);
        }
        g_object_unref(window);
        if (match != NULL) return match;
    }

    return NULL;
}

/* Read the human-visible text supported by common interactive GTK4 controls. */
static const char *automation_widget_text(GtkWidget *widget)
{
    if (GTK_IS_EDITABLE(widget)) return gtk_editable_get_text(GTK_EDITABLE(widget));
    if (GTK_IS_LABEL(widget)) return gtk_label_get_text(GTK_LABEL(widget));
    if (GTK_IS_BUTTON(widget)) return gtk_button_get_label(GTK_BUTTON(widget));
    if (GTK_IS_MENU_BUTTON(widget)) {
        return gtk_menu_button_get_label(GTK_MENU_BUTTON(widget));
    }
    return "";
}

/* Capture a consistent post-action state for reports and later HTML rendering. */
static void automation_observe(
    UmiGtk4AutomationDriver *driver,
    GtkWidget *widget,
    const char *target_id,
    UmiUiAutomationObservation *out_observation)
{
    if (driver == NULL || widget == NULL || out_observation == NULL) return;

    (void)memset(out_observation, 0, sizeof(*out_observation));
    (void)automation_copy_text(
        out_observation->target_id,
        sizeof(out_observation->target_id),
        target_id);
    (void)automation_copy_text(
        out_observation->role_name,
        sizeof(out_observation->role_name),
        G_OBJECT_TYPE_NAME(widget));
    (void)automation_copy_text(
        out_observation->text,
        sizeof(out_observation->text),
        automation_widget_text(widget));
    out_observation->visible = gtk_widget_get_visible(widget) != FALSE;
    out_observation->enabled = gtk_widget_get_sensitive(widget) != FALSE;
    out_observation->focused = gtk_widget_has_focus(widget) != FALSE;

    /* Toggle state and drop-down selection both represent a selected control. */
    if (GTK_IS_TOGGLE_BUTTON(widget)) {
        out_observation->selected =
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) != FALSE;
    } else if (GTK_IS_DROP_DOWN(widget)) {
        out_observation->selected =
            gtk_drop_down_get_selected(GTK_DROP_DOWN(widget)) !=
            GTK_INVALID_LIST_POSITION;
    }

    driver->revision = driver->revision == UINT64_MAX
        ? 1U
        : driver->revision + 1U;
    out_observation->revision = driver->revision;
}

/* Check the state needed by one of the two bounded wait operations. */
static int automation_wait_condition(
    GtkWidget *widget,
    UmiUiAutomationOperation operation)
{
    if (operation == UMI_UI_AUTOMATION_WAIT_VISIBLE) {
        return gtk_widget_get_visible(widget) != FALSE;
    }
    return gtk_widget_get_sensitive(widget) != FALSE;
}

/*
 * Process pending GTK work while waiting for a visible or enabled state. This
 * loop has a caller-supplied deadline and therefore cannot wait forever.
 */
static UmiStatus automation_wait(
    GtkWidget *widget,
    UmiUiAutomationOperation operation,
    uint32_t timeout_ms)
{
    const gint64 started = g_get_monotonic_time();
    const gint64 timeout_us = (gint64)timeout_ms * 1000;

    while (!automation_wait_condition(widget, operation)) {
        gint64 elapsed;

        while (g_main_context_pending(NULL)) {
            (void)g_main_context_iteration(NULL, FALSE);
        }

        elapsed = g_get_monotonic_time() - started;
        if (elapsed >= timeout_us) return UMI_STATUS_TIMEOUT;

        /* A one-millisecond pause prevents a wait from consuming a CPU core. */
        g_usleep(1000U);
    }

    return UMI_STATUS_OK;
}

/* Parse an explicit true, false or toggle request for a GTK toggle button. */
static UmiStatus automation_toggle(GtkWidget *widget, const char *value)
{
    gboolean active;

    if (!GTK_IS_TOGGLE_BUTTON(widget)) return UMI_STATUS_INVALID_ARGUMENT;

    active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    if (value == NULL || value[0] == '\0' || strcmp(value, "toggle") == 0) {
        active = !active;
    } else if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
        active = TRUE;
    } else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0) {
        active = FALSE;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), active);
    return UMI_STATUS_OK;
}

/* Select a drop-down row by a bounded numeric index. */
static UmiStatus automation_select(GtkWidget *widget, const char *value)
{
    GtkDropDown *drop_down;
    GListModel *model;
    char *end = NULL;
    guint64 selected;

    if (!GTK_IS_DROP_DOWN(widget) || value == NULL || value[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    selected = g_ascii_strtoull(value, &end, 10);
    if (end == value || end == NULL || *end != '\0' || selected > G_MAXUINT) {
        return UMI_STATUS_PARSE_ERROR;
    }

    drop_down = GTK_DROP_DOWN(widget);
    model = gtk_drop_down_get_model(drop_down);
    if (model == NULL || selected >= g_list_model_get_n_items(model)) {
        return UMI_STATUS_NOT_FOUND;
    }

    gtk_drop_down_set_selected(drop_down, (guint)selected);
    return UMI_STATUS_OK;
}

/* Perform one toolkit-neutral action against its tagged GTK4 control. */
static UmiStatus automation_perform(
    void *context,
    const UmiUiAutomationStep *step,
    UmiUiAutomationObservation *out_observation,
    char *out_message,
    size_t message_capacity)
{
    UmiGtk4AutomationDriver *driver = (UmiGtk4AutomationDriver *)context;
    GtkWidget *widget;
    UmiStatus status = UMI_STATUS_OK;
    const char *message = "Action completed.";

    if (driver == NULL || driver->root == NULL || step == NULL ||
        out_observation == NULL || out_message == NULL || message_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    widget = automation_resolve_widget(driver, step->target_id);
    if (widget == NULL) {
        (void)automation_copy_text(
            out_message,
            message_capacity,
            "No control has this stable automation ID.");
        return UMI_STATUS_NOT_FOUND;
    }

    /*
     * Keep the target alive until evidence is captured. A Close or Cancel
     * action may remove its widget synchronously while gtk_widget_activate is
     * still returning, but the driver must never inspect released memory.
     */
    g_object_ref(widget);

    switch (step->operation) {
        case UMI_UI_AUTOMATION_FOCUS:
            status = gtk_widget_grab_focus(widget)
                ? UMI_STATUS_OK
                : UMI_STATUS_UNAVAILABLE;
            message = "Control received keyboard focus.";
            break;
        case UMI_UI_AUTOMATION_CLICK:
        case UMI_UI_AUTOMATION_INVOKE_COMMAND:
            status = gtk_widget_activate(widget)
                ? UMI_STATUS_OK
                : UMI_STATUS_UNAVAILABLE;
            message = "Control was activated through GTK.";
            break;
        case UMI_UI_AUTOMATION_TYPE_TEXT:
            if (!GTK_IS_EDITABLE(widget)) {
                status = UMI_STATUS_INVALID_ARGUMENT;
            } else {
                gtk_editable_set_text(GTK_EDITABLE(widget), step->value);
                message = "Text was entered into the editable control.";
            }
            break;
        case UMI_UI_AUTOMATION_SELECT:
            status = automation_select(widget, step->value);
            message = "Drop-down selection was changed.";
            break;
        case UMI_UI_AUTOMATION_TOGGLE:
            status = automation_toggle(widget, step->value);
            message = "Toggle state was changed.";
            break;
        case UMI_UI_AUTOMATION_OPEN_MENU:
            if (!GTK_IS_MENU_BUTTON(widget)) {
                status = UMI_STATUS_INVALID_ARGUMENT;
            } else {
                gtk_menu_button_popup(GTK_MENU_BUTTON(widget));
                message = "Menu was opened.";
            }
            break;
        case UMI_UI_AUTOMATION_WAIT_VISIBLE:
        case UMI_UI_AUTOMATION_WAIT_ENABLED:
            status = automation_wait(widget, step->operation, step->timeout_ms);
            message = status == UMI_STATUS_OK
                ? "Expected control state became available."
                : "Timed out before the expected control state became available.";
            break;
        case UMI_UI_AUTOMATION_ASSERT_TEXT:
            status = strcmp(automation_widget_text(widget), step->value) == 0
                ? UMI_STATUS_OK
                : UMI_STATUS_INVALID_STATE;
            message = status == UMI_STATUS_OK
                ? "Displayed text matches the expected value."
                : "Displayed text does not match the expected value.";
            break;
        case UMI_UI_AUTOMATION_CAPTURE_EVIDENCE:
            message = "Current control state was captured.";
            break;
        default:
            status = UMI_STATUS_NOT_IMPLEMENTED;
            message = "The GTK4 driver does not implement this operation.";
            break;
    }

    automation_observe(driver, widget, step->target_id, out_observation);
    (void)automation_copy_text(out_message, message_capacity, message);
    g_object_unref(widget);
    return status;
}

/* Create a driver that safely retains the application widget tree it will inspect. */
UmiStatus umi_gtk4_automation_driver_create(
    void *native_root,
    UmiGtk4AutomationDriver **out_driver)
{
    UmiGtk4AutomationDriver *driver;

    if (out_driver == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_driver = NULL;

    if (native_root == NULL || !GTK_IS_WIDGET(native_root)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    driver = (UmiGtk4AutomationDriver *)calloc(1U, sizeof(*driver));
    if (driver == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    /* Retaining the root prevents a test from following a destroyed pointer. */
    driver->root = GTK_WIDGET(g_object_ref(native_root));
    driver->revision = 1U;
    *out_driver = driver;
    return UMI_STATUS_OK;
}

/* Release the retained GTK root before freeing the small driver object. */
void umi_gtk4_automation_driver_destroy(UmiGtk4AutomationDriver *driver)
{
    if (driver == NULL) return;

    if (driver->root != NULL) g_object_unref(driver->root);
    free(driver);
}

/* Build the toolkit-neutral callback value consumed by the scenario runner. */
UmiUiAutomationDriver umi_gtk4_automation_driver_interface(
    UmiGtk4AutomationDriver *driver)
{
    UmiUiAutomationDriver interface_value;

    (void)memset(&interface_value, 0, sizeof(interface_value));
    interface_value.structure_size = (uint32_t)sizeof(interface_value);
    interface_value.api_version = UMI_UI_AUTOMATION_API_VERSION;
    (void)automation_copy_text(
        interface_value.driver_id,
        sizeof(interface_value.driver_id),
        "umicom.gtk4.in-process");
    interface_value.context = driver;
    interface_value.perform = automation_perform;
    return interface_value;
}

/* Attach a copied semantic ID to a widget so layouts and captions may change safely. */
UmiStatus umi_gtk4_automation_tag_widget(
    void *native_widget,
    const char *automation_id)
{
    if (native_widget == NULL || !GTK_IS_WIDGET(native_widget) ||
        !umi_ui_id_is_valid(automation_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    g_object_set_data_full(
        G_OBJECT(native_widget),
        UMI_GTK4_AUTOMATION_ID_KEY,
        g_strdup(automation_id),
        g_free);
    return UMI_STATUS_OK;
}
