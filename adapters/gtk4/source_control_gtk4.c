/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/source_control_gtk4.c
 *
 * PURPOSE:
 *   Render Framework Source Control Centre presentations and tag selectable
 *   rows for reusable native interaction capture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"
#include <stdio.h>
#include <string.h>
#include "umicom/ui/command_view.h"

typedef struct UmiGtk4VcsBinding {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
} UmiGtk4VcsBinding;

/* Provide the vcs binding free operation used by this module and its client applications. */
static void vcs_binding_free(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/*
 * Provide the vcs action clicked operation used by this module and its client
 * applications.
 */
static void vcs_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4VcsBinding *binding =
        (UmiGtk4VcsBinding *)user_data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding != NULL && binding->adapter != NULL) {
        umi_gtk4_dispatch_action(
            binding->adapter,
            binding->action_id);
    }
}

/* Provide the property string operation used by this module and its client applications. */
static const char *property_string(
    const UmiUiViewPresentation *presentation,
    const char *key)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presentation == NULL || key == NULL) return "";
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < presentation->property_count;
         ++index) {
        const UmiUiPropertySnapshot *property =
            &presentation->properties[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(property->key, key) == 0 &&
            property->value.kind == UMI_UI_VALUE_STRING) {
            return property->value.string_value;
        }
    }
    return "";
}

/* Return the number of records represented by property without changing their state. */
static size_t property_count(
    const UmiUiViewPresentation *presentation,
    const char *key)
{
    UmiUiPropertySnapshot property;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_view_presentation_find_property(
            presentation, key, &property) != UMI_STATUS_OK ||
        property.value.kind != UMI_UI_VALUE_INTEGER ||
        property.value.integer_value <= 0) {
        return 0U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((uint64_t)property.value.integer_value >
        (uint64_t)SIZE_MAX) {
        return SIZE_MAX;
    }
    return (size_t)property.value.integer_value;
}

/* Provide the append heading operation used by this module and its client applications. */
static void append_heading(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_widget_add_css_class(label, "heading");
    gtk_box_append(GTK_BOX(box), label);
}

/* Provide the append rows operation used by this module and its client applications. */
static void append_rows(
    GtkWidget *box,
    const UmiUiViewPresentation *presentation,
    const char *view_kind,
    const char *prefix,
    size_t count)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        char key[96];
        const char *text;
        GtkWidget *label;

        (void)snprintf(
            key,
            sizeof(key),
            "%s.row.%zu",
            prefix,
            index);
        text = property_string(presentation, key);
        /* Apply this branch only when its contract condition is satisfied. */
        if (text[0] == '\0') continue;

        label = gtk_label_new(text);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_selectable(GTK_LABEL(label), TRUE);
        gtk_label_set_ellipsize(
            GTK_LABEL(label),
            PANGO_ELLIPSIZE_MIDDLE);
        gtk_widget_add_css_class(label, "monospace");

        /*
         * A source-control row remains presentation-only.  The generic GTK4
         * interaction binder owns the native click gesture and forwards only
         * stable text/view-kind data to the configured context sink.
         */
        umi_gtk4_context_interaction_tag_source_control_row(
            label,
            view_kind,
            text);

        gtk_box_append(GTK_BOX(box), label);
    }
}

/* Provide the append actions operation used by this module and its client applications. */
static void append_actions(
    GtkWidget *box,
    UmiGtk4Adapter *adapter,
    const UmiUiViewPresentation *presentation)
{
    UmiUiPropertySnapshot count_property;
    size_t index;
    size_t count;
    GtkWidget *actions;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_view_presentation_find_property(
            presentation,
            UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY,
            &count_property) != UMI_STATUS_OK ||
        count_property.value.kind != UMI_UI_VALUE_INTEGER ||
        count_property.value.integer_value <= 0) {
        return;
    }

    count = (size_t)count_property.value.integer_value;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_UI_COMMAND_VIEW_ACTION_MAX) {
        count = UMI_UI_COMMAND_VIEW_ACTION_MAX;
    }

    actions = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(
        GTK_FLOW_BOX(actions),
        GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(
        GTK_FLOW_BOX(actions),
        4);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        char id_key[96];
        char label_key[96];
        char enabled_key[96];
        const char *id;
        const char *label;
        UmiUiPropertySnapshot enabled;
        GtkWidget *button;
        UmiGtk4VcsBinding *binding;

        (void)snprintf(
            id_key,
            sizeof(id_key),
            "command-view.action.%02zu.id",
            index);
        (void)snprintf(
            label_key,
            sizeof(label_key),
            "command-view.action.%02zu.label",
            index);
        (void)snprintf(
            enabled_key,
            sizeof(enabled_key),
            "command-view.action.%02zu.enabled",
            index);

        id = property_string(presentation, id_key);
        label = property_string(presentation, label_key);
        /* Apply this branch only when its contract condition is satisfied. */
        if (id[0] == '\0' || label[0] == '\0') continue;

        button = gtk_button_new_with_label(label);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_view_presentation_find_property(
                presentation,
                enabled_key,
                &enabled) == UMI_STATUS_OK &&
            enabled.value.kind == UMI_UI_VALUE_BOOLEAN) {
            gtk_widget_set_sensitive(
                button,
                enabled.value.boolean_value);
        }

        binding = g_new0(UmiGtk4VcsBinding, 1);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (binding == NULL) continue;
        binding->adapter = adapter;
        (void)g_strlcpy(
            binding->action_id,
            id,
            sizeof(binding->action_id));
        g_signal_connect_data(
            button,
            "clicked",
            G_CALLBACK(vcs_action_clicked),
            binding,
            vcs_binding_free,
            0);
        gtk_flow_box_append(
            GTK_FLOW_BOX(actions),
            button);
    }

    gtk_box_append(GTK_BOX(box), actions);
}

/*
 * Provide the gtk4 source control widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_source_control_widget(
    UmiGtk4Adapter *adapter,
    const UmiUiViewPresentation *presentation)
{
    GtkWidget *box;
    GtkWidget *scroll;
    const char *title;
    const char *summary;
    const char *kind;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || presentation == NULL) return NULL;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);

    title = property_string(presentation, "title");
    summary = property_string(presentation, "summary");
    kind = property_string(presentation, "umicom.view-kind");

    append_heading(box, title);
    /* Apply this branch only when its contract condition is satisfied. */
    if (summary[0] != '\0') {
        GtkWidget *label = gtk_label_new(summary);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_widget_add_css_class(label, "dim-label");
        gtk_box_append(GTK_BOX(box), label);
    }

    append_actions(box, adapter, presentation);

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(kind, "source-control") == 0) {
        append_rows(
            box,
            presentation,
            kind,
            "vcs",
            property_count(presentation, "vcs.row-count"));
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(kind, "vcs-history") == 0) {
        append_rows(
            box,
            presentation,
            kind,
            "vcs-history",
            property_count(presentation, "vcs-history.row-count"));
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(kind, "vcs-branches") == 0) {
        append_rows(
            box,
            presentation,
            kind,
            "vcs-branches",
            property_count(presentation, "vcs-branches.row-count"));
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(kind, "vcs-remotes") == 0) {
        append_heading(box, "Remote Endpoints");
        append_rows(
            box,
            presentation,
            kind,
            "vcs-remotes",
            property_count(presentation, "vcs-remotes.row-count"));
        append_heading(box, "Recent Operations");
        append_rows(
            box,
            presentation,
            kind,
            "vcs-operations",
            property_count(presentation, "vcs-operations.row-count"));
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(kind, "vcs-diff") == 0) {
        append_rows(
            box,
            presentation,
            kind,
            "vcs-diff",
            property_count(presentation, "vcs-diff.row-count"));
    }

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroll),
        box);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    return scroll;
}
