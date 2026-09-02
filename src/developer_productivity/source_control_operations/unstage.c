/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/unstage.c
 *
 * PURPOSE:
 *   Define the reusable Unstage source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/unstage.h"

/*
 * Provide the developer source control operation unstage operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_unstage(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.unstage",
        .title = "Unstage",
        .description = "Unstage one path.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_UNSTAGE,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 1
    };
    return &definition;
}
