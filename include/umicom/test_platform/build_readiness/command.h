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

/**
 * Represent the test platform build readiness command data shared with callers of this
 * public contract.
 */
typedef struct UmiTestPlatformBuildReadinessCommand {
    const char *id;
    const char *label;
    const char *category;
    bool mutating;
} UmiTestPlatformBuildReadinessCommand;

/**
 * Return the number of records represented by test platform build readiness command
 * without changing their state.
 */
size_t umi_test_platform_build_readiness_command_count(void);
/**
 * Find test platform build readiness command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_at(size_t index);
/**
 * Find test platform build readiness command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_find(const char *id);

#ifdef __cplusplus
}
#endif
#endif
