/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_scan_cancellation.c
 *
 * PURPOSE:
 *   Verify cooperative CodeGuard boundaries using real scanning rules and deterministic cancellation requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/lifetime.h"
#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/pattern_scan.h"
#include "umicom/codeguard/rule_registry.h"
#include "umicom/platform/directory.h"
#include "../../src/codeguard/source_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; } } while (0)

static const char *scenario;
static UmiCancellationToken *scan_token;
static const UmiCancellationToken *forwarded_token;
static unsigned opens, closes, reads, patterns, architectures, duplicate_adds,
    duplicate_emits, walks, registries, registry_destroys;
static int walk_options_valid;

static void RequestAt(const char *phase)
{
    if (!strcmp(scenario, phase)) umi_cancellation_token_request(scan_token);
}

static FILE *ScanOpen(const char *path, const char *mode)
{
    ++opens;
    return fopen(path, mode);
}
static int ScanClose(FILE *file)
{
    ++closes;
    return fclose(file);
}
static char *ScanRead(char *buffer, int capacity, FILE *file)
{
    ++reads;
    char *line = fgets(buffer, capacity, file);
    RequestAt("read");
    RequestAt("read-error");
    return line;
}
static int ScanError(FILE *file)
{
    if (!strcmp(scenario, "read-error") && reads != 0U) return 1;
    return ferror(file);
}
static UmiStatus RegistryCreate(UmiCodeGuardRuleRegistry **out)
{
    ++registries;
    return umi_codeguard_rule_registry_create(out);
}
static void RegistryDestroy(UmiCodeGuardRuleRegistry *registry)
{
    ++registry_destroys;
    umi_codeguard_rule_registry_destroy(registry);
}
static UmiStatus RegistryBuiltin(UmiCodeGuardRuleRegistry *registry)
{
    UmiStatus status = umi_codeguard_rule_registry_add_builtin(registry);
    RequestAt("registry");
    return status;
}
static UmiStatus ScanPattern(const UmiCodeGuardRuleRegistry *registry,
    const char *path, size_t line, const char *raw, const char *code,
    UmiCodeGuardResult *result)
{
    ++patterns;
    UmiStatus status = umi_codeguard_pattern_scan_line(registry, path, line, raw, code, result);
    RequestAt("pattern");
    if (!strcmp(scenario, "pattern-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_IO_ERROR;
    }
    return status;
}
/* A failed rule must retain its own status when cancellation arrives too. */
static UmiStatus ScanLifetime(UmiCodeGuardLifetimeTracker *tracker,
    const char *path, size_t line, const char *text, UmiCodeGuardResult *result)
{
    UmiStatus status = umi_codeguard_lifetime_scan_checked(tracker, path, line, text, result);
    if (!strcmp(scenario, "lifetime-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    return status;
}
/* Forward the borrowed token through the native architecture operation while retaining the original phase seam for review. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus ScanArchitecture(const char *root, const char *path,
    const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result)
{
    ++architectures;
    UmiStatus status = umi_codeguard_architecture_scan_file(root, path, profile, result);
    RequestAt("architecture");
    return status;
}
#endif
static UmiStatus ScanArchitecture(const char *root, const char *path,
    const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation)
{
    ++architectures;
    UmiStatus status = umi_codeguard_architecture_scan_file_cancellable(
        root, path, profile, result, cancellation);
    RequestAt("architecture");
    return status;
}
/* Exercise bounded duplicate input and retain first-failure cancellation assertions. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus DuplicateAdd(UmiCodeGuardDuplicateSet *set, const char *path)
{
    ++duplicate_adds;
    UmiStatus status = umi_codeguard_duplicate_set_add(set, path);
    RequestAt("duplicate-add");
    if (!strcmp(scenario, "duplicate-add-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    return status;
}
#endif
static UmiStatus DuplicateAdd(UmiCodeGuardDuplicateSet *set, const char *path,
    uint64_t maximum, const UmiCancellationToken *cancellation)
{
    ++duplicate_adds;
    UmiStatus status = umi_codeguard_duplicate_set_add_bounded(set, path, maximum, cancellation);
    RequestAt("duplicate-add");
    if (!strcmp(scenario, "duplicate-add-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    return status;
}
static UmiStatus DuplicateEmit(const UmiCodeGuardDuplicateSet *set,
    UmiCodeGuardResult *result)
{
    ++duplicate_emits;
    UmiStatus status = umi_codeguard_duplicate_emit(set, result);
    RequestAt("duplicate-emit");
    if (!strcmp(scenario, "duplicate-emit-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return status;
}
static UmiStatus ScanWalk(const char *root, const UmiDirectoryWalkOptions *options,
    UmiDirectoryVisitor visitor, void *user, const UmiCancellationToken *token)
{
    ++walks;
    forwarded_token = token;
    walk_options_valid = options->recursive && options->include_files &&
        !options->include_directories && !options->include_hidden &&
        !options->follow_symbolic_links && options->max_depth == 64U;
    RequestAt("walk");
    if (!strcmp(scenario, "walk-error")) {
        umi_cancellation_token_request(scan_token);
        return UMI_STATUS_IO_ERROR;
    }
    return UmiDirectoryWalkCancellable(root, options, visitor, user, token);
}


/* Keep phase injection scoped to the first source scan. Architecture and
 * duplicate input use the real library reader; native reader qualification
 * covers actual operating-system failures separately. */
static UmiStatus NativeScanInput(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context, uint64_t *out_size)
{
    *out_size = 0U;
    FILE *file = ScanOpen(path, "rb");
    if (file == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_OK;
    if (fseek(file, 0L, SEEK_END) != 0) status = UMI_STATUS_IO_ERROR;
    long total = status == UMI_STATUS_OK ? ftell(file) : -1L;
    if (total < 0L) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK && (uint64_t)total > maximum)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && fseek(file, 0L, SEEK_SET) != 0)
        status = UMI_STATUS_IO_ERROR;
    char buffer[8192];
    while (status == UMI_STATUS_OK && ScanRead(buffer, sizeof buffer, file) != NULL) {
        if (ScanError(file)) { status = UMI_STATUS_IO_ERROR; break; }
        status = visitor((const unsigned char *)buffer, strlen(buffer), context);
    }
    if (ScanError(file) && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (ScanClose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *out_size = (uint64_t)total;
    return status;
}
#define UmiInputFileVisit NativeScanInput
#define umi_codeguard_source_visit_bytes CancellationVisitBytes
#define umi_codeguard_source_visit_lines CancellationVisitLines
#include "../../src/codeguard/source_reader.c"
#undef umi_codeguard_source_visit_lines
#undef umi_codeguard_source_visit_bytes
#undef UmiInputFileVisit

/* Include the production scanner once, redirecting only phase boundaries.
 * Rules, result storage, token atomics, directory traversal and duplicate
 * analysis remain the real Framework implementations. No sleeps or timing
 * assumptions are needed to exercise requests arriving during an operation. */
#define fopen ScanOpen
#define fclose ScanClose
#define fgets ScanRead
#define ferror ScanError
#define umi_codeguard_rule_registry_create RegistryCreate
#define umi_codeguard_rule_registry_destroy RegistryDestroy
#define umi_codeguard_rule_registry_add_builtin RegistryBuiltin
#define umi_codeguard_pattern_scan_line ScanPattern
#define umi_codeguard_lifetime_scan_checked ScanLifetime
#define umi_codeguard_architecture_scan_file_cancellable ScanArchitecture
#define umi_codeguard_duplicate_set_add_bounded DuplicateAdd
#define umi_codeguard_duplicate_emit DuplicateEmit
#define UmiDirectoryWalkCancellable ScanWalk
#define umi_codeguard_source_visit_lines CancellationVisitLines
#include "../../src/codeguard/scanner.c"
#undef umi_codeguard_source_visit_lines
#undef UmiDirectoryWalkCancellable
#undef umi_codeguard_duplicate_emit
#undef umi_codeguard_duplicate_set_add_bounded
#undef umi_codeguard_lifetime_scan_checked
#undef umi_codeguard_architecture_scan_file_cancellable
#undef umi_codeguard_pattern_scan_line
#undef umi_codeguard_rule_registry_add_builtin
#undef umi_codeguard_rule_registry_destroy
#undef umi_codeguard_rule_registry_create
#undef ferror
#undef fgets
#undef fclose
#undef fopen

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    scenario = argv[1];
    char path[UMI_PATH_CAPACITY];
    int length = snprintf(path, sizeof path, "%s/sample.c", argv[2]);
    CHECK(length > 0 && (size_t)length < sizeof path);
    FILE *fixture = fopen(path, "wb");
    CHECK(fixture != NULL);
    if (strcmp(scenario, "empty") != 0)
        CHECK(fputs("void example(void) {\n    malloc(4);\n}\n", fixture) >= 0);
    CHECK(fclose(fixture) == 0);
    CHECK(umi_cancellation_token_create(&scan_token) == UMI_STATUS_OK);
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(16U, &result) == UMI_STATUS_OK);
    UmiCodeGuardConfig config = umi_codeguard_config_default(argv[2]);
    config.profile.scan_architecture = 1;
    config.profile.scan_duplicates = 1;
    config.profile.scan_source_names = 0;
    config.profile.max_line_length = 4096U;
    UmiStatus status;
    if (!strcmp(scenario, "arguments")) {
        umi_cancellation_token_request(scan_token);
        CHECK(umi_codeguard_scan_cancellable(NULL, result, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scan_cancellable(&config, NULL, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scan_file_cancellable(&config, NULL, result, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scan_file_cancellable(NULL, path, result, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scan_file_cancellable(&config, path, NULL, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        config.root = NULL;
        CHECK(umi_codeguard_scan_cancellable(&config, result, scan_token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(opens == 0U && walks == 0U);
    } else if (!strcmp(scenario, "preflight")) {
        umi_cancellation_token_request(scan_token);
        CHECK(umi_codeguard_scan_cancellable(&config, result, scan_token) == UMI_STATUS_CANCELLED);
        CHECK(umi_codeguard_scan_file_cancellable(&config, path, result, scan_token) == UMI_STATUS_CANCELLED);
        CHECK(opens == 0U && walks == 0U && registries == 0U);
    } else if (!strcmp(scenario, "unsupported")) {
        CHECK(umi_codeguard_scan_file_cancellable(&config, "absent.txt", result, scan_token) == UMI_STATUS_OK);
        CHECK(opens == 0U);
        umi_cancellation_token_request(scan_token);
        CHECK(umi_codeguard_scan_file_cancellable(&config, "absent.txt", result, scan_token) == UMI_STATUS_CANCELLED);
    } else if (!strcmp(scenario, "missing")) {
        CHECK(umi_codeguard_scan_file_cancellable(&config, "missing/codeguard-source.c", result, scan_token) == UMI_STATUS_IO_ERROR);
        /* Registry ownership is established before the native visit; failure
         * still releases it. The original open-first assertion is retained. */
#if 0
        CHECK(opens == 1U && closes == 0U && registries == 0U);
#endif
        CHECK(opens == 1U && closes == 0U && registries == 1U && registry_destroys == 1U);
    } else if (!strcmp(scenario, "null-token") || !strcmp(scenario, "legacy")) {
        umi_cancellation_token_request(scan_token);
        status = !strcmp(scenario, "legacy") ? umi_codeguard_scan(&config, result)
            : umi_codeguard_scan_cancellable(&config, result, NULL);
        CHECK(status == UMI_STATUS_OK && walks == 1U && forwarded_token == NULL);
        CHECK(opens == 1U && closes == 1U && patterns == 3U);
        CHECK(architectures == 1U && duplicate_adds == 1U && duplicate_emits == 1U);
    } else if (!strcmp(scenario, "file-legacy")) {
        umi_cancellation_token_request(scan_token);
        CHECK(umi_codeguard_scan_file(&config, path, result) == UMI_STATUS_OK);
        CHECK(walks == 0U && patterns == 3U && architectures == 1U);
    } else {
        status = umi_codeguard_scan_cancellable(&config, result, scan_token);
        CHECK(walks == 1U && forwarded_token == scan_token && walk_options_valid);
        if (!strcmp(scenario, "walk-error") || !strcmp(scenario, "pattern-error") ||
            !strcmp(scenario, "read-error"))
            CHECK(status == UMI_STATUS_IO_ERROR);
        else if (!strcmp(scenario, "duplicate-emit-error"))
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        else if (!strcmp(scenario, "lifetime-error") || !strcmp(scenario, "duplicate-add-error"))
            CHECK(status == UMI_STATUS_OUT_OF_MEMORY);
        else if (!strcmp(scenario, "complete") || !strcmp(scenario, "empty"))
            CHECK(status == UMI_STATUS_OK);
        else CHECK(status == UMI_STATUS_CANCELLED);
        if (!strcmp(scenario, "walk") || !strcmp(scenario, "walk-error")) {
            CHECK(opens == 0U && duplicate_emits == 0U);
        } else {
            /* Cancellation during registry creation now avoids opening input. */
#if 0
            CHECK(opens == 1U && closes == 1U);
#endif
            CHECK(opens == (!strcmp(scenario, "registry") ? 0U : 1U));
            CHECK(closes == opens);
            CHECK(registries == 1U && registry_destroys == 1U);
        }
        if (!strcmp(scenario, "registry")) CHECK(reads == 0U && patterns == 0U);
        if (!strcmp(scenario, "read") || !strcmp(scenario, "read-error"))
            CHECK(reads == 1U && patterns == 0U);
        if (!strcmp(scenario, "pattern") || !strcmp(scenario, "pattern-error"))
            CHECK(reads == 1U && patterns == 1U && architectures == 0U);
        if (!strcmp(scenario, "lifetime-error"))
            CHECK(reads == 1U && patterns == 1U && architectures == 0U && duplicate_emits == 0U);
        if (!strcmp(scenario, "architecture"))
            CHECK(architectures == 1U && duplicate_adds == 0U);
        if (!strcmp(scenario, "duplicate-add") || !strcmp(scenario, "duplicate-add-error"))
            CHECK(duplicate_adds == 1U && duplicate_emits == 0U);
        if (!strcmp(scenario, "duplicate-emit") || !strcmp(scenario, "duplicate-emit-error"))
            CHECK(duplicate_adds == 1U && duplicate_emits == 1U);
        if (!strcmp(scenario, "complete"))
            CHECK(patterns == 3U && architectures == 1U && duplicate_emits == 1U);
        if (!strcmp(scenario, "empty")) CHECK(patterns == 0U && reads == 1U);
    }
    umi_codeguard_result_destroy(result);
    umi_cancellation_token_destroy(scan_token);
    CHECK(remove(path) == 0);
    return 0;
}
