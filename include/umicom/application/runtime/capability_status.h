/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/capability_status.h
 *
 * PURPOSE:
 *   Project unique capability requirements from panels and features and resolve their availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_CAPABILITY_STATUS_H
#define UMICOM_APPLICATION_RUNTIME_CAPABILITY_STATUS_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application capability status entry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationCapabilityStatusEntry {
    const char *capability_id;
    int available;
    size_t consumer_count;
} UmiApplicationCapabilityStatusEntry;

/**
 * Represent the application capability status data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationCapabilityStatus {
    uint32_t structure_size;
    UmiApplicationCapabilityStatusEntry entries[UMI_APPLICATION_RUNTIME_MAX_CAPABILITIES];
    size_t entry_count;
    size_t unavailable_count;
} UmiApplicationCapabilityStatus;

/**
 * Provide the application capability status build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_capability_status_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationCapabilityStatus *out_status);
/**
 * Find application capability status while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationCapabilityStatusEntry *umi_application_capability_status_find(
    const UmiApplicationCapabilityStatus *status,
    const char *capability_id);

#ifdef __cplusplus
}
#endif

#endif
