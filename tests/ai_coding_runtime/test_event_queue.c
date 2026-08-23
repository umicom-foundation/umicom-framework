/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_event_queue.c
 *
 * PURPOSE:
 *   Verify AI coding runtime event queue behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/event_queue.h"

int main(void)
{

    UmiAiCodingEventQueue *queue = NULL;
    UmiAiCodingEvent input = {0};
    UmiAiCodingEvent output;
    input.kind = UMI_AI_CODING_EVENT_STATE;
    input.state = UMI_AI_CODING_RUNTIME_PREPARING;
    (void)strcpy(input.task_id, "task");
    assert(umi_ai_coding_event_queue_create(&queue) == UMI_STATUS_OK);
    assert(umi_ai_coding_event_queue_push(queue, &input) == UMI_STATUS_OK);
    assert(umi_ai_coding_event_queue_count(queue) == 1U);
    assert(umi_ai_coding_event_queue_pop(queue, &output) == UMI_STATUS_OK);
    assert(strcmp(output.task_id, "task") == 0);
    umi_ai_coding_event_queue_destroy(queue);

    return 0;
}
