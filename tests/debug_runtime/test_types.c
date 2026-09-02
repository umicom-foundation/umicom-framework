/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_types.c
 *
 * PURPOSE:
 *   Verify stable DAP runtime diagnostic text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(
        umi_debug_runtime_message_kind_text(UMI_DEBUG_RUNTIME_MESSAGE_EVENT),
        "event") == 0);
    assert(strcmp(
        umi_debug_runtime_adapter_state_text(UMI_DEBUG_RUNTIME_ADAPTER_PAUSED),
        "paused") == 0);
    return 0;
}
