/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operations/branch_checkout.c
 *
 * PURPOSE:
 *   Define the reusable Checkout Branch source-control operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operations/branch_checkout.h"

/*
 * Provide the developer source control operation branch checkout operation used by this
 * module and its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_branch_checkout(void)
{
    static const UmiDeveloperSourceControlOperationDefinition definition = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperSourceControlOperationDefinition),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .operation_id = "developer.source-control.branch-checkout",
        .title = "Checkout Branch",
        .description = "Checkout a branch.",
        .kind = UMI_DEVELOPER_SOURCE_CONTROL_BRANCH_CHECKOUT,
        .mutates_repository = 1,
        .requires_trust = 1,
        .requires_argument = 1
    };
    return &definition;
}
