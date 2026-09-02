/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_context_links.c
 *
 * PURPOSE:
 *   Implement the test context links behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop context links | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/desktop/context_links.h"

/*
 * Exercise listener and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void listener(void *listener_context, const UmiApplicationContext *context)
{
    UmiApplicationContext *received = (UmiApplicationContext *)listener_context;
    *received = *context;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopContextLinks links;
    UmiApplicationContext received = {0};
    const char *routed[2] = {0};
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    umi_desktop_context_links_init(&links, hub);
    assert(umi_desktop_context_links_define(
        &links, "instrument-teal", "teal", UMI_UI_WINDOW_CONTEXT_ACCOUNT) ==
        UMI_STATUS_OK);
    assert(umi_desktop_context_links_join(
        &links, "instrument-teal", "chart", UMI_UI_WINDOW_GROUP_SOURCE) ==
        UMI_STATUS_OK);
    assert(umi_desktop_context_links_join(
        &links, "instrument-teal", "order-ticket",
        UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    assert(umi_desktop_context_links_route(
        &links, "instrument-teal", "chart", routed, 2U) == 1U);
    assert(strcmp(routed[0], "order-ticket") == 0);
    assert(umi_application_context_hub_subscribe(
        hub, "org.umicom.llm", "instrument-teal", listener, &received) ==
        UMI_STATUS_OK);
    assert(umi_desktop_context_links_publish(
        &links, "instrument-teal", "org.umicom.trader", "instrument",
        "EUR.USD", "selection-1") == UMI_STATUS_OK);
    assert(strcmp(received.subject_id, "EUR.USD") == 0);
    assert(strcmp(received.source_application_id, "org.umicom.trader") == 0);
    umi_application_context_hub_destroy(hub);
    return 0;
}
