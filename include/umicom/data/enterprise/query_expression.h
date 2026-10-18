/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_expression.h
 *
 * PURPOSE:
 *   Represent a portable query expression node for later backend translation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_EXPRESSION_H
#define UMICOM_DATA_ENTERPRISE_QUERY_EXPRESSION_H

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
 * Represent the data query expression data shared with callers of this public contract.
 */
typedef struct UmiDataQueryExpression {
    char expression_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char operation[32];
    char value[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool parameterized;
} UmiDataQueryExpression;

/* Initialise a validated query expression descriptor. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_query_expression_init(UmiDataQueryExpression *item, const char *expression_id, const char *field, const char *operation, const char *value);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_expression_validate(const UmiDataQueryExpression *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_query_expression_archive_encode(const UmiDataQueryExpression *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_query_expression_archive_decode(const void *bytes, size_t byte_count,
    UmiDataQueryExpression *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDataQueryExpression object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_data_query_expression_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDataQueryExpression *workspace, UmiDataQueryExpression *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_query_expression_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataQueryExpression *expected, UmiDataQueryExpression *value);


#ifdef __cplusplus
}
#endif
#endif
