/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/module_status.c
 *
 * PURPOSE:
 *   Build product-module readiness independently of reusable Framework capability state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/module_status.h"

#include <string.h>

/*
 * Initialise application module status from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_module_status_init(
    const UmiApplicationExperienceDefinition *experience,
    int composition_available,
    int executable_available,
    int tests_available,
    UmiApplicationModuleStatus *out_status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof(*out_status));
    out_status->structure_size = sizeof(*out_status);
    out_status->application_id = experience->application_id;
    out_status->readiness_percent = umi_application_experience_readiness_percent(experience);
    out_status->composition_available = composition_available != 0;
    out_status->executable_available = executable_available != 0;
    out_status->tests_available = tests_available != 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the application module status runnable operation used by this module and its
 * client applications.
 */
int umi_application_module_status_runnable(const UmiApplicationModuleStatus *status)
{
    return status != NULL && status->composition_available && status->executable_available;
}
