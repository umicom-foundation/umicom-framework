/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/application.h
 *
 * PURPOSE:
 *   Describe one independently launchable Umicom application and its published capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_APPLICATION_H
#define UMICOM_INTEGRATION_APPLICATION_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

/**
 * Represent the integration application data shared with callers of this public contract.
 */
typedef struct UmiIntegrationApplication {
    char id[UMI_INTEGRATION_ID_CAPACITY];
    char name[UMI_INTEGRATION_NAME_CAPACITY];
    char version[UMI_INTEGRATION_TEXT_CAPACITY];
    char path[UMI_INTEGRATION_PATH_CAPACITY];
    char executable[UMI_INTEGRATION_PATH_CAPACITY];
    bool enabled;
    unsigned frontends;
    char capabilities[UMI_INTEGRATION_MAX_CAPABILITIES]
                     [UMI_INTEGRATION_ID_CAPACITY];
    size_t capability_count;
} UmiIntegrationApplication;

/**
 * Initialise integration application from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_application_init(UmiIntegrationApplication *application);
/**
 * Provide the integration application set identity operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_application_set_identity(
    UmiIntegrationApplication *application,
    const char *id,
    const char *name);
/**
 * Provide the integration application add capability operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_application_add_capability(
    UmiIntegrationApplication *application,
    const char *capability_id);
/**
 * Provide the integration application has capability operation used by this module and its
 * client applications.
 */
bool umi_integration_application_has_capability(
    const UmiIntegrationApplication *application,
    const char *capability_id);

#endif
