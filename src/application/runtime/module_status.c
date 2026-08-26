/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/module_status.c
 *
 * PURPOSE:
 *   Build product-module readiness independently of reusable Framework capability state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/module_status.h"

#include <string.h>

UmiStatus umi_application_module_status_init(
    const UmiApplicationExperienceDefinition *experience,
    int composition_available,
    int executable_available,
    int tests_available,
    UmiApplicationModuleStatus *out_status)
{
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

int umi_application_module_status_runnable(const UmiApplicationModuleStatus *status)
{
    return status != NULL && status->composition_available && status->executable_available;
}
