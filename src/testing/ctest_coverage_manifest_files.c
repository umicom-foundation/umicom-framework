/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_manifest_files.c
 *
 * PURPOSE:
 *   Read explicitly selected host evidence under shared byte and path limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "ctest_coverage_manifest_private.h"
#include "umicom/base/csv_document.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiCtestCoverageManifestReviewFiles(const UmiCtestCoverageManifest *manifest,
    const char *directory, const char *source_identity, UmiCtestCoverageReview **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!manifest || !directory || !umi_path_is_absolute(directory) || !source_identity)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Reject invalid metadata before reading any report. Path syntax validation
     * uses an ordinary child path; it never creates that child. */
    char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    size_t identity_length = 0;
    while (identity_length < UMI_CTEST_COVERAGE_IDENTITY_CAPACITY &&
        source_identity[identity_length]) ++identity_length;
    if (identity_length == UMI_CTEST_COVERAGE_IDENTITY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiCtestEscapeName(source_identity, escaped, sizeof escaped);
    char probe[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, "coverage-boundary", probe, sizeof probe);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(probe);
    if (status != UMI_STATUS_OK) return status;
    size_t count = manifest->host_count;
    UmiCtestCoverageHost *hosts = count ? calloc(count, sizeof *hosts) : NULL;
    UmiCtestReport **reports = count ? calloc(count, sizeof *reports) : NULL;
    if (count && (!hosts || !reports)) {
        free(hosts); free(reports);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t remaining = UMI_CTEST_COVERAGE_REPORT_BYTES;
    for (size_t i = 0; i < count && status == UMI_STATUS_OK; ++i) {
        const UmiCtestCoverageHostFile *host = &manifest->hosts[i];
        char path[UMI_PATH_CAPACITY];
        status = umi_path_join(directory, host->report, path, sizeof path);
        if (status == UMI_STATUS_OK && !umi_path_is_within(directory, path))
            status = UMI_STATUS_INVALID_ARGUMENT;
        unsigned char *bytes = NULL;
        size_t size = 0;
        size_t maximum = remaining < UMI_CTEST_COLLECTION_MAX_BYTES
            ? remaining : UMI_CTEST_COLLECTION_MAX_BYTES;
        if (status == UMI_STATUS_OK && maximum == 0)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK) status = UmiInputFileRead(path, maximum, &bytes, &size);
        if (status == UMI_STATUS_OK)
            status = size ? UmiCtestReportImport((const char *)bytes, size, &reports[i])
                          : UMI_STATUS_PARSE_ERROR;
        UmiInputFileFree(bytes);
        if (status != UMI_STATUS_OK) break;
        remaining -= size;
        memcpy(hosts[i].host, host->host, sizeof hosts[i].host);
        memcpy(hosts[i].configuration, host->configuration, sizeof hosts[i].configuration);
        memcpy(hosts[i].source_identity, host->source_identity, sizeof hosts[i].source_identity);
        hosts[i].report = reports[i];
    }
    if (status == UMI_STATUS_OK)
        status = UmiCtestCoverageReviewCreate(source_identity, manifest->requirements,
            manifest->requirement_count, hosts, count, out);
    /* ReviewCreate owns copied evidence. Release every imported report even if
     * a later host could not be read, leaving no partly admitted result. */
    for (size_t i = 0; i < count; ++i) UmiCtestReportDestroy(reports[i]);
    free(reports);
    free(hosts);
    return status;
}

/* Keep the complete file boundary shared by CLI and GTK hosts. Every input is
 * owned through parsing and released before an immutable review is returned. */
UmiStatus UmiCtestCoverageReviewReadFile(const char *absolute_manifest,
    const char *source_identity, UmiCtestCoverageReview **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!absolute_manifest || !source_identity || !umi_path_is_absolute(absolute_manifest))
        return UMI_STATUS_INVALID_ARGUMENT;
    char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    size_t n = 0;
    while (n < UMI_CTEST_COVERAGE_IDENTITY_CAPACITY && source_identity[n]) ++n;
    if (n == UMI_CTEST_COVERAGE_IDENTITY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiCtestEscapeName(source_identity, escaped, sizeof escaped);
    char directory[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(absolute_manifest);
    if (status == UMI_STATUS_OK)
        status = umi_path_parent(absolute_manifest, directory, sizeof directory);
    unsigned char *bytes = NULL;
    size_t length = 0;
    UmiCtestCoverageManifest *manifest = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(absolute_manifest, UMI_CSV_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK)
        status = UmiCtestCoverageManifestImport(bytes, length, &manifest);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK)
        status = UmiCtestCoverageManifestReviewFiles(manifest, directory, source_identity, out);
    UmiCtestCoverageManifestDestroy(manifest);
    return status;
}
