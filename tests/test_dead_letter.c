/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_dead_letter.c
 *
 * PURPOSE:
 *   Verify retained dead-letter metadata, inspection and removal.
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
#include <string.h>


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
    UmiDeadLetterStore *store = NULL;
    UmiDeadLetterView view;
    UmiMessageEnvelope message = make_message("failed", 1U, 9U);
    uint64_t id = 0U;
    assert(umi_dead_letter_store_create(4U, &store) == UMI_STATUS_OK);
    assert(umi_dead_letter_store_add(store, &message, UMI_STATUS_TIMEOUT,
                                     "remote timeout", 3U, 100U, &id) == UMI_STATUS_OK);
    assert(umi_dead_letter_store_get(store, 0U, &view) == UMI_STATUS_OK);
    assert(view.dead_letter_id == id && strcmp(view.reason, "remote timeout") == 0);
    assert(umi_dead_letter_store_remove(store, id) == UMI_STATUS_OK);
    assert(umi_dead_letter_store_count(store) == 0U);
    umi_dead_letter_store_destroy(store);
    return 0;
}
