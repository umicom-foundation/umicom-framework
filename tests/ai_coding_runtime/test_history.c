/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_history.c
 *
 * PURPOSE:
 *   Verify AI coding runtime history behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/history.h"

int main(void)
{

    UmiAiCodingHistory *history = NULL;
    UmiAiCodingAgentSnapshot snapshot = {0};
    UmiAiCodingAgentSnapshot read_back;
    (void)strcpy(snapshot.task_id, "task");
    snapshot.state = UMI_AI_CODING_RUNTIME_COMPLETED;
    assert(umi_ai_coding_history_create(&history) == UMI_STATUS_OK);
    assert(umi_ai_coding_history_record(history, &snapshot) == UMI_STATUS_OK);
    assert(umi_ai_coding_history_count(history) == 1U);
    assert(umi_ai_coding_history_at(history, 0U, &read_back) == UMI_STATUS_OK);
    assert(strcmp(read_back.task_id, "task") == 0);
    umi_ai_coding_history_destroy(history);

    return 0;
}
