/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/feature_binding.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_FEATURE_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_FEATURE_BINDING_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

/**
 * Represent the application production feature binding data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionFeatureBinding {
    const UmiExperienceFeatureDefinition *feature;
    int open;
    int application_work;
    int external_adapter_work;
} UmiApplicationProductionFeatureBinding;

/**
 * Represent the application production feature bindings data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionFeatureBindings {
    UmiApplicationProductionFeatureBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_FEATURES];
    size_t count;
    size_t open_count;
    size_t application_work_count;
    size_t external_adapter_work_count;
} UmiApplicationProductionFeatureBindings;

/**
 * Provide the application production feature bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_feature_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionFeatureBindings *out_bindings);
/**
 * Find application production feature bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionFeatureBinding *
umi_application_production_feature_bindings_find(
    const UmiApplicationProductionFeatureBindings *bindings,
    const char *feature_id);

#ifdef __cplusplus
}
#endif
#endif
