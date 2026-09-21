/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/discovery.h
 *
 * PURPOSE:
 *   Discover CTest cases from machine-readable or standard CTest listing output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_DISCOVERY_H
#define UMICOM_TESTING_DISCOVERY_H

#include "umicom/base/status.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the test discovery parse ctest operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_discovery_parse_ctest(const char *output,
                                         const char *build_directory,
                                         UmiTestSuite *suite,
                                         size_t *out_discovered);

/** Parse one complete captured CTest human listing and append atomically.
 * length excludes the trailing NUL. Embedded NULs, malformed test rows,
 * duplicate identifiers, oversized identities, a missing/repeated summary,
 * and a Total Tests count that differs from the listing are errors. Failure
 * leaves the suite unchanged and writes zero to outDiscovered when supplied.
 * Names ending in the ambiguous human-output " (Disabled)" suffix are rejected
 * with UNAVAILABLE; use the JSON metadata provider for disabled catalogues.
 * No process or file is accessed. The caller supplies immutable text for the
 * call and serialises suite access. At most 8 MiB of text is accepted.
 */
UmiStatus UmiTestDiscoveryParseCtestComplete(const char *output, size_t length,
    const char *buildDirectory, UmiTestSuite *suite, size_t *outDiscovered);

#ifdef __cplusplus
}
#endif

#endif
