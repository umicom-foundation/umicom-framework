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

typedef struct UmiApplicationProductionPanelBinding {
    const UmiExperiencePanelDefinition *panel;
    UmiProductSurfaceProjection surface;
    int surface_available;
    int required;
    int covered;
} UmiApplicationProductionPanelBinding;

typedef struct UmiApplicationProductionPanelBindings {
    UmiApplicationProductionPanelBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_PANELS];
    size_t count;
    size_t required_count;
    size_t uncovered_count;
} UmiApplicationProductionPanelBindings;

UmiStatus umi_application_production_panel_bindings_build(
    const UmiApplicationProductionBinding *binding,
    const UmiProductSurfacePortfolio *surfaces,
    UmiApplicationProductionPanelBindings *out_bindings);
const UmiApplicationProductionPanelBinding *
umi_application_production_panel_bindings_find(
    const UmiApplicationProductionPanelBindings *bindings,
    const char *panel_id);

#ifdef __cplusplus
}
#endif
#endif
