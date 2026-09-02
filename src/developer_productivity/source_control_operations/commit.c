/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/commit.c
 *
 * PURPOSE:
 *   Define the reusable Commit source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/commit.h"

/*
 * Provide the developer source control operation commit operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_commit(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.commit",
        .title = "Commit",
        .description = "Create a source-control commit.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_COMMIT,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 1
    };
    return &definition;
}
