/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/readiness_report.h
 * PURPOSE: Summarise executable availability before CTest is allowed to run.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_READINESS_REPORT_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_READINESS_REPORT_H

#include <stdbool.h>
#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test platform build readiness report data shared with callers of this
 * public contract.
 */
typedef struct UmiTestPlatformBuildReadinessReport {
    size_t total_count;
    size_t ready_count;
    size_t missing_count;
    size_t blocked_count;
    size_t required_failure_count;
    bool ready_to_run;
} UmiTestPlatformBuildReadinessReport;

/**
 * Initialise test platform build readiness report from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_build_readiness_report_create(
    const UmiTestPlatformBuildInventory *inventory,
    UmiTestPlatformBuildReadinessReport *report);

#ifdef __cplusplus
}
#endif
#endif
