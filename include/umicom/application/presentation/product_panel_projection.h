/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/product_panel_projection.h
 *
 * PURPOSE:
 *   Join a named application layout panel to its reusable presentation
 *   component and expose an owned, toolkit-neutral snapshot for frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_PRODUCT_PANEL_PROJECTION_H
#define UMICOM_APPLICATION_PRESENTATION_PRODUCT_PANEL_PROJECTION_H

#include "umicom/application/experience.h"
#include "umicom/application/presentation/surface_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY 128U
#define UMI_APPLICATION_PRODUCT_PANEL_TITLE_CAPACITY 256U
#define UMI_APPLICATION_PRODUCT_PANEL_SUMMARY_CAPACITY 512U
#define UMI_APPLICATION_PRODUCT_PANEL_COMMAND_CAPACITY 128U
#define UMI_APPLICATION_PRODUCT_PANEL_API_VERSION 1U

/**
 * Connects one human-facing layout panel to one reusable runtime component.
 *
 * The catalogue stores immutable strings for the life of the process. A
 * frontend may borrow a returned binding but must never change or free it.
 */
typedef struct UmiApplicationProductPanelBinding {
    const char *application_id;
    const char *panel_id;
    const char *component_id;
    const char *command_id;
    const char *command_label;
} UmiApplicationProductPanelBinding;

/**
 * Owned product-panel state suitable for GTK, web, tests and future hosts.
 *
 * Every string is copied into this value. The caller can therefore keep the
 * projection after a runtime refresh without holding borrowed internal text.
 */
typedef struct UmiApplicationProductPanelProjection {
    uint32_t structure_size;
    uint32_t api_version;
    char application_id[UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY];
    char panel_id[UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY];
    char component_id[UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY];
    char title[UMI_APPLICATION_PRODUCT_PANEL_TITLE_CAPACITY];
    char summary[UMI_APPLICATION_PRODUCT_PANEL_SUMMARY_CAPACITY];
    char required_capability[UMI_APPLICATION_PRODUCT_PANEL_ID_CAPACITY];
    char message[UMI_APPLICATION_PRESENTATION_MESSAGE_CAPACITY];
    char badge[UMI_APPLICATION_PRESENTATION_BADGE_CAPACITY];
    char command_id[UMI_APPLICATION_PRODUCT_PANEL_COMMAND_CAPACITY];
    char command_label[UMI_APPLICATION_PRODUCT_PANEL_TITLE_CAPACITY];
    UmiApplicationPresentationSurfaceState state;
    uint32_t progress_percent;
    uint32_t panel_flags;
    uint64_t revision;
    int component_bound;
    int visible;
    int focused;
    int dirty;
    int has_progress;
    int command_enabled;
} UmiApplicationProductPanelProjection;

/** Summarises how much of one product experience can render live state. */
typedef struct UmiApplicationProductPanelCoverage {
    uint32_t structure_size;
    size_t panel_count;
    size_t bound_count;
    size_t actionable_count;
    size_t unbound_count;
} UmiApplicationProductPanelCoverage;

/** Return the number of explicit panel-to-component aliases. */
size_t umi_application_product_panel_binding_count(void);
/** Return one borrowed alias, or `NULL` when the index is outside the list. */
const UmiApplicationProductPanelBinding *
umi_application_product_panel_binding_at(size_t index);
/** Find an explicit alias by application and stable layout panel identifier. */
const UmiApplicationProductPanelBinding *
umi_application_product_panel_binding_find(
    const char *application_id,
    const char *panel_id);

/**
 * Project one layout panel from immutable metadata and current runtime state.
 *
 * Exact component suffixes are resolved automatically. The small alias
 * catalogue is used only where a friendly panel name differs from its shared
 * component name. An unbound panel still returns useful metadata and an honest
 * unavailable state, allowing frontends to explain what remains to implement.
 */
UmiStatus umi_application_product_panel_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *panel_id,
    UmiApplicationProductPanelProjection *out_projection);

/** Count bound and actionable panels without creating any frontend widgets. */
UmiStatus umi_application_product_panel_coverage(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationProductPanelCoverage *out_coverage);

#ifdef __cplusplus
}
#endif

#endif
