/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control_operations/branch_delete.h
 *
 * PURPOSE:
 *   Publish metadata for the Delete Branch source-control operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_BRANCH_DELETE_H
#define UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_BRANCH_DELETE_H

#include "umicom/developer_productivity/source_control_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer source control operation branch delete operation used by this
 * module and its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_branch_delete(void);

#ifdef __cplusplus
}
#endif

#endif
