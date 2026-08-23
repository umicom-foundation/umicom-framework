/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/refresh.c
 *
 * PURPOSE:
 *   Define the reusable Refresh Status source-control operation contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/refresh.h"

const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_refresh(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.refresh",
        .title = "Refresh Status",
        .description = "Refresh repository status.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_REFRESH,
        .mutates_repository = 0,
        .requires_trust = 0,
        .requires_argument = 0
    };
    return &definition;
}
