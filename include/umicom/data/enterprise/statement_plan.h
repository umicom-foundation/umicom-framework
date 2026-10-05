/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/statement_plan.h
 *
 * PURPOSE:
 *   Describe a prepared statement contract and its query/schema fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_STATEMENT_PLAN_H
#define UMICOM_DATA_ENTERPRISE_STATEMENT_PLAN_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data statement plan data shared with callers of this public contract.
 */
typedef struct UmiDataStatementPlan {
    char statement_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint64_t query_fingerprint;
    uint64_t schema_fingerprint;
    size_t parameter_count;
    bool read_only;
} UmiDataStatementPlan;

/* Initialise a validated statement plan descriptor. */
UmiStatus umi_data_statement_plan_init(UmiDataStatementPlan *item, const char *statement_id, uint64_t query_fingerprint, uint64_t schema_fingerprint, size_t parameter_count, bool read_only);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_statement_plan_validate(const UmiDataStatementPlan *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_statement_plan_archive_encode(const UmiDataStatementPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_statement_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiDataStatementPlan *value);

#ifdef __cplusplus
}
#endif
#endif
