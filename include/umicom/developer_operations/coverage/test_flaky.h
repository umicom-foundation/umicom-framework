/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/coverage/test_flaky.h
 *
 * PURPOSE:
 *   Project canonical Flaky Test state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_COVERAGE_TEST_FLAKY_H
#define UMICOM_DEVELOPER_OPERATIONS_COVERAGE_TEST_FLAKY_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateCoverageTestFlaky(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
