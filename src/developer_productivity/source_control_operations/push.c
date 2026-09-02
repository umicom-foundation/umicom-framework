/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/push.c
 *
 * PURPOSE:
 *   Define the reusable Push source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/push.h"

/*
 * Provide the developer source control operation push operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_push(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.push",
        .title = "Push",
        .description = "Push local commits.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_PUSH,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 0
    };
    return &definition;
}
