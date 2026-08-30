/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/summary_view.h
 *
 * PURPOSE:
 *   Summarise one canonical product experience for launchers, Studio and Desk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_SUMMARY_VIEW_H
#define UMICOM_APPLICATION_UI_SUMMARY_VIEW_H

#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationUiSummaryRow {
    const char *application_id;
    const char *display_name;
    const char *default_layout_id;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
    unsigned readiness_percent;
    const char *next_feature_id;
} UmiApplicationUiSummaryRow;

UmiStatus umi_application_ui_summary_row(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiSummaryRow *out_row);

#ifdef __cplusplus
}
#endif

#endif
