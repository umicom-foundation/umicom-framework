/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_coverage_manifest.h
 *
 * PURPOSE:
 *   Share bounded host-coverage plans between the CLI and native applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_CTEST_COVERAGE_MANIFEST_H
#define UMICOM_TESTING_CTEST_COVERAGE_MANIFEST_H
#include "umicom/testing/ctest_coverage.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CTEST_COVERAGE_REPORT_BYTES (64U * 1024U * 1024U)

typedef struct UmiCtestCoverageHostFile {
    char host[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char configuration[UMI_CTEST_COVERAGE_HOST_CAPACITY];
    char source_identity[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
    char report[UMI_PATH_CAPACITY];
} UmiCtestCoverageHostFile;
typedef struct UmiCtestCoverageManifest UmiCtestCoverageManifest;

/* Import UTF-8 CSV through the shared CSV reader, limited to UMI_CSV_MAX_BYTES.
 * The exact seven-column header is:
 * kind,host,configuration,source_identity,test,capability,report
 * host rows supply host/configuration/source_identity/report; test and
 * capability must be empty. require rows supply host/configuration/test and
 * optional capability; source_identity and report must be empty.
 * Every record has seven columns. Blank records and unknown kinds are errors.
 * Report paths are relative children: no absolute, drive, dot or dot-dot
 * components. Either slash is accepted; no control characters are accepted.
 * Duplicate host/configuration pairs and required name/host/configuration
 * triples are rejected. A requirement may intentionally have no host row.
 * Quoted commas and UTF-8 names are supported without whitespace trimming.
 * Import copies input and performs no file reads. Failure clears out. */
UmiStatus UmiCtestCoverageManifestImport(const void *bytes, size_t length,
    UmiCtestCoverageManifest **out);
size_t UmiCtestCoverageManifestHostCount(const UmiCtestCoverageManifest *manifest);
size_t UmiCtestCoverageManifestRequirementCount(const UmiCtestCoverageManifest *manifest);
const UmiCtestCoverageHostFile *UmiCtestCoverageManifestHostAt(
    const UmiCtestCoverageManifest *manifest, size_t index);
const UmiCtestCoverageRequirement *UmiCtestCoverageManifestRequirementAt(
    const UmiCtestCoverageManifest *manifest, size_t index);
void UmiCtestCoverageManifestDestroy(UmiCtestCoverageManifest *manifest);

/* Read declared reports relative to an explicit ordinary absolute directory,
 * never the process current directory. Total report bytes are capped at
 * UMI_CTEST_COVERAGE_REPORT_BYTES, with each report retaining its parser limit.
 * Reports must be regular local files. Parent directory aliases can be followed;
 * this is not a filesystem sandbox. Use a trusted evidence directory.
 * A missing or malformed declared report is an input error, not a skipped test.
 * A requirement without a declared host is a MISSING_HOST review row.
 * No execution, writes, network requests or source identity discovery occurs.
 * Caller-supplied source identity must match each host's recorded identity.
 * The returned review owns its evidence; no input lifetime is retained.
 * File I/O can block; native applications should call from a worker.
 * Failure clears out; no partial review is published. */
UmiStatus UmiCtestCoverageManifestReviewFiles(const UmiCtestCoverageManifest *manifest,
    const char *directory, const char *source_identity, UmiCtestCoverageReview **out);
#ifdef __cplusplus
}
#endif
#endif
