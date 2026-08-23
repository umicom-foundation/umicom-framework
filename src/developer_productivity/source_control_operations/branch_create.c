/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/branch_create.c
 *
 * PURPOSE:
 *   Define the reusable Create Branch source-control operation contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/branch_create.h"

const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_branch_create(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.branch-create",
        .title = "Create Branch",
        .description = "Create a branch.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_BRANCH_CREATE,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 1
    };
    return &definition;
}
