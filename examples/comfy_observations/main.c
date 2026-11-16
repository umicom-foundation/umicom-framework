/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_observations/main.c
 *
 * PURPOSE:
 *   Compare ComfyUI UI output and success messages through the installed native protocol SDK without connecting to a server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_events.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    const char ui[] = "{\"type\":\"executed\",\"data\":{\"prompt_id\":\"example-job\",\"node\":\"9\",\"output\":{}}}";
    const char success[] = "{\"type\":\"execution_success\",\"data\":{\"prompt_id\":\"example-job\",\"timestamp\":1}}";
    UmiComfyEvent event;
    UmiStatus status = UmiComfyEventDecode(ui, sizeof(ui)-1U, NULL, &event);
    if (status != UMI_STATUS_OK || event.kind != UMI_COMFY_EVENT_UI_OUTPUT) return 1;
    puts("example-job: UI output observation; completion remains unconfirmed.");
    status = UmiComfyEventDecode(success, sizeof(success)-1U, NULL, &event);
    if (status != UMI_STATUS_OK || event.kind != UMI_COMFY_EVENT_SUCCESS ||
        strcmp(event.prompt_id,"example-job") != 0) return 1;
    puts("example-job: synthetic provider success observation; no output was downloaded.");
    puts("No server connection, workflow execution or generated media in this example.");
    return 0;
}
