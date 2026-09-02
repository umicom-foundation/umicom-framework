/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/pull.c
 *
 * PURPOSE:
 *   Define the reusable Pull source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/pull.h"

/*
 * Provide the developer source control operation pull operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_pull(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.pull",
        .title = "Pull",
        .description = "Integrate remote changes.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_PULL,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 0
    };
    return &definition;
}
