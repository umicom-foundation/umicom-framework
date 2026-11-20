/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_manifest.c
 *
 * PURPOSE:
 *   Admit explicit host-coverage plans through Framework's bounded CSV reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "ctest_coverage_manifest_private.h"
#include "umicom/base/csv_reader.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* The original fields retain their order. The extended header appends one
 * digest after the relative report path. */
enum { COVERAGE_MANIFEST_STANDARD_COLUMNS = 7 };

static UmiStatus Copy(const UmiCsvReadRow *row, size_t field, char *out,
    size_t capacity, bool optional, bool path)
{
    size_t length = row->lengths[field];
    if (length == 0 && !optional) return UMI_STATUS_PARSE_ERROR;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *value = row->fields[field];
    /* The reader already validates UTF-8. Disallow line/control content even
     * inside quoted fields: identity and path cells are not free-form prose. */
    for (size_t i = 0; i < length; ++i)
        if ((unsigned char)value[i] < 32U || (unsigned char)value[i] == 127U)
            return UMI_STATUS_PARSE_ERROR;
    if (length != 0 && !path) {
        char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
        UmiStatus status = UmiCtestEscapeName(value, escaped, sizeof escaped);
        if (status != UMI_STATUS_OK) return status;
    }
    memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus RelativePath(const char *value)
{
    if (!value[0] || value[0] == '/' || value[0] == '\\' || strchr(value, ':') ||
        umi_path_is_absolute(value)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t start = 0;
    for (size_t i = 0;; ++i) {
        if (value[i] != '/' && value[i] != '\\' && value[i] != '\0') continue;
        size_t size = i - start;
        if (size == 0 || (size == 1 && value[start] == '.') ||
            (size == 2 && value[start] == '.' && value[start + 1] == '.'))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (!value[i]) return UMI_STATUS_OK;
        start = i + 1U;
    }
}

/* Capacity grows only after a complete row has been admitted. Retain the old
 * allocation on failure; no borrowed CSV pointer escapes the current record. */
static void *Grow(void *old, size_t count, size_t *capacity, size_t maximum,
    size_t item_size, UmiStatus *status)
{
    if (count < *capacity) return old;
    if (count >= maximum) { *status = UMI_STATUS_CAPACITY_EXCEEDED; return old; }
    size_t next = *capacity ? *capacity * 2U : 16U;
    if (next > maximum) next = maximum;
    if (next > SIZE_MAX / item_size) { *status = UMI_STATUS_CAPACITY_EXCEEDED; return old; }
    void *memory = realloc(old, next * item_size);
    if (!memory) { *status = UMI_STATUS_OUT_OF_MEMORY; return old; }
    *capacity = next;
    return memory;
}

/* Store only a complete canonical digest. Grow uses the host bound and the
 * slot is unpublished until the corresponding host row is also accepted. */
static UmiStatus AddReportDigest(UmiCtestCoverageManifest *manifest, const UmiCsvReadRow *row)
{
    const size_t field = COVERAGE_MANIFEST_STANDARD_COLUMNS;
    const char *digest = row->fields[field];
    const size_t digits = UMI_SHA256_HEX_CAPACITY - 1U;
    if (row->lengths[field] != digits) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < digits; ++i)
        if (!((digest[i] >= '0' && digest[i] <= '9') || (digest[i] >= 'a' && digest[i] <= 'f')))
            return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UMI_STATUS_OK;
    manifest->report_digests = Grow(manifest->report_digests, manifest->host_count,
        &manifest->digest_capacity, UMI_CTEST_COVERAGE_MAX_HOSTS,
        sizeof *manifest->report_digests, &status);
    if (status == UMI_STATUS_OK)
        memcpy(manifest->report_digests[manifest->host_count], digest, UMI_SHA256_HEX_CAPACITY);
    return status;
}

static UmiStatus AddHost(UmiCtestCoverageManifest *manifest, const UmiCsvReadRow *row)
{
    if (row->lengths[4] || row->lengths[5]) return UMI_STATUS_PARSE_ERROR;
    UmiCtestCoverageHostFile host = {0};
    UmiStatus status = Copy(row, 1, host.host, sizeof host.host, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 2, host.configuration, sizeof host.configuration, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 3, host.source_identity, sizeof host.source_identity, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 6, host.report, sizeof host.report, false, true);
    if (status == UMI_STATUS_OK) status = RelativePath(host.report);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0; i < manifest->host_count; ++i)
        if (!strcmp(host.host, manifest->hosts[i].host) &&
            !strcmp(host.configuration, manifest->hosts[i].configuration))
            return UMI_STATUS_ALREADY_EXISTS;
    manifest->hosts = Grow(manifest->hosts, manifest->host_count, &manifest->host_capacity,
        UMI_CTEST_COVERAGE_MAX_HOSTS, sizeof *manifest->hosts, &status);
    /* Host and digest rows are published together. A failed import destroys
     * both owned arrays, including any reserved but unpublished slot. */
    if (status == UMI_STATUS_OK && row->count > COVERAGE_MANIFEST_STANDARD_COLUMNS)
        status = AddReportDigest(manifest, row);
    if (status == UMI_STATUS_OK) manifest->hosts[manifest->host_count++] = host;
    return status;
}

static UmiStatus AddRequirement(UmiCtestCoverageManifest *manifest, const UmiCsvReadRow *row)
{
    if (row->lengths[3] || row->lengths[6]) return UMI_STATUS_PARSE_ERROR;
    /* A requirement points to a host. Only the host row owns report bytes. */
    if (row->count > COVERAGE_MANIFEST_STANDARD_COLUMNS &&
        row->lengths[COVERAGE_MANIFEST_STANDARD_COLUMNS] != 0U) return UMI_STATUS_PARSE_ERROR;
    UmiCtestCoverageRequirement requirement = {0};
    UmiStatus status = Copy(row, 1, requirement.host, sizeof requirement.host, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 2, requirement.configuration, sizeof requirement.configuration, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 4, requirement.name, sizeof requirement.name, false, false);
    if (status == UMI_STATUS_OK) status = Copy(row, 5, requirement.capability, sizeof requirement.capability, true, false);
    if (status != UMI_STATUS_OK) return status;
    manifest->requirements = Grow(manifest->requirements, manifest->requirement_count,
        &manifest->requirement_capacity, UMI_CTEST_COVERAGE_MAX_REQUIREMENTS,
        sizeof *manifest->requirements, &status);
    if (status == UMI_STATUS_OK)
        manifest->requirements[manifest->requirement_count++] = requirement;
    return status;
}

static int Order(const void *left, const void *right)
{
    const UmiCtestCoverageRequirement *const *a = left;
    const UmiCtestCoverageRequirement *const *b = right;
    int order = strcmp((*a)->host, (*b)->host);
    if (!order) order = strcmp((*a)->configuration, (*b)->configuration);
    return order ? order : strcmp((*a)->name, (*b)->name);
}

static UmiStatus UniqueRequirements(const UmiCtestCoverageManifest *manifest)
{
    size_t count = manifest->requirement_count;
    if (count < 2) return UMI_STATUS_OK;
    const UmiCtestCoverageRequirement **sorted = malloc(count * sizeof *sorted);
    if (!sorted) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0; i < count; ++i) sorted[i] = &manifest->requirements[i];
    qsort(sorted, count, sizeof *sorted, Order);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 1; i < count; ++i)
        if (Order(&sorted[i - 1], &sorted[i]) == 0) { status = UMI_STATUS_ALREADY_EXISTS; break; }
    free(sorted);
    return status;
}

UmiStatus UmiCtestCoverageManifestImport(const void *bytes, size_t length,
    UmiCtestCoverageManifest **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiCsvReader *reader = NULL;
    UmiStatus status = UmiCsvReaderCreate(bytes, length, &reader);
    if (status != UMI_STATUS_OK) return status;
    UmiCtestCoverageManifest *manifest = calloc(1U, sizeof *manifest);
    if (!manifest) { UmiCsvReaderDestroy(reader); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiCsvReadRow row;
    const char *columns[] = {"kind", "host", "configuration", "source_identity", "test", "capability", "report"};
    size_t manifest_columns = COVERAGE_MANIFEST_STANDARD_COLUMNS;
    status = UmiCsvReaderNext(reader, &row);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        /* Retain fixed-width admission for review. The optional exact digest
         * suffix adds a second known width while preserving the same prefix. */
#if 0
        if (row.count != 7) status = UMI_STATUS_PARSE_ERROR;
        else for (size_t i = 0; i < 7; ++i)
            if (strcmp(row.fields[i], columns[i])) { status = UMI_STATUS_PARSE_ERROR; break; }
#endif
        if (row.count == COVERAGE_MANIFEST_STANDARD_COLUMNS + 1U &&
            strcmp(row.fields[COVERAGE_MANIFEST_STANDARD_COLUMNS], "report_sha256") == 0)
            manifest_columns = row.count;
        if (row.count != manifest_columns) status = UMI_STATUS_PARSE_ERROR;
        else for (size_t i = 0U; i < COVERAGE_MANIFEST_STANDARD_COLUMNS; ++i)
            if (strcmp(row.fields[i], columns[i])) { status = UMI_STATUS_PARSE_ERROR; break; }
    }
    while (status == UMI_STATUS_OK) {
        status = UmiCsvReaderNext(reader, &row);
        if (status == UMI_STATUS_NOT_FOUND) { status = UniqueRequirements(manifest); break; }
        if (status != UMI_STATUS_OK) break;
        /* Retain the former seven-column row guard for review. Every record
         * now has to follow the single width established by the header. */
#if 0
        if (row.count != 7) status = UMI_STATUS_PARSE_ERROR;
#endif
        if (row.count != manifest_columns) status = UMI_STATUS_PARSE_ERROR;
        else if (!strcmp(row.fields[0], "host")) status = AddHost(manifest, &row);
        else if (!strcmp(row.fields[0], "require")) status = AddRequirement(manifest, &row);
        else status = UMI_STATUS_PARSE_ERROR;
    }
    UmiCsvReaderDestroy(reader);
    if (status == UMI_STATUS_OK) *out = manifest;
    else UmiCtestCoverageManifestDestroy(manifest);
    return status;
}

size_t UmiCtestCoverageManifestHostCount(const UmiCtestCoverageManifest *manifest)
{ return manifest ? manifest->host_count : 0; }
size_t UmiCtestCoverageManifestRequirementCount(const UmiCtestCoverageManifest *manifest)
{ return manifest ? manifest->requirement_count : 0; }
const UmiCtestCoverageHostFile *UmiCtestCoverageManifestHostAt(
    const UmiCtestCoverageManifest *manifest, size_t index)
{ return manifest && index < manifest->host_count ? &manifest->hosts[index] : NULL; }
const UmiCtestCoverageRequirement *UmiCtestCoverageManifestRequirementAt(
    const UmiCtestCoverageManifest *manifest, size_t index)
{ return manifest && index < manifest->requirement_count ? &manifest->requirements[index] : NULL; }
void UmiCtestCoverageManifestDestroy(UmiCtestCoverageManifest *manifest)
{
    if (!manifest) return;
    free(manifest->hosts);
    free(manifest->report_digests); /* Digest strings belong to the parsed plan. */
    free(manifest->requirements);
    free(manifest);
}
