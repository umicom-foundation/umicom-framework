/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control_operations/commit.h
 *
 * PURPOSE:
 *   Publish metadata for the Commit source-control operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_COMMIT_H
#define UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_COMMIT_H

#include "umicom/developer_productivity/source_control_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer source control operation commit operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_commit(void);

#ifdef __cplusplus
}
#endif

#endif
