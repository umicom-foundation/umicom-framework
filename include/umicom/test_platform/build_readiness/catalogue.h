/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/catalogue.h
 * PURPOSE: Expose the immutable cross-product build-artifact catalogue.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_CATALOGUE_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_CATALOGUE_H

#include <stddef.h>

#include "umicom/test_platform/build_readiness/artifact.h"
#include "umicom/test_platform/build_readiness/product_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by test platform build artifact catalogue
 * without changing their state.
 */
size_t umi_test_platform_build_artifact_catalogue_count(void);
/**
 * Find test platform build artifact catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_at(size_t index);
/**
 * Provide the test platform build artifact catalogue find target operation used by this
 * module and its client applications.
 */
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_target(const char *target_name);
/**
 * Provide the test platform build artifact catalogue find test operation used by this
 * module and its client applications.
 */
const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_test(const char *test_name);
/**
 * Return the number of records represented by test platform product validation profile
 * without changing their state.
 */
size_t umi_test_platform_product_validation_profile_count(void);
/**
 * Find test platform product validation profile while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_at(size_t index);
/**
 * Find test platform product validation profile while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_find(const char *product_id);

#ifdef __cplusplus
}
#endif
#endif
