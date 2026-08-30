/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/readiness_view.h
 *
 * PURPOSE:
 *   Project feature-state and priority counts into frontend-neutral readiness rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_READINESS_VIEW_H
#define UMICOM_APPLICATION_UI_READINESS_VIEW_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationUiReadinessRow {
    const char *label;
    size_t count;
} UmiApplicationUiReadinessRow;

#define UMI_APPLICATION_UI_READINESS_ROW_COUNT 6U

UmiStatus umi_application_ui_readiness_rows(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiReadinessRow rows[UMI_APPLICATION_UI_READINESS_ROW_COUNT]);

#ifdef __cplusplus
}
#endif

#endif
