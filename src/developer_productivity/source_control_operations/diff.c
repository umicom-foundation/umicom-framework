/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/diff.c
 *
 * PURPOSE:
 *   Define the reusable Compare Changes source-control operation contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/diff.h"

const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_diff(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.diff",
        .title = "Compare Changes",
        .description = "Load a source-control diff.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_DIFF,
        .mutates_repository = 0,
        .requires_trust = 0,
        .requires_argument = 1
    };
    return &definition;
}
