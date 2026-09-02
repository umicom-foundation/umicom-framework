/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/change_broker.c
 *
 * PURPOSE:
 *   Implement durable change publication by composing the Framework event bus
 *   and journal rather than introducing a second messaging implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/change_broker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/messaging/journal.h"
#include "umicom/messaging/message.h"

#define UMI_CHANGE_BROKER_SOURCE_CAPACITY 128U

struct UmiChangeBroker {
    UmiEventBus *event_bus;
    UmiJournal *journal;
    char *journal_path;
    char source_id[UMI_CHANGE_BROKER_SOURCE_CAPACITY];
    uint64_t next_sequence;
};

/* Copy an optional path into broker-owned memory for later journal replay. */
static char *copy_text(const char *text)
{
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }
    length = strlen(text);
    if (length == SIZE_MAX) {
        return NULL;
    }
    copy = (char *)malloc(length + 1U);
    if (copy != NULL) {
        (void)memcpy(copy, text, length + 1U);
    }
    return copy;
}

UmiChangeBrokerConfig umi_change_broker_config_default(void)
{
    UmiChangeBrokerConfig config;

    config.structure_size = (uint32_t)sizeof(config);
    config.journal_path = NULL;
    config.source_id = "umicom.change-broker";
    return config;
}

UmiStatus umi_change_broker_create(const UmiChangeBrokerConfig *config,
                                   UmiChangeBroker **out_broker)
{
    UmiChangeBrokerConfig effective_config;
    UmiChangeBroker *broker;
    UmiStatus status;

    if (out_broker == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_broker = NULL;
    if (config != NULL && config->structure_size < sizeof(*config)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective_config = config != NULL
        ? *config
        : umi_change_broker_config_default();
    if (effective_config.source_id != NULL &&
        strlen(effective_config.source_id) >=
            UMI_CHANGE_BROKER_SOURCE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (effective_config.source_id != NULL &&
        effective_config.source_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    broker = (UmiChangeBroker *)calloc(1U, sizeof(*broker));
    if (broker == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(broker->source_id,
                   sizeof(broker->source_id),
                   "%s",
                   effective_config.source_id != NULL
                       ? effective_config.source_id
                       : "umicom.change-broker");
    status = umi_event_bus_create(&broker->event_bus);
    if (status != UMI_STATUS_OK) {
        umi_change_broker_destroy(broker);
        return status;
    }

    /* A missing or empty path deliberately selects non-durable operation. */
    if (effective_config.journal_path != NULL &&
        effective_config.journal_path[0] != '\0') {
        broker->journal_path = copy_text(effective_config.journal_path);
        if (broker->journal_path == NULL) {
            umi_change_broker_destroy(broker);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = umi_journal_open(broker->journal_path, &broker->journal);
        if (status != UMI_STATUS_OK) {
            umi_change_broker_destroy(broker);
            return status;
        }
    }

    broker->next_sequence = 1U;
    *out_broker = broker;
    return UMI_STATUS_OK;
}

void umi_change_broker_destroy(UmiChangeBroker *broker)
{
    if (broker == NULL) {
        return;
    }
    umi_journal_close(broker->journal);
    umi_event_bus_destroy(broker->event_bus);
    free(broker->journal_path);
    free(broker);
}

UmiStatus umi_change_broker_subscribe(UmiChangeBroker *broker,
                                      const char *topic,
                                      UmiEventHandler handler,
                                      void *user_data)
{
    if (broker == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_event_bus_subscribe(
        broker->event_bus, topic, handler, user_data);
}

UmiStatus umi_change_broker_publish(UmiChangeBroker *broker,
                                    const char *topic,
                                    const char *payload,
                                    uint64_t correlation_id)
{
    UmiMessageEnvelope event;
    UmiStatus status;

    if (broker == NULL || topic == NULL || topic[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_message_envelope_init(
        &event, UMI_MESSAGE_EVENT, topic, payload != NULL ? payload : "");
    event.sequence = broker->next_sequence;
    event.correlation_id = correlation_id;
    event.flags = broker->journal != NULL
        ? (uint32_t)UMI_MESSAGE_FLAG_DURABLE
        : (uint32_t)UMI_MESSAGE_FLAG_NONE;
    event.schema_id = topic;
    event.source = broker->source_id;

    /* Persist before delivery so subscribers never observe an unrecorded event. */
    if (broker->journal != NULL) {
        status = umi_journal_append(broker->journal, &event);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = umi_event_bus_publish(
        broker->event_bus, topic, event.payload, correlation_id);
    if (status == UMI_STATUS_OK) {
        broker->next_sequence += 1U;
    }
    return status;
}

UmiStatus umi_change_broker_replay(UmiChangeBroker *broker,
                                   size_t *out_event_count)
{
    UmiStatus status;

    if (broker == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (broker->journal_path == NULL) {
        return UMI_STATUS_UNAVAILABLE;
    }

    /* Flush the writer before a second file handle begins replaying the log. */
    status = umi_journal_replay(
        broker->journal_path, broker->event_bus, out_event_count);
    if (status == UMI_STATUS_OK) {
        broker->next_sequence =
            umi_event_bus_last_sequence(broker->event_bus) + 1U;
    }
    return status;
}

uint64_t umi_change_broker_last_sequence(const UmiChangeBroker *broker)
{
    if (broker == NULL || broker->next_sequence == 0U) {
        return 0U;
    }
    return broker->next_sequence - 1U;
}

int umi_change_broker_is_durable(const UmiChangeBroker *broker)
{
    return broker != NULL && broker->journal != NULL;
}
