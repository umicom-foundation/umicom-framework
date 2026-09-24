/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/vcs/diff.h
 *
 * PURPOSE:
 *   Project canonical VCS Diff state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_VCS_DIFF_H
#define UMICOM_DEVELOPER_OPERATIONS_VCS_DIFF_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateVcsDiff(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
