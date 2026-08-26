/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/summary_view.c
 *
 * PURPOSE:
 *   Build reusable application summary rows from the canonical experience catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/summary_view.h"

#include <string.h>

UmiStatus umi_application_ui_summary_row(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiSummaryRow *out_row)
{
    UmiApplicationReadinessReport report;
    UmiStatus result;
    if (experience == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_readiness_report(experience, &report);
    if (result != UMI_STATUS_OK) return result;
    memset(out_row, 0, sizeof(*out_row));
    out_row->application_id = experience->application_id;
    out_row->display_name = experience->display_name;
    out_row->default_layout_id = experience->default_layout_id;
    out_row->panel_count = experience->panel_count;
    out_row->layout_count = experience->layout_count;
    out_row->feature_count = experience->feature_count;
    out_row->readiness_percent = report.readiness_percent;
    out_row->next_feature_id = report.next_feature != NULL ? report.next_feature->feature_id : NULL;
    return UMI_STATUS_OK;
}
