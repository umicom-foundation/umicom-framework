/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/builtin_source_control_operations.c
 *
 * PURPOSE:
 *   Enumerate professional source-control operation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/builtin_source_control_operations.h"

#include "umicom/developer_productivity/source_control_operations/operations.h"

typedef const UmiDeveloperSourceControlOperationDefinition *
    (*OperationFactory)(void);

static const OperationFactory FACTORIES[] = {
    umi_developer_source_control_operation_refresh,
    umi_developer_source_control_operation_stage,
    umi_developer_source_control_operation_unstage,
    umi_developer_source_control_operation_stage_all,
    umi_developer_source_control_operation_discard,
    umi_developer_source_control_operation_commit,
    umi_developer_source_control_operation_fetch,
    umi_developer_source_control_operation_pull,
    umi_developer_source_control_operation_push,
    umi_developer_source_control_operation_branch_create,
    umi_developer_source_control_operation_branch_checkout,
    umi_developer_source_control_operation_branch_delete,
    umi_developer_source_control_operation_diff
};

/*
 * Return the number of records represented by developer builtin source control operation
 * without changing their state.
 */
size_t umi_developer_builtin_source_control_operation_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer builtin source control operation while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiDeveloperSourceControlOperationDefinition *
umi_developer_builtin_source_control_operation_at(size_t index)
{
    return index < umi_developer_builtin_source_control_operation_count()
        ? FACTORIES[index]()
        : NULL;
}
