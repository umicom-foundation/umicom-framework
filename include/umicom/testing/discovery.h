/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/discovery.h
 *
 * PURPOSE:
 *   Discover CTest cases from machine-readable or standard CTest listing output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_DISCOVERY_H
#define UMICOM_TESTING_DISCOVERY_H

#include "umicom/base/status.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_test_discovery_parse_ctest(const char *output,
                                         const char *build_directory,
                                         UmiTestSuite *suite,
                                         size_t *out_discovered);

#ifdef __cplusplus
}
#endif

#endif
