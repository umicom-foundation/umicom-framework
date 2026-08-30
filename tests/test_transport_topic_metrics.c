/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_transport_topic_metrics.c
 *
 * PURPOSE:
 *   Verify in-memory transport, topic metadata and atomic Integration Fabric counters.
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
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiTransport transport;
    UmiOwnedMessage received;
    UmiMessageEnvelope message;
    UmiTopicRegistry *topics = NULL;
    UmiTopicDescriptor topic = {
        sizeof(UmiTopicDescriptor), "studio.events", "studio.event", 4U, 1
    };
    UmiMessageMetricsCounter *counter;
    UmiMessageMetrics snapshot;
    assert(umi_memory_transport_create(4U, &transport) == UMI_STATUS_OK);
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, "saved", "ok");
    assert(transport.send(transport.instance, &message) == UMI_STATUS_OK);
    assert(transport.receive(transport.instance, &received) == UMI_STATUS_OK);
    assert(strcmp(received.envelope.name, "saved") == 0);
    umi_message_dispose(&received);
    umi_transport_dispose(&transport);
    assert(umi_topic_registry_create(4U, &topics) == UMI_STATUS_OK);
    assert(umi_topic_registry_register(topics, &topic) == UMI_STATUS_OK);
    assert(umi_topic_registry_find(topics, "studio.events") != NULL);
    umi_topic_registry_destroy(topics);
    counter = (UmiMessageMetricsCounter *)calloc(1U,
        umi_message_metrics_counter_size());
    assert(counter != NULL);
    umi_message_metrics_init(counter);
    umi_message_metrics_increment(counter, UMI_MESSAGE_METRIC_ACCEPTED);
    snapshot = umi_message_metrics_snapshot(counter);
    assert(snapshot.accepted == 1U);
    free(counter);
    return 0;
}
