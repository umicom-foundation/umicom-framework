/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_probe.h
 *
 * PURPOSE:
 *   Probe compiler runtime, build output and platform dependency directories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_PROBE
#define UMICOM_TEST_RUNTIME_RUNTIME_PROBE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime runtime probe data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRuntimeProbe {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t probe_count;
    uint64_t failure_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRuntimeProbe;

/**
 * Initialise test runtime runtime probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_probe_init(UmiTestRuntimeRuntimeProbe *value, const char *id);
/**
 * Check that test runtime runtime probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_probe_validate(const UmiTestRuntimeRuntimeProbe *value);
/**
 * Provide the test runtime runtime probe set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_set_name(UmiTestRuntimeRuntimeProbe *value, const char *name);
/**
 * Provide the test runtime runtime probe set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_set_detail(UmiTestRuntimeRuntimeProbe *value, const char *detail);
/**
 * Return the number of records represented by test runtime runtime probe set probe without
 * changing their state.
 */
UmiStatus umi_test_runtime_runtime_probe_set_probe_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number);
/**
 * Return the number of records represented by test runtime runtime probe set failure
 * without changing their state.
 */
UmiStatus umi_test_runtime_runtime_probe_set_failure_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number);
/**
 * Provide the test runtime runtime probe touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_touch(UmiTestRuntimeRuntimeProbe *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime runtime probe same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_runtime_probe_same_identity(const UmiTestRuntimeRuntimeProbe *left, const UmiTestRuntimeRuntimeProbe *right);

#ifdef __cplusplus
}
#endif
#endif
