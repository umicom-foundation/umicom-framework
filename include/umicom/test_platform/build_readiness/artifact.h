/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/artifact.h
 * PURPOSE: Map a CTest name to the executable target that must exist first.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_ARTIFACT_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_ARTIFACT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

UmiStatus umi_test_platform_build_artifact_init(
    UmiTestPlatformBuildArtifact *artifact,
    const char *product_id,
    const char *target_name,
    const char *test_name,
    const char *labels,
    const char *preset,
    bool required);
UmiStatus umi_test_platform_build_artifact_validate(
    const UmiTestPlatformBuildArtifact *artifact);

#ifdef __cplusplus
}
#endif
#endif
