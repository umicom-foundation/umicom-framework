/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/build_readiness/command.h
 * PURPOSE: Publish stable commands for artifact readiness and Not Run diagnosis.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_BUILD_READINESS_COMMAND_H
#define UMICOM_TEST_PLATFORM_BUILD_READINESS_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformBuildReadinessCommand {
    const char *id;
    const char *label;
    const char *category;
    bool mutating;
} UmiTestPlatformBuildReadinessCommand;

size_t umi_test_platform_build_readiness_command_count(void);
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_at(size_t index);
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_find(const char *id);

#ifdef __cplusplus
}
#endif
#endif
