/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/capability_status.h
 *
 * PURPOSE:
 *   Project unique capability requirements from panels and features and resolve their availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_CAPABILITY_STATUS_H
#define UMICOM_APPLICATION_RUNTIME_CAPABILITY_STATUS_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationCapabilityStatusEntry {
    const char *capability_id;
    int available;
    size_t consumer_count;
} UmiApplicationCapabilityStatusEntry;

typedef struct UmiApplicationCapabilityStatus {
    uint32_t structure_size;
    UmiApplicationCapabilityStatusEntry entries[UMI_APPLICATION_RUNTIME_MAX_CAPABILITIES];
    size_t entry_count;
    size_t unavailable_count;
} UmiApplicationCapabilityStatus;

UmiStatus umi_application_capability_status_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationCapabilityStatus *out_status);
const UmiApplicationCapabilityStatusEntry *umi_application_capability_status_find(
    const UmiApplicationCapabilityStatus *status,
    const char *capability_id);

#ifdef __cplusplus
}
#endif

#endif
