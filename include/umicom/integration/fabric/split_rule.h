/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/split_rule.h
 *
 * PURPOSE:
 *   Describe splitter cardinality and empty-part handling independently of payload parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SPLIT_RULE_H
#define UMICOM_INTEGRATION_FABRIC_SPLIT_RULE_H

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
 * Represent the fabric split rule data shared with callers of this public contract.
 */
typedef struct UmiFabricSplitRule {
    char rule_id[UMI_FABRIC_ID_CAPACITY];
    char expression[UMI_FABRIC_TEXT_CAPACITY];
    size_t maximum_parts;
    bool discard_empty;
} UmiFabricSplitRule;

/**
 * Initialise fabric split rule from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_split_rule_init(UmiFabricSplitRule *item, const char *rule_id, const char *expression, size_t maximum_parts, bool discard_empty);
/**
 * Check that fabric split rule satisfies its contract before another service relies on it.
 */
UmiStatus umi_fabric_split_rule_validate(const UmiFabricSplitRule *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_split_rule_archive_encode(const UmiFabricSplitRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_split_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricSplitRule *value);

#ifdef __cplusplus
}
#endif
#endif
