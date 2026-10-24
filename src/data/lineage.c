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

/* Superseded implementation retained for engineering review. The checked
 * implementation below preserves valid legacy records, checks occupied keys
 * after reopening a store, and publishes outputs only after success. The former
 * sequence allocation and permissive parsing remain here for comparison. */
#if 0
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
#endif

#include "include/record_fields.h"

/* A store may already contain records when a second handle is opened. The
 * sequence is a search hint, not authority to replace an occupied record.
 * Provider-neutral UmiStore has no compare-and-insert operation; callers must
 * serialize legacy appenders. Transaction-owned record_history is available
 * when an application needs stale-write protection over a Data Server. */
UmiStatus umi_lineage_store_init(UmiLineageStore *store, const UmiStore *data_store)
{
    if (store == NULL || data_store == NULL || data_store->get == NULL ||
        data_store->set == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiLineageStore candidate = {0};
    UmiStatus status = umi_repository_init(&candidate.repository, data_store, "lineage");
    if (status == UMI_STATUS_OK) {
        candidate.next_sequence = 1U;
        *store = candidate;
    }
    return status;
}

UmiStatus umi_lineage_append(UmiLineageStore *store, UmiLineageRecord *record)
{
    if (store == NULL || record == NULL ||
        !umi_record_field_valid(record->entity_id, sizeof record->entity_id, false) ||
        !umi_record_field_valid(record->operation, sizeof record->operation, true) ||
        !umi_record_field_valid(record->source, sizeof record->source, true) ||
        !umi_record_field_valid(record->actor, sizeof record->actor, true)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->next_sequence == 0U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char text[1024];
    int written = snprintf(text, sizeof text, "%s\t%s\t%s\t%s\t%llu\t%llu",
        record->entity_id, record->operation, record->source, record->actor,
        (unsigned long long)record->correlation_id,
        (unsigned long long)record->occurred_at_nanoseconds);
    if (written < 0 || (size_t)written >= sizeof text) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char key[32];
    char occupied[2];
    uint64_t sequence = store->next_sequence;
    for (;;) {
        (void)snprintf(key, sizeof key, "%020llu", (unsigned long long)sequence);
        UmiStatus status = umi_repository_load(&store->repository, key, occupied, sizeof occupied);
        if (status == UMI_STATUS_NOT_FOUND) {
            break;
        }
        if (status != UMI_STATUS_OK && status != UMI_STATUS_CAPACITY_EXCEEDED) {
            return status;
        }
        if (sequence == UINT64_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        ++sequence;
    }
    UmiStatus status = umi_repository_save(&store->repository, key, text);
    if (status == UMI_STATUS_OK) {
        /* Publish caller-visible identity only after persistence succeeds. */
        record->sequence = sequence;
        store->next_sequence = sequence == UINT64_MAX ? 0U : sequence + 1U;
    }
    return status;
}

UmiStatus umi_lineage_load(const UmiLineageStore *store, uint64_t sequence,
                          UmiLineageRecord *out_record)
{
    if (store == NULL || sequence == 0U || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    char key[32];
    char text[1024];
    char *fields[6];
    (void)snprintf(key, sizeof key, "%020llu", (unsigned long long)sequence);
    memset(text, 0xff, sizeof text);
    UmiStatus status = umi_repository_load(&store->repository, key, text, sizeof text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiLineageRecord candidate = {0};
    candidate.sequence = sequence;
    if (!umi_record_fields_split(text, sizeof text, fields, 6U) ||
        !umi_record_field_copy(candidate.entity_id, sizeof candidate.entity_id, fields[0], false) ||
        !umi_record_field_copy(candidate.operation, sizeof candidate.operation, fields[1], true) ||
        !umi_record_field_copy(candidate.source, sizeof candidate.source, fields[2], true) ||
        !umi_record_field_copy(candidate.actor, sizeof candidate.actor, fields[3], true) ||
        !umi_record_field_u64(fields[4], &candidate.correlation_id) ||
        !umi_record_field_u64(fields[5], &candidate.occurred_at_nanoseconds)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_record = candidate;
    return UMI_STATUS_OK;
}
