/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/health_view.c
 *
 * PURPOSE:
 *   Project runtime health into a stable frontend-neutral row.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/health_view.h"

UmiStatus umi_application_ui_health_row(
    const UmiApplicationRuntimeHealth *health,
    UmiApplicationUiHealthRow *out_row)
{
    if (health == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_row->severity = umi_application_health_severity_text(health->severity);
    out_row->readiness_percent = health->readiness_percent;
    out_row->unavailable_capabilities = health->unavailable_capabilities;
    out_row->open_priority_features = health->open_p0_features + health->open_p1_features;
    return UMI_STATUS_OK;
}
