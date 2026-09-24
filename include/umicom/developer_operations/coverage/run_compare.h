/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/coverage/run_compare.h
 *
 * PURPOSE:
 *   Project canonical Coverage Run Compare state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_COVERAGE_RUN_COMPARE_H
#define UMICOM_DEVELOPER_OPERATIONS_COVERAGE_RUN_COMPARE_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateCoverageRunCompare(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
