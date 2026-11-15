/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_consumer_requirements.c
 *
 * PURPOSE:
 *   Verify SDK declaration bounds, unmet targets and complete caller-owned comparison results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/sdk.h"
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
    const char *required[] = {"Umicom::base", "Umicom::sdk", "Umicom::media"};
    UmiSdkComponent available[] = {
        {"sdk", "Umicom::sdk", 1}, {"base", "Umicom::base", 1},
        {"extra", "Umicom::extra", 0}
    };
    UmiSdkConsumer consumer = {"example.application", {{3U, 4U, 5U}, 7U}, required, 2U};
    UmiSdkManifest manifest = {{3U, 4U, 5U}, 7U, available, 2U};
    UmiSdkConsumerCheck result;
    unsigned char before[sizeof result];
    memset(&result, 0xa5, sizeof result);
    memcpy(before, &result, sizeof result);

    if (strncmp(mode, "consumer-", 9U) == 0) {
        UmiSdkConsumer *input = &consumer;
        if (strcmp(mode, "consumer-null") == 0) input = NULL;
        else if (strcmp(mode, "consumer-null-id") == 0) consumer.application_id = NULL;
        else if (strcmp(mode, "consumer-empty-id") == 0) consumer.application_id = "";
        else if (strcmp(mode, "consumer-zero-abi") == 0) consumer.requirement.required_abi = 0U;
        else if (strcmp(mode, "consumer-null-array") == 0) consumer.required_targets = NULL;
        else if (strcmp(mode, "consumer-extent") == 0)
            consumer.target_count = SIZE_MAX / sizeof *consumer.required_targets + 1U;
        else if (strcmp(mode, "consumer-null-entry") == 0) required[0] = NULL;
        else if (strcmp(mode, "consumer-empty-entry") == 0) required[0] = "";
        else { CHECK(strcmp(mode, "consumer-last-invalid") == 0); required[1] = NULL; }
        CHECK(umi_sdk_consumer_validate(input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_sdk_consumer_check(input, &manifest, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &result, sizeof result) == 0);
    } else if (strncmp(mode, "manifest-", 9U) == 0) {
        UmiSdkManifest *input = &manifest;
        if (strcmp(mode, "manifest-null") == 0) input = NULL;
        else if (strcmp(mode, "manifest-zero-abi") == 0) manifest.framework_abi = 0U;
        else if (strcmp(mode, "manifest-null-array") == 0) manifest.components = NULL;
        else if (strcmp(mode, "manifest-extent") == 0)
            manifest.component_count = SIZE_MAX / sizeof *manifest.components + 1U;
        else if (strcmp(mode, "manifest-null-id") == 0) available[0].component_id = NULL;
        else if (strcmp(mode, "manifest-empty-id") == 0) available[0].component_id = "";
        else if (strcmp(mode, "manifest-null-target") == 0) available[0].target_name = NULL;
        else if (strcmp(mode, "manifest-empty-target") == 0) available[0].target_name = "";
        else { CHECK(strcmp(mode, "manifest-last-invalid") == 0); available[1].target_name = NULL; }
        CHECK(umi_sdk_manifest_validate(input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_sdk_compatible(&consumer.requirement, input));
        CHECK(umi_sdk_consumer_check(&consumer, input, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &result, sizeof result) == 0);
    } else if (strcmp(mode, "null-output") == 0) {
        CHECK(umi_sdk_consumer_check(&consumer, &manifest, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "requirement-null") == 0 || strcmp(mode, "requirement-zero") == 0) {
        UmiSdkRequirement *input = &consumer.requirement;
        if (strcmp(mode, "requirement-null") == 0) input = NULL;
        else input->required_abi = 0U;
        CHECK(umi_sdk_requirement_validate(input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_sdk_compatible(input, &manifest));
    } else if (strcmp(mode, "owned-result") == 0) {
        char *borrowed = malloc(32U);
        CHECK(borrowed != NULL);
        strcpy(borrowed, "Umicom::absent");
        required[1] = borrowed;
        UmiStatus status = umi_sdk_consumer_check(&consumer, &manifest, &result);
        free(borrowed);
        required[1] = NULL;
        CHECK(status == UMI_STATUS_OK);
        CHECK(result.targets_checked == 2U && result.targets_missing == 1U);
        CHECK(result.first_missing_index == 1U && result.version_abi_compatible);
        CHECK(!result.declarations_satisfied);
    } else {
        size_t missing = 0U, first = SIZE_MAX;
        int version_abi = 1;
        if (strcmp(mode, "match") == 0) { /* Fixture order intentionally differs. */ }
        else if (strcmp(mode, "missing-one") == 0) {
            consumer.target_count = 3U; missing = 1U; first = 2U;
        } else if (strcmp(mode, "missing-all") == 0) {
            available[0].target_name = "Umicom::other";
            available[1].target_name = "Umicom::different"; missing = 2U; first = 0U;
        } else if (strcmp(mode, "empty-required") == 0) {
            consumer.required_targets = NULL; consumer.target_count = 0U;
        } else if (strcmp(mode, "empty-available") == 0) {
            manifest.components = NULL; manifest.component_count = 0U; missing = 2U; first = 0U;
        } else if (strcmp(mode, "both-empty") == 0) {
            manifest.components = NULL; manifest.component_count = 0U;
            consumer.required_targets = NULL; consumer.target_count = 0U;
        } else if (strcmp(mode, "duplicate-required") == 0) {
            required[0] = "Umicom::absent"; required[1] = "Umicom::absent";
            missing = 2U; first = 0U;
        } else if (strcmp(mode, "duplicate-present") == 0) {
            required[1] = required[0];
        } else if (strcmp(mode, "duplicate-available") == 0) {
            available[2] = available[0]; manifest.component_count = 3U;
        } else if (strcmp(mode, "case-sensitive") == 0) {
            required[1] = "Umicom::SDK"; missing = 1U; first = 1U;
        } else if (strcmp(mode, "optional-component") == 0) {
            available[0].required = 0;
        } else if (strcmp(mode, "extra-targets") == 0) {
            manifest.component_count = 3U;
        } else if (strcmp(mode, "older-version") == 0) {
            manifest.framework_version.minor = 3U; version_abi = 0;
        } else if (strcmp(mode, "older-patch") == 0) {
            manifest.framework_version.patch = 4U; version_abi = 0;
        } else if (strcmp(mode, "different-major") == 0) {
            manifest.framework_version.major = 4U; version_abi = 0;
        } else if (strcmp(mode, "low-abi") == 0) {
            manifest.framework_abi = 6U; version_abi = 0;
        } else if (strcmp(mode, "higher-abi") == 0) {
            manifest.framework_abi = 8U;
        } else if (strcmp(mode, "newer-minor") == 0) {
            manifest.framework_version.minor = 5U; manifest.framework_version.patch = 0U;
        } else if (strcmp(mode, "newer-patch") == 0) {
            manifest.framework_version.patch = 6U;
        } else if (strcmp(mode, "combined-mismatch") == 0) {
            manifest.framework_abi = 6U; version_abi = 0;
            required[0] = "Umicom::absent"; missing = 1U; first = 0U;
        } else if (strcmp(mode, "zero-minimum-version") == 0) {
            consumer.requirement.minimum_version = (UmiVersion){0U, 0U, 0U};
            manifest.framework_version = (UmiVersion){0U, 1U, 0U};
        } else { CHECK(0 && "unknown case"); }
        CHECK(umi_sdk_consumer_validate(&consumer) == UMI_STATUS_OK);
        CHECK(umi_sdk_manifest_validate(&manifest) == UMI_STATUS_OK);
        CHECK(umi_sdk_consumer_check(&consumer, &manifest, &result) == UMI_STATUS_OK);
        CHECK(result.targets_checked == consumer.target_count);
        CHECK(result.targets_missing == missing && result.first_missing_index == first);
        CHECK(result.version_abi_compatible == version_abi);
        CHECK(result.declarations_satisfied == (version_abi && missing == 0U));
    }
    return 0;
}
