/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/types.c
 *
 * PURPOSE:
 *   Implement stable text representations for test kinds and execution states.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/types.h"

const char *umi_test_kind_text(UmiTestKind kind)
{
    switch (kind) {
        case UMI_TEST_KIND_UNIT: return "unit";
        case UMI_TEST_KIND_CONTRACT: return "contract";
        case UMI_TEST_KIND_INTEGRATION: return "integration";
        case UMI_TEST_KIND_SYSTEM: return "system";
        case UMI_TEST_KIND_EXTERNAL: return "external";
        default: return "unknown";
    }
}

const char *umi_test_state_text(UmiTestState state)
{
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
