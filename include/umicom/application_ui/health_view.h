/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/health_view.h
 *
 * PURPOSE:
 *   Project application runtime health into frontend-neutral status data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_HEALTH_VIEW_H
#define UMICOM_APPLICATION_UI_HEALTH_VIEW_H

#include "umicom/application/runtime/runtime_health.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationUiHealthRow {
    const char *severity;
    unsigned readiness_percent;
    size_t unavailable_capabilities;
    size_t open_priority_features;
} UmiApplicationUiHealthRow;

UmiStatus umi_application_ui_health_row(
    const UmiApplicationRuntimeHealth *health,
    UmiApplicationUiHealthRow *out_row);

#ifdef __cplusplus
}
#endif

#endif
