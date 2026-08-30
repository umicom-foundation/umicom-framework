/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/controller.h
 * PURPOSE: Coordinate product test-artifact probes through one Master Controller.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_CONTROLLER_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_CONTROLLER_H

#include "umicom/test_platform/build_readiness/diagnostic.h"
#include "umicom/test_platform/build_readiness/readiness_report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiTestPlatformBuildProbe)(
    const UmiTestPlatformBuildArtifact *artifact,
    UmiTestPlatformBuildDiagnosticInput *input,
    void *user_data);

/* The Master Controller coordinates probes; platform Slave Controllers own
 * filesystem and process access and supply it through this narrow callback. */
UmiStatus umi_test_platform_build_readiness_assess_product(
    const char *product_id,
    UmiTestPlatformBuildProbe probe,
    void *user_data,
    UmiTestPlatformBuildInventory *inventory,
    UmiTestPlatformBuildReadinessReport *report);

#ifdef __cplusplus
}
#endif
#endif
