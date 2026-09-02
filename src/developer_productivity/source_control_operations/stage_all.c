/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/stage_all.c
 *
 * PURPOSE:
 *   Define the reusable Stage All source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/stage_all.h"

/*
 * Provide the developer source control operation stage all operation used by this module
 * and its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_stage_all(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.stage-all",
        .title = "Stage All",
        .description = "Stage every workspace change.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_STAGE_ALL,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 0
    };
    return &definition;
}
