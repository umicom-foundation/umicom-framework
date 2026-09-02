/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control_operations/pull.h
 *
 * PURPOSE:
 *   Publish metadata for the Pull source-control operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_PULL_H
#define UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_PULL_H

#include "umicom/developer_productivity/source_control_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer source control operation pull operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_pull(void);

#ifdef __cplusplus
}
#endif

#endif
