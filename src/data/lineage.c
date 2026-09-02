/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/lineage.c
 *
 * PURPOSE:
 *   Implement append-only lineage sequence allocation, serialisation and lookup through the Data Server repository boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/lineage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialise lineage store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_lineage_store_init(UmiLineageStore *store,
                                 const UmiStore *data_store)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || data_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(store, 0, sizeof(*store));
    status = umi_repository_init(&store->repository,
                                 data_store,
                                 "lineage");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) store->next_sequence = 1U;
    return status;
}

/* Add lineage only after its inputs and available capacity have been checked. */
UmiStatus umi_lineage_append(UmiLineageStore *store,
                             UmiLineageRecord *record)
{
    char key[32];
    char text[1024];
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || record == NULL || record->entity_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    record->sequence = store->next_sequence++;
    (void)snprintf(key, sizeof(key), "%020llu",
                   (unsigned long long)record->sequence);
    written = snprintf(text,
                       sizeof(text),
                       "%s\t%s\t%s\t%s\t%llu\t%llu",
                       record->entity_id,
                       record->operation,
                       record->source,
                       record->actor,
                       (unsigned long long)record->correlation_id,
                       (unsigned long long)record->occurred_at_nanoseconds);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_repository_save(&store->repository, key, text);
}

/* Read lineage into validated module state and return a status when input cannot be used. */
UmiStatus umi_lineage_load(const UmiLineageStore *store,
                           uint64_t sequence,
                           UmiLineageRecord *out_record)
{
    char key[32];
    char text[1024];
    char *fields[6];
    char *cursor;
    size_t count = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || sequence == 0U || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(key, sizeof(key), "%020llu",
                   (unsigned long long)sequence);
    status = umi_repository_load(&store->repository, key, text, sizeof(text));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    cursor = strtok(text, "\t");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && count < 6U) {
        fields[count++] = cursor;
        cursor = strtok(NULL, "\t");
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count != 6U) return UMI_STATUS_PARSE_ERROR;
    (void)memset(out_record, 0, sizeof(*out_record));
    out_record->sequence = sequence;
    (void)snprintf(out_record->entity_id,
                   sizeof(out_record->entity_id), "%s", fields[0]);
    (void)snprintf(out_record->operation,
                   sizeof(out_record->operation), "%s", fields[1]);
    (void)snprintf(out_record->source,
                   sizeof(out_record->source), "%s", fields[2]);
    (void)snprintf(out_record->actor,
                   sizeof(out_record->actor), "%s", fields[3]);
    out_record->correlation_id = (uint64_t)strtoull(fields[4], NULL, 10);
    out_record->occurred_at_nanoseconds =
        (uint64_t)strtoull(fields[5], NULL, 10);
    return UMI_STATUS_OK;
}
