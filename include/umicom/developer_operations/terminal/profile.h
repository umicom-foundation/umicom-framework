/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/terminal/profile.h
 *
 * PURPOSE:
 *   Project canonical Terminal Profile state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_TERMINAL_PROFILE_H
#define UMICOM_DEVELOPER_OPERATIONS_TERMINAL_PROFILE_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateTerminalProfile(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
