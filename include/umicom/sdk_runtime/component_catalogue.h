/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/component_catalogue.h
 *
 * PURPOSE:
 *   Maintain installed component inventory and compatibility evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPONENT_CATALOGUE
#define UMICOM_SDK_RUNTIME_COMPONENT_CATALOGUE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime component catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeComponentCatalogue {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t component_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeComponentCatalogue;
/**
 * Initialise sdk runtime component catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_component_catalogue_init(UmiSdkRuntimeComponentCatalogue *value, const char *id);
/**
 * Check that sdk runtime component catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_component_catalogue_validate(const UmiSdkRuntimeComponentCatalogue *value);
/**
 * Provide the sdk runtime component catalogue set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_component_catalogue_set_path(UmiSdkRuntimeComponentCatalogue *value, const char *path);
/**
 * Provide the sdk runtime component catalogue set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_component_catalogue_set_detail(UmiSdkRuntimeComponentCatalogue *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime component catalogue set
 * component without changing their state.
 */
UmiStatus umi_sdk_runtime_component_catalogue_set_component_count(UmiSdkRuntimeComponentCatalogue *value, uint64_t number);
/**
 * Provide the sdk runtime component catalogue set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_component_catalogue_set_generation(UmiSdkRuntimeComponentCatalogue *value, uint64_t number);
/**
 * Provide the sdk runtime component catalogue set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_component_catalogue_set_state(UmiSdkRuntimeComponentCatalogue *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime component catalogue same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_component_catalogue_same_identity(const UmiSdkRuntimeComponentCatalogue *left, const UmiSdkRuntimeComponentCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif
