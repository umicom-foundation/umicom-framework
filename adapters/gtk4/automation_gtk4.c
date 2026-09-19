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
#include "umicom/ui/gtk4/interaction_recording.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_GTK4_AUTOMATION_ID_KEY "umicom-automation-id"

struct UmiGtk4AutomationDriver {
    GtkWidget *root;
    GtkWidget *observed_scope;
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

/* Retain the first matching object while checking for ambiguous identifiers.
 * A partial or ambiguous tree is not a licence to activate its first match. */
typedef struct AutomationSearch {
    GtkWidget *found;
    size_t matches;
    size_t visited;
    UmiStatus status;
} AutomationSearch;
static void automation_find_widgets(GtkWidget *widget, const char *target_id,
    GtkWidget *excluded, unsigned depth, AutomationSearch *search)
{
    GtkWidget *child;
    const char *id;
    if (widget == NULL || widget == excluded || search->status != UMI_STATUS_OK) return;
    if (depth > 256U || search->visited >= 16384U) {
        search->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    ++search->visited;
    id = g_object_get_data(G_OBJECT(widget), UMI_GTK4_AUTOMATION_ID_KEY);
    if (id != NULL && strcmp(id, target_id) == 0 && search->found != widget) {
        ++search->matches;
        if (search->found == NULL) search->found = g_object_ref(widget);
    }
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        automation_find_widgets(child, target_id, excluded, depth + 1U, search);
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
static UmiStatus automation_resolve_widget(
    UmiGtk4AutomationDriver *driver, const char *target_id, GtkWidget **out_widget)
{
    AutomationSearch search = {0};
    GtkRoot *native_root;
    GtkWindow *driver_window = NULL;
    GListModel *top_levels;
    guint index;
    *out_widget = NULL;
    if (driver == NULL || driver->root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Explicit scopes remain confined after a host reparents one of them. */
    if (driver->observed_scope != NULL) {
        GtkRoot *primary_window = gtk_widget_get_root(driver->root);
        GtkRoot *scope_window = gtk_widget_get_root(driver->observed_scope);
        if (primary_window != NULL && scope_window != NULL && primary_window != scope_window)
            return UMI_STATUS_INVALID_STATE;
    }
    automation_find_widgets(driver->root, target_id, NULL, 0U, &search);
    if (driver->observed_scope != NULL &&
        !gtk_widget_is_ancestor(driver->observed_scope, driver->root))
        automation_find_widgets(driver->observed_scope, target_id, driver->root, 0U, &search);
    native_root = gtk_widget_get_root(driver->root);
    if (native_root == NULL && driver->observed_scope != NULL)
        native_root = gtk_widget_get_root(driver->observed_scope);
    if (native_root != NULL && GTK_IS_WINDOW(native_root)) driver_window = GTK_WINDOW(native_root);
    else if (GTK_IS_WINDOW(driver->root)) driver_window = GTK_WINDOW(driver->root);

    /* An unparented fixture can search its explicit trees, never every other
     * window in the process. This keeps independent document hosts isolated. */
    top_levels = driver_window != NULL ? gtk_window_get_toplevels() : NULL;
    for (index = 0U; search.status == UMI_STATUS_OK && top_levels != NULL &&
         index < g_list_model_get_n_items(top_levels); ++index) {
        GtkWindow *window = g_list_model_get_item(top_levels, index);
        if (window == NULL) continue;
        if (automation_window_is_related(window, driver_window) &&
            (driver->observed_scope == NULL || window != driver_window)) {
            automation_find_widgets(GTK_WIDGET(window), target_id,
                window == driver_window ? driver->root : NULL, 0U, &search);
        }
        g_object_unref(window);
    }
    if (search.status == UMI_STATUS_OK && search.matches > 1U)
        search.status = UMI_STATUS_ALREADY_EXISTS;
    if (search.status != UMI_STATUS_OK) { g_clear_object(&search.found); return search.status; }
    if (search.found == NULL) return UMI_STATUS_NOT_FOUND;
    *out_widget = search.found;
    return UMI_STATUS_OK;
}

/* Copy only bounded, explicitly requested test text. Private fields never
 * expose values. TextView covers the source editor as well as single-line UI. */
static UmiStatus automation_widget_text(GtkWidget *widget, char *out_text, size_t capacity)
{
    const char *text = "";
    char *owned = NULL;
    UmiStatus status;
    out_text[0] = '\0';
    if (UmiGtk4RecordingIsPrivate(widget)) return UMI_STATUS_PERMISSION_DENIED;
    if (GTK_IS_TEXT_VIEW(widget)) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
        GtkTextIter start, end;
        if ((size_t)gtk_text_buffer_get_char_count(buffer) >= capacity)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        owned = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
        text = owned;
    } else if (GTK_IS_EDITABLE(widget)) text = gtk_editable_get_text(GTK_EDITABLE(widget));
    else if (GTK_IS_LABEL(widget)) text = gtk_label_get_text(GTK_LABEL(widget));
    else if (GTK_IS_BUTTON(widget)) text = gtk_button_get_label(GTK_BUTTON(widget));
    else if (GTK_IS_CHECK_BUTTON(widget)) text = gtk_check_button_get_label(GTK_CHECK_BUTTON(widget));
    else if (GTK_IS_MENU_BUTTON(widget)) text = gtk_menu_button_get_label(GTK_MENU_BUTTON(widget));
    if (text == NULL) text = "";
    if (!g_utf8_validate(text, -1, NULL)) status = UMI_STATUS_PARSE_ERROR;
    else status = automation_copy_text(out_text, capacity, text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    if (status != UMI_STATUS_OK) out_text[0] = '\0';
    g_free(owned);
    return status;
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
    /* Oversized/private values are omitted from metadata observations. An
     * explicit ASSERT_TEXT handles the read error separately below. */
    (void)automation_widget_text(widget, out_observation->text, sizeof(out_observation->text));
    out_observation->visible = gtk_widget_is_visible(widget) && gtk_widget_get_mapped(widget);
    out_observation->enabled = gtk_widget_is_sensitive(widget) != FALSE;
    out_observation->focused = gtk_widget_has_focus(widget) != FALSE;

    /* Toggle state and drop-down selection both represent a selected control. */
    if (GTK_IS_TOGGLE_BUTTON(widget)) {
        out_observation->selected =
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) != FALSE;
    } else if (GTK_IS_CHECK_BUTTON(widget)) {
        out_observation->selected = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget)) != FALSE;
    } else if (GTK_IS_SWITCH(widget)) {
        out_observation->selected = gtk_switch_get_active(GTK_SWITCH(widget)) != FALSE;
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
        return gtk_widget_is_visible(widget) && gtk_widget_get_mapped(widget);
    }
    return gtk_widget_is_sensitive(widget) != FALSE;
}

/*
 * Process pending GTK work while waiting for a visible or enabled state. This
 * loop has a caller-supplied deadline and therefore cannot wait forever.
 */
static UmiStatus automation_wait(UmiGtk4AutomationDriver *driver,
    const UmiUiAutomationStep *step, GtkWidget **out_widget)
{
    const gint64 started = g_get_monotonic_time();
    const gint64 timeout_us = (gint64)step->timeout_ms * 1000;
    *out_widget = NULL;
    for (;;) {
        GtkWidget *widget = NULL;
        UmiStatus status = automation_resolve_widget(driver, step->target_id, &widget);
        if (status == UMI_STATUS_OK) {
            if (automation_wait_condition(widget, step->operation)) {
                *out_widget = widget;
                return UMI_STATUS_OK;
            }
            g_object_unref(widget);
        } else if (status != UMI_STATUS_NOT_FOUND) return status;
        if (g_get_monotonic_time() - started >= timeout_us) return UMI_STATUS_TIMEOUT;
        /* A recurring idle source must not trap the driver in an unbounded
         * inner loop. Re-resolve after pumping: callbacks can remove a widget
         * or create the dialog which this step is waiting to observe. */
        for (unsigned work = 0U; work < 16U && g_main_context_pending(NULL); ++work) {
            if (g_get_monotonic_time() - started >= timeout_us) return UMI_STATUS_TIMEOUT;
            (void)g_main_context_iteration(NULL, FALSE);
        }
        g_usleep(1000U);
    }
}

/* Parse an explicit true, false or toggle request for a GTK toggle button. */
static UmiStatus automation_toggle(GtkWidget *widget, const char *value)
{
    gboolean active;

    if (GTK_IS_TOGGLE_BUTTON(widget)) active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    else if (GTK_IS_CHECK_BUTTON(widget)) active = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget));
    else if (GTK_IS_SWITCH(widget)) active = gtk_switch_get_active(GTK_SWITCH(widget));
    else return UMI_STATUS_INVALID_ARGUMENT;
    if (value == NULL || value[0] == '\0' || strcmp(value, "toggle") == 0) {
        active = !active;
    } else if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
        active = TRUE;
    } else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0) {
        active = FALSE;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (GTK_IS_TOGGLE_BUTTON(widget)) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), active);
    else if (GTK_IS_CHECK_BUTTON(widget)) gtk_check_button_set_active(GTK_CHECK_BUTTON(widget), active);
    else gtk_switch_set_active(GTK_SWITCH(widget), active);
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
    UmiGtk4AutomationDriver *driver = context;
    UmiUiAutomationStep checked;
    GtkWidget *widget = NULL;
    UmiStatus status;
    const char *message = "Control state observed.";
    if (out_observation != NULL) memset(out_observation, 0, sizeof(*out_observation));
    if (out_message != NULL && message_capacity > 0U) out_message[0] = '\0';
    if (driver == NULL || driver->root == NULL || step == NULL ||
        out_observation == NULL || out_message == NULL || message_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    checked = *step;
    /* Older direct adapter callers did not use a scenario step ID. Keep those
     * calls valid; stored scenarios still require the caller's real step ID. */
    if (checked.step_id[0] == '\0')
        (void)automation_copy_text(checked.step_id, sizeof(checked.step_id), "direct-adapter-call");
    status = UmiUiAutomationStepValidate(&checked);
    if (status != UMI_STATUS_OK) {
        (void)automation_copy_text(out_message, message_capacity, "Invalid automation step.");
        return status;
    }
    if (step->operation == UMI_UI_AUTOMATION_WAIT_VISIBLE ||
        step->operation == UMI_UI_AUTOMATION_WAIT_ENABLED)
        status = automation_wait(driver, step, &widget);
    else status = automation_resolve_widget(driver, step->target_id, &widget);
    if (status != UMI_STATUS_OK) {
        const char *reason = status == UMI_STATUS_TIMEOUT ? "The control did not reach its required state before the deadline."
            : status == UMI_STATUS_ALREADY_EXISTS ? "More than one control has this test ID. Use an unambiguous window scope."
            : status == UMI_STATUS_NOT_FOUND ? "No control has this stable automation ID."
            : "The control scope could not be inspected completely.";
        (void)automation_copy_text(out_message, message_capacity, reason);
        return status;
    }
    /* The resolver owns a reference. Signals may remove the widget before
     * returning, so keep that reference through the post-action observation. */
    if (step->operation >= UMI_UI_AUTOMATION_FOCUS &&
        step->operation <= UMI_UI_AUTOMATION_INVOKE_COMMAND &&
        !gtk_widget_is_sensitive(widget)) {
        status = UMI_STATUS_UNAVAILABLE;
        message = "The control or one of its parents is disabled.";
    } else switch (step->operation) {
        case UMI_UI_AUTOMATION_FOCUS:
            status = gtk_widget_grab_focus(widget) ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
            message = "Keyboard focus was requested.";
            break;
        case UMI_UI_AUTOMATION_CLICK:
        case UMI_UI_AUTOMATION_INVOKE_COMMAND:
            status = gtk_widget_activate(widget) ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
            message = "GTK activation was requested; check the application outcome in the next step.";
            break;
        case UMI_UI_AUTOMATION_TYPE_TEXT:
            if (UmiGtk4RecordingIsPrivate(widget)) status = UMI_STATUS_PERMISSION_DENIED;
            else if (GTK_IS_TEXT_VIEW(widget)) {
                if (!gtk_text_view_get_editable(GTK_TEXT_VIEW(widget))) status = UMI_STATUS_PERMISSION_DENIED;
                else gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), step->value, -1);
            } else if (GTK_IS_EDITABLE(widget)) {
                if (!gtk_editable_get_editable(GTK_EDITABLE(widget))) status = UMI_STATUS_PERMISSION_DENIED;
                else gtk_editable_set_text(GTK_EDITABLE(widget), step->value);
            } else status = UMI_STATUS_INVALID_ARGUMENT;
            message = "Test text was assigned through the widget API; this is not operating-system typing.";
            break;
        case UMI_UI_AUTOMATION_SELECT:
            status = automation_select(widget, step->value);
            message = "Drop-down selection was requested.";
            break;
        case UMI_UI_AUTOMATION_TOGGLE:
            status = automation_toggle(widget, step->value);
            message = "Toggle state was requested.";
            break;
        case UMI_UI_AUTOMATION_OPEN_MENU:
            if (!GTK_IS_MENU_BUTTON(widget)) status = UMI_STATUS_INVALID_ARGUMENT;
            else gtk_menu_button_popup(GTK_MENU_BUTTON(widget));
            message = "Menu popup was requested.";
            break;
        case UMI_UI_AUTOMATION_WAIT_VISIBLE:
        case UMI_UI_AUTOMATION_WAIT_ENABLED:
            message = "The control reached the required state.";
            break;
        case UMI_UI_AUTOMATION_ASSERT_TEXT:
            status = automation_widget_text(widget, out_observation->text, sizeof(out_observation->text));
            break;
        case UMI_UI_AUTOMATION_ASSERT_VISIBLE:
        case UMI_UI_AUTOMATION_ASSERT_ENABLED:
        case UMI_UI_AUTOMATION_ASSERT_FOCUSED:
        case UMI_UI_AUTOMATION_ASSERT_SELECTED:
        case UMI_UI_AUTOMATION_CAPTURE_EVIDENCE:
            break;
        default: status = UMI_STATUS_NOT_IMPLEMENTED; break;
    }
    automation_observe(driver, widget, step->target_id, out_observation);
    if (status == UMI_STATUS_OK) status = UmiUiAutomationObservationCheck(&checked, out_observation);
    if (status == UMI_STATUS_OK) (void)automation_copy_text(out_message, message_capacity, message);
    else if (strcmp(message, "The control or one of its parents is disabled.") == 0)
        (void)automation_copy_text(out_message, message_capacity, message);
    else (void)snprintf(out_message, message_capacity, "Control operation failed: %s.", umi_status_text(status));
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

/* Observe one separate non-window scope without replacing the primary root.
 * Adding is transactional and additive; an existing scope is never dropped. */
UmiStatus umi_gtk4_automation_driver_add_observed_scope(
    UmiGtk4AutomationDriver *driver, void *native_scope)
{
    GtkWidget *scope = native_scope;
    GtkRoot *primary_window;
    GtkRoot *scope_window;
    if (driver == NULL || driver->root == NULL || scope == NULL ||
        !GTK_IS_WIDGET(scope) || GTK_IS_WINDOW(scope)) return UMI_STATUS_INVALID_ARGUMENT;
    if (driver->observed_scope != NULL || scope == driver->root)
        return UMI_STATUS_ALREADY_EXISTS;
    if (gtk_widget_is_ancestor(scope, driver->root) ||
        gtk_widget_is_ancestor(driver->root, scope)) return UMI_STATUS_INVALID_ARGUMENT;
    primary_window = gtk_widget_get_root(driver->root);
    scope_window = gtk_widget_get_root(scope);
    if (primary_window != NULL && scope_window != NULL && primary_window != scope_window)
        return UMI_STATUS_INVALID_ARGUMENT;
    driver->observed_scope = g_object_ref(scope);
    return UMI_STATUS_OK;
}

/* Release the retained GTK root before freeing the small driver object. */
void umi_gtk4_automation_driver_destroy(UmiGtk4AutomationDriver *driver)
{
    if (driver == NULL) return;

    if (driver->observed_scope != NULL) g_object_unref(driver->observed_scope);
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
