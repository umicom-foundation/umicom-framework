/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/coverage/test_result_summary.h
 *
 * PURPOSE:
 *   Project canonical Test Result Summary state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_COVERAGE_TEST_RESULT_SUMMARY_H
#define UMICOM_DEVELOPER_OPERATIONS_COVERAGE_TEST_RESULT_SUMMARY_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateCoverageTestResultSummary(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
