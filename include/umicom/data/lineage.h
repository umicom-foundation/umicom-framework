/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/lineage.h
 *
 * PURPOSE:
 *   Define append-only lineage records connecting an entity revision to its source, operation, actor and correlation identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_LINEAGE_H
#define UMICOM_DATA_LINEAGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/repository.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the lineage record data shared with callers of this public contract.
 */
typedef struct UmiLineageRecord {
    uint64_t sequence;
    char entity_id[128];
    char operation[96];
    char source[160];
    char actor[128];
    uint64_t correlation_id;
    uint64_t occurred_at_nanoseconds;
} UmiLineageRecord;

/**
 * Represent the lineage store data shared with callers of this public contract.
 */
typedef struct UmiLineageStore {
    UmiRepository repository;
    uint64_t next_sequence;
} UmiLineageStore;

/**
 * Initialise lineage store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_lineage_store_init(UmiLineageStore *store,
                                 const UmiStore *data_store);
/**
 * Add lineage only after its inputs and available capacity have been checked.
 */
UmiStatus umi_lineage_append(UmiLineageStore *store,
                             UmiLineageRecord *record);
/**
 * Read lineage into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_lineage_load(const UmiLineageStore *store,
                           uint64_t sequence,
                           UmiLineageRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif
