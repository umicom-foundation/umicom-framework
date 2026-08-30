/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_context_hub.c
 *
 * PURPOSE:
 *   Verify linked windows receive context through Framework rather than direct
 *   references to another application or pane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/application.h"

static void receive_context(void *listener_context,
                            const UmiApplicationContext *context)
{
    UmiApplicationContext *received = (UmiApplicationContext *)listener_context;
    *received = *context;
}

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiApplicationContext context = {0};
    UmiApplicationContext received = {0};
    UmiApplicationContext snapshot = {0};

    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_application_context_hub_subscribe(
        hub, "org.umicom.llm", "green", receive_context, &received) ==
        UMI_STATUS_OK);
    (void)snprintf(context.group_id, sizeof(context.group_id), "%s", "green");
    (void)snprintf(context.source_application_id,
                   sizeof(context.source_application_id), "%s",
                   "org.umicom.trader");
    (void)snprintf(context.context_type, sizeof(context.context_type), "%s",
                   "instrument");
    (void)snprintf(context.subject_id, sizeof(context.subject_id), "%s",
                   "CME.ES");
    (void)snprintf(context.correlation_id, sizeof(context.correlation_id), "%s",
                   "analysis-42");
    assert(umi_application_context_hub_publish(hub, &context) ==
           UMI_STATUS_OK);
    assert(strcmp(received.subject_id, "CME.ES") == 0);
    assert(received.revision == 1U);
    assert(umi_application_context_hub_snapshot(hub, "green", &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.source_application_id, "org.umicom.trader") == 0);
    assert(umi_application_context_hub_group_count(hub) == 1U);
    umi_application_context_hub_destroy(hub);
    return 0;
}
