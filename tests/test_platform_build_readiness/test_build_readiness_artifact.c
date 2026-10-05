/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_artifact.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/artifact.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/test_platform/build_readiness/artifact.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTestPlatformBuildArtifactTransferEqual(const UmiTestPlatformBuildArtifact *a, const UmiTestPlatformBuildArtifact *b)
{
    return a->structure_size == b->structure_size &&
        a->api_version == b->api_version &&
        strcmp(a->product_id, b->product_id) == 0 &&
        strcmp(a->target_name, b->target_name) == 0 &&
        strcmp(a->test_name, b->test_name) == 0 &&
        strcmp(a->labels, b->labels) == 0 &&
        strcmp(a->preset, b->preset) == 0 &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTestPlatformBuildArtifactTransferTails(UmiTestPlatformBuildArtifact *value)
{
    (void)value;
    {
        size_t used = strlen(value->product_id) + 1U;
        memset(value->product_id + used, 0xa5, sizeof(value->product_id) - used);
    }
    {
        size_t used = strlen(value->target_name) + 1U;
        memset(value->target_name + used, 0xa5, sizeof(value->target_name) - used);
    }
    {
        size_t used = strlen(value->test_name) + 1U;
        memset(value->test_name + used, 0xa5, sizeof(value->test_name) - used);
    }
    {
        size_t used = strlen(value->labels) + 1U;
        memset(value->labels + used, 0xa5, sizeof(value->labels) - used);
    }
    {
        size_t used = strlen(value->preset) + 1U;
        memset(value->preset + used, 0xa5, sizeof(value->preset) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTestPlatformBuildArtifactTransferMalformed(const UmiTestPlatformBuildArtifact *sample)
{
    (void)sample;
    {
        UmiTestPlatformBuildArtifact invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.product_id, 'x', sizeof(invalid.product_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_build_artifact_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_build_artifact_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated product_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformBuildArtifact invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_name, 'x', sizeof(invalid.target_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_build_artifact_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_build_artifact_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformBuildArtifact invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.test_name, 'x', sizeof(invalid.test_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_build_artifact_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_build_artifact_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated test_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformBuildArtifact invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.labels, 'x', sizeof(invalid.labels));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_build_artifact_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_build_artifact_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated labels was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestPlatformBuildArtifact invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preset, 'x', sizeof(invalid.preset));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_test_platform_build_artifact_validate(&invalid) != UMI_STATUS_OK) ||
            umi_test_platform_build_artifact_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preset was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTestPlatformBuildArtifactTransferCases, UmiTestPlatformBuildArtifact,
    umi_test_platform_build_artifact_archive_encode, umi_test_platform_build_artifact_archive_decode,
    UmiTestPlatformBuildArtifactTransferEqual, UmiTestPlatformBuildArtifactTransferTails, UmiTestPlatformBuildArtifactTransferMalformed)

int main(void) {
    UmiTestPlatformBuildArtifact artifact;
    assert(umi_test_platform_build_artifact_init(&artifact, "studio",
        "studio-test-target", "studio.test", "studio;smoke",
        "windows-ucrt64-debug", true) == UMI_STATUS_OK);
    assert(umi_test_platform_build_artifact_validate(&artifact) == UMI_STATUS_OK);
    if (UmiTestPlatformBuildArtifactTransferCases(&artifact) != 0) return 1;

    assert(strcmp(artifact.target_name, "studio-test-target") == 0);
    assert(artifact.required);
    artifact.test_name[0] = '\0';
    assert(umi_test_platform_build_artifact_validate(&artifact) ==
        UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

