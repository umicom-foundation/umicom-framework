/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/filter_expression.h
 *
 * PURPOSE:
 *   Describe a bounded textual equality/prefix filter used by route and workflow policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_FILTER_EXPRESSION_H
#define UMICOM_INTEGRATION_FABRIC_FILTER_EXPRESSION_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric filter expression data shared with callers of this public contract.
 */
typedef struct UmiFabricFilterExpression {
    char field[UMI_FABRIC_TEXT_CAPACITY];
    char operation[UMI_FABRIC_ID_CAPACITY];
    char value[UMI_FABRIC_TEXT_CAPACITY];
} UmiFabricFilterExpression;

/**
 * Initialise fabric filter expression from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_filter_expression_init(UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value);
/**
 * Check that fabric filter expression satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_filter_expression_validate(const UmiFabricFilterExpression *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_filter_expression_archive_encode(const UmiFabricFilterExpression *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_filter_expression_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricFilterExpression *value);

#ifdef __cplusplus
}
#endif
#endif
