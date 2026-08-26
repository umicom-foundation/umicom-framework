/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/ownership_view.c
 *
 * PURPOSE:
 *   Make Framework, application and external-adapter ownership visible in product planning views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/ownership_view.h"

UmiStatus umi_application_ui_ownership_rows(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiOwnershipRow rows[UMI_APPLICATION_UI_OWNERSHIP_ROW_COUNT])
{
    UmiApplicationReadinessReport report;
    UmiStatus result;
    if (rows == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_readiness_report(experience, &report);
    if (result != UMI_STATUS_OK) return result;
    rows[0] = (UmiApplicationUiOwnershipRow){UMI_EXPERIENCE_OWNER_FRAMEWORK,
        umi_experience_ownership_text(UMI_EXPERIENCE_OWNER_FRAMEWORK), report.framework_owned_count};
    rows[1] = (UmiApplicationUiOwnershipRow){UMI_EXPERIENCE_OWNER_APPLICATION,
        umi_experience_ownership_text(UMI_EXPERIENCE_OWNER_APPLICATION), report.application_owned_count};
    rows[2] = (UmiApplicationUiOwnershipRow){UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER,
        umi_experience_ownership_text(UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER), report.external_adapter_count};
    return UMI_STATUS_OK;
}
