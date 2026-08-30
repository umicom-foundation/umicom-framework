/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_message_store.c
 *
 * PURPOSE:
 *   Verify durable message serialisation, ordered read and journal consumer positions.
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

int main(void)
{
    UmiDataServer *server = NULL;
    UmiStore store;
    UmiJournalStore journal;
    UmiMessageEnvelope message;
    UmiOwnedMessage loaded;
    uint64_t sequence = 0U;
    uint64_t position = 0U;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_store_from_data_server(server, &store) == UMI_STATUS_OK);
    assert(umi_journal_store_init(&journal, &store, "studio") == UMI_STATUS_OK);
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT,
                              "studio.saved", "document.c");
    message.schema_id = "studio.event";
    message.correlation_id = 99U;
    assert(umi_journal_store_append(&journal, &message, &sequence) == UMI_STATUS_OK);
    assert(sequence == 1U);
    assert(umi_journal_store_read(&journal, 0U, &loaded) == UMI_STATUS_OK);
    assert(strcmp(loaded.envelope.name, "studio.saved") == 0);
    assert(loaded.envelope.correlation_id == 99U);
    umi_message_dispose(&loaded);
    assert(umi_journal_store_set_consumer_position(&journal,
                                                    "consumer", 1U) == UMI_STATUS_OK);
    assert(umi_journal_store_consumer_position(&journal,
                                                "consumer", &position) == UMI_STATUS_OK);
    assert(position == 1U);
    umi_journal_store_dispose(&journal);
    umi_data_server_destroy(server);
    return 0;
}
