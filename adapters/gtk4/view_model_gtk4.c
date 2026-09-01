/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/view_model_gtk4.c
 *
 * PURPOSE:
 *   Render a registered toolkit-neutral UmiUiViewModel presentation into GTK4
 *   without exposing GTK objects through the public Framework UI contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The product supplies data through a Framework view factory. This GTK adapter
 * only decides how to draw that data. A future Qt or Wt adapter can render the
 * exact same UmiUiViewPresentation without changing Studio business logic.
 */

#include "gtk4_internal.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"
#include "umicom/ui/view_presentation.h"

typedef struct UmiGtk4CommandViewBinding {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
} UmiGtk4CommandViewBinding;

static void command_view_binding_free(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

static void command_view_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4CommandViewBinding *binding =
        (UmiGtk4CommandViewBinding *)user_data;
    (void)button;
    if (binding != NULL && binding->adapter != NULL) {
        umi_gtk4_dispatch_action(binding->adapter, binding->action_id);
    }
}

static GtkWidget *fallback_widget(const UmiUiPaneSnapshot *pane)
{
    GtkWidget *label = gtk_label_new(pane != NULL ? pane->view_type : "");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_widget_add_css_class(label, "dim-label");
    return label;
}

static int property_is(const UmiUiPropertySnapshot *property, const char *key)
{
    return property != NULL && key != NULL &&
           strcmp(property->key, key) == 0;
}

static GtkWidget *presentation_widget(
    UmiGtk4Adapter *adapter,
    const UmiUiViewPresentation *presentation,
    const UmiUiPaneSnapshot *pane)
{
    GtkWidget *box;
    GtkWidget *scroll;
    UmiUiPropertySnapshot title_property;
    UmiUiPropertySnapshot summary_property;
    UmiUiPropertySnapshot action_count_property;
    size_t index;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);

    if (umi_ui_view_presentation_find_property(
            presentation, "title", &title_property) == UMI_STATUS_OK &&
        title_property.value.kind == UMI_UI_VALUE_STRING) {
        GtkWidget *title = gtk_label_new(title_property.value.string_value);
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_widget_add_css_class(title, "heading");
        gtk_box_append(GTK_BOX(box), title);
    } else if (pane != NULL && pane->title[0] != '\0') {
        GtkWidget *title = gtk_label_new(pane->title);
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_widget_add_css_class(title, "heading");
        gtk_box_append(GTK_BOX(box), title);
    }

    if (umi_ui_view_presentation_find_property(
            presentation, "summary", &summary_property) == UMI_STATUS_OK &&
        summary_property.value.kind == UMI_UI_VALUE_STRING &&
        summary_property.value.string_value[0] != '\0') {
        GtkWidget *summary = gtk_label_new(summary_property.value.string_value);
        gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
        gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
        gtk_widget_add_css_class(summary, "dim-label");
        gtk_box_append(GTK_BOX(box), summary);
    }

    if (adapter != NULL &&
        umi_ui_view_presentation_find_property(
            presentation,
            UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY,
            &action_count_property) == UMI_STATUS_OK &&
        action_count_property.value.kind == UMI_UI_VALUE_INTEGER &&
        action_count_property.value.integer_value > 0) {
        GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        int64_t requested = action_count_property.value.integer_value;
        size_t action_count = (size_t)requested;
        if (action_count > UMI_UI_COMMAND_VIEW_ACTION_MAX) {
            action_count = UMI_UI_COMMAND_VIEW_ACTION_MAX;
        }
        for (index = 0U; index < action_count; ++index) {
            char id_key[UMI_UI_PROPERTY_KEY_CAPACITY];
            char label_key[UMI_UI_PROPERTY_KEY_CAPACITY];
            char tooltip_key[UMI_UI_PROPERTY_KEY_CAPACITY];
            char enabled_key[UMI_UI_PROPERTY_KEY_CAPACITY];
            UmiUiPropertySnapshot id_property;
            UmiUiPropertySnapshot label_property;
            UmiUiPropertySnapshot tooltip_property;
            UmiUiPropertySnapshot enabled_property;
            UmiGtk4CommandViewBinding *binding;
            GtkWidget *button;
            (void)snprintf(id_key, sizeof(id_key),
                           "command-view.action.%02zu.id", index);
            (void)snprintf(label_key, sizeof(label_key),
                           "command-view.action.%02zu.label", index);
            (void)snprintf(tooltip_key, sizeof(tooltip_key),
                           "command-view.action.%02zu.tooltip", index);
            (void)snprintf(enabled_key, sizeof(enabled_key),
                           "command-view.action.%02zu.enabled", index);
            if (umi_ui_view_presentation_find_property(
                    presentation, id_key, &id_property) != UMI_STATUS_OK ||
                umi_ui_view_presentation_find_property(
                    presentation, label_key, &label_property) != UMI_STATUS_OK ||
                id_property.value.kind != UMI_UI_VALUE_STRING ||
                label_property.value.kind != UMI_UI_VALUE_STRING) {
                continue;
            }
            button = gtk_button_new_with_label(label_property.value.string_value);
            if (umi_ui_view_presentation_find_property(
                    presentation, tooltip_key, &tooltip_property) == UMI_STATUS_OK &&
                tooltip_property.value.kind == UMI_UI_VALUE_STRING) {
                gtk_widget_set_tooltip_text(
                    button, tooltip_property.value.string_value);
            }
            if (umi_ui_view_presentation_find_property(
                    presentation, enabled_key, &enabled_property) == UMI_STATUS_OK &&
                enabled_property.value.kind == UMI_UI_VALUE_BOOLEAN) {
                gtk_widget_set_sensitive(button,
                                         enabled_property.value.boolean_value);
            }
            binding = g_new0(UmiGtk4CommandViewBinding, 1);
            if (binding == NULL) continue;
            binding->adapter = adapter;
            (void)g_strlcpy(binding->action_id,
                            id_property.value.string_value,
                            sizeof(binding->action_id));
            g_signal_connect_data(button,
                                  "clicked",
                                  G_CALLBACK(command_view_clicked),
                                  binding,
                                  command_view_binding_free,
                                  0);
            gtk_box_append(GTK_BOX(actions), button);
        }
        gtk_box_append(GTK_BOX(box), actions);
    }

    for (index = 0U; index < presentation->property_count; ++index) {
        const UmiUiPropertySnapshot *property =
            &presentation->properties[index];
        char value_text[UMI_UI_VALUE_STRING_CAPACITY + 64U];
        GtkWidget *row;
        GtkWidget *name;
        GtkWidget *value;

        if (property_is(property, "title") ||
            property_is(property, "summary") ||
            umi_ui_command_view_property_is_reserved(property->key)) {
            continue;
        }

        if (umi_ui_view_presentation_value_text(
                &property->value, value_text, sizeof(value_text)) !=
            UMI_STATUS_OK) {
            continue;
        }

        row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        name = gtk_label_new(property->key);
        value = gtk_label_new(value_text);

        gtk_label_set_xalign(GTK_LABEL(name), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(value), 1.0F);
        gtk_label_set_wrap(GTK_LABEL(value), TRUE);
        gtk_widget_set_hexpand(value, TRUE);
        gtk_widget_add_css_class(name, "dim-label");

        gtk_box_append(GTK_BOX(row), name);
        gtk_box_append(GTK_BOX(row), value);
        gtk_box_append(GTK_BOX(box), row);
    }

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    return scroll;
}

UmiStatus umi_gtk4_build_view_widget(
    UmiGtk4Adapter *adapter,
    UmiUiWorkbench *workbench,
    const UmiUiPaneSnapshot *pane,
    GtkWidget **out_widget)
{
    UmiUiViewPresentation presentation;
    UmiStatus status;

    if (adapter == NULL || workbench == NULL || pane == NULL ||
        out_widget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_widget = NULL;
    status = umi_ui_view_presentation_build(
        umi_ui_workbench_view_factories(workbench),
        pane->view_type,
        pane->pane_id,
        &presentation);

    if (status == UMI_STATUS_NOT_FOUND) {
        *out_widget = fallback_widget(pane);
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;

    {
        UmiUiPropertySnapshot kind;
        if (umi_ui_view_presentation_find_property(
                &presentation, "umicom.view-kind", &kind) == UMI_STATUS_OK &&
            kind.value.kind == UMI_UI_VALUE_STRING) {
            if (strcmp(kind.value.string_value, "problems") == 0) {
                *out_widget = umi_gtk4_problems_widget(&presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
            if (strcmp(kind.value.string_value, "output") == 0) {
                *out_widget = umi_gtk4_output_widget(&presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
            if (strcmp(kind.value.string_value, "terminal") == 0 ||
                strcmp(kind.value.string_value, "terminal-history") == 0) {
                *out_widget = umi_gtk4_terminal_widget(adapter, &presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
            if (strcmp(kind.value.string_value, "processes") == 0) {
                *out_widget = umi_gtk4_process_widget(adapter, &presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
            if (strcmp(kind.value.string_value, "tasks") == 0) {
                *out_widget = umi_gtk4_task_widget(adapter, &presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
            if (strcmp(kind.value.string_value, "source-control") == 0 ||
                strcmp(kind.value.string_value, "vcs-history") == 0 ||
                strcmp(kind.value.string_value, "vcs-branches") == 0 ||
                strcmp(kind.value.string_value, "vcs-remotes") == 0 ||
                strcmp(kind.value.string_value, "vcs-diff") == 0) {
                *out_widget = umi_gtk4_source_control_widget(adapter, &presentation);
                return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
            }
        }
    }

    *out_widget = presentation_widget(adapter, &presentation, pane);
    return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
