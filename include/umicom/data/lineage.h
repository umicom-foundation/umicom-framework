/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/lineage.h
 *
 * PURPOSE:
 *   Define append-only lineage records connecting an entity revision to its source, operation, actor and correlation identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiLineageRecord {
    uint64_t sequence;
    char entity_id[128];
    char operation[96];
    char source[160];
    char actor[128];
    uint64_t correlation_id;
    uint64_t occurred_at_nanoseconds;
} UmiLineageRecord;

typedef struct UmiLineageStore {
    UmiRepository repository;
    uint64_t next_sequence;
} UmiLineageStore;

UmiStatus umi_lineage_store_init(UmiLineageStore *store,
                                 const UmiStore *data_store);
UmiStatus umi_lineage_append(UmiLineageStore *store,
                             UmiLineageRecord *record);
UmiStatus umi_lineage_load(const UmiLineageStore *store,
                           uint64_t sequence,
                           UmiLineageRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif
