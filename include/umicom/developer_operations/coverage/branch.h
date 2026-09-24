/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/coverage/branch.h
 *
 * PURPOSE:
 *   Project canonical Coverage Branch state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_COVERAGE_BRANCH_H
#define UMICOM_DEVELOPER_OPERATIONS_COVERAGE_BRANCH_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateCoverageBranch(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
