/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_outbox.c
 *
 * PURPOSE:
 *   Verify outbox enqueue, in-flight selection, retry, successful completion and compaction.
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
 * Exercise make message and return a clear result when the behaviour no longer matches its
 * contract.
 */
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOutbox *outbox = NULL;
    UmiOutboxRecordView view;
    UmiMessageEnvelope message = make_message("event", 1U, 7U);
    uint64_t id = 0U;
    size_t removed = 0U;
    assert(umi_outbox_create(4U, &outbox) == UMI_STATUS_OK);
    assert(umi_outbox_enqueue(outbox, &message, &id) == UMI_STATUS_OK);
    assert(umi_outbox_next_pending(outbox, &view) == UMI_STATUS_OK);
    assert(view.record_id == id && view.attempts == 1U);
    assert(umi_outbox_complete(outbox, id, UMI_STATUS_TIMEOUT, 1) == UMI_STATUS_OK);
    assert(umi_outbox_next_pending(outbox, &view) == UMI_STATUS_OK);
    assert(view.attempts == 2U);
    assert(umi_outbox_complete(outbox, id, UMI_STATUS_OK, 0) == UMI_STATUS_OK);
    assert(umi_outbox_remove_sent(outbox, &removed) == UMI_STATUS_OK);
    assert(removed == 1U && umi_outbox_stats(outbox).records == 0U);
    umi_outbox_destroy(outbox);
    return 0;
}
