/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_router_filter_transformer.c
 *
 * PURPOSE:
 *   Verify message filters, content-based routing and owned transformations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

static int accept_events(const UmiMessageEnvelope *message, void *user_data)
{
    (void)user_data;
    return message->kind == UMI_MESSAGE_EVENT;
}

static UmiStatus rename_message(const UmiMessageEnvelope *input,
                                UmiOwnedMessage *output,
                                void *user_data)
{
    UmiMessageEnvelope transformed = *input;
    transformed.name = (const char *)user_data;
    return umi_message_copy(&transformed, output);
}

int main(void)
{
    UmiFilterChain *filters = NULL;
    UmiTransformerChain *transforms = NULL;
    UmiMessageRouter *router = NULL;
    UmiChannel *channel = NULL;
    UmiChannelConfig config = {4U, 1};
    UmiSubscription subscription = umi_subscription_all();
    UmiMessageEnvelope message;
    UmiOwnedMessage transformed;
    UmiOwnedMessage routed;
    size_t deliveries = 0U;
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, "source.event", "x");
    assert(umi_filter_chain_create(2U, &filters) == UMI_STATUS_OK);
    assert(umi_filter_chain_add(filters, accept_events, NULL) == UMI_STATUS_OK);
    assert(umi_filter_chain_accepts(filters, &message));
    assert(umi_transformer_chain_create(2U, &transforms) == UMI_STATUS_OK);
    assert(umi_transformer_chain_add(transforms, rename_message,
                                     "target.event") == UMI_STATUS_OK);
    assert(umi_transformer_chain_apply(transforms, &message,
                                       &transformed) == UMI_STATUS_OK);
    assert(strcmp(transformed.envelope.name, "target.event") == 0);
    assert(umi_channel_create(&config, &channel) == UMI_STATUS_OK);
    assert(umi_message_router_create(2U, &router) == UMI_STATUS_OK);
    subscription.name = "target.event";
    assert(umi_message_router_add(router, &subscription, channel) == UMI_STATUS_OK);
    assert(umi_message_router_route(router, &transformed.envelope,
                                    &deliveries) == UMI_STATUS_OK);
    assert(deliveries == 1U);
    assert(umi_channel_pop(channel, &routed) == UMI_STATUS_OK);
    umi_message_dispose(&routed);
    umi_message_dispose(&transformed);
    umi_message_router_destroy(router);
    umi_channel_destroy(channel);
    umi_transformer_chain_destroy(transforms);
    umi_filter_chain_destroy(filters);
    return 0;
}
