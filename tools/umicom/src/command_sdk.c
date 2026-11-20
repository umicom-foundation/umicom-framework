/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_sdk.c
 *
 * PURPOSE:
 *   Inspect SDK presence and receipt-bound contents through shared native services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/sdk/installation_inspection.h"
#include "umicom/sdk/artifact_inventory.h"
#include "umicom/sdk/artifact_fingerprint.h"
#include "umicom/platform/filesystem.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* A report is complete only after the C stream accepts its buffered bytes.
 * Preserve content-specific exit codes, but give delivery failures precedence.
 * This does not promise disk durability or that a pipe consumer accepted data. */
static int SdkReportResult(int content_result, int output_failed)
{
    int flush_failed = fflush(stdout) != 0;
    int stream_failed = ferror(stdout) != 0;
    return output_failed || flush_failed || stream_failed ? 2 : content_result;
}

/* SDK artifact evidence is read through the same Framework service available to
 * Studio. A missing or old receipt must not become a successful empty inventory. */
/* Track report delivery separately from SDK artifact presence so redirected output failures cannot report success. The former implementation is retained for owner review. */
#if 0
static int SdkArtifactCommand(int argc, char **argv)
{
    if (argc < 2 || argc > 3 || argv[1][0] == '\0' ||
        (argc == 3 && strcmp(argv[2], "--all") != 0)) {
        (void)fputs("Usage: umicom sdk artifacts SDK_PREFIX [--all]\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[1])) status = umi_path_normalise(argv[1], prefix, sizeof prefix);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK) status = umi_path_absolute(argv[1], current, prefix, sizeof prefix);
    }
    UmiSdkArtifactInventory *inventory = NULL;
    if (status == UMI_STATUS_OK) status = UmiSdkArtifactInventoryInspect(prefix, &inventory);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect SDK artifact receipt: %s. "
            "Select an installation containing sdk-artifacts.txt; older SDKs may not include it.\n",
            umi_status_text(status));
        return 2;
    }
    UmiSdkArtifactSummary summary;
    status = UmiSdkArtifactInventoryReadSummary(inventory, &summary);
    if (status != UMI_STATUS_OK) { UmiSdkArtifactInventoryDestroy(inventory); return 2; }
    (void)printf("Declared configuration: %s\nRecords: %zu | Files: %zu | Present: %zu | Missing: %zu\n",
        summary.configuration[0] == '\0' ? "(unspecified)" : summary.configuration,
        summary.entries, summary.files, summary.present, summary.missing);
    for (size_t index = 0U; index < summary.entries; ++index) {
        const UmiSdkArtifactEntry *entry = UmiSdkArtifactInventoryEntryAt(inventory, index);
        bool is_interface = entry->kind == UMI_SDK_ARTIFACT_INTERFACE;
        if (argc != 3 && (is_interface || entry->present)) continue;
        (void)printf("[%s] %s : %s\n", is_interface ? "interface" : entry->present ? "present" : "missing",
            entry->target, entry->relative_path);
    }
    (void)puts("Declared file-presence evidence only. Unlisted exports, contents, ABI and runtime behavior are not verified.");
    if (summary.files == 0U) (void)puts("No physical artifacts declared; this is not a qualified SDK.");
    int result = summary.inspected && summary.files != 0U && summary.missing == 0U ? 0 : 1;
    UmiSdkArtifactInventoryDestroy(inventory);
    return result;
}

#endif
static int SdkArtifactCommand(int argc, char **argv)
{
    if (argc < 2 || argc > 3 || argv[1][0] == '\0' ||
        (argc == 3 && strcmp(argv[2], "--all") != 0)) {
        (void)fputs("Usage: umicom sdk artifacts SDK_PREFIX [--all]\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[1])) status = umi_path_normalise(argv[1], prefix, sizeof prefix);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK) status = umi_path_absolute(argv[1], current, prefix, sizeof prefix);
    }
    UmiSdkArtifactInventory *inventory = NULL;
    if (status == UMI_STATUS_OK) status = UmiSdkArtifactInventoryInspect(prefix, &inventory);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect SDK artifact receipt: %s. "
            "Select an installation containing sdk-artifacts.txt; older SDKs may not include it.\n",
            umi_status_text(status));
        return 2;
    }
    UmiSdkArtifactSummary summary;
    status = UmiSdkArtifactInventoryReadSummary(inventory, &summary);
    if (status != UMI_STATUS_OK) { UmiSdkArtifactInventoryDestroy(inventory); return 2; }
    int output_failed = 0;
    if (printf("Declared configuration: %s\nRecords: %zu | Files: %zu | Present: %zu | Missing: %zu\n",
        summary.configuration[0] == '\0' ? "(unspecified)" : summary.configuration,
        summary.entries, summary.files, summary.present, summary.missing) < 0) output_failed = 1;
    for (size_t index = 0U; index < summary.entries; ++index) {
        const UmiSdkArtifactEntry *entry = UmiSdkArtifactInventoryEntryAt(inventory, index);
        bool is_interface = entry->kind == UMI_SDK_ARTIFACT_INTERFACE;
        if (argc != 3 && (is_interface || entry->present)) continue;
        if (printf("[%s] %s : %s\n", is_interface ? "interface" : entry->present ? "present" : "missing",
            entry->target, entry->relative_path) < 0) output_failed = 1;
    }
    if (puts("Declared file-presence evidence only. Unlisted exports, contents, ABI and runtime behavior are not verified.") < 0) output_failed = 1;
    if (summary.files == 0U) if (puts("No physical artifacts declared; this is not a qualified SDK.") < 0) output_failed = 1;
    int result = summary.inspected && summary.files != 0U && summary.missing == 0U ? 0 : 1;
    UmiSdkArtifactInventoryDestroy(inventory);
    return SdkReportResult(result, output_failed);
}


/* The expected digest is data, never a command or a downloaded trust claim.
 * Accept either hex case while requiring exactly one complete SHA-256. */
static int SdkExpectedFingerprint(const char *text, char out[UMI_SHA256_HEX_CAPACITY])
{
    if (text == NULL || strlen(text) != UMI_SHA256_HEX_CAPACITY - 1U) return 0;
    for (size_t i = 0U; i < UMI_SHA256_HEX_CAPACITY - 1U; ++i) {
        char c = text[i];
        if (c >= 'A' && c <= 'F') c = (char)(c - 'A' + 'a');
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return 0;
        out[i] = c;
    }
    out[UMI_SHA256_HEX_CAPACITY - 1U] = '\0';
    return 1;
}

#define UMI_SDK_FINGERPRINT_ARGUMENTS \
    "umicom sdk fingerprint SDK_PREFIX [--expect SHA256] [--max-file-bytes N] [--max-total-bytes N]"

/* Accept exact decimal byte counts for reproducible CLI policies. Explicit
 * zero is a real ceiling. The shared SDK service still owns read enforcement. */
static bool SdkFingerprintBudget(const char *text, uint64_t ceiling, uint64_t *out_bytes)
{
    if (*text == '\0') return false;
    for (const char *digit = text; *digit != '\0'; ++digit) {
        if (*digit < '0' || *digit > '9') return false;
    }
    errno = 0;
    uintmax_t bytes = strtoumax(text, NULL, 10);
    if (errno == ERANGE || bytes > ceiling) return false;
    *out_bytes = (uint64_t)bytes;
    return true;
}

/* Parse each optional pair once. All validation finishes before the command
 * resolves the prefix or observes any files from the receipt. */
static bool SdkFingerprintOptions(int argc, char **argv,
    char expected[UMI_SHA256_HEX_CAPACITY], UmiSdkFingerprintLimits *limits)
{
    if (argc < 2 || argc > 8 || argc % 2 != 0 || argv[1][0] == '\0') return false;
    bool file_set = false, total_set = false;
    for (int i = 2; i < argc; i += 2) {
        const char *option = argv[i], *value = argv[i + 1];
        if (strcmp(option, "--expect") == 0) {
            if (expected[0] != '\0' || !SdkExpectedFingerprint(value, expected)) return false;
        } else if (strcmp(option, "--max-file-bytes") == 0) {
            if (file_set || !SdkFingerprintBudget(value, UMI_SDK_FINGERPRINT_FILE_LIMIT,
                    &limits->maximum_file_bytes)) return false;
            file_set = true;
        } else if (strcmp(option, "--max-total-bytes") == 0) {
            if (total_set || !SdkFingerprintBudget(value, UMI_SDK_FINGERPRINT_TOTAL_LIMIT,
                    &limits->maximum_total_bytes)) return false;
            total_set = true;
        } else {
            return false;
        }
    }
    return true;
}

static int SdkFingerprintCommand(int argc, char **argv)
{
    char expected[UMI_SHA256_HEX_CAPACITY] = {0};
    /* Named byte ceilings extend the previous digest-only syntax. Keep the
     * former argument handling for owner review and preserve its valid calls. */
#if 0
    if ((argc != 2 && argc != 4) || argv[1][0] == '\0' ||
        (argc == 4 && (strcmp(argv[2], "--expect") != 0 ||
            !SdkExpectedFingerprint(argv[3], expected)))) {
        (void)fputs("Usage: umicom sdk fingerprint SDK_PREFIX [--expect SHA256]\n", stderr);
        return 2;
    }
#endif
    UmiSdkFingerprintLimits limits = {UMI_SDK_FINGERPRINT_FILE_LIMIT, UMI_SDK_FINGERPRINT_TOTAL_LIMIT};
    if (!SdkFingerprintOptions(argc, argv, expected, &limits)) {
        (void)fputs("Usage: " UMI_SDK_FINGERPRINT_ARGUMENTS "\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[1])) status = umi_path_normalise(argv[1], prefix, sizeof prefix);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK) status = umi_path_absolute(argv[1], current, prefix, sizeof prefix);
    }
    UmiSdkFingerprint observed;
    /* Pass the caller's policy to the same service Studio can use. Preserve
     * the former default-only call for review of compatibility at the boundary. */
#if 0
    if (status == UMI_STATUS_OK)
        status = UmiSdkArtifactFingerprint(prefix, NULL, NULL, &observed);
#endif
    if (status == UMI_STATUS_OK)
        status = UmiSdkArtifactFingerprint(prefix, &limits, NULL, &observed);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot fingerprint SDK artifacts: %s. No complete fingerprint was produced.\n",
            umi_status_text(status));
        return 2;
    }
    int matched = expected[0] == '\0' || strcmp(expected, observed.sha256) == 0;
    int written = printf("Fingerprint schema: umicom.sdk.fingerprint/1\n"
        "Declared configuration: %s\nRecords: %zu | Files: %zu | Bytes: %" PRIu64 "\n"
        "Receipt SHA-256: %s\nFingerprint SHA-256: %s\n"
        "Expected fingerprint: %s\n"
        "Content evidence only. Not authentication, an atomic snapshot, ABI or runtime qualification.\n",
        observed.configuration[0] == '\0' ? "(unspecified)" : observed.configuration,
        observed.records, observed.files, observed.bytes, observed.receipt_sha256, observed.sha256,
        expected[0] == '\0' ? "not supplied" : matched ? "match" : "MISMATCH");
    /* A redirected report that could not be delivered is an I/O failure even
     * when the file comparison itself completed. Do not report a successful CLI. */
/* Use the same checked output completion for every SDK report. The former implementation is retained for owner review. */
#if 0
    if (written < 0 || fflush(stdout) != 0 || ferror(stdout)) return 2;
    return matched ? 0 : 1;
#endif
    return SdkReportResult(matched ? 0 : 1, written < 0);
}

/* The CLI resolves a typed relative prefix once. The shared worker-facing
 * service receives an absolute path and never consults a later process cwd. */
/* Keep SDK presence inspection read-only while publishing a successful result only after report delivery. The former implementation is retained for owner review. */
#if 0
int umi_cli_command_sdk(int argc, char **argv)
{
    if (argc > 0 && strcmp(argv[0], "artifacts") == 0) return SdkArtifactCommand(argc, argv);
    if (argc > 0 && strcmp(argv[0], "fingerprint") == 0) return SdkFingerprintCommand(argc, argv);
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
/* Keep the earlier help wording for API review; the artifact reader extends this command. */
#if 0
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX");
#else
/* Preserve the earlier two-operation help while exposing additive content evidence. */
#if 0
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX\n"
            "       umicom sdk artifacts SDK_PREFIX [--all]");
#else
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX\n"
            "       umicom sdk artifacts SDK_PREFIX [--all]\n"
            "       umicom sdk fingerprint SDK_PREFIX [--expect SHA256]");
#endif
#endif
        return 0;
    }
    if (argc != 2 || strcmp(argv[0], "inspect") != 0 || argv[1][0] == '\0') {
        (void)fputs("Usage: umicom sdk inspect SDK_PREFIX\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[1]))
        status = umi_path_normalise(argv[1], prefix, sizeof prefix);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(argv[1], current, prefix, sizeof prefix);
    }
    UmiSdkInstallationObservation *observation = calloc(1U, sizeof *observation);
    if (status == UMI_STATUS_OK && observation == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = UmiSdkInstallationInspect(prefix, observation);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect SDK: %s\n", umi_status_text(status));
        if (status == UMI_STATUS_NOT_FOUND)
            (void)fputs("The selected prefix has no SDK metadata. Install the updated Framework SDK into that prefix.\n", stderr);
        free(observation); return 1;
    }
    (void)printf("SDK prefix: %s\nDeclared Framework: %u.%u.%u\nDeclared ABI: %" PRIu32 "\n"
        "Package directory: %s\nPublic header: %s\nBase library: %s\n"
        "Package configuration: %s\nExported targets: %s\nPackage version: %s\n"
        "Public header present: %s\nBase library present: %s\n",
        observation->prefix,
        (unsigned)observation->metadata.framework_version.major,
        (unsigned)observation->metadata.framework_version.minor,
        (unsigned)observation->metadata.framework_version.patch,
        observation->metadata.framework_abi, observation->package_directory,
        observation->public_header, observation->base_library,
        observation->package_config ? "yes" : "no",
        observation->package_targets ? "yes" : "no",
        observation->package_version ? "yes" : "no",
        observation->header_present ? "yes" : "no",
        observation->base_library_present ? "yes" : "no");
    (void)puts("File-presence evidence only. No package scripts, compiler, linker or application were executed.");
    int exit_code = observation->complete ? 0 : 1;
    free(observation);
    return exit_code;
}

#endif
int umi_cli_command_sdk(int argc, char **argv)
{
    int output_failed = 0;
    if (argc > 0 && strcmp(argv[0], "artifacts") == 0) return SdkArtifactCommand(argc, argv);
    if (argc > 0 && strcmp(argv[0], "fingerprint") == 0) return SdkFingerprintCommand(argc, argv);
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
/* Keep the earlier help wording for API review; the artifact reader extends this command. */
#if 0
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX");
#else
/* Preserve the earlier two-operation help while exposing additive content evidence. */
#if 0
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX\n"
            "       umicom sdk artifacts SDK_PREFIX [--all]");
#else
        /* Expose the optional ceilings in checked SDK help. Preserve the
         * prior digest-only synopsis for owner review. */
#if 0
        if (puts("Usage: umicom sdk inspect SDK_PREFIX\n"
            "       umicom sdk artifacts SDK_PREFIX [--all]\n"
            "       umicom sdk fingerprint SDK_PREFIX [--expect SHA256]" ) == EOF) output_failed = 1;
#endif
        if (puts("Usage: umicom sdk inspect SDK_PREFIX\n"
            "       umicom sdk artifacts SDK_PREFIX [--all]\n"
            "       " UMI_SDK_FINGERPRINT_ARGUMENTS) == EOF) output_failed = 1;
#endif
#endif
        return SdkReportResult(0, output_failed);
    }
    if (argc != 2 || strcmp(argv[0], "inspect") != 0 || argv[1][0] == '\0') {
        (void)fputs("Usage: umicom sdk inspect SDK_PREFIX\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[1]))
        status = umi_path_normalise(argv[1], prefix, sizeof prefix);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(argv[1], current, prefix, sizeof prefix);
    }
    UmiSdkInstallationObservation *observation = calloc(1U, sizeof *observation);
    if (status == UMI_STATUS_OK && observation == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = UmiSdkInstallationInspect(prefix, observation);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect SDK: %s\n", umi_status_text(status));
        if (status == UMI_STATUS_NOT_FOUND)
            (void)fputs("The selected prefix has no SDK metadata. Install the updated Framework SDK into that prefix.\n", stderr);
        free(observation); return 1;
    }
    if (printf("SDK prefix: %s\nDeclared Framework: %u.%u.%u\nDeclared ABI: %" PRIu32 "\n"
        "Package directory: %s\nPublic header: %s\nBase library: %s\n"
        "Package configuration: %s\nExported targets: %s\nPackage version: %s\n"
        "Public header present: %s\nBase library present: %s\n",
        observation->prefix,
        (unsigned)observation->metadata.framework_version.major,
        (unsigned)observation->metadata.framework_version.minor,
        (unsigned)observation->metadata.framework_version.patch,
        observation->metadata.framework_abi, observation->package_directory,
        observation->public_header, observation->base_library,
        observation->package_config ? "yes" : "no",
        observation->package_targets ? "yes" : "no",
        observation->package_version ? "yes" : "no",
        observation->header_present ? "yes" : "no",
        observation->base_library_present ? "yes" : "no") < 0) output_failed = 1;
    if (puts("File-presence evidence only. No package scripts, compiler, linker or application were executed.") == EOF) output_failed = 1;
    int exit_code = observation->complete ? 0 : 1;
    free(observation);
    return SdkReportResult(exit_code, output_failed);
}
