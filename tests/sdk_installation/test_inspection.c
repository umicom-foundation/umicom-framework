/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_inspection.c
 * PURPOSE: Distinguish SDK file presence, selected location and declared compatibility.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/sdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)
int main(int argc, char **argv)
{
    int failed = 0;
    UmiSdkInstallationObservation *observation = calloc(1U, sizeof *observation);
    CHECK(argc == 3 && observation != NULL);
    const char *name = argv[1], *prefix = argv[2];
    UmiStatus expected = strcmp(name, "no_metadata") == 0 || strcmp(name, "legacy") == 0
        ? UMI_STATUS_NOT_FOUND : strcmp(name, "malformed") == 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    CHECK(UmiSdkInstallationInspect(prefix, observation) == expected);
    int complete = strcmp(name, "complete") == 0 || strcmp(name, "lib64") == 0 ||
        strcmp(name, "multiarch") == 0 || strcmp(name, "old_version") == 0 ||
        strcmp(name, "low_abi") == 0;
    if (expected == UMI_STATUS_OK) {
        CHECK(observation->complete == complete);
        CHECK(observation->metadata.framework_version.major == 7U);
        if (strcmp(name, "lib64") == 0) CHECK(strstr(observation->package_directory, "lib64") != NULL);
        if (strcmp(name, "multiarch") == 0) CHECK(strstr(observation->package_directory, "cpu-linux") != NULL);
        if (strcmp(name, "library_directory") == 0) CHECK(!observation->base_library_present);
    }
    UmiSdkProbeResult probe;
    UmiStatus probeStatus = umi_sdk_probe_prefix(prefix, &probe);
    if (strcmp(name, "malformed") == 0) CHECK(probeStatus == UMI_STATUS_PARSE_ERROR);
    else {
        CHECK(probeStatus == UMI_STATUS_OK);
        CHECK(probe.passed == (complete || strcmp(name, "legacy") == 0));
    }
    UmiSdkRequirement requirement = {{7U, 12U, 3U}, 17U};
    UmiSdkValidationResult validation;
    CHECK(umi_sdk_validate_prefix(prefix, &requirement, &validation) == expected);
    if (expected == UMI_STATUS_OK) {
        int compatible = strcmp(name, "old_version") != 0 && strcmp(name, "low_abi") != 0;
        CHECK(validation.compatible == compatible);
        CHECK(validation.passed == (complete && compatible));
    } else CHECK(validation.passed == 0 && validation.compatible == 0);
    char located[UMI_PATH_CAPACITY];
    UmiStatus locatedStatus = umi_sdk_locate(prefix, located, sizeof located);
    if (complete || strcmp(name, "legacy") == 0) {
        CHECK(locatedStatus == UMI_STATUS_OK && strcmp(located, prefix) == 0);
    } else CHECK(locatedStatus != UMI_STATUS_OK && located[0] == '\0');
cleanup:
    free(observation);
    return failed;
}
