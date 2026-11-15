/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_conformance_evidence.c
 *
 * PURPOSE:
 *   Exercise SDK evidence invariants, checked counter admission, compatibility refusal and complete report publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk/conformance.h"
#include "umicom/sdk/report.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiSdkConformance state = {0};
    UmiSdkProbeResult probe = {1, 1, 1, 1};
    char output[256], before[256];
    memset(output, 0xa5, sizeof output);
    memcpy(before, output, sizeof output);
    if (strcmp(mode, "null") == 0) {
        CHECK(umi_sdk_conformance_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_sdk_conformance_record_checked(NULL, 1) == UMI_STATUS_INVALID_ARGUMENT);
        umi_sdk_conformance_record(NULL, 1);
        CHECK(!umi_sdk_conformance_passed(NULL));
    } else if (strcmp(mode, "empty") == 0) {
        CHECK(umi_sdk_conformance_validate(&state) == UMI_STATUS_OK);
        CHECK(!umi_sdk_conformance_passed(&state));
    } else if (strcmp(mode, "pass") == 0 || strcmp(mode, "fail") == 0 ||
               strcmp(mode, "truthy") == 0 || strcmp(mode, "mixed") == 0) {
        int passed = strcmp(mode, "fail") == 0 ? 0 : strcmp(mode, "truthy") == 0 ? -7 : 1;
        CHECK(umi_sdk_conformance_record_checked(&state, passed) == UMI_STATUS_OK);
        CHECK(state.checks_run == 1U && state.checks_passed == (size_t)(passed != 0) &&
              state.checks_failed == (size_t)(passed == 0));
        if (strcmp(mode, "mixed") == 0) {
            CHECK(umi_sdk_conformance_record_checked(&state, 0) == UMI_STATUS_OK);
            CHECK(state.checks_run == 2U && state.checks_passed == 1U && state.checks_failed == 1U);
            passed = 0;
        }
        CHECK(umi_sdk_conformance_passed(&state) == (passed != 0));
    } else if (strcmp(mode, "last-pass") == 0 || strcmp(mode, "last-fail") == 0) {
        state = (UmiSdkConformance){SIZE_MAX - 1U, SIZE_MAX - 1U, 0U};
        int passed = strcmp(mode, "last-pass") == 0;
        CHECK(umi_sdk_conformance_record_checked(&state, passed) == UMI_STATUS_OK);
        CHECK(state.checks_run == SIZE_MAX && state.checks_failed == (size_t)!passed);
        CHECK(state.checks_passed == SIZE_MAX - (size_t)!passed);
        CHECK(umi_sdk_conformance_validate(&state) == UMI_STATUS_OK);
        CHECK(umi_sdk_conformance_passed(&state) == passed);
    } else if (strncmp(mode, "exhausted-", 10U) == 0) {
        if (strcmp(mode, "exhausted-pass") == 0) state = (UmiSdkConformance){SIZE_MAX, SIZE_MAX, 0U};
        else if (strcmp(mode, "exhausted-failed") == 0) state = (UmiSdkConformance){SIZE_MAX, 0U, SIZE_MAX};
        else { CHECK(strcmp(mode, "exhausted-mixed") == 0); state = (UmiSdkConformance){SIZE_MAX, 1U, SIZE_MAX - 1U}; }
        UmiSdkConformance saved; memcpy(&saved, &state, sizeof saved);
        CHECK(umi_sdk_conformance_validate(&state) == UMI_STATUS_OK);
        for (int passed = 0; passed <= 1; ++passed) {
            CHECK(umi_sdk_conformance_record_checked(&state, passed) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(&state, &saved, sizeof state) == 0);
        }
    } else if (strcmp(mode, "missing-results") == 0 || strcmp(mode, "excess-passed") == 0 ||
               strcmp(mode, "excess-failed") == 0 || strcmp(mode, "sum-overflow") == 0) {
        if (strcmp(mode, "missing-results") == 0) state = (UmiSdkConformance){1U, 0U, 0U};
        else if (strcmp(mode, "excess-passed") == 0) state = (UmiSdkConformance){1U, 2U, 0U};
        else if (strcmp(mode, "excess-failed") == 0) state = (UmiSdkConformance){2U, 1U, 2U};
        else state = (UmiSdkConformance){SIZE_MAX, 1U, SIZE_MAX};
        UmiSdkConformance saved; memcpy(&saved, &state, sizeof saved);
        CHECK(umi_sdk_conformance_validate(&state) == UMI_STATUS_INVALID_STATE);
        CHECK(!umi_sdk_conformance_passed(&state));
        CHECK(umi_sdk_conformance_record_checked(&state, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&state, &saved, sizeof state) == 0);
    } else if (strcmp(mode, "legacy-refusal") == 0 || strcmp(mode, "legacy-recovery") == 0) {
        state = (UmiSdkConformance){SIZE_MAX, SIZE_MAX, 0U};
        umi_sdk_conformance_record(&state, 0);
        CHECK(state.checks_run == SIZE_MAX && state.checks_passed == SIZE_MAX &&
              state.checks_failed == SIZE_MAX && !umi_sdk_conformance_passed(&state));
        CHECK(umi_sdk_conformance_validate(&state) == UMI_STATUS_INVALID_STATE);
        umi_sdk_conformance_record(&state, 1);
        CHECK(!umi_sdk_conformance_passed(&state));
        if (strcmp(mode, "legacy-recovery") == 0) {
            state = (UmiSdkConformance){0};
            umi_sdk_conformance_record(&state, 1);
            CHECK(umi_sdk_conformance_passed(&state) && state.checks_run == 1U);
        }
    } else if (strncmp(mode, "report-", 7U) == 0) {
        state = (UmiSdkConformance){2U, 2U, 0U};
        if (strcmp(mode, "report-null") == 0) {
            CHECK(umi_sdk_report_format(NULL, &state, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_sdk_report_format(&probe, NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_sdk_report_format(&probe, &state, NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_sdk_report_format(&probe, &state, output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(output, before, sizeof output) == 0);
        } else if (strcmp(mode, "report-inconsistent") == 0) {
            state.checks_passed = 0U;
            CHECK(umi_sdk_report_format(&probe, &state, output, sizeof output) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(output, before, sizeof output) == 0);
        } else if (strcmp(mode, "report-capacity") == 0) {
            const char *expected = "sdk_probe=pass conformance=2/2 failed=0";
            for (size_t capacity = 1U; capacity <= strlen(expected); ++capacity) {
                CHECK(umi_sdk_report_format(&probe, &state, output, capacity) == UMI_STATUS_CAPACITY_EXCEEDED);
                CHECK(memcmp(output, before, sizeof output) == 0);
            }
            CHECK(umi_sdk_report_format(&probe, &state, output, strlen(expected) + 1U) == UMI_STATUS_OK);
            CHECK(strcmp(output, expected) == 0);
            CHECK(memcmp(output + strlen(expected) + 1U, before + strlen(expected) + 1U,
                sizeof output - strlen(expected) - 1U) == 0);
        } else {
            const char *expected = "sdk_probe=pass conformance=2/2 failed=0";
            char maximum[256];
            if (strcmp(mode, "report-empty") == 0) {
                state = (UmiSdkConformance){0};
                expected = "sdk_probe=pass conformance=0/0 failed=0";
            } else if (strcmp(mode, "report-failed") == 0) {
                state = (UmiSdkConformance){3U, 2U, 1U}; probe.passed = 0;
                expected = "sdk_probe=fail conformance=2/3 failed=1";
            } else if (strcmp(mode, "report-max") == 0) {
                state = (UmiSdkConformance){SIZE_MAX, SIZE_MAX - 1U, 1U};
                int length = snprintf(maximum, sizeof maximum,
                    "sdk_probe=pass conformance=%zu/%zu failed=1", SIZE_MAX - 1U, SIZE_MAX);
                CHECK(length > 0 && (size_t)length < sizeof maximum);
                expected = maximum;
            } else CHECK(strcmp(mode, "report-complete") == 0);
            CHECK(umi_sdk_report_format(&probe, &state, output, sizeof output) == UMI_STATUS_OK);
            CHECK(strcmp(output, expected) == 0);
        }
    } else CHECK(0);
    return 0;
}
