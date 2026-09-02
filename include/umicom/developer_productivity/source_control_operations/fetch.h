/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control_operations/fetch.h
 *
 * PURPOSE:
 *   Publish metadata for the Fetch source-control operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_FETCH_H
#define UMICOM_DEVELOPER_SOURCE_CONTROL_OPERATION_FETCH_H

#include "umicom/developer_productivity/source_control_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer source control operation fetch operation used by this module and
 * its client applications.
 */
const UmiDeveloperSourceControlOperationDefinition *umi_developer_source_control_operation_fetch(void);

#ifdef __cplusplus
}
#endif

#endif
