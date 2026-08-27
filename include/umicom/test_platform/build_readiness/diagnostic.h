/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/diagnostic.h
 * PURPOSE: Classify Not Run results by their earliest failed build gate.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_DIAGNOSTIC_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_DIAGNOSTIC_H

#include <stdbool.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/build_readiness/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformBuildDiagnosticInput {
    bool configure_completed;
    bool target_enabled;
    bool build_completed;
    bool test_registered;
    bool executable_exists;
} UmiTestPlatformBuildDiagnosticInput;

typedef struct UmiTestPlatformBuildDiagnostic {
    UmiTestPlatformBuildReadinessState state;
    char summary[256];
    char remediation_command_id[128];
} UmiTestPlatformBuildDiagnostic;

UmiStatus umi_test_platform_build_diagnostic_classify(
    const UmiTestPlatformBuildDiagnosticInput *input,
    UmiTestPlatformBuildDiagnostic *diagnostic);

#ifdef __cplusplus
}
#endif
#endif
