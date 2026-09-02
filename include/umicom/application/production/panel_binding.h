/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/panel_binding.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_PANEL_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_PANEL_BINDING_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"
#include "umicom/application/productisation/surface_projection.h"

/**
 * Represent the application production panel binding data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionPanelBinding {
    const UmiExperiencePanelDefinition *panel;
    UmiProductSurfaceProjection surface;
    int surface_available;
    int required;
    int covered;
} UmiApplicationProductionPanelBinding;

/**
 * Represent the application production panel bindings data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionPanelBindings {
    UmiApplicationProductionPanelBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_PANELS];
    size_t count;
    size_t required_count;
    size_t uncovered_count;
} UmiApplicationProductionPanelBindings;

/**
 * Provide the application production panel bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_panel_bindings_build(
    const UmiApplicationProductionBinding *binding,
    const UmiProductSurfacePortfolio *surfaces,
    UmiApplicationProductionPanelBindings *out_bindings);
/**
 * Find application production panel bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionPanelBinding *
umi_application_production_panel_bindings_find(
    const UmiApplicationProductionPanelBindings *bindings,
    const char *panel_id);

#ifdef __cplusplus
}
#endif
#endif
