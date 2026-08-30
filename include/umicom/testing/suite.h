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

typedef struct UmiTestSuite UmiTestSuite;

UmiStatus umi_test_suite_create(const char *suite_id,
                                const char *name,
                                UmiTestSuite **out_suite);
void umi_test_suite_destroy(UmiTestSuite *suite);
UmiStatus umi_test_suite_add(UmiTestSuite *suite,
                             const UmiTestCase *test_case);
size_t umi_test_suite_count(const UmiTestSuite *suite);
const UmiTestCase *umi_test_suite_at(const UmiTestSuite *suite,
                                     size_t index);
const UmiTestCase *umi_test_suite_find(const UmiTestSuite *suite,
                                       const char *test_id);
const char *umi_test_suite_id(const UmiTestSuite *suite);
const char *umi_test_suite_name(const UmiTestSuite *suite);

#ifdef __cplusplus
}
#endif

#endif
