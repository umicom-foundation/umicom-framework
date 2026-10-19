/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_ctest.h
 * Purpose: Import complete CTest evidence into an identified qualification attempt.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_CTEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_CTEST_H
#include "umicom/distribution/runtime/qualification.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Read a complete saved CTest JUnit document using the existing strict parser.
 * The document must contain every selected test exactly once and no extra names.
 * No test or process result is changed on malformed, incomplete or mismatched
 * evidence. Existing outcomes cannot be replaced. Empty suites are UNAVAILABLE.
 *
 * testProcess is the executor's actual aggregate process outcome, status, exit
 * code, duration and captured-log digest. A passing process requires known exit
 * zero. Per-test exit codes are unavailable in JUnit and remain explicitly unknown.
 * Each test's log digest identifies the supplied XML bytes, not the stdout log.
 * Disabled, runtime skipped and not-run outcomes remain separate.
 *
 * The parser's existing 64-MiB, 100,000-test and 255-byte test-name limits apply.
 * Longer names are refused, never truncated. The caller must establish that
 * saved XML belongs to this attempt; XML contains no authenticated source identity.
 * This service reads no file and launches nothing. Hosts must serialize mutation
 * and retain the original report privately for comparison with its digest. */
UmiStatus UmiReleaseQualificationImportCtest(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, const char *xml, size_t length,
    const UmiReleaseQualificationResult *testProcess);
#ifdef __cplusplus
}
#endif
#endif
