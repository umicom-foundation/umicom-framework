/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/application_suite_workstation_gtk4.c
 *
 * PURPOSE:
 *   Connect canonical Application Suite layout runtime and selector metadata to
 *   the reusable GTK4 workspace-layout host for all thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_workstation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiApplicationSuiteGtk4Workstation {
    UmiApplicationSuiteLayoutRuntime runtime;
    UmiApplicationSuiteLayoutSelectorModel selector;
    UmiGtk4WorkspaceLayoutHost *host;
    GtkWidget *root;
    GtkWidget *title_label;
    GtkWidget *layout_dropdown;
    int changing_selection;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static void refresh_heading(UmiApplicationSuiteGtk4Workstation *workstation)
{
    const UmiApplicationSuiteLayoutChoice *choice =
        umi_application_suite_layout_selector_current(&workstation->selector);
    if (choice != NULL)
        gtk_label_set_text(GTK_LABEL(workstation->title_label), choice->title);
}

UmiStatus umi_application_suite_gtk4_workstation_select_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id)
{
    UmiStatus status;
    size_t index;
    if (workstation == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_layout_runtime_select(
        &workstation->runtime, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_suite_layout_selector_select(
        &workstation->selector, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_gtk4_workspace_layout_host_rebuild(
        workstation->host,
        umi_application_suite_layout_runtime_active(&workstation->runtime));
    if (status != UMI_STATUS_OK) return status;
    workstation->changing_selection = 1;
    for (index = 0U; index < workstation->selector.count; ++index) {
        if (workstation->selector.choices[index].selected) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown),
                                       (guint)index);
            break;
        }
    }
    workstation->changing_selection = 0;
    refresh_heading(workstation);
    workstation->revision += 1U;
    return UMI_STATUS_OK;
}

static void on_layout_selected(GObject *object,
                               GParamSpec *pspec,
                               gpointer data)
{
    UmiApplicationSuiteGtk4Workstation *workstation =
        (UmiApplicationSuiteGtk4Workstation *)data;
    guint selected;
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)pspec;
    if (workstation == NULL || workstation->changing_selection) return;
    selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    choice = umi_application_suite_layout_selector_at(
        &workstation->selector, (size_t)selected);
    if (choice != NULL)
        (void)umi_application_suite_gtk4_workstation_select_layout(
            workstation, choice->layout_id);
}

UmiStatus umi_application_suite_gtk4_workstation_create(
    const UmiApplicationSuiteGtk4WorkstationConfig *config,
    UmiApplicationSuiteGtk4Workstation **out_workstation)
{
    UmiApplicationSuiteGtk4Workstation *workstation;
    const UmiUiWorkspaceLayout *layout;
    GtkStringList *choices;
    GtkWidget *header;
    GtkWidget *label;
    size_t index;
    UmiStatus status;
    if (config == NULL || out_workstation == NULL ||
        config->application_id == NULL || config->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_workstation = NULL;
    workstation = (UmiApplicationSuiteGtk4Workstation *)calloc(
        1U, sizeof(*workstation));
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_application_suite_layout_runtime_init(&workstation->runtime);
    status = umi_application_suite_layout_runtime_load(
        &workstation->runtime, config->application_id);
    if (status != UMI_STATUS_OK) goto fail;
    layout = umi_application_suite_layout_runtime_active(&workstation->runtime);
    if (layout == NULL) { status = UMI_STATUS_INVALID_STATE; goto fail; }
    status = umi_application_suite_layout_selector_build(
        workstation->runtime.experience, layout->layout_id, &workstation->selector);
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_gtk4_workspace_layout_host_create(
        layout, config->panel_factory, config->user_data, &workstation->host);
    if (status != UMI_STATUS_OK) goto fail;

    workstation->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(workstation->root);
    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    workstation->title_label = gtk_label_new(
        config->title != NULL ? config->title : layout->name);
    gtk_widget_add_css_class(header, "umicom-suite-layout-header");
    gtk_widget_add_css_class(workstation->title_label, "title-4");
    gtk_label_set_xalign(GTK_LABEL(workstation->title_label), 0.0F);
    gtk_widget_set_hexpand(workstation->title_label, TRUE);
    gtk_box_append(GTK_BOX(header), workstation->title_label);
    label = gtk_label_new("Layout");
    gtk_widget_add_css_class(label, "dim-label");
    gtk_box_append(GTK_BOX(header), label);

    choices = gtk_string_list_new(NULL);
    for (index = 0U; index < workstation->selector.count; ++index)
        gtk_string_list_append(choices, workstation->selector.choices[index].title);
    workstation->layout_dropdown = gtk_drop_down_new(G_LIST_MODEL(choices), NULL);
    g_object_unref(choices);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workstation->layout_dropdown),
                               (guint)workstation->selector.selected_index);
    g_signal_connect(workstation->layout_dropdown, "notify::selected",
                     G_CALLBACK(on_layout_selected), workstation);
    gtk_box_append(GTK_BOX(header), workstation->layout_dropdown);
    gtk_box_append(GTK_BOX(workstation->root), header);
    gtk_widget_set_hexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_widget_set_vexpand(umi_gtk4_workspace_layout_host_widget(workstation->host),
                           TRUE);
    gtk_box_append(GTK_BOX(workstation->root),
                   umi_gtk4_workspace_layout_host_widget(workstation->host));
    refresh_heading(workstation);
    workstation->revision = 1U;
    *out_workstation = workstation;
    return UMI_STATUS_OK;

fail:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    return status;
}

void umi_application_suite_gtk4_workstation_destroy(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    if (workstation == NULL) return;
    umi_gtk4_workspace_layout_host_destroy(workstation->host);
    workstation->host = NULL;
    if (workstation->root != NULL) g_object_unref(workstation->root);
    workstation->root = NULL;
    free(workstation);
}

GtkWidget *umi_application_suite_gtk4_workstation_widget(
    UmiApplicationSuiteGtk4Workstation *workstation)
{
    return workstation != NULL ? workstation->root : NULL;
}

UmiApplicationSuiteGtk4WorkstationSnapshot
umi_application_suite_gtk4_workstation_snapshot(
    const UmiApplicationSuiteGtk4Workstation *workstation)
{
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiGtk4WorkspaceLayoutHostSnapshot host_snapshot;
    const UmiApplicationSuiteLayoutChoice *choice;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (workstation == NULL) return snapshot;
    choice = umi_application_suite_layout_selector_current(&workstation->selector);
    host_snapshot = umi_gtk4_workspace_layout_host_snapshot(workstation->host);
    if (workstation->runtime.experience != NULL)
        (void)copy_text(snapshot.application_id, sizeof(snapshot.application_id),
                        workstation->runtime.experience->application_id);
    if (choice != NULL) {
        (void)copy_text(snapshot.active_layout_id,
                        sizeof(snapshot.active_layout_id), choice->layout_id);
        (void)copy_text(snapshot.active_layout_name,
                        sizeof(snapshot.active_layout_name), choice->title);
    }
    snapshot.layout_count = workstation->selector.count;
    snapshot.rendered_panel_count = host_snapshot.panel_count;
    snapshot.placeholder_count = host_snapshot.placeholder_count;
    snapshot.revision = workstation->revision + host_snapshot.revision;
    return snapshot;
}
