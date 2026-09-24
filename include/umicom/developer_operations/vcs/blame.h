/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/vcs/blame.h
 *
 * PURPOSE:
 *   Project canonical VCS Blame state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_VCS_BLAME_H
#define UMICOM_DEVELOPER_OPERATIONS_VCS_BLAME_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateVcsBlame(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
