/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/types.c
 *
 * PURPOSE:
 *   Implement stable text representations for test kinds and execution states.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/types.h"

/* Provide the test kind text operation used by this module and its client applications. */
const char *umi_test_kind_text(UmiTestKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TEST_KIND_UNIT: return "unit";
        case UMI_TEST_KIND_CONTRACT: return "contract";
        case UMI_TEST_KIND_INTEGRATION: return "integration";
        case UMI_TEST_KIND_SYSTEM: return "system";
        case UMI_TEST_KIND_EXTERNAL: return "external";
        default: return "unknown";
    }
}

/* Provide the test state text operation used by this module and its client applications. */
const char *umi_test_state_text(UmiTestState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TEST_STATE_NOT_RUN: return "not-run";
        case UMI_TEST_STATE_RUNNING: return "running";
        case UMI_TEST_STATE_PASSED: return "passed";
        case UMI_TEST_STATE_FAILED: return "failed";
        case UMI_TEST_STATE_SKIPPED: return "skipped";
        case UMI_TEST_STATE_CANCELLED: return "cancelled";
        case UMI_TEST_STATE_TIMED_OUT: return "timed-out";
        default: return "unknown";
    }
}
