/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_platform_profile.c
 *
 * PURPOSE:
 *   Verify the platform profile contract and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/platform_profile.h"
/* A refused edit must preserve the whole record, including its observation
 * token. Capture bytes from this same object so padding is compared only for
 * an operation that promises to perform no writes. */
static int CheckRefusedEdits(void)
{
    UmiSdkRuntimePlatformProfile value;
    umi_sdk_runtime_platform_profile_init(&value, "publication-check");
    unsigned char before[sizeof(value)];
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_platform_profile_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_platform_profile_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_platform_profile_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_sdk_runtime_platform_profile_set_detail(&value, value.detail + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    {
        char oversized[sizeof(value.path) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_platform_profile_set_path(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_platform_profile_set_path(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_platform_profile_set_path(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_sdk_runtime_platform_profile_set_path(&value, value.path + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.path, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    /* The final valid edit may reach the boundary. Every later mutator must
     * refuse before changing any field, rather than wrapping the counter. */
    value.revision = UINT64_MAX - 1U;
    if (umi_sdk_runtime_platform_profile_set_path(&value, "share/umicom/runtime") != UMI_STATUS_OK || value.revision != UINT64_MAX) return 1;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_platform_profile_set_path(&value, "share/umicom/runtime") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_sdk_runtime_platform_profile_set_detail(&value, "validated runtime evidence") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_sdk_runtime_platform_profile_set_architecture_bits(&value, 3U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_sdk_runtime_platform_profile_set_dependency_count(&value, 5U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_sdk_runtime_platform_profile_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

int main(void)
{
    if (CheckRefusedEdits() != 0) return 1;
    UmiSdkRuntimePlatformProfile value; UmiSdkRuntimePlatformProfile same; uint64_t revision;
    umi_sdk_runtime_platform_profile_init(&value, "sdk-runtime.platform_profile");
    assert(umi_sdk_runtime_platform_profile_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_platform_profile_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_architecture_bits(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_dependency_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.architecture_bits == 3U && value.dependency_count == 5U);
    umi_sdk_runtime_platform_profile_init(&same, "sdk-runtime.platform_profile");
    assert(umi_sdk_runtime_platform_profile_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
