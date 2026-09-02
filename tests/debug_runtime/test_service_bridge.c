/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_service_bridge.c
 *
 * PURPOSE:
 *   Verify decoded DAP threads/stack frames project into existing Debug Service.
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

#include "umicom/debug_runtime/service_bridge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugService *service = NULL;
    UmiDebugRuntimeServiceBridge bridge;
    UmiDebugRuntimeThreadList threads;
    UmiDebugRuntimeStackTrace stack;

    assert(umi_debug_service_create(&service) == UMI_STATUS_OK);
    assert(umi_debug_runtime_service_bridge_init(
        &bridge, service, "session") == UMI_STATUS_OK);

    (void)memset(&threads, 0, sizeof(threads));
    threads.items[0].id = 7U;
    (void)strcpy(threads.items[0].name, "main");
    threads.count = 1U;

    assert(umi_debug_runtime_publish_threads(
        &bridge, &threads, 7U, 1) == UMI_STATUS_OK);
    assert(umi_debug_thread_registry_count(
        umi_debug_service_thread(service)) == 1U);

    (void)memset(&stack, 0, sizeof(stack));
    stack.items[0].id = 11U;
    (void)strcpy(stack.items[0].name, "main");
    (void)strcpy(stack.items[0].source.path, "C:/work/main.c");
    stack.items[0].line = 10U;
    stack.count = 1U;

    assert(umi_debug_runtime_publish_stack(
        &bridge, 7U, &stack) == UMI_STATUS_OK);
    assert(umi_debug_stack_frame_registry_count(
        umi_debug_service_stack_frame(service)) == 1U);

    umi_debug_service_destroy(service);
    return 0;
}
