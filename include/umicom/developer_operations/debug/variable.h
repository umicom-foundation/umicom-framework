/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/debug/variable.h
 *
 * PURPOSE:
 *   Project canonical Debug Variable state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_DEBUG_VARIABLE_H
#define UMICOM_DEVELOPER_OPERATIONS_DEBUG_VARIABLE_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateDebugVariable(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
