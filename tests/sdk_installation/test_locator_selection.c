/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_locator_selection.c
 *
 * PURPOSE:
 *   Verify explicit SDK selection and complete output with a controlled presence probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/locator.h"
#include "umicom/sdk/probe.h"
#include "umicom/base/version.h"
#include <stdio.h>
#include <string.h>

typedef struct ProbeStep {
    const char *prefix;
    UmiStatus status;
    int passed;
} ProbeStep;
static ProbeStep steps[3];
static size_t step_count, probe_calls;
static int probe_mismatch;
static UmiStatus SelectionProbe(const char *prefix, UmiSdkProbeResult *out)
{
    size_t current = probe_calls++;
    if (current >= step_count || prefix == NULL || out == NULL ||
        strcmp(prefix, steps[current].prefix) != 0) {
        probe_mismatch = 1;
        return UMI_STATUS_INVALID_STATE;
    }
    memset(out, 0, sizeof *out);
    out->passed = steps[current].passed;
    return steps[current].status;
}

/* Exercise the production selection logic without inspecting any SDK on the
 * test machine. Shared text replacement remains the real Framework service. */
#define umi_sdk_probe_prefix SelectionProbe
#define umi_sdk_locate_checked SelectionChecked
#define umi_sdk_locate SelectionLegacy
#include "../../src/sdk/locator.c"
#undef umi_sdk_locate
#undef umi_sdk_locate_checked
#undef umi_sdk_probe_prefix

typedef UmiStatus (*LocateFunction)(const char *, char *, size_t);
static const LocateFunction locators[] = {SelectionChecked, SelectionLegacy};
static const char *const defaults[] = {
    "C:/Dev/umicom/sdk/umicom-framework-" UMICOM_FRAMEWORK_VERSION_STRING "-local",
    "/usr/local", "/usr"
};
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

static void ResetProbe(void)
{
    memset(steps, 0, sizeof steps);
    step_count = probe_calls = 0U;
    probe_mismatch = 0;
}
static void AddStep(const char *prefix, UmiStatus status, int passed)
{
    if (step_count >= sizeof steps / sizeof steps[0]) {
        probe_mismatch = 1;
        return;
    }
    steps[step_count++] = (ProbeStep){prefix, status, passed};
}

static int ExplicitSelection(int overlap)
{
    const char selected[] = "/selected/sdk";
    for (size_t api = 0U; api < sizeof locators / sizeof locators[0]; ++api) {
        for (size_t variant = 0U; variant < (overlap ? 3U : 1U); ++variant) {
            unsigned char storage[256], expected[256];
            memset(storage, 0xa5, sizeof storage);
            char *output = (char *)storage;
            const char *preferred = selected;
            if (overlap) {
                size_t input_offset = variant == 1U ? 7U : 0U;
                size_t output_offset = variant == 2U ? 3U : 0U;
                memcpy(storage + input_offset, selected, sizeof selected);
                preferred = (const char *)storage + input_offset;
                output += output_offset;
            }
            size_t offset = (size_t)(output - (char *)storage);
            memcpy(expected, storage, sizeof expected);
            memcpy(expected + offset, selected, sizeof selected);
            ResetProbe(); AddStep(selected, UMI_STATUS_OK, 1);
            CHECK(locators[api](preferred, output, sizeof storage - offset) == UMI_STATUS_OK);
            CHECK(probe_calls == 1U && !probe_mismatch);
            CHECK(memcmp(storage, expected, sizeof storage) == 0);
        }
    }
    return 0;
}

static int RefusedSelection(void)
{
    const UmiStatus refusals[] = {UMI_STATUS_NOT_FOUND, UMI_STATUS_INVALID_ARGUMENT,
        UMI_STATUS_INVALID_STATE, UMI_STATUS_CAPACITY_EXCEEDED, UMI_STATUS_OUT_OF_MEMORY};
    for (size_t api = 0U; api < sizeof locators / sizeof locators[0]; ++api) {
        for (size_t failure = 0U; failure <= sizeof refusals / sizeof refusals[0]; ++failure) {
            char output[128], expected[128];
            memset(output, 0xa5, sizeof output);
            memcpy(output, "/selected/sdk", sizeof "/selected/sdk");
            memcpy(expected, output, sizeof expected);
            if (api == 1U) expected[0] = '\0';
            int no_presence = failure == sizeof refusals / sizeof refusals[0];
            UmiStatus status = no_presence ? UMI_STATUS_OK : refusals[failure];
            ResetProbe(); AddStep("/selected/sdk", status, 0);
            CHECK(locators[api](output, output, sizeof output) ==
                (no_presence ? UMI_STATUS_NOT_FOUND : status));
            CHECK(probe_calls == 1U && !probe_mismatch);
            CHECK(memcmp(output, expected, sizeof output) == 0);
        }
    }
    return 0;
}

static int CapacitySelection(void)
{
    const char selected[] = "/selected/sdk";
    for (size_t api = 0U; api < sizeof locators / sizeof locators[0]; ++api) {
        for (size_t capacity = 1U; capacity <= sizeof selected + 1U; ++capacity) {
            char output[128], expected[128];
            memset(output, 0xa5, sizeof output);
            memcpy(output, selected, sizeof selected);
            memcpy(expected, output, sizeof expected);
            int fits = capacity >= sizeof selected;
            if (!fits && api == 1U) expected[0] = '\0';
            ResetProbe(); AddStep(selected, UMI_STATUS_OK, 1);
            CHECK(locators[api](output, output, capacity) ==
                (fits ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
            CHECK(probe_calls == 1U && !probe_mismatch);
            CHECK(memcmp(output, expected, sizeof output) == 0);
        }
    }
    return 0;
}

static int InvalidOutput(void)
{
    for (size_t api = 0U; api < sizeof locators / sizeof locators[0]; ++api) {
        char output[] = "preserve";
        ResetProbe();
        CHECK(locators[api]("/selected/sdk", NULL, 16U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(locators[api](output, output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(locators[api](NULL, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "preserve") == 0 && probe_calls == 0U && !probe_mismatch);
    }
    return 0;
}

static int DefaultSelection(const char *mode)
{
    for (size_t api = 0U; api < sizeof locators / sizeof locators[0]; ++api) {
        char output[256], expected[256];
        memset(output, 0xa5, sizeof output);
        memcpy(expected, output, sizeof expected);
        const char *preferred = NULL;
        UmiStatus wanted = UMI_STATUS_OK;
        size_t capacity = sizeof output, selected = 0U, expected_calls = 1U;
        ResetProbe();
        if (strcmp(mode, "default-first") == 0) selected = 0U;
        else if (strcmp(mode, "default-second") == 0) selected = 1U;
        else if (strcmp(mode, "default-third") == 0) selected = 2U;
        else if (strcmp(mode, "default-empty") == 0) {
            /* Empty preferred may itself borrow the destination. */
            output[0] = expected[0] = '\0'; preferred = output; selected = 1U;
        } else if (strcmp(mode, "default-none") == 0) {
            wanted = UMI_STATUS_NOT_FOUND; expected_calls = 3U;
            for (size_t i = 0U; i < 3U; ++i) AddStep(defaults[i], UMI_STATUS_OK, 0);
        } else if (strcmp(mode, "default-capacity") == 0 || strcmp(mode, "default-memory") == 0) {
            wanted = strcmp(mode, "default-memory") == 0 ?
                UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_CAPACITY_EXCEEDED;
            expected_calls = 2U;
            AddStep(defaults[0], UMI_STATUS_NOT_FOUND, 0);
            AddStep(defaults[1], wanted, 0);
        } else if (strcmp(mode, "default-skip-errors") == 0) {
            selected = 2U; expected_calls = 3U;
            AddStep(defaults[0], UMI_STATUS_INVALID_ARGUMENT, 0);
            AddStep(defaults[1], UMI_STATUS_INVALID_STATE, 0);
            AddStep(defaults[2], UMI_STATUS_OK, 1);
        } else if (strcmp(mode, "default-short-output") == 0) {
            capacity = 1U; wanted = UMI_STATUS_CAPACITY_EXCEEDED;
            AddStep(defaults[0], UMI_STATUS_OK, 1);
        } else CHECK(0);
        if (step_count == 0U) {
            expected_calls = selected + 1U;
            for (size_t i = 0U; i < selected; ++i) AddStep(defaults[i], UMI_STATUS_NOT_FOUND, 0);
            AddStep(defaults[selected], UMI_STATUS_OK, 1);
        }
        if (wanted == UMI_STATUS_OK)
            memcpy(expected, defaults[selected], strlen(defaults[selected]) + 1U);
        else if (api == 1U) expected[0] = '\0';
        CHECK(locators[api](preferred, output, capacity) == wanted);
        CHECK(probe_calls == expected_calls && !probe_mismatch);
        CHECK(memcmp(output, expected, sizeof output) == 0);
    }
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "explicit") == 0) return ExplicitSelection(0);
    if (strcmp(argv[1], "overlap") == 0) return ExplicitSelection(1);
    if (strcmp(argv[1], "refusal") == 0) return RefusedSelection();
    if (strcmp(argv[1], "capacity") == 0) return CapacitySelection();
    if (strcmp(argv[1], "invalid-output") == 0) return InvalidOutput();
    return DefaultSelection(argv[1]);
}
