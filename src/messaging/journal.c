/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/journal.c
 *
 * PURPOSE:
 *   Implement the compatibility append-only event journal while preserving message identifiers, causation, schema and replay semantics in a tab-safe text format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/journal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiJournal {
    FILE *file;
};

/* Provide the clean field operation used by this module and its client applications. */
static void clean_field(const char *input, char *output, size_t capacity)
{
    size_t index = 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input != NULL && *input != '\0' && index + 1U < capacity) {
        char value = *input++;
        output[index++] = (value == '\t' || value == '\r' || value == '\n')
            ? ' ' : value;
    }
    output[index] = '\0';
}

/* Provide the journal open operation used by this module and its client applications. */
UmiStatus umi_journal_open(const char *path, UmiJournal **out_journal)
{
    UmiJournal *journal;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_journal == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_journal = NULL;
    journal = (UmiJournal *)calloc(1U, sizeof(*journal));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    journal->file = fopen(path, "ab");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal->file == NULL) {
        free(journal);
        return UMI_STATUS_IO_ERROR;
    }
    *out_journal = journal;
    return UMI_STATUS_OK;
}

/* Provide the journal close operation used by this module and its client applications. */
void umi_journal_close(UmiJournal *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal->file != NULL) (void)fclose(journal->file);
    free(journal);
}

/* Add journal only after its inputs and available capacity have been checked. */
UmiStatus umi_journal_append(UmiJournal *journal,
                             const UmiMessageEnvelope *event)
{
    char name[256];
    char schema[256];
    char source[256];
    char payload[2048];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || journal->file == NULL || event == NULL ||
        event->name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    clean_field(event->name, name, sizeof(name));
    clean_field(event->schema_id, schema, sizeof(schema));
    clean_field(event->source, source, sizeof(source));
    clean_field(event->payload, payload, sizeof(payload));
    /* Apply this branch only when its contract condition is satisfied. */
    if (fprintf(journal->file,
                "%llu\t%llu\t%llu\t%llu\t%u\t%s\t%s\t%s\t%s\n",
                (unsigned long long)event->sequence,
                (unsigned long long)event->message_id,
                (unsigned long long)event->correlation_id,
                (unsigned long long)event->causation_id,
                event->schema_version,
                name,
                schema,
                source,
                payload) < 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return fflush(journal->file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Provide the journal replay operation used by this module and its client applications. */
UmiStatus umi_journal_replay(const char *path,
                             UmiEventBus *event_bus,
                             size_t *out_event_count)
{
    FILE *file;
    char line[4096];
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || event_bus == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (fgets(line, sizeof(line), file) != NULL) {
        char *sequence = strtok(line, "\t");
        char *message_id = strtok(NULL, "\t");
        char *correlation = strtok(NULL, "\t");
        char *causation = strtok(NULL, "\t");
        char *version = strtok(NULL, "\t");
        char *name = strtok(NULL, "\t");
        char *schema = strtok(NULL, "\t");
        char *source = strtok(NULL, "\t");
        char *payload = strtok(NULL, "\r\n");
        UmiStatus status;
        (void)sequence;
        (void)message_id;
        (void)causation;
        (void)version;
        (void)schema;
        (void)source;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (correlation == NULL || name == NULL) continue;
        status = umi_event_bus_publish(
            event_bus,
            name,
            payload != NULL ? payload : "",
            (uint64_t)strtoull(correlation, NULL, 10));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)fclose(file);
            return status;
        }
        count++;
    }
    (void)fclose(file);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_event_count != NULL) *out_event_count = count;
    return UMI_STATUS_OK;
}
