/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_manifest_private.h
 *
 * PURPOSE:
 *   Own parsed host files and requirements without retaining borrowed CSV fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CTEST_COVERAGE_MANIFEST_PRIVATE_H
#define UMICOM_CTEST_COVERAGE_MANIFEST_PRIVATE_H
#include "umicom/testing/ctest_coverage_manifest.h"
#include "umicom/base/sha256.h"
struct UmiCtestCoverageManifest {
    UmiCtestCoverageHostFile *hosts;
    /* Allocate by host index only for the digest-bound form. Public host
     * descriptors keep their existing size and owning manifest lifetime. */
    char (*report_digests)[UMI_SHA256_HEX_CAPACITY];
    size_t digest_capacity;
    UmiCtestCoverageRequirement *requirements;
    size_t host_count, requirement_count, host_capacity, requirement_capacity;
};
#endif
