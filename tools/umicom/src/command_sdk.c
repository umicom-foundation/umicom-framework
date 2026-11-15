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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* SDK artifact evidence is read through the same Framework service available to
 * Studio. A missing or old receipt must not become a successful empty inventory. */
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
static int SdkFingerprintCommand(int argc, char **argv)
{
    char expected[UMI_SHA256_HEX_CAPACITY] = {0};
    if ((argc != 2 && argc != 4) || argv[1][0] == '\0' ||
        (argc == 4 && (strcmp(argv[2], "--expect") != 0 ||
            !SdkExpectedFingerprint(argv[3], expected)))) {
        (void)fputs("Usage: umicom sdk fingerprint SDK_PREFIX [--expect SHA256]\n", stderr);
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
    if (status == UMI_STATUS_OK)
        status = UmiSdkArtifactFingerprint(prefix, NULL, NULL, &observed);
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
    if (written < 0 || fflush(stdout) != 0 || ferror(stdout)) return 2;
    return matched ? 0 : 1;
}

/* The CLI resolves a typed relative prefix once. The shared worker-facing
 * service receives an absolute path and never consults a later process cwd. */
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
