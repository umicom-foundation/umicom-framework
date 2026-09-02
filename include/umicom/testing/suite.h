/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/suite.h
 *
 * PURPOSE:
 *   Group related test cases into an owned, bounded suite with deterministic ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_SUITE_H
#define UMICOM_TESTING_SUITE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/testing/case.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test suite data shared with callers of this public contract.
 */
typedef struct UmiTestSuite UmiTestSuite;

/**
 * Initialise test suite from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_test_suite_create(const char *suite_id,
                                const char *name,
                                UmiTestSuite **out_suite);
/**
 * Release or reset state held by test suite so the same storage can be reused safely.
 */
void umi_test_suite_destroy(UmiTestSuite *suite);
/**
 * Add test suite only after its inputs and available capacity have been checked.
 */
UmiStatus umi_test_suite_add(UmiTestSuite *suite,
                             const UmiTestCase *test_case);
/**
 * Return the number of records represented by test suite without changing their state.
 */
size_t umi_test_suite_count(const UmiTestSuite *suite);
/**
 * Find test suite while leaving the underlying catalogue or model owned by this module.
 */
const UmiTestCase *umi_test_suite_at(const UmiTestSuite *suite,
                                     size_t index);
/**
 * Find test suite while leaving the underlying catalogue or model owned by this module.
 */
const UmiTestCase *umi_test_suite_find(const UmiTestSuite *suite,
                                       const char *test_id);
/**
 * Provide the test suite id operation used by this module and its client applications.
 */
const char *umi_test_suite_id(const UmiTestSuite *suite);
/**
 * Provide the test suite name operation used by this module and its client applications.
 */
const char *umi_test_suite_name(const UmiTestSuite *suite);

#ifdef __cplusplus
}
#endif

#endif
