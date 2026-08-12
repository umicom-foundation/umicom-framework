/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/view_model_gtk4.c
 *
 * PURPOSE:
 *   Render a registered toolkit-neutral UmiUiViewModel presentation into GTK4
 *   without exposing GTK objects through the public Framework UI contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The product supplies data through a Framework view factory. This GTK adapter
 * only decides how to draw that data. A future Qt or Wt adapter can render the
 * exact same UmiUiViewPresentation without changing Studio business logic.
 */

#include "gtk4_internal.h"

#include <string.h>

#include "umicom/ui/view_presentation.h"

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
    const UmiUiViewPresentation *presentation,
    const UmiUiPaneSnapshot *pane)
{
    GtkWidget *box;
    GtkWidget *scroll;
    UmiUiPropertySnapshot title_property;
    UmiUiPropertySnapshot summary_property;
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

    for (index = 0U; index < presentation->property_count; ++index) {
        const UmiUiPropertySnapshot *property =
            &presentation->properties[index];
        char value_text[UMI_UI_VALUE_STRING_CAPACITY + 64U];
        GtkWidget *row;
        GtkWidget *name;
        GtkWidget *value;

        if (property_is(property, "title") ||
            property_is(property, "summary")) {
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
    UmiUiWorkbench *workbench,
    const UmiUiPaneSnapshot *pane,
    GtkWidget **out_widget)
{
    UmiUiViewPresentation presentation;
    UmiStatus status;

    if (workbench == NULL || pane == NULL || out_widget == NULL) {
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

    *out_widget = presentation_widget(&presentation, pane);
    return *out_widget != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
