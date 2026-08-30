/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_aggregator.c
 *
 * PURPOSE:
 *   Verify correlation grouping and completion only after the expected message count.
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


static UmiMessageEnvelope make_message(const char *name,
                                       uint64_t sequence,
                                       uint64_t correlation)
{
    UmiMessageEnvelope message;
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, name, "payload");
    message.sequence = sequence;
    message.correlation_id = correlation;
    message.schema_id = "test.event";
    message.schema_version = 1U;
    return message;
}

int main(void)
{
    UmiMessageAggregator *aggregator = NULL;
    UmiAggregateView view;
    UmiMessageEnvelope one = make_message("one", 1U, 100U);
    UmiMessageEnvelope two = make_message("two", 2U, 100U);
    assert(umi_message_aggregator_create(4U, 2U, &aggregator) == UMI_STATUS_OK);
    assert(umi_message_aggregator_add(aggregator, &one, &view) == UMI_STATUS_OK);
    assert(view.count == 0U);
    assert(umi_message_aggregator_add(aggregator, &two, &view) == UMI_STATUS_OK);
    assert(view.count == 2U && view.correlation_id == 100U);
    assert(umi_message_aggregator_release(aggregator, 100U) == UMI_STATUS_OK);
    umi_message_aggregator_destroy(aggregator);
    return 0;
}
