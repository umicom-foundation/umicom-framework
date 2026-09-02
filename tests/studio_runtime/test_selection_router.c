/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_selection_router.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime selection router.
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
#include "umicom/studio_runtime/selection_router.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeSelectionRouter router;
    umi_studio_selection_router_init(&router);
    assert(umi_studio_selection_router_test(
        &router,"test.a","test a")==UMI_STATUS_OK);
    assert(strcmp(router.command_context.test_item_id,"test.a")==0);
    assert(umi_studio_selection_router_source_control(
        &router,"src/a.c",7U)==UMI_STATUS_OK);
    assert(strcmp(router.command_context.source_control_path,"src/a.c")==0);
    return 0;
}

