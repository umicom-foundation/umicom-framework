/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/application.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
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

#include "umicom/integration/application.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Initialise integration application from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_application_init(UmiIntegrationApplication *application)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application != NULL) {
        (void)memset(application, 0, sizeof(*application));
    }
}

/*
 * Provide the integration application set identity operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_application_set_identity(
    UmiIntegrationApplication *application,
    const char *id,
    const char *name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application == NULL || id == NULL || id[0] == '\0' ||
        name == NULL || name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(application->id, sizeof(application->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(application->name, sizeof(application->name), name);
}

/*
 * Provide the integration application add capability operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_application_add_capability(
    UmiIntegrationApplication *application,
    const char *capability_id)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application == NULL || capability_id == NULL ||
        capability_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < application->capability_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(application->capabilities[index], capability_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (application->capability_count >= UMI_INTEGRATION_MAX_CAPABILITIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(
        application->capabilities[application->capability_count],
        UMI_INTEGRATION_ID_CAPACITY,
        capability_id
    );
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        ++application->capability_count;
    }
    return status;
}

/*
 * Provide the integration application has capability operation used by this module and its
 * client applications.
 */
bool umi_integration_application_has_capability(
    const UmiIntegrationApplication *application,
    const char *capability_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application == NULL || capability_id == NULL) {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < application->capability_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(application->capabilities[index], capability_id) == 0) {
            return true;
        }
    }
    return false;
}
