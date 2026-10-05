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

#ifdef __cplusplus
}
#endif
#endif
