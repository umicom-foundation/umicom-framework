/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_path_assembly.c
 * PURPOSE: Exercise checked path construction without touching an installation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/package.h"
#include "umicom/sdk/installation.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)
int main(int argc, char **argv)
{
    int failed = 0;
    UmiSdkPackage *package = malloc(sizeof *package), *savedPackage = malloc(sizeof *savedPackage);
    UmiSdkInstallation *installation = malloc(sizeof *installation), *savedInstallation = malloc(sizeof *savedInstallation);
    CHECK(argc == 2 && package != NULL && savedPackage != NULL && installation != NULL && savedInstallation != NULL);
    memset(package, 0x5a, sizeof *package); *savedPackage = *package;
    memset(installation, 0x5a, sizeof *installation); *savedInstallation = *installation;
    const char *mode = argv[1];
    if (strcmp(mode, "normal") == 0) {
        CHECK(umi_sdk_package_from_prefix("sdk folder", package) == UMI_STATUS_OK);
        CHECK(umi_sdk_installation_from_prefix("sdk folder", installation) == UMI_STATUS_OK);
        CHECK(strcmp(package->prefix, "sdk folder") == 0 && strcmp(installation->prefix, "sdk folder") == 0);
        char expected[UMI_PATH_CAPACITY];
        CHECK(umi_path_join("sdk folder", "lib/cmake/UmicomFramework/UmicomFrameworkTargets.cmake",
            expected, sizeof expected) == UMI_STATUS_OK);
        CHECK(strcmp(package->targets_file, expected) == 0);
        CHECK(umi_path_join("sdk folder", "include", expected, sizeof expected) == UMI_STATUS_OK);
        CHECK(strcmp(installation->include_dir, expected) == 0);
    } else {
        char path[UMI_SDK_PATH_CAPACITY + 1U];
        memset(path, 'x', sizeof path - 1U); path[sizeof path - 1U] = '\0';
        const char *input = path;
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (strcmp(mode, "derived_capacity") == 0) path[UMI_SDK_PATH_CAPACITY - 5U] = '\0';
        else if (strcmp(mode, "empty") == 0) { input = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (strcmp(mode, "null") == 0) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else CHECK(strcmp(mode, "prefix_capacity") == 0);
        CHECK(umi_sdk_package_from_prefix(input, package) == expected);
        CHECK(umi_sdk_installation_from_prefix(input, installation) == expected);
        CHECK(memcmp(package, savedPackage, sizeof *package) == 0);
        CHECK(memcmp(installation, savedInstallation, sizeof *installation) == 0);
    }
cleanup:
    free(savedInstallation); free(installation); free(savedPackage); free(package);
    return failed;
}
