/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_sdk.c
 * PURPOSE: Explain a selected installed SDK without launching CMake or compiling code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/sdk/installation_inspection.h"
#include "umicom/platform/filesystem.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The CLI resolves a typed relative prefix once. The shared worker-facing
 * service receives an absolute path and never consults a later process cwd. */
int umi_cli_command_sdk(int argc, char **argv)
{
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
        (void)puts("Usage: umicom sdk inspect SDK_PREFIX");
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
