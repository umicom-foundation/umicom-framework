/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/application_product_workstation_gtk4.c
 *
 * PURPOSE:
 *   Render live application presentation state inside the shared customisable
 *   GTK4 suite workstation so thin products need no copied shell or panel code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_product_workstation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/ui/command_view.h"
#include "umicom/ui/gtk4/workstation/view_model_panel.h"
#include "umicom/ui/value.h"
#include "umicom/ui/view_model.h"

typedef struct ProductPanelWidgetData {
    UmiApplicationProductGtk4Workstation *workstation;
    UmiUiViewModel *view;
    char component_id[UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY];
} ProductPanelWidgetData;

struct UmiApplicationProductGtk4Workstation {
    UmiApplicationPresentationProductSurface product;
    UmiApplicationSuiteGtk4Workstation *layout;
    const UmiApplicationExperienceDefinition *experience;
    guint pending_refresh_id;
    uint64_t revision;
};

/* Each GTK panel owns its toolkit-neutral view. Destroying a layout therefore
 * releases old projections before a refreshed set of widgets is constructed. */
static void product_panel_widget_data_destroy(gpointer data)
{
    ProductPanelWidgetData *panel_data =
        (ProductPanelWidgetData *)data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel_data == NULL) return;
    umi_ui_view_model_destroy(panel_data->view);
    panel_data->view = NULL;
    free(panel_data);
}

/* Store a string through the normal owned property value contract. */
static UmiStatus set_string_property(
    UmiUiViewModel *view,
    const char *key,
    const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/* Store a number without formatting it into text so every frontend may choose
 * presentation suitable for its platform and accessibility settings. */
static UmiStatus set_integer_property(
    UmiUiViewModel *view,
    const char *key,
    int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/* Boolean state remains typed, allowing future web and mobile hosts to render
 * toggles or status indicators without parsing English words. */
static UmiStatus set_boolean_property(
    UmiUiViewModel *view,
    const char *key,
    int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

/* Build one portable view from the projection. User-facing state remains in
 * the normal panel body while diagnostic facts use a reserved technical
 * namespace which native renderers place behind an explicit disclosure. */
static UmiStatus create_product_panel_view(
    const UmiApplicationProductPanelProjection *projection,
    UmiUiViewModel **out_view)
{
    UmiUiViewModel *view = NULL;
    UmiUiCommandViewAction action;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_view = NULL;
    status = umi_ui_view_model_create(
        projection->panel_id, "product-panel", UMI_UI_ROLE_PANE, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(view, "title", projection->title);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(view, "summary", projection->summary);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(
            view, "umicom.view-kind", "product-panel");
    }
    /* Technical identity remains available for diagnostics without occupying
     * the ordinary product experience. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(
            view, "umicom.technical.Component", projection->component_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(
            view, "umicom.technical.Capability",
            projection->required_capability);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(
            view, "State",
            umi_application_presentation_surface_state_text(
                projection->state));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string_property(view, "Message", projection->message);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && projection->badge[0] != '\0') {
        status = set_string_property(view, "Badge", projection->badge);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_boolean_property(
            view, "umicom.technical.Runtime connected",
            projection->component_bound);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_boolean_property(
            view, "umicom.technical.Focused", projection->focused);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_boolean_property(
            view, "umicom.technical.Unsaved changes", projection->dirty);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && projection->has_progress) {
        status = set_integer_property(
            view, "Progress percent", (int64_t)projection->progress_percent);
    }

    /* Commands are optional. When present, the renderer creates a normal
     * button and passes only the stable command identifier back to this host. */
    if (status == UMI_STATUS_OK && projection->command_id[0] != '\0') {
        (void)memset(&action, 0, sizeof(action));
        (void)snprintf(action.action_id, sizeof(action.action_id), "%s",
                       projection->command_id);
        (void)snprintf(action.label, sizeof(action.label), "%s",
                       projection->command_label);
        (void)snprintf(action.tooltip, sizeof(action.tooltip),
                       "Run %s through the product controller",
                       projection->command_label);
        action.enabled = projection->command_enabled;
        status = umi_ui_command_view_set_action(view, 0U, &action);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(view);
        return status;
    }
    *out_view = view;
    return UMI_STATUS_OK;
}

/* Refreshing from an idle callback avoids destroying the button hierarchy in
 * the middle of its own click signal. Only one refresh may wait at a time. */
static gboolean refresh_from_idle(gpointer data)
{
    UmiApplicationProductGtk4Workstation *workstation =
        (UmiApplicationProductGtk4Workstation *)data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return G_SOURCE_REMOVE;
    workstation->pending_refresh_id = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_suite_gtk4_workstation_refresh(
            workstation->layout) == UMI_STATUS_OK) {
        workstation->revision += 1U;
    }
    return G_SOURCE_REMOVE;
}

/* Route the command to the exact component represented by this panel. Product
 * controllers decide whether the command executes, stages approval or fails. */
static UmiStatus on_product_panel_action(
    const char *action_id,
    void *user_data)
{
    ProductPanelWidgetData *panel_data =
        (ProductPanelWidgetData *)user_data;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel_data == NULL || panel_data->workstation == NULL ||
        panel_data->component_id[0] == '\0' || action_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_presentation_product_surface_command(
        &panel_data->workstation->product,
        panel_data->component_id,
        action_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        panel_data->workstation->pending_refresh_id == 0U) {
        panel_data->workstation->pending_refresh_id = g_idle_add(
            refresh_from_idle, panel_data->workstation);
    }
    return status;
}

/* The suite layout supplies a stable tool identifier for each window. This
 * factory projects current product state and renders it with the shared panel. */
static GtkWidget *create_panel_widget(
    const UmiUiWorkspaceWindow *window,
    void *user_data)
{
    UmiApplicationProductGtk4Workstation *workstation =
        (UmiApplicationProductGtk4Workstation *)user_data;
    UmiApplicationProductPanelProjection projection;
    ProductPanelWidgetData *panel_data;
    GtkWidget *widget;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || workstation == NULL) return NULL;
    status = umi_application_product_panel_project(
        workstation->experience, &workstation->product.runtime,
        window->tool_id, &projection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return NULL;

    panel_data = (ProductPanelWidgetData *)calloc(1U, sizeof(*panel_data));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel_data == NULL) return NULL;
    panel_data->workstation = workstation;
    (void)snprintf(panel_data->component_id,
                   sizeof(panel_data->component_id), "%s",
                   projection.component_id);
    status = create_product_panel_view(&projection, &panel_data->view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        product_panel_widget_data_destroy(panel_data);
        return NULL;
    }
    widget = umi_gtk4_view_model_panel_create(
        panel_data->view, on_product_panel_action, panel_data);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (widget == NULL) {
        product_panel_widget_data_destroy(panel_data);
        return NULL;
    }
    g_object_set_data_full(
        G_OBJECT(widget), "umicom-product-panel-data", panel_data,
        product_panel_widget_data_destroy);
    return widget;
}

/* Provide a complete standard configuration so thin products only replace
 * values that are genuinely different for their domain. */
UmiApplicationProductGtk4WorkstationConfig
umi_application_product_gtk4_workstation_config_default(
    const char *application_id,
    const char *title,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context)
{
    UmiApplicationProductGtk4WorkstationConfig config;
    config.application_id = application_id;
    config.title = title;
    config.mode_badge = "Ready";
    config.audience = UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD;
    config.register_controllers = register_controllers;
    config.controller_context = controller_context;
    return config;
}

/* Construct the toolkit-neutral product surface before creating any widgets,
 * ensuring the first visible panel already has honest controller state. */
UmiStatus umi_application_product_gtk4_workstation_create(
    const UmiApplicationProductGtk4WorkstationConfig *config,
    UmiApplicationProductGtk4Workstation **out_workstation)
{
    UmiApplicationProductGtk4Workstation *workstation;
    UmiApplicationSuiteGtk4WorkstationConfig layout_config;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_workstation == NULL ||
        config->application_id == NULL || config->title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workstation = NULL;
    workstation = (UmiApplicationProductGtk4Workstation *)calloc(
        1U, sizeof(*workstation));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    workstation->experience = umi_application_experience_catalogue_find(
        config->application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation->experience == NULL) {
        status = UMI_STATUS_NOT_FOUND;
        goto fail;
    }
    status = umi_application_presentation_product_surface_init_for_audience(
        config->application_id, config->audience,
        config->register_controllers, config->controller_context,
        &workstation->product);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;

    /* The shared suite workstation owns menus, identity, appearance, layout
     * selection, docking and customisation. This host only supplies panels. */
    layout_config.application_id = config->application_id;
    layout_config.title = config->title;
    layout_config.mode_badge = config->mode_badge;
    layout_config.panel_factory = create_panel_widget;
    layout_config.user_data = workstation;
    status = umi_application_suite_gtk4_workstation_create(
        &layout_config, &workstation->layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    workstation->revision = 1U;
    *out_workstation = workstation;
    return UMI_STATUS_OK;

fail:
    umi_application_product_gtk4_workstation_destroy(workstation);
    return status;
}

/* Remove deferred work, then release widgets before the state they reference. */
void umi_application_product_gtk4_workstation_destroy(
    UmiApplicationProductGtk4Workstation *workstation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workstation->pending_refresh_id != 0U) {
        (void)g_source_remove(workstation->pending_refresh_id);
        workstation->pending_refresh_id = 0U;
    }
    umi_application_suite_gtk4_workstation_destroy(workstation->layout);
    workstation->layout = NULL;
    umi_application_presentation_product_surface_dispose(
        &workstation->product);
    workstation->experience = NULL;
    free(workstation);
}

/* Return a borrowed root so the owning native window cannot double-free it. */
GtkWidget *umi_application_product_gtk4_workstation_widget(
    UmiApplicationProductGtk4Workstation *workstation)
{
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_widget(workstation->layout)
        : NULL;
}

/* Forward layout selection and record an observable product revision. */
UmiStatus umi_application_product_gtk4_workstation_select_layout(
    UmiApplicationProductGtk4Workstation *workstation,
    const char *layout_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_gtk4_workstation_select_layout(
        workstation->layout, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Apply a Framework profile rather than introducing product-specific CSS. */
UmiStatus umi_application_product_gtk4_workstation_select_appearance(
    UmiApplicationProductGtk4Workstation *workstation,
    const char *profile_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_gtk4_workstation_select_appearance(
        workstation->layout, profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Refresh model state first and rebuild widgets only after controllers finish. */
UmiStatus umi_application_product_gtk4_workstation_refresh(
    UmiApplicationProductGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_presentation_product_surface_refresh(
        &workstation->product);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_suite_gtk4_workstation_refresh(
            workstation->layout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Start the existing reversible Framework layout transaction. */
UmiStatus umi_application_product_gtk4_workstation_begin_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation)
{
    return workstation != NULL
        ? umi_application_suite_gtk4_workstation_begin_layout_edit(
              workstation->layout)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Commit a valid layout transaction and publish its new revision. */
UmiStatus umi_application_product_gtk4_workstation_commit_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_gtk4_workstation_commit_layout_edit(
        workstation->layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Roll back an unfinished layout transaction without touching product data. */
UmiStatus umi_application_product_gtk4_workstation_cancel_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_gtk4_workstation_cancel_layout_edit(
        workstation->layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workstation->revision += 1U;
    return status;
}

/* Join independent layout, surface and coverage snapshots into one report. */
UmiStatus umi_application_product_gtk4_workstation_snapshot(
    const UmiApplicationProductGtk4Workstation *workstation,
    UmiApplicationProductGtk4WorkstationSnapshot *out_snapshot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workstation == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_APPLICATION_PRODUCT_GTK4_WORKSTATION_API_VERSION;
    out_snapshot->layout = umi_application_suite_gtk4_workstation_snapshot(
        workstation->layout);
    status = umi_application_presentation_product_surface_snapshot(
        &workstation->product, &out_snapshot->surface);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_product_panel_coverage(
            workstation->experience, &workstation->product.runtime,
            &out_snapshot->coverage);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        out_snapshot->revision = workstation->revision;
    }
    return status;
}
