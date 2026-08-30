/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_history.c
 *
 * PURPOSE:
 *   Verify bounded chronological build-history retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/build/history.h"

int main(void)
{
    UmiBuildHistory *history = NULL;
    UmiBuildResult result;
    UmiBuildResult copy;
    assert(umi_build_history_create(2U, &history) == UMI_STATUS_OK);
    umi_build_result_init(&result, 1U, UMI_BUILD_PHASE_CONFIGURE, "debug");
    assert(umi_build_history_append(history, &result) == UMI_STATUS_OK);
    umi_build_result_init(&result, 2U, UMI_BUILD_PHASE_BUILD, "debug");
    assert(umi_build_history_append(history, &result) == UMI_STATUS_OK);
    umi_build_result_init(&result, 3U, UMI_BUILD_PHASE_TEST, "debug");
    assert(umi_build_history_append(history, &result) == UMI_STATUS_OK);
    assert(umi_build_history_count(history) == 2U);
    assert(umi_build_history_at(history, 0U, &copy) == UMI_STATUS_OK);
    assert(copy.operation_id == 2U);
    assert(umi_build_history_latest(history, &copy) == UMI_STATUS_OK);
    assert(copy.operation_id == 3U);
    umi_build_history_destroy(history);
    return 0;
}
