/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_dispatcher.c
 *
 * PURPOSE:
 *   Verify typed subscription matching, schema validation, dispatch and unsubscription.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

/*
 * Exercise count handler and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus count_handler(const UmiMessageEnvelope *message, void *user_data)
{
    int *count = (int *)user_data;
    assert(message != NULL);
    (*count)++;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSchemaRegistry *schemas = NULL;
    UmiDispatcher *dispatcher = NULL;
    UmiSchemaDescriptor schema = {
        sizeof(UmiSchemaDescriptor), "studio.event", 1U,
        UMI_MESSAGE_EVENT, 128U, UMI_SCHEMA_EXACT
    };
    UmiSubscription subscription = umi_subscription_all();
    UmiMessageEnvelope message;
    uint64_t id = 0U;
    size_t delivered = 0U;
    int count = 0;
    assert(umi_schema_registry_create(&schemas) == UMI_STATUS_OK);
    assert(umi_schema_registry_register(schemas, &schema) == UMI_STATUS_OK);
    assert(umi_dispatcher_create(schemas, &dispatcher) == UMI_STATUS_OK);
    subscription.kind = UMI_MESSAGE_EVENT;
    subscription.name = "studio.";
    subscription.prefix_match = 1;
    assert(umi_dispatcher_subscribe(dispatcher, &subscription,
                                    count_handler, &count, &id) == UMI_STATUS_OK);
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT,
                              "studio.saved", "ok");
    message.schema_id = "studio.event";
    assert(umi_dispatcher_dispatch(dispatcher, &message, &delivered) == UMI_STATUS_OK);
    assert(delivered == 1U && count == 1);
    assert(umi_dispatcher_unsubscribe(dispatcher, id) == UMI_STATUS_OK);
    umi_dispatcher_destroy(dispatcher);
    umi_schema_registry_destroy(schemas);
    return 0;
}
