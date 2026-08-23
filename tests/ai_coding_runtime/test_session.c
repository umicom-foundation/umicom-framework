/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_session.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime session contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/session.h"

int main(void)
{

    UmiAiCodingSession session;
    assert(umi_ai_coding_session_init(
        &session, "session", "C:/work") == UMI_STATUS_OK);
    assert(umi_ai_coding_session_begin_task(
        &session, "task") == UMI_STATUS_OK);
    assert(session.task_count == 1U);
    assert(umi_ai_coding_session_complete_task(
        &session, UMI_AI_CODING_RUNTIME_COMPLETED) == UMI_STATUS_OK);
    assert(session.completed_count == 1U);
    assert(session.active_task_id[0] == '\0');

    return 0;
}
