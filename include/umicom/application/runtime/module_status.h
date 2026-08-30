/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/module_status.h
 *
 * PURPOSE:
 *   Describe thin application module readiness separately from Framework feature readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_MODULE_STATUS_H
#define UMICOM_APPLICATION_RUNTIME_MODULE_STATUS_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationModuleStatus {
    uint32_t structure_size;
    const char *application_id;
    unsigned readiness_percent;
    int composition_available;
    int executable_available;
    int tests_available;
} UmiApplicationModuleStatus;

UmiStatus umi_application_module_status_init(
    const UmiApplicationExperienceDefinition *experience,
    int composition_available,
    int executable_available,
    int tests_available,
    UmiApplicationModuleStatus *out_status);
int umi_application_module_status_runnable(
    const UmiApplicationModuleStatus *status);

#ifdef __cplusplus
}
#endif

#endif
