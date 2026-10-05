/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/artifact.h
 * PURPOSE: Map a CTest name to the executable target that must exist first.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_ARTIFACT_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_ARTIFACT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test platform build artifact data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformBuildArtifact {
    uint32_t structure_size;
    uint32_t api_version;
    char product_id[64];
    char target_name[160];
    char test_name[192];
    char labels[192];
    char preset[96];
    bool required;
} UmiTestPlatformBuildArtifact;

/**
 * Initialise test platform build artifact from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_build_artifact_init(
    UmiTestPlatformBuildArtifact *artifact,
    const char *product_id,
    const char *target_name,
    const char *test_name,
    const char *labels,
    const char *preset,
    bool required);
/**
 * Check that test platform build artifact satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_platform_build_artifact_validate(
    const UmiTestPlatformBuildArtifact *artifact);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_platform_build_artifact_archive_encode(const UmiTestPlatformBuildArtifact *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_platform_build_artifact_archive_decode(const void *bytes, size_t byte_count,
    UmiTestPlatformBuildArtifact *value);

#ifdef __cplusplus
}
#endif
#endif
