/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_adapter.h
 *
 * PURPOSE:
 *   Discover and execute CTest suites through the portable Framework test model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_ADAPTER_H
#define UMICOM_TESTING_CTEST_ADAPTER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/testing/runner.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ctest_discover(const char *build_directory,
                             UmiTestSuite *suite,
                             size_t *out_discovered);
UmiStatus umi_ctest_run(const char *build_directory,
                        const char *test_name,
                        UmiTestResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
