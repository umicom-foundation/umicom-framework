/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_event.c
 *
 * PURPOSE:
 *   Verify normalised UI event FIFO ordering and generated identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiEventQueue *queue = NULL;
    UmiUiEvent event = {0};
    UmiUiEvent popped;
    uint64_t id = 0U;
    assert(umi_ui_event_queue_create(&queue) == UMI_STATUS_OK);
    event.kind = UMI_UI_EVENT_ACTIVATE;
    (void)snprintf(event.source_id, sizeof(event.source_id), "%s", "toolbar.build");
    (void)snprintf(event.command_id, sizeof(event.command_id), "%s", "studio.build");
    assert(umi_ui_event_push(queue, &event, &id) == UMI_STATUS_OK);
    assert(id != 0U && umi_ui_event_count(queue) == 1U);
    assert(umi_ui_event_pop(queue, &popped) == UMI_STATUS_OK);
    assert(strcmp(popped.command_id, "studio.build") == 0);
    umi_ui_event_queue_destroy(queue);
    return EXIT_SUCCESS;
}
