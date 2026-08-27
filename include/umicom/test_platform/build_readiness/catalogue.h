/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/catalogue.h
 * PURPOSE: Expose the immutable cross-product build-artifact catalogue.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_CATALOGUE_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_CATALOGUE_H

#include <stddef.h>

#include "umicom/test_platform/build_readiness/artifact.h"
#include "umicom/test_platform/build_readiness/product_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_test_platform_build_artifact_catalogue_count(void);
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_at(size_t index);
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_target(const char *target_name);
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_test(const char *test_name);
size_t umi_test_platform_product_validation_profile_count(void);
const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_at(size_t index);
const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_find(const char *product_id);

#ifdef __cplusplus
}
#endif
#endif
