/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/executable_probe.h
 *
 * PURPOSE:
 *   Record executable discovery, file existence and launch readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTABLE_PROBE
#define UMICOM_TEST_RUNTIME_EXECUTABLE_PROBE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime executable probe data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutableProbe {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t exists;
    uint64_t launchable;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeExecutableProbe;

/**
 * Initialise test runtime executable probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_executable_probe_init(UmiTestRuntimeExecutableProbe *value, const char *id);
/**
 * Check that test runtime executable probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_executable_probe_validate(const UmiTestRuntimeExecutableProbe *value);
/**
 * Provide the test runtime executable probe set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_executable_probe_set_name(UmiTestRuntimeExecutableProbe *value, const char *name);
/**
 * Provide the test runtime executable probe set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_executable_probe_set_detail(UmiTestRuntimeExecutableProbe *value, const char *detail);
/**
 * Provide the test runtime executable probe set exists operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_executable_probe_set_exists(UmiTestRuntimeExecutableProbe *value, uint64_t number);
/**
 * Provide the test runtime executable probe set launchable operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_probe_set_launchable(UmiTestRuntimeExecutableProbe *value, uint64_t number);
/**
 * Provide the test runtime executable probe touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_executable_probe_touch(UmiTestRuntimeExecutableProbe *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime executable probe same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_executable_probe_same_identity(const UmiTestRuntimeExecutableProbe *left, const UmiTestRuntimeExecutableProbe *right);

#ifdef __cplusplus
}
#endif
#endif
