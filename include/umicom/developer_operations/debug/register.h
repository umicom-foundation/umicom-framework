/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/debug/register.h
 *
 * PURPOSE:
 *   Project canonical Debug Register state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_DEBUG_REGISTER_H
#define UMICOM_DEVELOPER_OPERATIONS_DEBUG_REGISTER_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateDebugRegister(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
