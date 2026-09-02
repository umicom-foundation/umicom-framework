/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/messaging/journal.c
 *
 * PURPOSE:
 *   Implement the journal behavior for
 *   Umicom Framework.
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
    while (input != 0 && *input != '\0' && index + 1U < capacity) {
        char value = *input++;
        output[index++] = (value == '\t' || value == '\r' || value == '\n') ? ' ' : value;
    }
    output[index] = '\0';
}

/* Provide the journal open operation used by this module and its client applications. */
UmiStatus umi_journal_open(const char *path, UmiJournal **out_journal)
{
    UmiJournal *journal;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path == 0 || out_journal == 0) return UMI_STATUS_INVALID_ARGUMENT;
    journal = (UmiJournal *)calloc(1U, sizeof(*journal));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (journal == 0) return UMI_STATUS_OUT_OF_MEMORY;
    journal->file = fopen(path, "ab");
    /* Apply this branch only when its contract condition is satisfied. */
    if (journal->file == 0) {
        free(journal);
        return UMI_STATUS_IO_ERROR;
    }
    *out_journal = journal;
    return UMI_STATUS_OK;
}

/* Provide the journal close operation used by this module and its client applications. */
void umi_journal_close(UmiJournal *journal)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (journal == 0) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (journal->file != 0) fclose(journal->file);
    free(journal);
}

/* Add journal only after its inputs and available capacity have been checked. */
UmiStatus umi_journal_append(UmiJournal *journal, const UmiMessageEnvelope *event)
{
    char name[256];
    char payload[2048];
    /* Apply this branch only when its contract condition is satisfied. */
    if (journal == 0 || journal->file == 0 || event == 0 || event->name == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    clean_field(event->name, name, sizeof(name));
    clean_field(event->payload, payload, sizeof(payload));
    /* Apply this branch only when its contract condition is satisfied. */
    if (fprintf(journal->file, "%llu\t%llu\t%s\t%s\n",
                (unsigned long long)event->sequence,
                (unsigned long long)event->correlation_id,
                name, payload) < 0) return UMI_STATUS_IO_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fflush(journal->file) != 0) return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}

/* Provide the journal replay operation used by this module and its client applications. */
UmiStatus umi_journal_replay(const char *path, UmiEventBus *event_bus,
                             size_t *out_event_count)
{
    FILE *file;
    char line[4096];
    size_t count = 0U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path == 0 || event_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (file == 0) return UMI_STATUS_IO_ERROR;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (fgets(line, sizeof(line), file) != 0) {
        char *sequence = strtok(line, "\t");
        char *correlation = strtok(0, "\t");
        char *name = strtok(0, "\t");
        char *payload = strtok(0, "\r\n");
        UmiStatus status;
        (void)sequence;
        /* Apply this branch only when its contract condition is satisfied. */
        if (correlation == 0 || name == 0) continue;
        status = umi_event_bus_publish(event_bus, name, payload != 0 ? payload : "",
                                       (uint64_t)strtoull(correlation, 0, 10));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            fclose(file);
            return status;
        }
        count += 1U;
    }
    fclose(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_event_count != 0) *out_event_count = count;
    return UMI_STATUS_OK;
}
