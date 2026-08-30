/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/fetch.c
 *
 * PURPOSE:
 *   Define the reusable Fetch source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/fetch.h"

const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_fetch(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.fetch",
        .title = "Fetch",
        .description = "Fetch remote references.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_FETCH,
        .mutates_repository = 0,
        .requires_trust = 1,
        .requires_argument = 0
    };
    return &definition;
}
