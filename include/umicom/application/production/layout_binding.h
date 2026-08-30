/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/layout_binding.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_LAYOUT_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_LAYOUT_BINDING_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

typedef struct UmiApplicationProductionLayoutBinding {
    const UmiExperienceLayoutDefinition *layout;
    size_t projected_window_count;
    int default_layout;
    int projection_ready;
} UmiApplicationProductionLayoutBinding;

typedef struct UmiApplicationProductionLayoutBindings {
    UmiApplicationProductionLayoutBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_LAYOUTS];
    size_t count;
    size_t projected_count;
    size_t missing_projection_count;
} UmiApplicationProductionLayoutBindings;

UmiStatus umi_application_production_layout_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionLayoutBindings *out_bindings);
const UmiApplicationProductionLayoutBinding *
umi_application_production_layout_bindings_find(
    const UmiApplicationProductionLayoutBindings *bindings,
    const char *layout_id);

#ifdef __cplusplus
}
#endif
#endif
