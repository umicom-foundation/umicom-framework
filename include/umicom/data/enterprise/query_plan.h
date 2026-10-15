/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_plan.h
 *
 * PURPOSE:
 *   Compose predicate, projection, ordering and join counts into a reviewable backend-neutral query plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PLAN_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PLAN_H

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
 * Represent the data query plan data shared with callers of this public contract.
 */
typedef struct UmiDataQueryPlan { char plan_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char root_table[UMI_DATA_ENTERPRISE_ID_CAPACITY]; size_t predicate_count; size_t projection_count; size_t join_count; size_t order_count; uint64_t row_limit; bool read_only; } UmiDataQueryPlan;
/* Initialise a query plan rooted at one logical table. */ /** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_query_plan_init(UmiDataQueryPlan *plan,const char *plan_id,const char *root_table);
/* Add bounded structural complexity evidence. */ UmiStatus umi_data_query_plan_shape(UmiDataQueryPlan *plan,size_t predicates,size_t projections,size_t joins,size_t orders,uint64_t row_limit);
/* Validate safe, bounded query plan state. */ UmiStatus umi_data_query_plan_validate(const UmiDataQueryPlan *plan);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_query_plan_archive_encode(const UmiDataQueryPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_query_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiDataQueryPlan *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_query_plan_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataQueryPlan *expected, UmiDataQueryPlan *value);


#ifdef __cplusplus
}
#endif
#endif
